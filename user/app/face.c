/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: the eyes image, draw them by algorithm
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-07
 */
#include "face.h"
#include "string.h"
#include "stdlib.h"
#include "cmsis_os.h"

#include "components/oled/dev_oled.h"

#define DELAY_MS(a) osDelay(a)

/* you must konw the the size of the eys is 48*48, 8 fps.
 */
// ---------------------------
// |            |            |
// |  ******    |    ******  |
// |  ******    |    ******  |
// |  ******    |    ******  |
// |            |            |
// ---------------------------
//when you want to stop a eye animation, you must clear eyes befor a new one.
static uint8_t eye_reset_flag = true;
static int eye_emotion = 0;

void eye_reset(void)
{
    eye_reset_flag = true;
}

int eye_get_emotion(void)
{
    return eye_emotion;
}

void eye_set_emotion(int emotion)
{
    eye_reset();
    eye_emotion = emotion;
}

int eye_show_sleep(void)
{ 	
    static uint8_t step, delay = 0;

    if (eye_reset_flag) {
        step = 0;
        oled_clear();
        eye_reset_flag = false;
    }

    if (step == 0) {    //clear
        oled_clear();
        step = 1;
    } else if (step == 1) {
        if (++delay%4 == 0) {
            oled_show_char_align8(48, 48, 'Z', Font_6x8, COLOR_WHITE);
            step = 2;
        }
    } else if (step == 2) {
        if (++delay%4 == 0) {
            oled_show_char_align8(64, 32, 'Z', Font_8x16, COLOR_WHITE);
            step = 3;
        }
    } else if (step == 3) {
        if (++delay%4 == 0) {
            oled_show_char_align8(80, 8, 'Z', Font_12x24, COLOR_WHITE);
            step = 4;
        }
    } else if (step == 4) {
        step = 0;   //a period over
        eye_reset_flag = true;
        delay = 0;
    }
    oled_update_screen();
    if (step == 0) {
        return 0;
    }
    return -1;
}

/**
 * @brief: wake up from sleeping
 * @param {type} 
 * @return: 
 */
int eye_show_wakeup(void)
{ 	
    static uint16_t x0, y0;
    static uint8_t step, delay;
    static char logo[] = "Virgo";
    static uint8_t count = 0;

    if (eye_reset_flag) {
        x0 = 0;
        y0 = 32;
        step = 0;
        delay = 0;
        oled_clear();
        eye_reset_flag = false;
    }

    if (step == 0) {
        x0 += 8;
        oled_fill_chunk(x0, y0 - 8, x0 + 16, y0 + 8, COLOR_WHITE);
        x0 += 24;
        step = 1;
    } else if (step == 1) {
        ++delay;
        if (delay%2 == 0) {
            delay = 0;
            oled_show_char_align8(x0, y0 - Font_16x32.height/2, logo[count], Font_16x32, COLOR_WHITE);
            count++;
            x0 += Font_16x32.width;
            if (count == strlen(logo)) {
                count = 0;
                step = 2;
            }
        }
        //TODO, 方块增加旋转特效
    } else if (step == 2) {
        if (++delay > 4) {
            step = 0;   //a period over
            eye_reset_flag = true;
            delay = 0;
        }
    }
    oled_update_screen();
    if (step == 0) {
        return 0;
    }
    return -1;
}

// blink eyes
int eye_show_nictation(void)
{ 	
    static uint16_t x0, y0, x1,y1;
    static uint8_t step = 0, speed = 8, keep = 0;

    if (eye_reset_flag) {
        x0 = 8;
        y0 = 16;
        x1 = 56;
        y1 = 48;
        step = 0;
        keep = 0;
        oled_clear();
        eye_reset_flag = false;
    }

    if (step == 0) {
        oled_fill_chunk(x0, y0, x1, y1, COLOR_WHITE);
        oled_fill_chunk(x0+64, y0, x1+64, y1, COLOR_WHITE);
        step = 1;
    } else if (step == 1) {    //close eyes
        y0 += speed;
        y1 -= speed;
        oled_fill_chunk(x0, y0-speed, x1, y0, COLOR_BLACK);
        oled_fill_chunk(x0, y1, x1, y1+speed, COLOR_BLACK);
        oled_fill_chunk(x0 + 64, y0-speed, x1 + 64, y0, COLOR_BLACK);
        oled_fill_chunk(x0 + 64, y1, x1 + 64, y1+speed, COLOR_BLACK);
        if (y0 == y1) {
            step = 2;
        }
    } else if (step == 2) {
        if (++keep > 1) {
        y0 -= speed;
        y1 += speed;
        //left eye
        oled_fill_chunk(x0, y0, x1, y0+speed, COLOR_WHITE);
        oled_fill_chunk(x0, y1-speed, x1, y1, COLOR_WHITE);
        //right eye
        oled_fill_chunk(x0 + 64, y0, x1 + 64, y0+speed, COLOR_WHITE);
        oled_fill_chunk(x0 + 64, y1-speed, x1 + 64, y1, COLOR_WHITE);
        }
        if (y0 <= 16) {
            step = 3;
            keep = 0;
        }
    } else if (step == 3) {
        if (++keep > 16) {
            step = 0;   //a period over
            //eye_reset_flag = true;
            keep = 0;
        }
    }
    oled_update_screen();
    if (step == 0) {
        return 0;
    }
    return -1;
}

void task_screen(void *prm)
{
    int current_emotion = 0;

    oled_init();
    eye_set_emotion(EmEmotionWakeUp);
    while(1) {
        DELAY_MS(125);  //8 fps
        current_emotion = eye_get_emotion();
        switch(current_emotion) {
            case EmEmotionSleep:
                eye_show_sleep();
                break;
            case EmEmotionWakeUp:
                if (eye_show_wakeup() == 0) {
                    eye_set_emotion(EmEmotionNictation);
                    //eye_set_emotion(EmEmotionSleep);
                }
                break;
            case EmEmotionNictation:
                eye_show_nictation();
                break;
            default:
                break;
        }
        //ESP_LOGI(TAG, "display continue\n");
    } 
}
