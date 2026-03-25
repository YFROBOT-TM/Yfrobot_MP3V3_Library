/*
 * 示例名称：40_PlayMode_Save_Demo
 * 示例功能：演示播放模式设置、循环次数设置、读取播放模式、
 *           保存参数以及恢复默认参数。
 * 演示逻辑：
 *   1. 设置“单曲播放停止”，播放根目录 00010
 *   2. 设置“单曲循环”，再次播放 00010
 *   3. 设置“文件夹循环，循环 2 次”，播放 /chabo/00001.*
 *   4. 查询当前播放模式和循环次数
 *   5. 保存参数
 *   6. 恢复出厂默认参数
 * 提醒：
 *   恢复默认参数会修改模块内部保存的设置，请根据需要使用。
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

/**
 * @brief 打印当前播放模式与循环次数。
 */
static void printPlaybackMode() {
  YfrobotMP3V3::PlaybackModeSettings settings;
  if (player.readPlaybackMode(settings)) {
    Serial.print(F("当前播放模式："));
    Serial.println(static_cast<uint8_t>(settings.mode));
    Serial.print(F("当前循环次数："));
    Serial.println(settings.repeatCount);
  } else {
    Serial.println(F("读取播放模式失败。"));
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

  Serial.println(F("播放模式与参数保存示例开始。"));

  // 设置为“单曲播放停止”，试听根目录 00010。
  Serial.println(F("步骤1：设置为单曲播放停止。"));
  player.setPlaybackMode(YfrobotMP3V3::PLAY_MODE_STOP_AFTER_SINGLE, 0);
  player.playTrack(10);
  delay(4000);
  player.stop();

  // 设置为“单曲循环”，再次试听根目录 00010。
  Serial.println(F("步骤2：设置为单曲循环。"));
  player.setPlaybackMode(YfrobotMP3V3::PLAY_MODE_LOOP_SINGLE, 0);
  player.playTrack(10);
  delay(4000);
  player.stop();

  // 设置为“文件夹循环 2 次”，然后播放 chabo 文件夹中的曲目。
  Serial.println(F("步骤3：设置为文件夹循环 2 次。"));
  player.setPlaybackMode(YfrobotMP3V3::PLAY_MODE_LOOP_FOLDER, 2);
  player.playPath(YfrobotMP3V3::STORAGE_FLASH, "/chabo/00001.*");
  delay(4000);

  // 查询并打印当前播放模式。
  Serial.println(F("步骤4：读取当前播放模式。"));
  printPlaybackMode();

  // 保存当前参数，掉电后仍可保留。
  Serial.println(F("步骤5：保存当前参数。"));
  if (player.saveSettings()) {
    Serial.println(F("参数保存命令已发送。"));
  } else {
    Serial.println(F("参数保存命令发送失败。"));
  }
  delay(1500);

  // 恢复默认参数，方便再次演示出厂状态。
  Serial.println(F("步骤6：恢复默认参数。"));
  if (player.restoreFactorySettings()) {
    Serial.println(F("恢复默认参数命令已发送。"));
  } else {
    Serial.println(F("恢复默认参数命令发送失败。"));
  }

  player.stop();
}

void loop() {
}
