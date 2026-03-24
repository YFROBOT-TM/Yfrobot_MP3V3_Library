/*
 * 示例名称：03_Board_ESP32_CustomPins
 * 示例功能：演示 ESP32 / ESP32-S3 使用自定义引脚的硬件串口连接模块。
 * 演示内容：
 *   1. 使用 Serial2 和自定义引脚初始化模块
 *   2. 查询当前音量
 *   3. 播放根目录 00010 长音频
 *   4. 周期性读取当前曲目名称
 * 默认引脚：
 *   1. 模块 TX -> GPIO16
 *   2. 模块 RX -> GPIO17
 *   3. 模块 VCC -> 5V
 *   4. 模块 GND -> GND
 * 作者：YFROBOT
 * 日期：2026-03-24
 */

#include <Yfrobot_MP3V3.h>

#if defined(ESP32)
YfrobotMP3V3 player;

static const int MP3_RX_PIN = 16;
static const int MP3_TX_PIN = 17;

void setup() {
  Serial.begin(115200);
  delay(300);

  // 使用 ESP32 硬件串口 Serial2，并指定自定义引脚。
  player.beginHardwareSerial(Serial2, MP3_RX_PIN, MP3_TX_PIN);
  player.setDeviceId(YfrobotMP3V3::kBroadcastDeviceId);

  Serial.println(F("ESP32 自定义引脚示例启动。"));

  // 读取当前音量，用于确认串口通信正常。
  uint8_t volume = 0;
  if (player.readVolume(volume)) {
    Serial.print(F("当前音量："));
    Serial.println(volume);
  } else {
    Serial.println(F("读取音量失败。"));
  }

  // 播放根目录 00010，便于后续试听暂停、插播、EQ 等效果。
  player.playTrack(10);
}

void loop() {
  static uint32_t lastQueryMs = 0;

  if (millis() - lastQueryMs >= 3000) {
    lastQueryMs = millis();

    char trackName[32];
    if (player.readCurrentTrackName(trackName, sizeof(trackName))) {
      Serial.print(F("当前曲目："));
      Serial.println(trackName);
    } else {
      Serial.println(F("当前暂无曲目名称返回。"));
    }
  }
}
#else
void setup() {}
void loop() {}
#endif
