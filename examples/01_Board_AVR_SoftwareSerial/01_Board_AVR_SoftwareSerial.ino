/*
 * 示例名称：01_Board_AVR_SoftwareSerial
 * 示例功能：演示 UNO R3 / Mega2560 使用软串口连接模块的基础写法。
 * 演示内容：
 *   1. 初始化软串口
 *   2. 查询当前音量
 *   3. 播放根目录 00001
 *   4. 周期性查询播放状态
 * 接线说明：
 *   1. 模块 TX -> Arduino D11
 *   2. 模块 RX -> Arduino D10
 *   3. 模块 VCC -> 5V
 *   4. 模块 GND -> GND
 * 作者：YFROBOT
 * 日期：2026-03-24
 */

#include <Yfrobot_MP3V3.h>

#if defined(ARDUINO_ARCH_AVR)
YfrobotMP3V3 player;

void setup() {
  Serial.begin(115200);
  delay(300);

  // 使用软串口初始化模块，D11 作为接收，D10 作为发送。
  if (!player.beginSoftwareSerial(11, 10)) {
    Serial.println(F("软串口初始化失败。"));
    while (true) {
      delay(1000);
    }
  }

  // 使用广播地址，便于直接控制单个模块。
  player.setDeviceId(YfrobotMP3V3::kBroadcastDeviceId);

  Serial.println(F("AVR 软串口示例启动。"));

  // 查询当前音量，确认通信正常。
  uint8_t volume = 0;
  if (player.readVolume(volume)) {
    Serial.print(F("当前音量："));
    Serial.println(volume);
  } else {
    Serial.println(F("读取音量失败。"));
  }

  // 播放根目录下的 00001 曲目。
  player.playTrack(1);
}

void loop() {
  static uint32_t lastQueryMs = 0;

  // 每 2 秒查询一次播放状态，方便观察模块响应。
  if (millis() - lastQueryMs >= 2000) {
    lastQueryMs = millis();

    YfrobotMP3V3::PlayState state;
    if (player.readPlayState(state)) {
      Serial.print(F("当前播放状态："));
      Serial.println(static_cast<uint8_t>(state));
    } else {
      Serial.println(F("读取播放状态失败。"));
    }
  }
}
#else
void setup() {}
void loop() {}
#endif
