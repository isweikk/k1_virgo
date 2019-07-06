/**
 * the handler of the events.
 */
#include "event_manager.h"

#include "lcd_img.h"
#include "bsp.h"
#include "general_api.h"
#include "motion_control.h"

void EventLoop(void)
{
    if (UltrasonicWaveWaitTime > 100) {				 //===100ms读取一次超声波的数据
        UltrasonicWaveWaitTime = 0;
        UltrasonicWaveDetect();
    }

    if (VoltageWaitTime > 1000) { //200ms 采集一次 过多采集没有意义
        VoltageWaitTime = 0;
        GetBatteryVolt();
        // float pitch, roll, yaw;
        // MotorGetConstEuler(&pitch, &roll, &yaw);
        // ulog("test0: %f, %f, %f", pitch, roll, yaw);
    //    short gyrox, gyroy, gyroz;
    //    MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);
    //    ulog("test1: %d, %d, %d", gyrox, gyroy, gyroz);
        //ulog ("encoder: %d, %d", ReadEncoder(2), ReadEncoder(4));
        // OLED_Float(2, 2, pitch, 3);
        // OLED_Float(3, 2, roll, 3);
        // OLED_Float(4, 2, yaw, 3);
		// OLED_Num5(2, 5, -ReadEncoder(EmEncoderLeft));
		// OLED_Num5(2, 6, ReadEncoder(EmEncoderRight));
    }
}

void KEY_mode_Select(void)
{
    float voltage = GetBatteryVolt();
    
        key=KeyScan();
        if(key==1)
        {
            ulogd("test key");
            ulogd("bat:v=%f, uw=%f", voltage, UltrasonicWaveDistance);
            KEY_MODE++;
            if(KEY_MODE>3) KEY_MODE=0;
        }
        if(KEY_MODE==0)
        {
            if(DIS_STATE!=0)
            {
                //OLED_DrawBMP(0,0,128,8,BMP_hello);
                DIS_STATE=0;
            }
        }
        else if(KEY_MODE==1)
        {
            if(GetChargingStatus()==1) //USB插入	
            {
                if(DIS_STATE!=1)
                {
                    //OLED_DrawBMP(0,0,128,8,BMP_bm);
                    DIS_STATE=1;
                }
            }
            else
            {
                if(voltage>=4.2)
                {
                    if(DIS_STATE!=100)
                    {
                        //OLED_DrawBMP(0,0,128,8,BMP_100);
                        DIS_STATE=100;
                    }
                }
                else if(voltage>=4.1)
                {
                    if(DIS_STATE!=80)
                    {
                        //OLED_DrawBMP(0,0,128,8,BMP_80);
                        DIS_STATE=80;
                    }
                }
                else if(voltage>=4.0)
                {
                    if(DIS_STATE!=60)
                    {
                        //OLED_DrawBMP(0,0,128,8,BMP_60);
                        DIS_STATE=60;
                    }
                }
                else if(voltage>=3.9)
                {
                    if(DIS_STATE!=40)
                    {
                        //OLED_DrawBMP(0,0,128,8,BMP_40);
                        DIS_STATE=40;
                    }
                }
                else if(voltage>=3.8)
                {
                    if(DIS_STATE!=20)
                    {
                        //OLED_DrawBMP(0,0,128,8,BMP_20);
                        DIS_STATE=20;
                    }
                }
                else
                {
                    if(DIS_STATE!=200)
                    {
                        //OLED_DrawBMP(0,0,128,8,BMP_0);
                        DIS_STATE=200;
                    }
                }
            }
        }
        else if(KEY_MODE==2)
        {
            if(UltrasonicWaveWarning())
            {
                if(DIS_STATE!=21)
                {
                    //OLED_DrawBMP(0,0,128,8,BMP_cry_Ultrasonic);
                    DIS_STATE=21;
                }
            }
            else if(BodyPosture == EmPostureFall)
            {
                if(DIS_STATE!=21)
                {
                    //OLED_DrawBMP(0,0,128,8,BMP_cry_fall);
                    DIS_STATE=21;
                }
            }
            else 
            {
                if(DIS_STATE!=22)
                {
                    //OLED_DrawBMP(0,0,128,8,BMP_smile);
                    DIS_STATE=22;
                }
            }
        }
        else if(KEY_MODE==3)
        {
            if(DIS_STATE!=3)
            {
                //OLED_Clear();
                //oled_first_show();
                DIS_STATE=3;
            }
            //oled_show();
        }
}		
