/*-------------------------------------*/
/*              头文件加载             */
/*====================================*/
#include "wifi_set.h"
/*------------------------------------*/
/*              变量定义              */
/*===================================*/
comStruct com;
/*------------------------------------*/
/*              函数定义              */
/*===================================*/
void wifiSetInit()
{
    com.buff = new char[UDP_MAX_TIMES][UDP_ONECUT_LEN];
}

void serialEvent() // 伪串口接收中断函数
{
    while (Serial.available())
    {
        // comdata += char(Serial.read());
    }
    // Serial.println(comdata);
}