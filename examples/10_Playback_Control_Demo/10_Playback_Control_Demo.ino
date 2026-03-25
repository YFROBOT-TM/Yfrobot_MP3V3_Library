/*
 * 示例名称：10_Playback_Control_Demo
 * 示例功能：集中演示最常用的播放控制命令。
 * 演示内容：
 *   1. 查询总曲目数
 *   2. 播放根目录 00010
 *   3. 暂停与继续播放
 *   4. 上一曲 / 下一曲
 *   5. 停止播放
 *   6. 选曲不播放 + 再执行播放
 * 预存音频树：
 *   根目录：00001 ~ 00010
 * 作者：YFROBOT
 * 日期：2026-03-24
 */

#include <Yfrobot_MP3V3.h>

YfrobotMP3V3 player;

/**
 * @brief 根据当前板卡自动选择合适的串口初始化方式。
 * @return `true` 表示初始化成功；`false` 表示当前板卡未匹配。
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

/**
 * @brief 通过串口打印当前曲目名。
 */
static void printTrackName() {
  char trackName[32];
  if (player.readCurrentTrackName(trackName, sizeof(trackName))) {
    Serial.print(F("当前曲目："));
    Serial.println(trackName);
  } else {
    Serial.println(F("当前曲目名称暂无返回。"));
  }
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

  player.setVolume(25);

  Serial.println(F("播放控制综合示例开始。"));

  // 查询总曲目数，确认根目录资源已被正确识别。
  uint16_t totalTracks = 0;
  if (player.readTotalTracks(totalTracks)) {
    Serial.print(F("总曲目数："));
    Serial.println(totalTracks);
  } else {
    Serial.println(F("读取总曲目数失败。"));
  }

  // 直接播放根目录 00010 长音频。
  Serial.println(F("步骤1：播放根目录 00010。"));
  player.playTrack(10);
  delay(4000);
  printTrackName();

  // 暂停后稍等，用于观察暂停效果。
  Serial.println(F("步骤2：暂停播放。"));
  player.pause();
  delay(2000);

  // 继续播放同一曲目。
  Serial.println(F("步骤3：继续播放。"));
  player.play();
  delay(3000);

  // 切换到上一曲。
  Serial.println(F("步骤4：上一曲。"));
  player.previousTrack();
  delay(2000);
  printTrackName();

  // 再切换到下一曲。
  Serial.println(F("步骤5：下一曲。"));
  player.nextTrack();
  delay(2000);
  printTrackName();

  // 先停止，再用“选曲不播放”选择 00003。
  Serial.println(F("步骤6：停止播放。"));
  player.stop();
  delay(1500);

  Serial.println(F("步骤7：选中根目录 00003，但暂不播放。"));
  player.selectTrack(3);
  delay(1500);

  // 再发送播放命令，让模块从已选曲目开始播放。
  Serial.println(F("步骤8：执行播放。"));
  player.play();
  delay(3000);
  printTrackName();

  // 示例结束后停止，避免循环反复打断试听。
  Serial.println(F("步骤9：结束示例并停止播放。"));
  player.stop();
}

void loop() {
}
