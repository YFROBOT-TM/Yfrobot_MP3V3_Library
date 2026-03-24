/*
 * 示例名称：20_Advert_Path_Demo
 * 示例功能：演示指定路径播放、指定路径插播、结束插播、
 *           上一文件夹播放、下一文件夹播放。
 * 演示资源：
 *   1. 根目录：00010.mp3
 *   2. chabo 文件夹：00001 ~ 00008.mp3
 * 说明：
 *   本示例优先播放根目录 00010，再插播 /chabo/00001.*，
 *   插播结束后继续回到原来的播放位置。
 * 作者：YFROBOT
 * 日期：2026-03-24
 */

#include <Yfrobot_MP3V3.h>

YfrobotMP3V3 player;

/**
 * @brief 根据当前板卡自动初始化模块。
 * @return `true` 表示初始化成功；`false` 表示初始化失败。
 */
static bool beginModule() {
#if defined(ESP32)
  return player.beginHardwareSerial(Serial2, 16, 17);
#elif defined(ARDUINO_AVR_MEGA2560)
  return player.beginHardwareSerial(Serial1);
#elif defined(ARDUINO_ARCH_AVR)
  return player.beginSoftwareSerial(11, 10);
#else
  return false;
#endif
}

void setup() {
  Serial.begin(115200);
  delay(300);

  if (!beginModule()) {
    Serial.println(F("模块初始化失败。"));
    while (true) {
      delay(1000);
    }
  }

  player.setDeviceId(YfrobotMP3V3::kBroadcastDeviceId);
  player.setVolume(25);

  Serial.println(F("插播与路径播放示例开始。"));

  // 先播放根目录 00010 长音频，便于听出插播前后的衔接。
  Serial.println(F("步骤1：播放根目录 00010。"));
  player.playTrack(10);
  delay(4000);

  // 指定路径插播 chabo/00001。
  Serial.println(F("步骤2：插播 /chabo/00001.*。"));
  player.insertAdvert(YfrobotMP3V3::STORAGE_FLASH, "/chabo/00001.*");
  delay(3000);

  // 主动结束插播，恢复到插播前的主节目。
  Serial.println(F("步骤3：结束插播。"));
  player.stopAdvert();
  delay(3000);

  // 直接指定路径播放 chabo/00002。
  Serial.println(F("步骤4：指定路径播放 /chabo/00002.*。"));
  player.playPath(YfrobotMP3V3::STORAGE_FLASH, "/chabo/00002.*");
  delay(3000);

  // 在 chabo 文件夹内切换到上一个文件夹播放。
  Serial.println(F("步骤5：执行上一文件夹播放。"));
  player.playPreviousFolder();
  delay(3000);

  // 再执行下一文件夹播放。
  Serial.println(F("步骤6：执行下一文件夹播放。"));
  player.playNextFolder();
  delay(3000);

  Serial.println(F("步骤7：停止播放，示例结束。"));
  player.stop();
}

void loop() {
}
