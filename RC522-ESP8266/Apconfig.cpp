#include "ApConfig.h"

// String randomName = "CloudLight-device" + (String)random(1000);
String randomName = "CloudLight-device";
const char *ap_ssid = randomName.c_str();
// 开放式网络，不设置密码
const char *ap_password = "";

IPAddress local_IP(192, 168, 7, 1);
IPAddress gateway(192, 168, 7, 1);
IPAddress subnet(255, 255, 255, 0);
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

/**
 * 启动AP模式
 */
void startApConfig()
{
  // ledStatus(true);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ap_ssid, ap_password);
  printMsg("已启动AP配网，IP地址：" + WiFi.softAPIP().toString() + "， 热点名称：" + (String)ap_ssid);
  // 网页更新固件
  httpUpdater.setup(&server);
  printMsg("启动网页更新固件");
  // 启动web服务
  startWebServer();
}

/**
 * 启动Web服务
 */
void startWebServer()
{
  isApMode = true;
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/clear", HTTP_GET, clearFlashConfig);
  server.on("/add", HTTP_GET, addCard);
  server.on("/query", HTTP_GET, query);
  server.on("/delete", HTTP_GET, deleteById);
  server.onNotFound(handleNotFound);
  server.enableCORS(true);
  server.begin();
  printMsg("HTTP服务已启动");
}

/**
 * 检测设备接口
 */
void handleStatus()
{
  server.send(200, "text/plain;charset=utf-8", "已准备就绪");
}

/**
 * 添加卡片接口
 */
void addCard()
{
  printMsg("开始添加卡片......");
  if (config.allCardsLength >= MAX_CARDUUID)
  {
    server.send(200, "text/plain;charset=utf-8", "添加卡片失败，卡片数量超出最大值7");
    return;
  }

  //用于测试的代码
  // strcpy(config.cardUUIDs[config.allCardsLength], "123123");
  // config.allCardsLength++;
  // saveConfig(config);

  isAddCardMode = true;

  server.send(200, "text/plain;charset=utf-8", "添加卡片，已准备就绪");
  // 蜂鸣器发声
  buzzerSound('b');
}
/**
 * 清空配置接口
 */
void clearFlashConfig()
{
  printMsg("开始清空配置......");
  clearConfig();
  server.send(200, "text/plain;charset=utf-8", "清空配置成功");
  printMsg("开始进行复位RST...");
  delay(500);
  ESP.reset(); // 进行复位
}
/**
 * 查询设备数据接口
 */
void query()
{
  printMsg("开始查询设备数据......");
  DynamicJsonDocument jsonDoc(300);
  jsonDoc["flag"] = config.flag;
  jsonDoc["allCardsLength"] = config.allCardsLength;
  jsonDoc["volumePercentage"] = config.volumePercentage;
  JsonArray cardUUIDsArray = jsonDoc.createNestedArray("cardUUIDs");
  for (int i = 0; i < MAX_CARDUUID; i++)
  {
    cardUUIDsArray.add(config.cardUUIDs[i]);
  }
  String json;
  serializeJson(jsonDoc, json);

  server.send(200, "text/plain;charset=utf-8", json);
}
/**
 * 按照序号删除卡片
 */
void deleteById()
{
  printMsg("开始按照序号删除卡片......");
  if (server.hasArg("id"))
  {
    uint16_t id = ((server.arg("id").toInt()) - 1);
    if (!(id < 7))
    {
      server.send(200, "text/plain;charset=utf-8", "删除失败：参数有误，最多7张卡");
      return;
    }if (!config.allCardsLength>0)
    {
      server.send(200, "text/plain;charset=utf-8", "删除失败：请刷新");
      return;
    }
    // 从指定索引开始，将该索引后面的元素向前移动一位
    for (int i = id; i < MAX_CARDUUID - 1; ++i)
    {
      strcpy(config.cardUUIDs[i], config.cardUUIDs[i + 1]);
    }
    // 重置最后一个元素为无效值
    strcpy(config.cardUUIDs[MAX_CARDUUID - 1], "");
    config.allCardsLength--;
    saveConfig(config);
    server.send(200, "text/plain;charset=utf-8", "按照序号删除卡片成功");
  }
  else
  {
    printMsg("必须使用正确的id删除卡片");
    server.send(200, "text/plain;charset=utf-8", "必须使用正确的id删除卡片");
  }
}
/**
 * 找不到页面和跨域处理
 */
void handleNotFound()
{
  printMsg("进入预检请求或请求地址找不到");
  if (server.method() == HTTP_OPTIONS)
  {
    // 处理浏览器跨域问题
    server.sendHeader("Access-Control-Max-Age", "10000");
    server.sendHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "*");
    server.send(204);
  }
  else
  {
    server.send(404, "text/plain;charset=utf-8", "请求的地址找不到或无法访问");
  }
}
