#include "lv_port.h"
#include "../HAL/inc/HAL.h"

// 这里简单实现，暂时不添加文件系统支持
void lv_port_fs_init(void)
{
    DISPLAY_PRINTF("LVGL文件系统接口初始化\n");
    // 未来可以在这里添加SD卡等文件系统支持
}