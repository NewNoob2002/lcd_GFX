#pragma once

#include <stdint.h>
#include <stdio.h>

// 定义通用日志模块宏
#define MODULE_TAG_PRINTF(tag, format, ...) printf("[%s] [%s:%d]: " format, tag, __FILE__, __LINE__, ##__VA_ARGS__)

// 各模块的日志宏
#define FS_PRINTF(format, ...) MODULE_TAG_PRINTF("FS", format, ##__VA_ARGS__)
#define DISPLAY_PRINTF(format, ...) MODULE_TAG_PRINTF("Display", format, ##__VA_ARGS__)
#define LVGL_PRINTF(format, ...) MODULE_TAG_PRINTF("LVGL", format, ##__VA_ARGS__)
#define BUTTON_PRINTF(format, ...) MODULE_TAG_PRINTF("Button", format, ##__VA_ARGS__)
#define BL_PRINTF(format, ...) MODULE_TAG_PRINTF("BackLight", format, ##__VA_ARGS__)
#define SD_PRINTF(format, ...) MODULE_TAG_PRINTF("SD", format, ##__VA_ARGS__)
#define SYSTEM_PRINTF(format, ...) MODULE_TAG_PRINTF("System", format, ##__VA_ARGS__)

// 定义按键操作模式
typedef enum {
    BUTTON_MODE_NAVIGATION = 0,  // 导航模式 - 用于在控件间切换焦点
    BUTTON_MODE_ADJUSTMENT,      // 调整模式 - 用于调整值(如滑块)
    BUTTON_MODE_MAX
} button_mode_t;

// 按键动作类型枚举
typedef enum {
  KEY_ACTION_NONE = 0,
  KEY_ACTION_SINGLE_CLICK, // 单击
  KEY_ACTION_DOUBLE_CLICK, // 双击
  KEY_ACTION_TRIPLE_CLICK, // 三击
} key_action_t;

namespace HAL
{
    /* Clock */
    typedef struct
    {
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t week;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        uint16_t millisecond;
    } Clock_Info_t;
}