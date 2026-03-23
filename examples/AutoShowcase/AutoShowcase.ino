#include <Yfrobot_MP3V3.h>

YfrobotMP3V3 player;

static void beginPlayer() {
#if defined(ESP32)
  player.beginHardwareSerial(Serial2, 16, 17);
#elif defined(ARDUINO_AVR_MEGA2560)
  player.beginHardwareSerial(Serial1);
#elif defined(ARDUINO_ARCH_AVR)
  player.beginSoftwareSerial(11, 10);
#else
  player.beginHardwareSerial(Serial1);
#endif
}

void setup() {
  Serial.begin(115200);
  delay(300);

  beginPlayer();
  player.setVolume(22);
  player.setPlaybackMode(YfrobotMP3V3::PLAY_MODE_STOP_AFTER_SINGLE);

  Serial.println(F("Auto showcase started."));
}

void loop() {
  Serial.println(F("1) play track 1"));
  player.playTrack(1);
  delay(4000);

  Serial.println(F("2) volume down"));
  player.volumeDown();
  delay(1000);

  Serial.println(F("3) next track"));
  player.nextTrack();
  delay(4000);

  Serial.println(F("4) insert advert"));
  player.insertAdvert(YfrobotMP3V3::STORAGE_FLASH, "/ad/00001.*");
  delay(3000);

  Serial.println(F("5) stop advert and resume"));
  player.stopAdvert();
  delay(2000);

  Serial.println(F("6) pause / resume"));
  player.pause();
  delay(1000);
  player.play();
  delay(3000);

  Serial.println(F("7) stop"));
  player.stop();
  delay(2000);
}
