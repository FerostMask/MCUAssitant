/*-------------------------------------*/
/*              头文件加载              */
/*===================================*/
#include "wifi.h"
#include <SPISlave.h>
#include <cstring>
/*------------------------------------*/
/*               宏定义                */
/*===================================*/
#define LED_PIN 2
#define UART_BAUD 115200
/*------------------------------------*/
/*              枚举类型               */
/*===================================*/
typedef enum {
  INSTRUCTION = 0,//通信指示
  NUMBER = 1,//编号
  DATA_HEAD = 2,//数据起始位

  MAX_DATA_LEN = 30,//SPI最大数据长度

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

  UDP_ONECUT_LEN = 1120,//UDP发送数据长度
  UDP_MAX_TIMES = 4,//UDP最大通信次数
} COM_enum;
/*------------------------------------*/
/*              函数声明               */
/*===================================*/
void set_info(unsigned char num);
void spi_data_receive(unsigned char num);
/*------------------------------------*/
/*              结构体定义              */
/*===================================*/
typedef struct spi_s {
  char (*buff)[UDP_ONECUT_LEN];
  char *data;
  int spi_count;
} spi_s;
/*------------------------------------*/
/*              变量定义               */
/*===================================*/
wifi_info wifi = {//wifi信息存储
  .send_port = 9000,
  .receive_port = 8000,
  .staname = "智能车实验室601",
  .stapassword = "51927,488349.znC",
  .ip = "192.168.31.26",
};
spi_s slave;
WiFiUDP Udp;//UDP通信结构体
char blk = 0;
/*------------------------------------*/
/*              函数定义               */
/*===================================*/
/*--------------------*/
/*       WIFI模块     */
/*==================*/
void UDP_read() {

}
/*--------------------*/
/*     SPI从机接收     */
/*===================*/
void spi_rcv_init() {
  SPISlave.onData([](uint8_t * data, size_t len) {//接收部分
    char *message = (char *)data;
    (void) len;
    // LED灯闪烁
    blk = !blk;
    digitalWrite(LED_PIN, blk);
    slave.data = &message[DATA_HEAD];
    switch (message[INSTRUCTION] & 0xF0) {
      case SPI_COMN://SPI通信
        spi_data_receive(message[INSTRUCTION]);
        break;
      case WIFI_INFO_SET://wifi信息设置
        set_info(message[INSTRUCTION]);
        break;
      case WIFI_INITIALIZATION://wifi初始化
        if (message[INSTRUCTION] == WIFI_INITIALIZATION) WiFi_init();//初始化WiFi
        break;
    }
  });
}
/*------------------------------------*/
/*              默认函数               */
/*===================================*/
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT); //初始化LED引脚
  digitalWrite(LED_PIN, 0); //点亮LED灯
  WiFi_init();
  Serial.begin(UART_BAUD);
  slave.buff = new char[UDP_MAX_TIMES][UDP_ONECUT_LEN];
  spi_rcv_init();
}

void loop() {
  // put your main code here, to run repeatedly:
  int packetSize = Udp.parsePacket();
  Serial.printf("local IP:%s, receive data length: %d\n", WiFi.localIP().toString().c_str(), packetSize);
  if (packetSize) {
    String udpReceive = Udp.readString();
    Serial.printf("接收到字符串%s\n", udpReceive);
    Udp.beginPacket(wifi.ip, wifi.send_port);
    Udp.write("hello there!");
    Udp.endPacket();
    Udp.begin(wifi.receive_port);
  }
  delay(1000);
}
/*------------------------------------*/
/*              辅助函数               */
/*===================================*/
void spi_data_receive(unsigned char num) {
  switch (num) {
    case SPI_COMN_START://开始通信
      slave.spi_count = 0;
      memcpy(slave.buff, slave.data, MAX_DATA_LEN);
      break;
    case SPI_COMN_UNDERWAY://通信进行中
      slave.spi_count++;
      memcpy(&(*slave.buff)[MAX_DATA_LEN * slave.spi_count], slave.data, MAX_DATA_LEN);
      break;
    case SPI_COMN_END://通信结束
      if (slave.spi_count != 0) slave.spi_count++;
      memcpy(&(*slave.buff)[MAX_DATA_LEN * slave.spi_count], slave.data, sizeof(slave.data));
      //  计算数据长度并发送至上位机
      short data_size = slave.spi_count * MAX_DATA_LEN + sizeof(slave.data);
      char data_size2um[2] = {data_size & 0xFF00 >> 16, data_size & 0x00FF};
      Udp.write(data_size2um);
      Udp.endPacket();
      delay(2);
      for (int i = 0; i < data_size / UDP_ONECUT_LEN; i++) { //循环发送数据
        Udp.write(slave.buff[i]);
        Udp.endPacket();
        delay(2);
      }
      slave.spi_count = 0;
      break;
  }
}

void set_info(unsigned char num) {
  switch (num) {
    case WIFI_SET_NAME://设置wifi名称
      delete wifi.staname;//释放内存
      wifi.staname = new char[sizeof(slave.data) + 1]; //申请新的内存空间
      memcpy(wifi.staname, slave.data, strlen(slave.data));//copy数据到新的地址
      break;
    case WIFI_SET_PASSWORD://设置wifi密码
      delete wifi.stapassword;
      wifi.stapassword = new char[sizeof(slave.data) + 1];
      memcpy(wifi.stapassword, slave.data, strlen(slave.data));
      break;
    case WIFI_SET_IP://设置指定发送IP
      delete wifi.ip;
      wifi.ip = new char[sizeof(slave.data) + 1];
      memcpy(wifi.ip, slave.data, strlen(slave.data));
      break;
    case WIFI_SET_SENDPORT://设置指定UDP发送端口
      wifi.send_port = atoi(slave.data);//字符串转整型
      break;
    case WIFI_SET_RECEIVEPORT://设置指定UDP读取端口
      wifi.receive_port = atoi(slave.data);
      break;
  }
}
String Read_Udp() {
  //  String da = "";
  //  int packetSize = Udp.parsePacket();
  //  if (packetSize) {
  //    memset(readdata, 0x00, sizeof(readdata));
  //    Udp.read(readdata, 511);
  //    Udp.flush();
  //    for (int i = 0; i < packetSize; i++)
  //    {
  //      da += readdata[i];
  //    }
  //    Serial.println(da);
  //  }
  //  return da;
}

void Udp_Handler(String da) {

}

void myPrint(String da, bool a) {
  //  if (a) {
  //    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  //  }
  //  else {
  //    Udp.beginPacket(h, send_port);
  //  }
  //  Udp.print(da);
  //  Udp.write("1");
  //  Udp.endPacket();
}
