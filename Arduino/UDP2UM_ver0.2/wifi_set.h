#ifndef _WIFI_SET_H
#define _WIFI_SET_H
/*-------------------------------------*/
/*              头文件加载             */
/*====================================*/
#include <Arduino.h>
#include "network.h"
/*-------------------------------------*/
/*              枚举类型               */
/*====================================*/
typedef enum
{
    INSTRUCTION = 0, //通信指示
    NUMBER = 1,      //编号
    DATA_HEAD = 2,   //数据起始位

    MAX_DATA_LEN = 64, // UART最大数据长度

    SPI_COMN = 0xA0,
    SPI_COMN_START = 0xA0,
    SPI_COMN_UNDERWAY = 0xA1,
    SPI_COMN_END = 0xA3,

    WIFI_INFO_SET = 0xC0,
    WIFI_SET_NAME = 0xC0,
    WIFI_SET_PASSWORD = 0xC1,
    WIFI_SET_IP = 0xC2,
    WIFI_SET_SENDPORT = 0xC3,
    WIFI_SET_RECEIVEPORT = 0xC4,

    WIFI_INITIALIZATION = 0xD0,
    WIFI_INIT_FAIL = 0x00,
    WIFI_INIT_SUCCESS = 0x01,

    UDP_ONECUT_LEN = 1120, // UDP发送数据长度
    UDP_MAX_TIMES = 4,     // UDP最大通信次数
} COM_enum;
/*------------------------------------*/
/*              结构体定义            */
/*===================================*/
typedef struct comStruct
{
    char (*buff)[UDP_ONECUT_LEN];
    char *data;
    int spi_count;
} comStruct;
/*------------------------------------*/
/*           函数、变量声明           */
/*==================================*/
void wifiSetInit();
#endif