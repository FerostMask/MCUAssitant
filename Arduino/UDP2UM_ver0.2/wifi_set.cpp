/*-------------------------------------*/
/*              头文件加载             */
/*====================================*/
#include "wifi_set.h"
#include "network.h"
#include "stdint.h"
#include "stdbool.h"

#define LED_PIN 2
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
    // Serial.println(comdata);
    // udpSend(comdata.c_str());
    udpSend("receive!");
    comdata = "";
}