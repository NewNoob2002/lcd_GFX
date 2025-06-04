#include "lv_port.h"
#include "../HAL/inc/HAL.h"
// #include "../ui/screens.h"

// 全局变量
static lv_group_t *g_input_group;  // 输入组
static lv_indev_t *g_keypad_indev; // 按键输入设备


// 按键输入处理函数
static void keypad_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    static uint32_t last_key = 0;
    static bool key_pressed = false;
    
    // 获取按键动作
    key_action_t action = HAL::Button_GetAction();
    
    // 根据按键动作执行不同操作
    if (action != KEY_ACTION_NONE) {
        
        // 处理不同类型的按键动作
        switch (action) {
            case KEY_ACTION_NONE:
                break;
            case KEY_ACTION_SINGLE_CLICK:
                last_key = LV_KEY_ENTER;
                key_pressed = true;
                break;
                
            case KEY_ACTION_DOUBLE_CLICK:
                // 双击 - 移动到下一个组件
                last_key = LV_KEY_NEXT;
                key_pressed = true;
                break;
                
            case KEY_ACTION_TRIPLE_CLICK:
                // 三击 - 切换页面
                break;
        }
    } else {
        // 没有新的按键动作，释放按键
        if (key_pressed) {
            key_pressed = false;
        }
    }
    
    // 设置按键状态
    data->state = key_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    data->key = last_key;
    
    // 释放按键后重置按键值
    if (data->state == LV_INDEV_STATE_RELEASED) {
        last_key = 0;
    }
}

// 添加所有需要焦点控制的UI元素到组
void add_all_objs_to_group(lv_obj_t *parent, lv_group_t *group)
{
    // 如果父对象为空，返回
    if (parent == NULL) return;
    
    // 获取子对象数量
    uint32_t child_cnt = lv_obj_get_child_cnt(parent);
    
    // 遍历所有子对象
    for (uint32_t i = 0; i < child_cnt; i++) {
        lv_obj_t *child = lv_obj_get_child(parent, i);
        
        // 检查是否是可聚焦控件类型
        if (lv_obj_check_type(child, &lv_switch_class) ||   // Switch
            lv_obj_check_type(child, &lv_btn_class) ||      // Button
            lv_obj_check_type(child, &lv_slider_class) ||   // Slider
            lv_obj_check_type(child, &lv_dropdown_class) || // Dropdown
            lv_obj_check_type(child, &lv_roller_class) ||   // Roller
            lv_obj_check_type(child, &lv_textarea_class)) { // TextArea
            
            // 添加到组
            lv_group_add_obj(group, child);
            printf("Added control to focus group\n");
        }
        
        // 递归处理子对象的子对象
        add_all_objs_to_group(child, group);
    }
}

void lv_port_indev_init(void)
{
    DISPLAY_PRINTF("lv_port_indev_init\n");
    
    // 创建输入设备组
    g_input_group = lv_group_create();
    
    // 创建并初始化按键输入设备驱动
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = keypad_read;
    g_keypad_indev = lv_indev_drv_register(&indev_drv);

    // 将输入设备与组关联
    lv_indev_set_group(g_keypad_indev, g_input_group);
    
    // 设置为默认组
    lv_group_set_default(g_input_group);
    
    // 使能循环聚焦 - 当到达组的结尾时，回到第一个控件
    lv_group_set_wrap(g_input_group, true);
}

// 当屏幕创建后调用此函数添加其控件到焦点组
void lv_port_focus_init(lv_obj_t *screen)
{
    // 先清除之前的所有对象
    lv_group_remove_all_objs(g_input_group);
    
    // 添加新屏幕上的所有控件
    add_all_objs_to_group(screen, g_input_group);
    
    // 设置第一个对象为焦点
    lv_group_focus_next(g_input_group);
}