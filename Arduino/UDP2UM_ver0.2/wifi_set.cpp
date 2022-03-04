/*-------------------------------------*/
/*              头文件加载             */
/*====================================*/
#include "wifi_set.h"
#include "network.h"
/*------------------------------------*/
/*              变量定义              */
/*===================================*/
String comdata;
/*------------------------------------*/
/*              函数定义              */
/*===================================*/
void serialEvent() // 伪串口接收中断函数
{
    while (Serial.available())
    {
        comdata += char(Serial.read());
    }
    Serial.println(comdata);
    udpSend(comdata.c_str());
    comdata = "";
}