/*******************************************************************************
 ******************************************************************************/
#include "HAL/inc/HAL.h"
#include "lv_port.h"
#include "App.h"

void setup(void)
{
    HAL::HAL_Init();

    lv_init();
    lv_port_init();

    HAL::Backlight_SetGradually(2048, 1000);
    App_Init();
}

void loop()
{
    delay(1000);
}
