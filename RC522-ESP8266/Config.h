/*********************************************************************
 * 功能： 设备配置和系统功能
 * 模组:     esp8266--esp12s
 * 相关库：
 * 来源:
 * 作者: 云辉物联网平台·暇享
 * 联系vx：chaoji4
 ********************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H

#include "Apconfig.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

const byte BUZZER_PIN = 16; // 蜂鸣器引脚
const byte RX_PIN = 2;      // 确定 ESP8266 返回数据的引脚
const byte TX_PIN = 3;      // ESP8266 数据发送引脚
const byte INA_PIN = 5;     // 磁保器INA
const byte INB_PIN = 4;     // 磁保器INB

const byte MAX_CARDUUID = 7; // 最大卡片数量

// 存储的配置类型结构
struct config_type
{
  byte flag; // 是否有数据标识，等于1表示有数据
  char cardUUIDs[MAX_CARDUUID][20];
  byte allCardsLength;   // 卡片数量
  byte volumePercentage; // 设置音量大小，以百分比为单位
  config_type()
  {
    for (int i = 0; i < MAX_CARDUUID; i++)
    {
      if (i == 0)
      {
        // strcpy(cardUUIDs[i], "1212121212");//测试代码
        strcpy(cardUUIDs[i], "");
        continue;
      }

      strcpy(cardUUIDs[i], "");
    }
    // allCardsLength = 1;//测试代码
    allCardsLength = 0;
    volumePercentage = 100;
  }
};

extern config_type config;

extern WiFiClient wifiClient;

// 全局变量
extern bool isApMode;      // 是否进入AP模式
extern bool isAddCardMode; // 是否进入添加卡片模式
extern bool isATMode;      // 是否进入AT指令模式

extern uint16_t frequency; // 发出声音的频率

// 初始化系统配置和引脚
void initConfig();
// 加载配置
void loadConfig();
// 保存配置
void saveConfig(config_type config);
// 清空配置
void clearConfig();
// 打印提示信息
void printMsg(String msg);
// 控制蜂鸣器发声 'a'正常声音；'b'功能设置声音；
void buzzerSound(char key);

#endif
