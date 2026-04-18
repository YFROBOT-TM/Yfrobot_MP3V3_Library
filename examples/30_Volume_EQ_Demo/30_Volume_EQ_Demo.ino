/*
 * 示例名称：30_Volume_EQ_Demo
 * 示例功能：演示音量设置、音量加减以及 EQ 模式切换。
 * 演示逻辑：
 *   1. 播放根目录 00010 长音频
 *   2. 将音量设置为 10
 *   3. 连续增加 5 级音量
 *   4. 再直接设置为 25
 *   5. 再连续减小 3 级
 *   6. 通过暂停 / 继续播放切换不同 EQ 模式试听
 * 音量范围：
 *   0 ~ 30，模块默认 25
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
 * @brief 查询并打印当前音量。
 */
static void printVolume() {
  uint8_t volume = 0;
  if (player.readVolume(volume)) {
    Serial.print(F("当前音量："));
    Serial.println(volume);
  } else {
    Serial.println(F("读取音量失败。"));
  }
}

/**
 * @brief 暂停后切换 EQ，再继续播放，便于试听差异。
 * @param eq 需要设置的 EQ 模式。
 * @param eqName 串口输出显示的 EQ 名称。
 */
static void applyEqAndResume(YfrobotMP3V3::EqPreset eq, const __FlashStringHelper *eqName) {
  player.pause();
  delay(800);
  player.setEq(eq);
  Serial.print(F("切换 EQ："));
  Serial.println(eqName);
  player.play();
  delay(3000);
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

  Serial.println(F("音量与 EQ 示例开始。"));
  
  // 通过暂停和继续播放切换多组 EQ，方便试听效果。
  Serial.println(F("自行注释选择EQ模式，试听不同 EQ 模式。"));
  // applyEqAndResume(YfrobotMP3V3::EQ_NORMAL, F("NORMAL"));
  applyEqAndResume(YfrobotMP3V3::EQ_ROCK, F("ROCK"));
  // applyEqAndResume(YfrobotMP3V3::EQ_POP, F("POP"));
  // applyEqAndResume(YfrobotMP3V3::EQ_CLASSIC, F("CLASSIC"));
  // applyEqAndResume(YfrobotMP3V3::EQ_JAZZ, F("JAZZ"));
  delay(200);

  // 播放根目录 00010 长音频，便于听出变化。
  Serial.println(F("步骤1：播放根目录 00010。"));
  player.playTrack(10);
  delay(2000);

  // 直接设置音量到 10。
  Serial.println(F("步骤2：音量直接设置为 10。"));
  player.setVolume(10);
  delay(1000);
  printVolume();

  // 连续增加 5 次音量，每次间隔 800ms。
  Serial.println(F("步骤3：连续增加 5 级音量。"));
  for (uint8_t i = 0; i < 5; ++i) {
    player.volumeUp();
    delay(800);
  }
  printVolume();

  // 再直接设置为 25，模拟恢复较大音量。
  Serial.println(F("步骤4：音量直接设置为 25。"));
  player.setVolume(25);
  delay(1000);
  printVolume();

  // 再连续降低 3 次音量。
  Serial.println(F("步骤5：连续减小 3 级音量。"));
  for (uint8_t i = 0; i < 3; ++i) {
    player.volumeDown();
    delay(800);
  }
  printVolume();

  Serial.println(F("步骤6：示例结束，停止播放。"));
  player.stop();
}

void loop() {
}
