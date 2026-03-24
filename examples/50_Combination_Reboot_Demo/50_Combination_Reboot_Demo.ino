/*
 * 示例名称：50_Combination_Reboot_Demo
 * 示例功能：演示组合播放与重启模块。
 * 演示资源：
 *   /ZH/01.mp3
 *   /ZH/02.mp3
 *   /ZH/03.mp3
 * 演示逻辑：
 *   1. 组合播放 01 + 02
 *   2. 停止组合播放
 *   3. 组合播放 01 + 02 + 03
 *   4. 重启模块
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

  Serial.println(F("组合播放与重启示例开始。"));

  // 先演示 01 + 02 两首组合播放。
  Serial.println(F("步骤1：组合播放 01 + 02。"));
  player.playCombination("0102");
  delay(5000);

  // 停止组合播放，准备演示更长的组合。
  Serial.println(F("步骤2：停止组合播放。"));
  player.stop();
  delay(1500);

  // 再演示 01 + 02 + 03 三首组合播放。
  Serial.println(F("步骤3：组合播放 01 + 02 + 03。"));
  player.playCombination("010203");
  delay(7000);

  // 演示重启模块命令。
  Serial.println(F("步骤4：发送重启模块命令。"));
  player.reboot();
}

void loop() {
}
