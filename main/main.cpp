/*******************************************************************************
 ******************************************************************************/
#include <Arduino_GFX_Library.h>

#define TFT_BL 14

/* More data bus class: https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class */
//Arduino_DataBus *bus = create_default_Arduino_DataBus();

/* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
//Arduino_GFX *gfx = new Arduino_ILI9341(bus, DF_GFX_RST, 0 /* rotation */, false /* IPS */);

Arduino_DataBus *bus = new Arduino_ESP32SPIDMA(11 /* DC */, 10 /* CS */, 12 /* SCK */, 13 /* MOSI */, GFX_NOT_DEFINED /* MISO */);
Arduino_GFX *gfx = new Arduino_ST7789(bus, 1 /* RST */, 1 /* rotation */, true /* IPS */, 170 /* width */, 320 /* height */, 35 /* col offset 1 */, 0 /* row offset 1 */, 35 /* col offset 2 */, 0 /* row offset 2 */);

/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

void setup(void)
{
    gfx->begin();
    gfx->fillScreen(BLACK);
#ifdef TFT_BL
    ledcAttach(TFT_BL, 5000, 8); // Attach the backlight pin to PWM channel 0
    ledcWrite(TFT_BL, 25); // Set the backlight to maximum brightness
#endif

    gfx->setTextWrap(true);
    gfx->setTextColor(RED);
    gfx->setTextSize(1); // x scale, y scale, pixel_margin
    gfx->setCursor(0, 0);
    gfx->printf("PSRAM: %ld kb\r\n", ESP.getFreePsram()/ 1024);
    gfx->println("This is Arduino GFX Library");
    gfx->println("Press any key to continue...");
}

void loop()
{
    delay(1000);
}
