#include "../inc/HAL.h"
// #include "../APP/Version.h"
#include "lv_port.h"

void HAL::HAL_Init(void)
{
    FileSystem_Init();
    Backlight_Init();
    Button_Init();
    Display_Init();
}