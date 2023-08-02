#include "Config.h"

WiFiClient wifiClient;
config_type config; // 构造音量大小

bool isAddCardMode = false;
bool isApMode = false;
bool isATMode = true;

/********************************** begin 可配置的项 **********************************/

uint16_t frequency = 1000; // 发出声音的频率

/********************************** end 可配置的项 **********************************/

void initConfig()
{
  pinMode(INA_PIN, OUTPUT);    // 设置INA引脚为输出
  pinMode(INB_PIN, OUTPUT);    // 设置INB引脚为输出
  pinMode(BUZZER_PIN, OUTPUT); // 初始化蜂鸣器引脚
}

// 存储配置
void saveConfig(config_type configSAVE)
{
  // 标识为已经存储数据
  configSAVE.flag = 1;

  // 初始化 EEPROM 存储空间并指定大小
  EEPROM.begin(390);

  // 写入数据到 EEPROM 存储空间
  for (int i = 0; i < sizeof(configSAVE); i++)
  {
    EEPROM.write(i, *((char *)&configSAVE + i));
  }

  // 结束 EEPROM 访问
  EEPROM.end();
}

// 加载配置
void loadConfig()
{
  printMsg("正在加载配置中.....");
  config_type configROM;
  // 初始化 EEPROM 存储空间并指定大小
  EEPROM.begin(390);

  // 读取 EEPROM 存储空间中的数据
  for (int i = 0; i < sizeof(configROM); i++)
  {
    *((char *)&configROM + i) = EEPROM.read(i);
  }
  // 结束 EEPROM 访问
  EEPROM.end();

  if (configROM.flag != 1)
  {
    printMsg("flash暂无数据");
    return;
  }

  printMsg("config.allCardsLength:" + (String)configROM.allCardsLength);
  printMsg("config.volumePercentage:" + (String)configROM.volumePercentage);
  printMsg("cardUUIDs:");
  for (int i = 0; i < MAX_CARDUUID; i++)
  {
    printMsg(configROM.cardUUIDs[i]);
  }
  config.flag = configROM.flag;
  config.allCardsLength = configROM.allCardsLength;
  config.volumePercentage = configROM.volumePercentage;
  for (int i = 0; i < MAX_CARDUUID; i++)
  {
    strcpy(config.cardUUIDs[i], configROM.cardUUIDs[i]);
  }
}

// 清空配置
void clearConfig()
{
  EEPROM.begin(240);
  for (int i = 0; i < 240; i++)
  {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
  printMsg("清空配置成功");
}

// 打印提示信息
void printMsg(String msg)
{
  // 检查Serial对象是否处于活动状态（串口连接正常）
  if (Serial)
  {
    Serial.print("\r\n[");
    Serial.print(millis());
    Serial.print("ms]");
    Serial.print(msg);
  }
}

// 控制蜂鸣器发声
void buzzerSound(char key)
{
  printMsg("蜂鸣器发声...");
  uint16_t pulseWidth = (255 * config.volumePercentage) / 100; // 根据音量大小计算 PWM 占空比
  analogWrite(BUZZER_PIN, pulseWidth);                         // 输出 PWM 信号
  if (key = 'a')
  {
    tone(BUZZER_PIN, frequency); // 发出指定频率的声音
    delay(380);                  // 等待指定时间
    noTone(BUZZER_PIN);          // 停止声音
  }
  else if (key = 'b')
  {
    for (size_t i = 0; i < 3; i++)
    {
      tone(BUZZER_PIN, frequency); // 发出指定频率的声音
      delay(380);                  // 等待指定时间
      noTone(BUZZER_PIN);          // 停止声音
      delay(380);                  // 等待下一个音符
    }
  }
  delay(50);                  // 等待下一个音符
}