# Yfrobot MP3 V3 Arduino Library

Arduino library for the YFROBOT MP3 Audio Module V3 based on the `CX1000A` chip.

## Features

- Works with `Arduino UNO R3`
- Works with `Arduino Mega2560`
- Works with `ESP32 / ESP32-S3`
- AVR boards support both software serial and hardware serial
- ESP32 boards support hardware serial with custom pins
- Friendly playback APIs such as `playTrack()`, `playPath()` and `insertAdvert()`
- Demo sketches for teaching, product videos and quick showcase videos

## Main API

- `beginHardwareSerial(...)`
- `beginSoftwareSerial(...)`
- `play()`
- `pause()`
- `stop()`
- `previousTrack()`
- `nextTrack()`
- `playTrack(trackNumber)`
- `playPath(storage, path)`
- `insertAdvert(storage, path)`
- `stopAdvert()`
- `setVolume(volume)`
- `readVolume(volume)`
- `readPlayState(state)`
- `readCurrentTrackName(buffer, size)`
- `setPlaybackMode(mode, repeatCount)`
- `setEq(eq)`
- `saveSettings()`
- `restoreFactorySettings()`

## Quick Start

```cpp
#include <Yfrobot_MP3V3.h>

YfrobotMP3V3 player;

void setup() {
  player.beginHardwareSerial(Serial2, 16, 17);
  player.setVolume(20);
  player.playTrack(1);
}

void loop() {
}
```

You can also include the chip-name wrapper:

```cpp
#include <CX1000A.h>

CX1000A player;
```
