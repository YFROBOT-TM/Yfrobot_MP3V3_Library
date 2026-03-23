#include <Yfrobot_MP3V3.h>

#if defined(ESP32)
YfrobotMP3V3 player;

static const int MP3_RX_PIN = 16;
static const int MP3_TX_PIN = 17;

void setup() {
  Serial.begin(115200);
  delay(300);

  player.beginHardwareSerial(Serial2, MP3_RX_PIN, MP3_TX_PIN);
  player.setDeviceId(YfrobotMP3V3::kBroadcastDeviceId);

  Serial.println(F("ESP32 custom pins demo"));
  Serial.println(F("Module TX -> GPIO16, Module RX -> GPIO17"));

  uint8_t volume = 0;
  if (player.readVolume(volume)) {
    Serial.print(F("Current volume: "));
    Serial.println(volume);
  } else {
    Serial.println(F("Volume query failed."));
  }

  player.playPath(YfrobotMP3V3::STORAGE_FLASH, "/demo/00001.*");
}

void loop() {
  static uint32_t lastQuery = 0;

  if (millis() - lastQuery < 2000) {
    return;
  }

  lastQuery = millis();

  YfrobotMP3V3::PlayState state;
  if (player.readPlayState(state)) {
    Serial.print(F("Play state: "));
    Serial.println(static_cast<uint8_t>(state));
  } else {
    Serial.println(F("Play state query failed."));
  }
}
#else
void setup() {}
void loop() {}
#endif
