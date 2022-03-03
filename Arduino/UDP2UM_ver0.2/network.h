#ifndef _NETWORK_H
#define _NETWORK_H
/*-------------------------------------*/
/*              头文件加载             */
/*====================================*/
#include <Arduino.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
/*------------------------------------*/
/*              结构体定义            */
/*===================================*/
typedef struct wifi_info
{
    unsigned short send_port;
    unsigned short receive_port;
    char *staname;
    char *stapassword;
    char *ip;
    char *localname;
} wifi_info;
/*-------------------------------------*/
/*              枚举类型               */
/*====================================*/

/*------------------------------------*/
/*           函数、变量声明           */
/*==================================*/
void WiFi_init(wifi_info *wifi);
void broadcastMyIP(wifi_info *wifi);
#endif