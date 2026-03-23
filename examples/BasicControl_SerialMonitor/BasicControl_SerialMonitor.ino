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

static void printMenu() {
  Serial.println(F("=== YFROBOT MP3 V3 Control Menu ==="));
  Serial.println(F("p play"));
  Serial.println(F("u pause"));
  Serial.println(F("x stop"));
  Serial.println(F("n next track"));
  Serial.println(F("m previous track"));
  Serial.println(F("1 play track 1"));
  Serial.println(F("2 play track 2"));
  Serial.println(F("f play /demo/00001.* from Flash"));
  Serial.println(F("r insert advert /ad/00001.*"));
  Serial.println(F("e stop advert"));
  Serial.println(F("+ volume up"));
  Serial.println(F("- volume down"));
  Serial.println(F("v set volume 15"));
  Serial.println(F("q query state/volume/name"));
  Serial.println(F("h print this help"));
}

void setup() {
  Serial.begin(115200);
  delay(300);

  beginPlayer();
  player.setDeviceId(YfrobotMP3V3::kBroadcastDeviceId);

  printMenu();
}

void loop() {
  if (!Serial.available()) {
    return;
  }

  const char cmd = static_cast<char>(Serial.read());

  if (cmd == 'p') player.play();
  if (cmd == 'u') player.pause();
  if (cmd == 'x') player.stop();
  if (cmd == 'n') player.nextTrack();
  if (cmd == 'm') player.previousTrack();
  if (cmd == '1') player.playTrack(1);
  if (cmd == '2') player.playTrack(2);
  if (cmd == 'f') player.playPath(YfrobotMP3V3::STORAGE_FLASH, "/demo/00001.*");
  if (cmd == 'r') player.insertAdvert(YfrobotMP3V3::STORAGE_FLASH, "/ad/00001.*");
  if (cmd == 'e') player.stopAdvert();
  if (cmd == '+') player.volumeUp();
  if (cmd == '-') player.volumeDown();
  if (cmd == 'v') player.setVolume(15);
  if (cmd == 'h') printMenu();

  if (cmd == 'q') {
    YfrobotMP3V3::PlayState state;
    uint8_t volume = 0;
    char trackName[32];

    if (player.readPlayState(state)) {
      Serial.print(F("State: "));
      Serial.println(static_cast<uint8_t>(state));
    } else {
      Serial.println(F("State query failed."));
    }

    if (player.readVolume(volume)) {
      Serial.print(F("Volume: "));
      Serial.println(volume);
    } else {
      Serial.println(F("Volume query failed."));
    }

    if (player.readCurrentTrackName(trackName, sizeof(trackName))) {
      Serial.print(F("Track: "));
      Serial.println(trackName);
    } else {
      Serial.println(F("Track name query failed."));
    }
  }
}
