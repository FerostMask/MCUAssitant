/*-------------------------------------*/
/*              头文件加载              */
/*===================================*/
#include "network.h"
#include "wifi_set.h"
/*------------------------------------*/
/*              变量定义               */
/*===================================*/
wifi_info wifi = {//wifi信息存储
  .send_port = 9000,
  .receive_port = 8000,
  .staname = "智能车实验室601",
  .stapassword = "51927,488349.znC",
  .ip = "192.168.31.26",
  .localname = "ESP8266",
};
/*------------------------------------*/
/*               宏定义                */
/*===================================*/
#define LED_PIN 2
#define UART_BAUD 115200
/*------------------------------------*/
/*              函数定义               */
/*===================================*/
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT); //初始化LED引脚
  digitalWrite(LED_PIN, 0); //点亮LED灯
  Serial.begin(UART_BAUD);
  wifiSetInit();
  WiFi_init(&wifi);
}

void loop() {
  // put your main code here, to run repeatedly:
  broadcastMyIP(&wifi);
  delay(1000);
}
