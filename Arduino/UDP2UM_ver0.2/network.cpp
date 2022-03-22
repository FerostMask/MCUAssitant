/*-------------------------------------*/
/*              头文件加载             */
/*====================================*/
#include "network.h"
#include "wifi_set.h"
/*------------------------------------*/
/*              变量定义              */
/*===================================*/
#define LED_PIN 2
WiFiUDP udp4Send; // UDP通信结构体
WiFiUDP udp4Receive;
int packetSize;
static wifi_info *_localInfo;
/*------------------------------------*/
/*              函数声明              */
/*===================================*/
static void _startWiFi();
/*------------------------------------*/
/*              函数定义              */
/*===================================*/
/*--------------------*/
/*       UDP模块     */
/*==================*/
void udpSend(const char *adress)
{
    udp4Send.write(adress);
    udp4Send.endPacket();
}

void udpReceive()
{
    packetSize = udp4Receive.parsePacket();
    if (packetSize)
    {
        // Serial.printf("IP: %s\n", udp4Receive.remoteIP().toString().c_str());
        String value = udp4Receive.readString();
        Serial.println(value);
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
    udp4Receive.begin(_localInfo->receive_port);     // 开启UDP接收数据
    udp4Send.beginPacket(wifi->ip, wifi->send_port); // 开启UDP发送数据
}

void infoUpdate(wifi_info *wifi)
{
    _localInfo = wifi; // 留存本地备份
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
