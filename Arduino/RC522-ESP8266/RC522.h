/*********************************************************************
 * 功能:    RFID配置和功能
 * 模组:    RFID-RC511
 * 相关库:
 * 来源:
 * 作者: CloudLightIOT·云辉物联网平台·暇享
 * 联系vx：chaoji4
 ********************************************************************/
#ifndef _RC522_H
#define _RC522_H

#include <SPI.h>
#include <MFRC522.h>
#include "Config.h"

const byte SS_PIN = 15; // RC522  SDA
const byte RST_PIN = 2; // RST

extern MFRC522 mfrc522;

// 初始化RC522
void initRC522();
// 读取标签
String readRFID();

#endif