/*******************************************************************************
 ******************************************************************************/
#include "HAL/inc/HAL.h"
#include "lvgl.h"
#include "lv_port.h"
#include "lv_demos.h"
#include "ui.h"

void setup(void)
{
    HAL::HAL_Init();

    lv_init();
    lv_port_init();

    HAL::Backlight_SetGradually(2048, 1500);
    ui_init();
}

void loop()
{
    delay(1000);
}
