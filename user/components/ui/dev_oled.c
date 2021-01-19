/*
 * @Copyright: Copyright (C) 2021-2021 Kevin group. All rights reserved.
 * @Description: oled driver
 * @Author: Kevin
 * @Email: weikaiup@163.com
 * @Date: 2021-01-07
 */

// 说明: 0.69寸OLED
// ----------------------------------------------------------------
// GND   电源地
// VCC   接5V或3.3v电源
// SCL   接PB8（SCL）
// SDA   接PB9（SDA）            
// ----------------------------------------------------------------
// 0------> X
//  |
//  V Y
// OLED的显存 存放格式如下.
// [0]0 1 2 3 ... 127
// [1]0 1 2 3 ... 127
// [2]0 1 2 3 ... 127
// [3]0 1 2 3 ... 127
// [4]0 1 2 3 ... 127
// [5]0 1 2 3 ... 127
// [6]0 1 2 3 ... 127
// [7]0 1 2 3 ... 127
#include "string.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

#include "dev_oled.h"
#include "font_dot.h"

// OLED IIC端口定义				   
#define I2C_SCL_CLR() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET)
#define I2C_SCL_SET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET)

#define I2C_SDA_CLR() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET)
#define I2C_SDA_SET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET)

void I2cStart()
{
    I2C_SCL_SET() ;
    I2C_SDA_SET();
    I2C_SDA_CLR();
    I2C_SCL_CLR();
}

void I2cStop()
{
    I2C_SCL_SET() ;
    I2C_SDA_CLR();
    I2C_SDA_SET();
    
}

void I2cWaitAck()
{
    I2C_SCL_SET() ;
    I2C_SCL_CLR();
}

void I2cSendByte(uint8_t data)
{
    I2C_SCL_CLR();
    for (uint8_t i = 0; i < 8; i++) {
        if (data & 0x80) {
            I2C_SDA_SET();
        } else {
            I2C_SDA_CLR();
        }
        data <<= 1;
        I2C_SCL_SET();
        I2C_SCL_CLR();
    }
}

void I2cSendCmd(uint8_t cmd)
{
    I2cStart();
    I2cSendByte(0x78);  //Slave address,SA0=0
    I2cWaitAck();	
    I2cSendByte(0x00);  //write command
    I2cWaitAck();	
    I2cSendByte(cmd); 
    I2cWaitAck();	
    I2cStop();
}

void I2cSendData(uint8_t data)
{
    I2cStart();
    I2cSendByte(0x78);  //D/C#=0; R/W#=0
    I2cWaitAck();	
    I2cSendByte(0x40);  //write data
    I2cWaitAck();	
    I2cSendByte(data);
    I2cWaitAck();	
    I2cStop();
}

// 以下oled的操作

//OLED缓存128*64bit
static uint8_t g_frameBuf[OLED_WIDTH * OLED_HEIGHT / 8];
//OLED实时信息
static OledOpt oled;
//OLED flush after operation，1=yes，0=no
static uint8_t do_flush = true;

int oled_write_cmd(uint8_t cmd)
{
    I2cSendCmd(cmd);
    return 0;
}

int oled_write_data(uint8_t data)
{
    I2cSendData(data);
    return 0;
}
/**
 * @brief: 向oled写长数据
 * @param[in]   data   要写入的数据
 * @param[in]   len     数据长度
 * @return: 
 *      - ESP_OK 
 */
int oled_write_long_data(uint8_t *data, uint16_t len)
{
    for (int i = 0; i < len; i++) {
        oled_write_data(data[i]);
    }
    return 0;
}

/**
 * @brief: init oled
 * @param {in}  NULL
 * @return: 
 */
void oled_init(void)
{
    //oled配置
    oled_write_cmd(0xAE);  //display off
    oled_write_cmd(0X20);  //Set Memory Addressing Mode	
    oled_write_cmd(0X10);  //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    oled_write_cmd(0XB0);  //Set Page Start Address for Page Addressing Mode,0-7
    oled_write_cmd(0xC0);//oled_write_cmd(0XC8);  //Set COM Output Scan Direction
    oled_write_cmd(0X00);  //---set low column address
    oled_write_cmd(0X10);  //---set high column address
    oled_write_cmd(0X40);  //--set start line address
    oled_write_cmd(0X81);  //--set contrast control register
    oled_write_cmd(0XFF);  //亮度调节 0x00~0xff
    oled_write_cmd(0x01);//oled_write_cmd(0XA1);  //--set segment re-map 0 to 127
    oled_write_cmd(0XA6);  //--set normal display
    oled_write_cmd(0XA8);  //--set multiplex ratio(1 to 64)
    oled_write_cmd(0X3F);  //
    oled_write_cmd(0XA4);  //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    oled_write_cmd(0XD3);  //-set display offset
    oled_write_cmd(0X00);  //-not offset
    oled_write_cmd(0XD5);  //--set display clock divide ratio/oscillator frequency
    oled_write_cmd(0XF0);  //--set divide ratio
    oled_write_cmd(0XD9);  //--set pre-charge period
    oled_write_cmd(0X22);  //
    oled_write_cmd(0XDA);  //--set com pins hardware configuration
    oled_write_cmd(0X12);  
    oled_write_cmd(0XDB);  //--set vcomh
    oled_write_cmd(0X20);  //0x20,0.77xVcc
    oled_write_cmd(0X8D);  //--set DC-DC enable
    oled_write_cmd(0X14);  //
    oled_write_cmd(0XAF);  //--turn on oled panel
    //清屏
    oled_clear();
}

/**
 * @brief: 唤醒屏幕
 * @param {type} 
 * @return: 
 */
void oled_on(void)
{
    oled_write_cmd(0x8D);   //设置电源
    oled_write_cmd(0x14);   //开启电源
    oled_write_cmd(0xAF);   //OLED唤醒
}

/**
 * @brief: 休眠屏幕，功耗<10uA
 * @param {type} 
 * @return: 
 */
void oled_off(void)
{
    oled_write_cmd(0x8D);   //设置电源
    oled_write_cmd(0x10);   //关闭电源
    oled_write_cmd(0xAE);   //OLED休眠
}

/**
 * @brief: 将显存内容刷新到oled显示区
 * @param {in}  NULL
 * @return: 
 */
void oled_update_screen(void)
{
    uint8_t line_index;
    for(line_index=0 ; line_index<8; line_index++)
    {
        oled_write_cmd(0xb0+line_index);
        oled_write_cmd(0x00);
        oled_write_cmd(0x10);
        
        oled_write_long_data(&g_frameBuf[OLED_WIDTH * line_index], OLED_WIDTH);
    }
}

/**
 * @brief: fill all the screen 
 * @param {in} data: only choose '0x00' or '0xff'
 * @return: 
 */
void oled_fill(uint8_t data)
{
    //置ff缓存
    memset(g_frameBuf, data, sizeof(g_frameBuf));
    oled_update_screen();
}

/**
 * @brief: clear all screen with black
 * @param {in} NULL
 * @return: 
 */
void oled_clear(void)
{
    //清0缓存
    oled_fill(COLOR_BLACK);
}

/**
 * @brief: 移动坐标
 * @param[in]   x   显示区坐标 x
 * @param[in]   y   显示去坐标 y
 * @return: 
 */
void oled_goto_cursor(uint16_t x, uint16_t y) 
{
	oled.cursor_x = x;
	oled.cursor_y = y;
}

/**
 * @brief: 向显存写入
 * @param[in]   x   坐标
 * @param[in]   y   坐标
 * @param[in]   color   色值0/1
 * @return: 
 */
void oled_draw_pixel(uint16_t x, uint16_t y, uint8_t color) 
{
	if (x >= OLED_WIDTH || y >= OLED_HEIGHT) {
		return;
	}
	if (color == COLOR_WHITE) {
		g_frameBuf[x + (y / 8) * OLED_WIDTH] |= 1 << (y % 8);
	} else {
		g_frameBuf[x + (y / 8) * OLED_WIDTH] &= ~(1 << (y % 8));
	}
}

/**
 * @brief: 向显存写入byte
 * @param[in]   x   坐标
 * @param[in]   y   坐标
 * @param[in]   color   色值0/1
 * @return: 
 */
void oled_draw_byte(uint16_t x, uint16_t y, uint8_t data, uint8_t color) 
{
	if (x >= OLED_WIDTH || y+8 > OLED_HEIGHT) {
		return;
	}
	if (color == COLOR_WHITE) {
		g_frameBuf[x + (y / 8) * OLED_WIDTH] = data;
	} else {
		g_frameBuf[x + (y / 8) * OLED_WIDTH] = ~data;
	}
}

/**
 * @brief: draw a line by Bresenham algorithm
 * @param {type} 
 * @return: 
 */
void oled_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t color)
{
    int dx,dy,e;

    dx = x1-x0; 
    dy = y1-y0;
    
    if (dx >= 0) {
        if (dy >= 0) {// dy>=0
            if (dx >= dy) {// 1/8 octant
                e = dy-dx/2;
                while (x0 <= x1) {
                    oled_draw_pixel(x0, y0, color);
                    if (e > 0) {
                        y0 += 1;
                        e -= dx;
                    }   
                    x0 += 1;
                    e += dy;
                }
            } else {// 2/8 octant
                e = dx - dy/2;
                while (y0 <= y1) {
                    oled_draw_pixel(x0, y0, color);
                    if (e > 0) {
                        x0 += 1;
                        e -= dy;
                    }   
                    y0 += 1;
                    e += dx;
                }
            }
        } else {// dy<0
            dy = -dy;   // dy=abs(dy)
            if (dx >= dy) {// 8/8 octant
                e = dy - dx/2;
                while (x0 <= x1) {
                    oled_draw_pixel(x0, y0, color);
                    if (e > 0) {
                        y0 -= 1;
                        e -= dx;
                    }   
                    x0 += 1;
                    e += dy;
                }
            } else {// 7/8 octant
                e = dx - dy/2;
                while (y0 >= y1) {
                    oled_draw_pixel(x0, y0, color);
                    if (e > 0) {
                        x0 += 1;
                        e -= dy;
                    }   
                    y0 -= 1;
                    e += dx;
                }
            }
        }
    } else {//dx<0
        dx = -dx;     //dx=abs(dx)
        if (dy >= 0) {// dy>=0
            if (dx >= dy) {// 4/8 octant
                e = dy - dx/2;
                while (x0 >= x1) {
                    oled_draw_pixel(x0, y0, color);
                    if (e > 0) {
                        y0 += 1;
                        e -= dx;
                    }   
                    x0 -= 1;
                    e += dy;
                }
            } else {// 3/8 octant
                e = dx - dy/2;
                while (y0 <= y1) {
                    oled_draw_pixel(x0, y0, color);
                    if (e > 0) {
                        x0 -= 1;
                        e -= dy;
                    }   
                    y0 += 1;
                    e += dx;
                }
            }
        } else {// dy<0
            dy = -dy;   // dy=abs(dy)
            if (dx >= dy) {// 5/8 octant
                e = dy - dx/2;
                while (x0 >= x1) {
                    oled_draw_pixel(x0, y0, color);
                    if (e > 0) {
                        y0 -= 1;
                        e -= dx;
                    }   
                    x0 -= 1;
                    e += dy;
                }
            } else {// 6/8 octant
                e = dx - dy/2;
                while (y0 >= y1) {
                    oled_draw_pixel(x0, y0, color);
                    if (e > 0) {
                        x0 -= 1;
                        e -= dy;
                    }   
                    y0 -= 1;
                    e += dx;
                }
            }
        }   
    }
}

/**
 * @brief: 在x，y位置显示字符
 * @param[in]   x    显示坐标x 
 * @param[in]   y    显示坐标y 
 * @param[in]   ch   要显示的字符
 * @param[in]   font 显示的字形
 * @param[in]   color 颜色  1显示 0不显示
 * @return: 
 */
char oled_show_char(uint16_t x, uint16_t y, char ch, FontInfo font, uint8_t color) 
{
	uint32_t i, b, j;
	if (OLED_WIDTH < (x + font.width)
        || OLED_HEIGHT < (y + font.height)) {
		return 0;
	}
    oled_goto_cursor(x,y);

    if (font.height <= 8) {
        for (i = 0; i < font.width; i++) {
            b = font.data[(ch - ' ') * font.width + i];
            for (j = 0; j < font.height; j++) {
                if ((b >> j) & 0x1) {
                    oled_draw_pixel(oled.cursor_x + i, (oled.cursor_y + j), color);
                } else {
                    oled_draw_pixel(oled.cursor_x + i, (oled.cursor_y + j), !color);
                }
            }
        }
    } else if (font.height <= 16) {
        for (i = 0; i < font.width; i++) {
            b = font.data[(ch - ' ') * font.width * 2 + i];
            b += font.data[(ch - ' ') * font.width * 2 + font.width + i] << 8;
            for (j = 0; j < font.height; j++) {
                if ((b >> j) & 0x1) {
                    oled_draw_pixel(oled.cursor_x + i, (oled.cursor_y + j), color);
                } else {
                    oled_draw_pixel(oled.cursor_x + i, (oled.cursor_y + j), !color);
                }
            }
        }
    }

	if(true == do_flush) {
       oled_update_screen(); 
    }
	return ch;
}

/**
 * @brief: 在x，y位置显示字符串 
 * @param[in]   x    显示坐标x 
 * @param[in]   y    显示坐标y，8的倍数
 * @param[in]   str   要显示的字符串
 * @param[in]   font 显示的字形
 * @param[in]   color 颜色  1显示 0不显示
 * @return: 
 */
char oled_show_str(uint16_t x, uint16_t y, char* str, FontInfo font, uint8_t color) 
{
    do_flush = false;
	while (*str) {
		if (oled_show_char(x, y, *str, font, color) != *str) {
            do_flush = 0;
			return *str;
		}
		str++;
        x += font.width;
	}
    do_flush = true;
    oled_update_screen();
	return *str;
}

/**
 * @brief: 在x，y位置显示字符，且是8位对齐方式，即y必须是8的倍数
 * @param[in]   x    显示坐标x 
 * @param[in]   y    显示坐标y，8的倍数
 * @param[in]   str   要显示的字符串
 * @param[in]   font 显示的字形
 * @param[in]   color 颜色  1显示 0不显示
 * @return: 
 */
char oled_show_char_align8(uint16_t x, uint16_t y, char ch, FontInfo font, uint8_t color) 
{
    uint32_t i, j;
    uint8_t tmp;
    uint8_t line_total = font.height/8 + ((font.height%8)?1:0);
	if (OLED_WIDTH < (x + font.width)
        || OLED_HEIGHT < (y + font.height)) {
		return 0;
	}
    oled_goto_cursor(x,y);

    //ESP_LOGI("OLED", "LINE=%d", line_total);
    for (i = 0; i < line_total; i++) {
        for (j = 0; j < font.width; j++) {
            tmp = font.data[(ch - ' ') * (font.width *line_total) + i*font.width + j];
            oled_draw_byte(oled.cursor_x + j, oled.cursor_y + i*8, tmp, color);
        }
    }

	if(true == do_flush) {
       oled_update_screen(); 
    }
	return ch;
}

/**
 * @brief: 在第line行，偏移offset个字符的位置显示字符串 
 * @param[in]   line    第几行，8像素为1行,0 ~ 7
 * @param[in]   offset  偏移多少个字符，按字体大小
 * @param[in]   str   要显示的字符串
 * @param[in]   font 显示的字形
 * @param[in]   color 颜色  1显示 0不显示
 * @return: 
 */
char oled_show_str_line(uint8_t line, uint8_t offset, char* str, FontInfo font, uint8_t color) 
{
    uint16_t x, y;

    if ((8*line + font.height) > OLED_HEIGHT
    || font.width * (offset+1) > OLED_WIDTH) {
        return 0;
    }
    do_flush = false;
    y = line * 8;
    x = offset * font.width;
	while (*str) {
		if (oled_show_char_align8(x, y, *str, font, color) != *str) {
			break;
		}
		str++;
        x += font.width;
	}
    do_flush = true;
    oled_update_screen();
	return *str;
}

/**
 * @brief: 
 * @param {type} 
 * @return: 
 */
void oled_fill_chunk(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t color)
{ 	
    //TODO
    uint32_t i, j;
    uint16_t width, height;

	if (x0 >= OLED_WIDTH
        || y0 >= OLED_HEIGHT
        || x1 >= OLED_WIDTH
        || y1 >= OLED_HEIGHT) {
		return;
	}
    width = x1 -x0;
    height = y1 - y0;
    for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++) {
            oled_draw_pixel(x0 + i, y0 + j, color);
        }
    }
}

void oled_fill_chunk_update(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t color)
{
    oled_fill_chunk(x0, y0, x1, y1, color);
    oled_update_screen();
}

/**
 * @brief: 
 * @param {type} 
 * @return: 
 */
void oled_draw_bmp(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint8_t bmp[])
{ 	
    //TODO
    uint32_t i, b, j, line = 0, bits = 0;
	if (OLED_WIDTH <= (x0 + width)
        || OLED_HEIGHT <= (y0 + height)) {
		return;
	}
    oled_goto_cursor(x0,y0);
    
    while(height) {
        if (height < 8) {
            bits = height;
            height = 0;
        } else {
            bits = 8;
            height -= 8;
        }
        for (i = 0; i < width; i++) {
            b = bmp[width * line + i];
            for (j = 0; j < bits; j++) {
                if ((b >> j) & 0x1) {
                    oled_draw_pixel(x0 + i, y0 + j, COLOR_WHITE);
                } else {
                    oled_draw_pixel(x0 + i, y0 + j, COLOR_BLACK);
                }
            }
        }
        line++;
        y0 += 8;
    }
    oled_update_screen();
}
