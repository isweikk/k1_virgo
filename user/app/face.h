/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: the eyes image, draw them by algorithm
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-07
 */
#ifndef APP_FACE_H
#define APP_FACE_H

#include <stdio.h>

enum EyeEmotion{
	EmEmotionSleep = 0,
	EmEmotionWakeUp,
	EmEmotionNictation,
};

void eye_reset(void);
int eye_get_emotion(void);
void eye_set_emotion(int emotion);
int eye_show_sleep(void);
int eye_show_wakeup(void);
int eye_show_nictation(void);

void task_screen(void *prm);

#endif  // APP_FACE_H

