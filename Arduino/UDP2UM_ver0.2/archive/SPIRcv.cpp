/*-------------------------------------*/
/*              头文件加载             */
/*====================================*/
#include "SPIRcv.h"
/*------------------------------------*/
/*              变量定义              */
/*===================================*/
#define LED_PIN 2
static spi_s slave;
static char blk = 0;
/*------------------------------------*/
/*              函数声明              */
/*===================================*/

/*------------------------------------*/
/*              函数定义              */
/*===================================*/
/*--------------------*/
/*     SPI从机接收    */
/*===================*/
void spi_rcv_init()
{
    // 申请内存
    slave.buff = new char[UDP_MAX_TIMES][UDP_ONECUT_LEN];
    SPISlave.onData([](uint8_t *data, size_t len) { // 接收处理 | 回调函数
        char *message = (char *)data;
        (void)len;
        // LED灯闪烁
        Serial.println("receive!\n");
        blk = !blk;
        digitalWrite(LED_PIN, blk);
        slave.data = &message[DATA_HEAD];
        switch (message[INSTRUCTION] & 0xF0)
        {
        case SPI_COMN: // SPI通信
            spi_data_receive(message[INSTRUCTION]);
            break;
        case WIFI_INFO_SET: // wifi信息设置
            set_info(message[INSTRUCTION]);
            break;
        case WIFI_INITIALIZATION: // wifi初始化
            if (message[INSTRUCTION] == WIFI_INITIALIZATION)
                WiFi_init(); //初始化WiFi
            break;
        }
    });
}
/*------------------------------------*/
/*              辅助函数               */
/*===================================*/
void spi_data_receive(unsigned char num)
{
    switch (num)
    {
    case SPI_COMN_START: //开始通信
        slave.spi_count = 0;
        memcpy(slave.buff, slave.data, MAX_DATA_LEN);
        break;
    case SPI_COMN_UNDERWAY: //通信进行中
        slave.spi_count++;
        memcpy(&(*slave.buff)[MAX_DATA_LEN * slave.spi_count], slave.data, MAX_DATA_LEN);
        break;
    case SPI_COMN_END: //通信结束
        if (slave.spi_count != 0)
            slave.spi_count++;
        memcpy(&(*slave.buff)[MAX_DATA_LEN * slave.spi_count], slave.data, sizeof(slave.data));
        //  计算数据长度并发送至上位机
        short data_size = slave.spi_count * MAX_DATA_LEN + sizeof(slave.data);
        char data_size2um[2] = {data_size & 0xFF00 >> 16, data_size & 0x00FF};
        Udp.write(data_size2um);
        Udp.endPacket();
        delay(2);
        for (int i = 0; i < data_size / UDP_ONECUT_LEN; i++)
        { //循环发送数据
            Udp.write(slave.buff[i]);
            Udp.endPacket();
            delay(2);
        }
        slave.spi_count = 0;
        break;
    }
}

void set_info(unsigned char num)
{
    switch (num)
    {
    case WIFI_SET_NAME:                                       //设置wifi名称
        delete wifi.staname;                                  //释放内存
        wifi.staname = new char[sizeof(slave.data) + 1];      //申请新的内存空间
        memcpy(wifi.staname, slave.data, strlen(slave.data)); // copy数据到新的地址
        break;
    case WIFI_SET_PASSWORD: //设置wifi密码
        delete wifi.stapassword;
        wifi.stapassword = new char[sizeof(slave.data) + 1];
        memcpy(wifi.stapassword, slave.data, strlen(slave.data));
        break;
    case WIFI_SET_IP: //设置指定发送IP
        delete wifi.ip;
        wifi.ip = new char[sizeof(slave.data) + 1];
        memcpy(wifi.ip, slave.data, strlen(slave.data));
        break;
    case WIFI_SET_SENDPORT:                //设置指定UDP发送端口
        wifi.send_port = atoi(slave.data); //字符串转整型
        break;
    case WIFI_SET_RECEIVEPORT: //设置指定UDP读取端口
        wifi.receive_port = atoi(slave.data);
        break;
    }
}
