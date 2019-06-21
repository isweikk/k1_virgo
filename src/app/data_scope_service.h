/**
 * the API is using for uploading the data to the data monitor of PC
 */
#ifndef __DATA_SCOPE_SERVICE_H__
#define __DATA_SCOPE_SERVICE_H__
 
/**************************************************************************
上位机使用的是ACE-TECH提供的开源上位机，十分感谢！所以在此注明出处。
本店铺的原创上位机正在开发中，敬请期待！ 
**************************************************************************/

extern unsigned char DataScope_OutPut_Buffer[42];	   //待发送帧数据缓存区


void DataScope_Get_Channel_Data(float Data,unsigned char Channel);    // 写通道数据至 待发送帧数据缓存区

unsigned char DataScope_Data_Generate(unsigned char Channel_Number);  // 发送帧数据生成函数 
 
 
#endif 



