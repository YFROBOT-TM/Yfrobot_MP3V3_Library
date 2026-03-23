#include <Yfrobot_MP3V3.h>

#if defined(ARDUINO_ARCH_AVR)
YfrobotMP3V3 player;

static void printHelp() {
  Serial.println(F("Commands:"));
  Serial.println(F("  p = play"));
  Serial.println(F("  a = pause"));
  Serial.println(F("  s = stop"));
  Serial.println(F("  n = next"));
  Serial.println(F("  b = previous"));
  Serial.println(F("  + = volume up"));
  Serial.println(F("  - = volume down"));
  Serial.println(F("  1 = play track 1"));
  Serial.println(F("  2 = play track 2"));
  Serial.println(F("  i = insert advert /ad/00001.*"));
  Serial.println(F("  q = query state and volume"));
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  if (!player.beginSoftwareSerial(11, 10)) {
    Serial.println(F("SoftwareSerial init failed."));
    while (true) {
      delay(1000);
    }
  }

  player.setDeviceId(YfrobotMP3V3::kBroadcastDeviceId);

  Serial.println(F("CX1000A MP3 V3 - AVR software serial demo"));
  Serial.println(F("RX <- module TX on D11, TX -> module RX on D10"));
  printHelp();
}

void loop() {
  if (!Serial.available()) {
    return;
  }

  const char cmd = static_cast<char>(Serial.read());

  if (cmd == 'p') player.play();
  if (cmd == 'a') player.pause();
  if (cmd == 's') player.stop();
  if (cmd == 'n') player.nextTrack();
  if (cmd == 'b') player.previousTrack();
  if (cmd == '+') player.volumeUp();
  if (cmd == '-') player.volumeDown();
  if (cmd == '1') player.playTrack(1);
  if (cmd == '2') player.playTrack(2);
  if (cmd == 'i') player.insertAdvert(YfrobotMP3V3::STORAGE_FLASH, "/ad/00001.*");

  if (cmd == 'q') {
    YfrobotMP3V3::PlayState state;
    uint8_t volume = 0;

    if (player.readPlayState(state)) {
      Serial.print(F("Play state = "));
      Serial.println(static_cast<uint8_t>(state));
    } else {
      Serial.println(F("Play state query failed."));
    }

    if (player.readVolume(volume)) {
      Serial.print(F("Volume = "));
      Serial.println(volume);
    } else {
      Serial.println(F("Volume query failed."));
    }
  }
}
#else
void setup() {}
void loop() {}
#endif
