#include "../inc/HAL.h"
#include "button_gpio.h"
#include "iot_button.h"

// 全局变量用于存储按键状态，供 LVGL 输入设备读取
volatile key_action_t g_key_action = KEY_ACTION_NONE;

// 三击回调函数
static void functionKey_triple_click_event_cb(void *arg, void *data) {
  // 只设置按键动作，不执行具体操作
  g_key_action = KEY_ACTION_TRIPLE_CLICK;
}

// 双击回调函数
static void functionKey_double_click_event_cb(void *arg, void *data) {
  // 只设置按键动作，不执行具体操作
  g_key_action = KEY_ACTION_DOUBLE_CLICK;
}

// 单击回调函数
static void functionKey_click_event_cb(void *arg, void *data) {
  // 只设置按键动作，不执行具体操作
  g_key_action = KEY_ACTION_SINGLE_CLICK;
}

/*FUNCTION*/
void HAL::Button_Init(void) {
  BUTTON_PRINTF("Button_Init\n");
  // 按钮配置
  button_config_t btn_cfg = {
      .long_press_time = 1500,
      .short_press_time = 200,
  };

  button_gpio_config_t functionKey_gpio_cfg = {
      .gpio_num = 0,
      .active_level = 0,
      .enable_power_save = true,
      .disable_pull = false,
  };

  // 创建按钮设备
  button_handle_t functionKey;
  esp_err_t ret =
      iot_button_new_gpio_device(&btn_cfg, &functionKey_gpio_cfg, &functionKey);
  assert(ret == ESP_OK);

  // 注册点击回调
  button_event_args_t triple_click_args = {
      .multiple_clicks = {
          .clicks = 3,
      }
  };
  
  iot_button_register_cb(functionKey, BUTTON_MULTIPLE_CLICK, &triple_click_args,
                         functionKey_triple_click_event_cb, nullptr);
  iot_button_register_cb(functionKey, BUTTON_DOUBLE_CLICK, NULL,
                         functionKey_double_click_event_cb, nullptr);
  iot_button_register_cb(functionKey, BUTTON_SINGLE_CLICK, NULL,
                         functionKey_click_event_cb, nullptr);
}

// 获取当前按键动作，供LVGL输入设备读取
key_action_t HAL::Button_GetAction(void) {
  key_action_t action = g_key_action;
  g_key_action = KEY_ACTION_NONE; // 读取后清除动作状态
  return action;
}