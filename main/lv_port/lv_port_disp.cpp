#include "lv_port.h"
#include "../HAL/inc/HAL.h"

#define SCREEN_BUFFER_SIZE (CONFIG_SCREEN_HOR_RES * CONFIG_SCREEN_VER_RES / 2)
#define SCREEN_USE_DOUBLE_BUFFER 1 // 禁用双缓冲以节省内存

// 静态分配缓冲区以避免动态内存分配问题
#if SCREEN_USE_DOUBLE_BUFFER
static lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(SCREEN_BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
static lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(SCREEN_BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
#else
static lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(SCREEN_BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
#endif

// LVGL显示刷新回调函数
static void disp_flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{

    const lv_coord_t w = (area->x2 - area->x1 + 1);
    const lv_coord_t h = (area->y2 - area->y1 + 1);

    // 发送像素数据到LCD
    HAL::Display_SendPixels(area->x1, area->y1, w, h, (uint16_t *)color_p);

    // 通知LVGL刷新完成
    lv_disp_flush_ready(disp);
}

static TaskHandle_t g_lvgl_task_handle = NULL;

static SemaphoreHandle_t xGuiSemaphore = NULL;

// LVGL任务处理函数
static void gui_task(void *args)
{
    DISPLAY_PRINTF("Start LVGL Task\n");

    while (1)
    {
        // 使用超时来防止死锁
        if (xSemaphoreTake(xGuiSemaphore, 10 / portTICK_PERIOD_MS) == pdTRUE)
        {
            lv_task_handler();
            // ui_tick();
            xSemaphoreGive(xGuiSemaphore);
        }
        else
        {
            DISPLAY_PRINTF("无法获取LVGL信号量，跳过此次更新\n");
        }

        // 给其他任务一些时间
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void lv_tick_task(void *arg)
{
    (void)arg;
    lv_tick_inc(1);
}

void lv_port_disp_init()
{
    DISPLAY_PRINTF("Init LVGL Display Port\n");

    // 初始化显示缓冲区
    static lv_disp_draw_buf_t disp_buf;

#if SCREEN_USE_DOUBLE_BUFFER
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, SCREEN_BUFFER_SIZE);
#else
    lv_disp_draw_buf_init(&disp_buf, buf1, NULL, SCREEN_BUFFER_SIZE);
#endif

    // 初始化并注册显示驱动
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    // 设置分辨率
    disp_drv.hor_res = CONFIG_SCREEN_HOR_RES;
    disp_drv.ver_res = CONFIG_SCREEN_VER_RES;
    // 设置回调函数
    disp_drv.flush_cb = disp_flush_cb;
    // disp_drv.wait_cb = disp_wait_cb;
    disp_drv.draw_buf = &disp_buf;

    // 注册显示驱动
    lv_disp_drv_register(&disp_drv);

    DISPLAY_PRINTF("Init LVGL Tick Timer\n");
    const esp_timer_create_args_t lv_periodic_timer_args = {
        .callback = &lv_tick_task,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "periodic_gui",
        .skip_unhandled_events = true};

    esp_timer_handle_t lv_periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&lv_periodic_timer_args, &lv_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lv_periodic_timer, 1 * 1000));
    // 创建互斥信号量
    xGuiSemaphore = xSemaphoreCreateMutex();
    if (!xGuiSemaphore)
    {
        DISPLAY_PRINTF("Failed to create LVGL semaphore\n");
        return;
    }
    xSemaphoreGive(xGuiSemaphore);
    DISPLAY_PRINTF("Init LVGL Task\n");
    // 创建LVGL任务，增加优先级并放在核心0上运行
    BaseType_t ret = xTaskCreatePinnedToCore(
        gui_task,            // 任务函数
        "lv_gui",            // 任务名称
        8 * 1024,            // 堆栈大小
        NULL,                // 任务参数
        3,                   // 任务优先级 (降低优先级避免抢占系统关键任务)
        &g_lvgl_task_handle, // 任务句柄
        1                    // 在核心0上运行 (让核心1处理其他工作)
    );

    if (ret != pdPASS)
    {
        DISPLAY_PRINTF("Failed to create LVGL task\n");
        return;
    }
}