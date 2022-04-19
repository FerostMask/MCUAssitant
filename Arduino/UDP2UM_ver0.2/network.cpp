/*-------------------------------------*/
/*              头文件加载             */
/*====================================*/
#include "network.h"
#include "wifi_set.h"
#include "string.h"
#include "stdio.h"
/*------------------------------------*/
/*              函数声明              */
/*===================================*/
static void _startWiFi();
static void deviceBinding(String value);
static void deviceUnbind(String value);
/*------------------------------------*/
/*              变量定义              */
/*===================================*/
#define LED_PIN 2
WiFiUDP udp4Send; // UDP通信结构体
WiFiUDP udp4Receive;
int packetSize;
static wifi_info *_localInfo;
void (*parsingFunctions[])(String) = {deviceBinding, deviceUnbind};
/*------------------------------------*/
/*              函数定义              */
/*===================================*/
/*--------------------*/
/*       UDP模块     */
/*==================*/
void udpSend(const char *adress)
{
    if (_localInfo->bind == true) // 总开关，连接上设备时才能发送数据
    {
        udp4Send.write(adress);
        udp4Send.endPacket();
    }
}

inline static void messageParsing(String value)
{
    String frame;
    RESPONSE_EVENT eventSelect;
    // Serial.println(value);
    for (int i = 0; i < value.length(); i++)
    {
        if (value[i] == ',') // 第一个','前为指令
        {
            if (strcmp(frame.c_str(), "BIND") == 0) // 绑定设备
            {
                eventSelect = BINDING;
            }
            else if (strcmp(frame.c_str(), "UNBIND") == 0) // 解绑设备
            {
                eventSelect = UNBIND;
            }
            parsingFunctions[eventSelect](&value[i + 1]);
            return;
        }
        frame += value[i];
    }
}

void udpReceive()
{
    packetSize = udp4Receive.parsePacket();
    if (packetSize)
    {
        // Serial.printf("IP: %s\n", udp4Receive.remoteIP().toString().c_str());
        String value = udp4Receive.readString();
        messageParsing(value);
        // Serial.println(value);
    }
}
/*--------------------*/
/*     数据解析模块    */
/*==================*/
static void deviceBinding(String value) // 绑定设备
{
    if (_localInfo->bind == true) // 已连接设备，不再连接新的设备
    {
        return;
    }
    char ip[value.length()];
    sprintf(ip, "%s", value.c_str());
    Serial.println(value);
    _localInfo->ip = ip;                                         // 赋值目的地IP
    udp4Send.beginPacket(_localInfo->ip, _localInfo->send_port); // 重新开启UDP发送数据| 以新目的地IP发送数据
    udp4Send.write("BIND");
    udp4Send.endPacket();
    digitalWrite(LED_PIN, 0); // 点亮LED灯
    delay(500);               // 延时，等待BIND发送完毕
    _localInfo->bind = true;  // 标记为已绑定
}

static void deviceUnbind(String value) // 解绑设备
{
    if (_localInfo->bind == false) // 未绑定设备，无法解绑
    {
        return;
    }
    _localInfo->bind = false;                                    // 给设备解绑
    _localInfo->ip = _localInfo->defaultIp;                      // 重设目的地IP
    udp4Send.beginPacket(_localInfo->ip, _localInfo->send_port); // 重新开启UDP发送数据| 以默认IP发送数据
    digitalWrite(LED_PIN, 1);                                    // 熄灭LED灯
    udp4Send.endPacket();                                        // 把剩余信息发送出去
    delay(500);                                                  // 延时，等待之前信息发送完毕
    for (int i = 0; i < 5; i++)                                  // 循环发送设备解绑信息
    {
        udp4Send.write("UNBIND");
        udp4Send.endPacket();
        delay(10);
    }
}
/*--------------------*/
/*       WIFI模块    */
/*==================*/
void broadcastMyIP(const wifi_info *wifi)
{
    udp4Send.write("IP,");
    udp4Send.write(wifi->localname);
    udp4Send.write(",");
    udp4Send.write(WiFi.localIP().toString().c_str());
    udp4Send.endPacket();
}

void WiFi_init(const wifi_info *wifi)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi->staname, wifi->stapassword);
    _startWiFi();
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.disconnect();
        WiFi.begin(wifi->staname, wifi->stapassword);
        _startWiFi();
    }
    digitalWrite(LED_PIN, 1); //熄灭LED灯
    Serial.println("WiFi_init success!");
    udp4Receive.begin(_localInfo->receive_port);            // 开启UDP接收数据
    udp4Send.beginPacket(wifi->defaultIp, wifi->send_port); // 开启UDP发送数据 | 以默认IP发送数据
}

void infoUpdate(wifi_info *wifi)
{
    _localInfo = wifi; // 留存指针
}

static void _startWiFi()
{
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
}
