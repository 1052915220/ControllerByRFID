#include "RC522.h"


MFRC522 mfrc522(SS_PIN, RST_PIN);


void initRC522()
{
  SPI.begin();                  // 初始化SPI通信
  mfrc522.PCD_Init();           // 初始化MFRC522,阻塞式
  printMsg("RFID初始化完成...");
}

//读取标签
String readRFID()
{
    // 读取卡号
    String cardUUID = "";
    // 检测卡片是否在读卡器范围内
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
    {
        for (byte i = 0; i < mfrc522.uid.size; i++)
        {
            char temp[3];
            sprintf(temp, "%02X", mfrc522.uid.uidByte[i]);
            cardUUID += temp;
        }

        printMsg("Card UID: ");
        Serial.print(cardUUID.c_str());
        // rc522进入休眠状态
        mfrc522.PICC_HaltA();
        // rc522停止加密通信
        mfrc522.PCD_StopCrypto1();
    }
    return cardUUID;
}