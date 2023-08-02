/*********************************************************************
 * 功能:    设备AP配网
 * 模组:    esp8266--esp12s
 * 相关库:  PubSubClient2.8.0  & ArduinoJson6.19.1 & OneButton2.0.4
 * 来源:    
 * 作者:    云辉物联网平台·暇享
 * 联系vx： chaoji4
 ********************************************************************/

#ifndef _APCONFIG_H
#define _APCONFIG_H

#include "Config.h"
#include "RC522.h"
#include <ESP8266WebServer.h>


extern ESP8266WebServer server;

// 启动AP配网
void startApConfig();
// 启动Web服务
void startWebServer();
// 检测设备接口
void handleStatus();
// 找不到页面和浏览器跨域处理
void handleNotFound();
// 添加卡片接口
void addCard();
// 清空配置接口
void clearFlashConfig();
// 查询设备数据接口
void query();
// 按照序号删除卡片
void deleteById();

#endif
