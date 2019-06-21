/**
 * the handler of the events.
 */
#ifndef __EVENT_MANAGER_H__
#define __EVENT_MANAGER_H__

#include "sys.h"


extern u8 key;								 //按键的键值
extern u8 KEY_MODE;					 //模式0,HELLO BLIZ
                                                    //模式1，电量
                                                 //模式2，表情模式
                                                 //模式3，参数模式
extern u8 DIS_STATE;		 //用来实现只刷一次屏幕的变量			

void KEY_mode_Select(void);
void EventLoop(void);

#endif
