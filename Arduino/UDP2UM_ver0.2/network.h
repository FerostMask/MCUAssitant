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
    bool bind;
    unsigned short send_port;
    unsigned short receive_port;
    char *staname;
    char *stapassword;
    char *ip;
    char *defaultIp;
    char *localname;
} wifi_info;

/*-------------------------------------*/
/*              枚举类型               */
/*====================================*/
typedef enum RESPONSE_EVENT
{
    BINDING = 0,
    UNBIND = 1,
} RESPONSE_EVENT;
/*------------------------------------*/
/*           函数、变量声明           */
/*==================================*/
void WiFi_init(const wifi_info *wifi);
void infoUpdate(wifi_info *wifi);
void broadcastMyIP(const wifi_info *wifi);
void udpSend(const char *adress);
void udpReceive();
#endif