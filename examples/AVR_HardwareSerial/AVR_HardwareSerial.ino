#include <Yfrobot_MP3V3.h>

#if defined(ARDUINO_ARCH_AVR)
YfrobotMP3V3 player;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

#if defined(ARDUINO_AVR_MEGA2560)
  player.beginHardwareSerial(Serial1);
#else
  player.beginHardwareSerial(Serial);
#endif

  player.setVolume(20);
}

static void blinkOnce() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(120);
  digitalWrite(LED_BUILTIN, LOW);
  delay(120);
}

void loop() {
  player.playTrack(1);
  blinkOnce();
  delay(3000);

  player.nextTrack();
  blinkOnce();
  delay(3000);

  player.pause();
  blinkOnce();
  delay(1000);

  player.play();
  blinkOnce();
  delay(3000);
}
#else
void setup() {}
void loop() {}
#endif
