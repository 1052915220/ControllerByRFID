#include "Apconfig.h"
#include "Config.h"
#include "RC522.h"

bool carState = false; // 磁保器状态 false = "OFF", true = "ON"

unsigned long lastRFID;           // 上次读取卡片时间
unsigned long lastConnectionTime; // 上次连接时间

void setup()
{
  // 初始化串口通信
  Serial.begin(115200);

  printMsg("CloudLightIOT·RFID开关设备初始化中...");
  // 初始化系统配置和引脚
  initConfig();
  // 加载配置
  loadConfig();
  // 启动AP模式
  startApConfig();
  lastConnectionTime = millis();

  // 初始化RC522
  initRC522();

  printMsg("设备初始化完成...");
}

void loop()
{
  if (isApMode)
  {
    // AP模式时的web监控服务
    server.handleClient();
  }
  RFIDController();
  OUTTimeStop();
}

/*
 *  RFID 控制器(非阻塞，间隔2s)
 */
void RFIDController()
{
  long now = millis();
  if (now - lastRFID > 2000)
  {
    String cardUUID = readRFID();
    if (cardUUID.isEmpty())
    {
      return;
    }

    if (isAddCardMode)
    {
      printMsg("开始添加卡片......");
      for (int i = 0; i < MAX_CARDUUID; i++)
      {
        // 使用 strcmp 函数比较 C 风格字符串
        if (strcmp(config.cardUUIDs[i], cardUUID.c_str()) == 0)
        {
          printMsg("重复添加卡片......");
          return;
        }
      }
      strcpy(config.cardUUIDs[config.allCardsLength], cardUUID.c_str());
      config.allCardsLength++;

      String cards = "";
      for (int i = 0; i < MAX_CARDUUID; i++)
      {
        cards += config.cardUUIDs[i]; // 将当前UUID添加到字符串中
        cards += ",";                 // 添加逗号分隔符
      }
      printMsg(cards.c_str());

      // 存储配置
      saveConfig(config);
      isAddCardMode = false;
      printMsg("添加卡片成功......");
    }
    else
    {
      carService(cardUUID);
    }
    lastRFID = now;
  }
}

void carService(String cardUUID)
{
  printMsg("开始处理卡片是否匹配......");
  bool isContained = false; // 初始化为不包含

  for (int i = 0; i < MAX_CARDUUID; i++)
  {
    // 使用 strcmp 函数比较 C 风格字符串
    if (strcmp(config.cardUUIDs[i], cardUUID.c_str()) == 0)
    {
      printMsg("识别到匹配的卡片......");
      isContained = true;
      break;
    }
  }

  if (isContained)
  // 识别到匹配的卡片
  {
    if (carState)
    // 识别到磁保器状态为ON
    {
      buzzerSound('a');
      printMsg("磁保器状态设置为OFF......");
      carState = false;
      digitalWrite(INA_PIN, LOW);  // 将INA引脚输出LOW
      digitalWrite(INB_PIN, HIGH); // 将INB引脚输出HIGH
      delay(200);
      digitalWrite(INB_PIN, LOW); // 将INA引脚输出HIGH
    }
    else
    {
      buzzerSound('a');
      printMsg("磁保器状态设置为ON......");
      carState = true;
      digitalWrite(INA_PIN, HIGH); // 将INA引脚输出HIGH
      digitalWrite(INB_PIN, LOW);  // 将INB引脚输出LOW
      delay(200);
      digitalWrite(INA_PIN, LOW); // 将INA引脚输出HIGH
    }
    if (!isApMode)
    {
      startApConfig();
      lastConnectionTime = millis();
    }
  }
}

/**
 * 超过60秒WiFi无连接，就关闭AP与web服务
 */
void OUTTimeStop()
{
  if (!isApMode)
  {
    return;
  }
  // 检查是否需要断开连接
  unsigned long currentTime = millis();
  if (WiFi.softAPgetStationNum() != 0)
  {
    lastConnectionTime = currentTime; // 重置连接时间
  }
  else if (currentTime - lastConnectionTime > 120000)
  {
    printMsg("开始关闭AP与web服务");
    isApMode = false;
    // 已经超过60秒没有发现连接，断开WiFi连接
    WiFi.disconnect();
    // 关闭AP模式
    WiFi.softAPdisconnect(true);
    // 关闭Web服务器
    server.stop();
  }
}