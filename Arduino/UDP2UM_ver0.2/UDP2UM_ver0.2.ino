/*-------------------------------------*/
/*              头文件加载              */
/*===================================*/
#include "network.h"
#include "wifi_set.h"
#include "stdint.h"
/*------------------------------------*/
/*              变量定义               */
/*===================================*/
wifi_info wifi = {
    // wifi信息存储
    .bind = false,
    .send_port = 9000,
    .receive_port = 8000,
    .staname = "智能车实验室601",
    .stapassword = "51927,488349.znC",
    .ip = "192.168.31.255",
    .defaultIp = "192.168.31.255",
    .localname = "ESP8266",
};
uint64_t systemTime = 0;
uint64_t lastTime = 0;
/*------------------------------------*/
/*               宏定义                */
/*===================================*/
#define LED_PIN 2
#define UART_BAUD 115200
/*------------------------------------*/
/*              函数定义               */
/*===================================*/
void setup()
{
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT); //初始化LED引脚
  digitalWrite(LED_PIN, 0); //点亮LED灯
  Serial.begin(UART_BAUD);
  infoUpdate(&wifi);
  WiFi_init(&wifi);
}

void loop()
{
  // put your main code here, to run repeatedly:
  systemTime = millis();
  udpReceive();
  if (wifi.bind == false && systemTime - lastTime > 1000) // 1秒执行一次
  {
    lastTime = systemTime;
    broadcastMyIP(&wifi);
  }
}
