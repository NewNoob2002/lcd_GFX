#pragma once

#include "HAL_Config.h"
#include "HAL_Def.h"
#include "CommonMacros.h"
#include <Arduino.h>
// #include "../APP/Version.h"
// #include "ui.h"

namespace HAL {
/*Functions*/
void HAL_Init(void);
/*Fs*/
void FileSystem_Init(void);
/*Display*/
void Display_Init(void);
void Display_SendPixels(uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                        const uint16_t *pixels);
void Backlight_Init(void);
uint16_t Backlight_GetVal(void);
void Backlight_SetVal(uint16_t brightness);
void Backlight_SetGradually(uint16_t target, uint16_t time_ms = 500);

typedef void (*Display_CallbackFunc_t)(void);
void Display_SetSendFinishCallback(Display_CallbackFunc_t func);

/*Button*/
void Button_Init(void);
key_action_t Button_GetAction(void);

/*SD Card*/
bool SD_Init(void);
void SD_Update(void);
float SD_GetCardSizeMB();
const char *SD_GetCardInfo(void);

/*Panic处理*/
void InitPanicHandler(void);
} // namespace HAL