/*******************************************************************************
 ******************************************************************************/
#include "HAL/inc/HAL.h"
#include "lv_port.h"
#include "App.h"
#include "ResourcePool.h"

void lv_example_img_1(void)
{
    lv_obj_t *cont = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_HOR_RES, 16);
    lv_obj_set_y(cont, 16);


    LV_IMG_DECLARE(img_src_sd_card);
    lv_obj_t *img1 = lv_img_create(cont);
    lv_img_set_src(img1, &img_src_sd_card);

    lv_obj_center(img1);
}

void setup(void)
{
    HAL::HAL_Init();

    lv_init();
    lv_port_init();

    HAL::Backlight_SetGradually(2048, 1000);

    App_Init();

    lv_example_img_1();
}

void loop()
{
    delay(1000);
}
