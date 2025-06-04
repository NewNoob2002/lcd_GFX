#include "../HAL/inc/HAL.h"
#include "lv_port.h"
#include "lvgl.h"

// LVGL初始化函数
void lv_port_init(void)
{
    lv_port_disp_init();
    lv_port_indev_init();
}