#pragma once

/*=========================
   Hardware Configuration
 *=========================*/
// /* Screen */
#define CONFIG_SCREEN_CS_PIN        10
#define CONFIG_SCREEN_DC_PIN        11
#define CONFIG_SCREEN_RST_PIN       1
#define CONFIG_SCREEN_SCK_PIN       12
#define CONFIG_SCREEN_MOSI_PIN      13
#define CONFIG_SCREEN_BLK_PIN       14

#define CONFIG_SCREEN_ROTATION      1
#if CONFIG_SCREEN_ROTATION == 0 || CONFIG_SCREEN_ROTATION == 2
#define CONFIG_SCREEN_HOR_RES       170
#define CONFIG_SCREEN_VER_RES       320
#else
#define CONFIG_SCREEN_HOR_RES       320
#define CONFIG_SCREEN_VER_RES       170
#endif

#define SPI_DEFAULT_FREQUENCY SPI_MASTER_FREQ_40M
/* Battery */
#define CONFIG_BAT_DET_PIN          PA1
#define CONFIG_BAT_CHG_DET_PIN      PA11

/* Buzzer */
#define CONFIG_BUZZ_PIN             PA0  // TIM2

/* GPS */
#define CONFIG_GPS_SERIAL           Serial2
#define CONFIG_GPS_USE_TRANSPARENT  0
#define CONFIG_GPS_BUF_OVERLOAD_CHK 0
#define CONFIG_GPS_TX_PIN           PA3
#define CONFIG_GPS_RX_PIN           PA2

/* IMU */
#define CONFIG_IMU_INT1_PIN         PB10
#define CONFIG_IMU_INT2_PIN         PB11

/* I2C */
#define CONFIG_MCU_SDA_PIN          PB7
#define CONFIG_MCU_SDL_PIN          PB6

/* Encoder */
#define CONFIG_ENCODER_B_PIN        PB5
#define CONFIG_ENCODER_A_PIN        PB4
#define CONFIG_ENCODER_PUSH_PIN     PB3

/*Key*/
#define CONFIG_BOOT_KEY_PIN         0

/* Power */
#define CONFIG_POWER_EN_PIN         PA12
#define CONFIG_POWER_WAIT_TIME      1000
#define CONFIG_POWER_SHUTDOWM_DELAY 5000
#define CONFIG_POWER_BATT_CHG_DET_PULLUP    true

/* Debug USART */
#define CONFIG_DEBUG_SERIAL         Serial

/* SD CARD */
#define CONFIG_SD_SPI               SPI_2
#define CONFIG_SD_CD_PIN            PA8
#define CONFIG_SD_MOSI_PIN          PB15
#define CONFIG_SD_MISO_PIN          PB14
#define CONFIG_SD_SCK_PIN           PB13
#define CONFIG_SD_CS_PIN            PB12

/* Show Stack & Heap Info */
#define CONFIG_SHOW_STACK_INFO      0
#define CONFIG_SHOW_HEAP_INFO       0