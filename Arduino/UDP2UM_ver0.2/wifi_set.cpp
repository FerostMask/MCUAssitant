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
    static bool blink = false;
    static bool completeFlag = false;
    while (Serial.available())
    {
        blink = !blink;
        digitalWrite(LED_PIN, blink);
        char inChar = (char)Serial.read();
        comdata += inChar;
        if (inChar == '\n')
            completeFlag = true;
    }
    if (completeFlag)
    {
        // Serial.println(comdata);
        udpSend(comdata.c_str());
        comdata = "";
        completeFlag = false;
    }
    udpSend("Receive");
}