#pragma once

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_timer.h>
#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* LVGL初始化 */
void lv_port_init(void);

/* 显示接口初始化 */
void lv_port_disp_init(void);

/* 输入设备初始化 */
void lv_port_indev_init(void);

/* 焦点初始化 - 为指定屏幕上的控件添加焦点 */
void lv_port_focus_init(lv_obj_t *screen);

/* 文件系统初始化 */
void lv_port_fs_init(void);

#ifdef __cplusplus
}
#endif