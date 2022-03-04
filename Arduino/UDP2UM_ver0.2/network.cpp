/*-------------------------------------*/
/*              头文件加载             */
/*====================================*/
#include "network.h"
#include "wifi_set.h"
/*------------------------------------*/
/*              变量定义              */
/*===================================*/
#define LED_PIN 2
WiFiUDP Udp; // UDP通信结构体
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
    Udp.beginPacket(_localInfo->ip, _localInfo->send_port);
    Udp.write(adress);
    Udp.endPacket();
}
/*--------------------*/
/*       WIFI模块    */
/*==================*/
void broadcastMyIP(const wifi_info *wifi)
{
    Udp.beginPacket(wifi->ip, wifi->send_port); // 开启UDP发送数据
    Udp.write("IP,");
    Udp.write(wifi->localname);
    Udp.write(",");
    Udp.write(wifi->ip);
    Udp.endPacket();
}

void WiFi_init(const wifi_info *wifi)
{
    // SPISlave.setStatus(WIFI_INIT_FAIL);
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
    // SPISlave.setStatus(WIFI_INIT_SUCCESS);   //设置状态寄存器值为1
    Serial.println("WiFi_init success!");
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
