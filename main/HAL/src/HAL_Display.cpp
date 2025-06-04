#include "../inc/HAL.h"
#include "lvgl.h"
#include <Arduino_GFX_Library.h>

Arduino_DataBus *bus = new Arduino_ESP32SPIDMA(
    CONFIG_SCREEN_DC_PIN /* DC */, CONFIG_SCREEN_CS_PIN /* CS */,
    CONFIG_SCREEN_SCK_PIN /* SCK */, CONFIG_SCREEN_MOSI_PIN /* MOSI */,
    GFX_NOT_DEFINED /* MISO */);
Arduino_GFX *gfx = new Arduino_ST7789(
    bus, CONFIG_SCREEN_RST_PIN /* RST */, 0 /* rotation */, true /* IPS */,
    170 /* width */, 320 /* height */, 35 /* col offset 1 */,
    0 /* row offset 1 */, 35 /* col offset 2 */, 0 /* row offset 2 */);




void HAL::Display_Init(void) {
  // 初始化SPI和显示屏 - 使用定义的配置
  DISPLAY_PRINTF("Init Display...\n");
  gfx->begin(SPI_DEFAULT_FREQUENCY);
  gfx->setRotation(CONFIG_SCREEN_ROTATION);
  gfx->fillScreen(BLACK);
}

void HAL::Display_SetSendFinishCallback(Display_CallbackFunc_t func) {
  DISPLAY_PRINTF("Set Send Finish Callback\n");
  // 暂未实现回调机制
}

void HAL::Display_SendPixels(uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                        const uint16_t *pixels) {
  gfx->draw16bitBeRGBBitmap(x, y, (uint16_t *)pixels, w, h);
  // #else
  // gfx->draw16bitRGBBitmap(x, y, (uint16_t *)pixels, w, h);
  // #endif
}

void HAL::Backlight_Init(void) {
#ifdef CONFIG_SCREEN_BLK_PIN
  ledcAttach(CONFIG_SCREEN_BLK_PIN, 5000, 12);
  // Attach the backlight pin to PWM channel 0
  ledcWrite(CONFIG_SCREEN_BLK_PIN, 0);
  // Set the backlight to maximum brightness
#endif
}

void HAL::Backlight_SetVal(uint16_t brightness) {
#ifdef CONFIG_SCREEN_BLK_PIN
  ledcWrite(CONFIG_SCREEN_BLK_PIN, brightness);
#endif
}

uint16_t HAL::Backlight_GetVal(void) {
#ifdef CONFIG_SCREEN_BLK_PIN
  return ledcRead(CONFIG_SCREEN_BLK_PIN);
#endif
}

void Backlight_AnimCallback(void *var, int32_t val) {
  HAL::Backlight_SetVal(val);
}

void HAL::Backlight_SetGradually(uint16_t target, uint16_t time_ms) {
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)Backlight_AnimCallback);
  lv_anim_set_values(&a, Backlight_GetVal(), target);
  lv_anim_set_time(&a, time_ms);
  lv_anim_set_path_cb(&a, lv_anim_path_ease_out);

  lv_anim_start(&a);
}