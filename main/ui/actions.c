#include "actions.h"
#include "ui.h"
#include "screens.h"
#include <stdio.h>

// void action_next_page(lv_event_t* e) {
//     // 从主页面切换到page_b页面
//     printf("nextPage cb\r\n");
//     loadScreen(SCREEN_ID_PAGE_B);
// }

// void action_main_page(lv_event_t* e) {
//     // 从page_b页面切换回主页面
//     printf("return MainPage cb event\r\n");
//     loadScreen(SCREEN_ID_MAIN);
// }

// 标签动画完成后的回调函数
static void anim_x_cb(void *var, int32_t v)
{
    lv_obj_set_x(var, v);
}

void action_switch_checked()
{
    printf("Switch checked cb\r\n");

    // 获取HelloWorld标签对象
    lv_obj_t *label = objects.main_label1;

    // 创建动画
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, label);
    lv_anim_set_values(&a, lv_obj_get_x(label), -lv_obj_get_width(label)); // 从当前位置移动到屏幕外
    lv_anim_set_time(&a, 500);                                             // 动画时长500ms
    lv_anim_set_exec_cb(&a, anim_x_cb);                                    // 设置X坐标
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);                        // 使用缓出效果，更加丝滑
    lv_anim_start(&a);
}

void action_switch_unchecked()
{
    printf("Switch Unchecked cb\r\n");

    // 获取HelloWorld标签对象
    lv_obj_t *label = objects.main_label1;

    // 创建动画
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, label);
    lv_anim_set_values(&a, lv_obj_get_x(label), 120); // 从当前位置移回原位
    lv_anim_set_time(&a, 500);                        // 动画时长500ms
    lv_anim_set_exec_cb(&a, anim_x_cb);               // 设置X坐标
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);   // 使用缓出效果，更加丝滑
    lv_anim_start(&a);
}

void action_switch1_action(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    // lv_obj_t *menu = lv_event_get_user_data(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        if (lv_obj_has_state(obj, LV_STATE_CHECKED))
        {
            action_switch_checked();
        }
        else
        {
            action_switch_unchecked();
        }
    }
}

void action_button1_action(lv_event_t * e)
{
    printf("button1 action\r\n");
}