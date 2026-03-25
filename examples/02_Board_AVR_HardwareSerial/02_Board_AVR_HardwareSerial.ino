/*
 * 示例名称：02_Board_AVR_HardwareSerial
 * 示例功能：演示 UNO R3 / Mega2560 使用硬件串口连接模块。
 * 演示内容：
 *   1. 播放根目录 00001
 *   2. 切换到下一曲
 *   3. 暂停
 *   4. 继续播放
 * 使用提示：
 *   1. UNO R3 使用硬件串口时会占用 Serial 引脚。
 *   2. 上传程序时建议暂时断开模块 RX/TX。
 *   3. Mega2560 推荐使用 Serial1。
 * 作者：YFROBOT
 * 日期：2026-03-24
 */

#include <Yfrobot_MP3V3.h>

#if defined(ARDUINO_ARCH_AVR)
YfrobotMP3V3 player;

/**
 * @brief 让板载 LED 闪烁一次，作为执行步骤提示。
 */
static void blinkOnce() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(120);
  digitalWrite(LED_BUILTIN, LOW);
  delay(120);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

#if defined(ARDUINO_AVR_MEGA2560)
  // Mega2560 使用 Serial1 与模块通信。
  player.beginHardwareSerial(Serial1);
#else
  // UNO R3 使用 Serial 与模块通信。注意通讯占用硬件串口，移除MP3模块，才可再次上传程序。
  player.beginHardwareSerial(Serial);
#endif

  player.setVolume(20);
}

void loop() {
  // 播放根目录第 9 曲。
  player.playTrack(9);
  blinkOnce();
  delay(1000);

  // 切换到下一曲。
  player.nextTrack();
  blinkOnce();
  delay(2000);

  // 暂停 1 秒。
  player.pause();
  blinkOnce();
  delay(1000);

  // 恢复播放。
  player.play();
  blinkOnce();
  delay(3000);
}
#else
void setup() {}
void loop() {}
#endif
