# Yfrobot MP3 V3 Arduino Library [![中文版本](https://img.shields.io/badge/-中文-red)](README.md)

## Language Switch
- [English](README.en.md)
- [中文 (Chinese)](README.md)

## 1. Overview

This library is used to control the YFROBOT MP3 V3 module through the UART protocol, and it supports:

- Arduino UNO R3
- Arduino Mega2560
- ESP32
- ESP32-S3

Supported connection modes:

- UNO R3 / Mega2560: software serial and hardware serial
- ESP32 / ESP32-S3: hardware serial with custom pins

This library is suitable for:

- teaching demos
- product showcase demos
- video demonstrations of audio effects
- quickly integrating UART audio control into projects

## 2. Preloaded Audio Tree

The module is preloaded from the factory with the following audio resources:

- Root directory: `00001` ~ `00010`
- `ZH` combination playback folder: `01.mp3` ~ `03.mp3`
- `chabo` folder: `00001.mp3` ~ `00008.mp3`

The example sketches are designed around this default audio tree as much as possible, so they can be demonstrated directly after uploading.

## 3. UART Protocol Basics

- Baud rate: `9600`
- Data bits: `8`
- Stop bits: `1`
- Parity: `N`
- Frame header: `0x7E`
- Frame end: `0xEF`
- Default device ID: `0x0001`
- Recommended broadcast address: `0xFFFF`

## 4. Installation

Place the entire library folder into:

`Documents/Arduino/libraries/Yfrobot_MP3V3_Library`

Then reopen Arduino IDE.

## 5. Quick Start

### 5.1 UNO R3 / Mega2560 Software Serial

```cpp
#include <Yfrobot_MP3V3.h>

YfrobotMP3V3 player;

void setup() {
  Serial.begin(115200);
  player.beginSoftwareSerial(11, 10);  // RX, TX
  player.setVolume(25);
  player.playTrack(10);
}

void loop() {
}
```

### 5.2 Mega2560 Hardware Serial

```cpp
#include <Yfrobot_MP3V3.h>

YfrobotMP3V3 player;

void setup() {
  player.beginHardwareSerial(Serial1);
  player.playTrack(10);
}

void loop() {
}
```

### 5.3 ESP32 / ESP32-S3 Custom Pins

```cpp
#include <Yfrobot_MP3V3.h>

YfrobotMP3V3 player;

void setup() {
  Serial.begin(115200);
  player.beginHardwareSerial(Serial2, 16, 17);  // RX, TX
  player.playTrack(10);
}

void loop() {
}
```

## 6. API Description

### 6.1 Initialization and Connection

- `beginHardwareSerial(HardwareSerial &serial, uint32_t baud = 9600)`
  Initializes the module with a hardware serial port. Suitable for Mega2560, ESP32, ESP32-S3, and also for the UNO R3 hardware serial solution.

- `beginHardwareSerial(HardwareSerial &serial, int8_t rxPin, int8_t txPin, uint32_t baud = 9600)`
  Only for ESP32 / ESP32-S3. Supports custom serial pins.

- `beginSoftwareSerial(uint8_t rxPin, uint8_t txPin, uint32_t baud = 9600)`
  Only for AVR platforms. Initializes the module with software serial, suitable for UNO R3 and Mega2560.

- `end()`
  Closes the current serial connection and releases internal resources.

- `isConnected()`
  Checks whether the library has finished serial initialization.

- `isUsingSoftwareSerial()`
  Checks whether the current mode is software serial.

- `baud()`
  Gets the current serial baud rate.

### 6.2 Device Address
- `deviceId()`
  Reads the currently configured target device address.

### 6.3 Playback Control

- `play()`
  Plays the current track.

- `pause()`
  Pauses the current track.

- `stop()`
  Stops current playback.

- `previousTrack()`
  Switches to the previous track.

- `nextTrack()`
  Switches to the next track.

- `playTrack(uint16_t trackNumber)`
  Plays a root directory track immediately, for example `00010`.

- `selectTrack(uint16_t trackNumber)`
  Selects a root directory track without playing it immediately. You can call `play()` afterward.

- `playPath(StorageDevice storage, const char *path)`
  Plays a file by storage and path, for example:
  `playPath(STORAGE_FLASH, "/chabo/00001.*")`

- `insertAdvert(StorageDevice storage, const char *path)`
  Starts insert playback by storage and path. After the insert track finishes, playback returns to the previous state.

- `stopAdvert()`
  Ends the current insert playback in advance.

- `playPreviousFolder()`
  Switches to the previous folder and starts playback.

- `playNextFolder()`
  Switches to the next folder and starts playback.

- `playCombination(const char *trackList)`
  Plays a combination of two-digit numbered tracks in the `/ZH` folder. For example:
  `"0102"` means play 01 + 02,
  `"010203"` means play 01 + 02 + 03.

### 6.4 Volume and EQ

- `setVolume(uint8_t volume)`
  Sets the volume directly. Range: `0 ~ 30`.

- `volumeUp()`
  Increases the volume by 1.

- `volumeDown()`
  Decreases the volume by 1.

- `readVolume(uint8_t &volume, uint32_t timeoutMs = 300)`
  Reads the current volume.

- `setEq(EqPreset eq)`
  Sets the EQ mode. Available values:
  `EQ_NORMAL`, `EQ_ROCK`, `EQ_POP`, `EQ_CLASSIC`, `EQ_JAZZ`, `EQ_COUNTRY`, `EQ_CUSTOM`

### 6.5 Playback Mode and Parameter Saving

- `setPlaybackMode(PlayMode mode, uint16_t repeatCount = 0)`
  Sets the playback mode and repeat count. `repeatCount = 0` means infinite repeat.

- `readPlaybackMode(PlaybackModeSettings &settings, uint32_t timeoutMs = 300)`
  Reads the current playback mode and repeat count.

- `saveSettings()`
  Saves the current settings into the module.

- `restoreFactorySettings()`
  Restores the factory default settings.

- `reboot()`
  Reboots the module.

### 6.6 Query Interfaces

- `readPlayState(PlayState &state, uint32_t timeoutMs = 300)`
  Reads the current playback state.

- `readOnlineStorageMask(uint8_t &mask, uint32_t timeoutMs = 300)`
  Reads the bit mask of currently online storage devices.

- `readCurrentStorage(StorageDevice &storage, uint32_t timeoutMs = 300)`
  Reads the currently active playback storage device.

- `switchStorage(StorageDevice storage)`
  Switches the current playback storage device.

- `readTotalTracks(uint16_t &trackCount, uint32_t timeoutMs = 300)`
  Reads the total number of tracks on the current storage device.

- `readFolderTrackCount(uint16_t &trackCount, uint32_t timeoutMs = 300)`
  Reads the number of tracks in the current folder.

- `readCurrentTrackName(char *buffer, size_t bufferLength, uint32_t timeoutMs = 300)`
  Reads the current track file name.

- `readShortFileName(char *buffer, size_t bufferLength, uint32_t timeoutMs = 300)`
  Reads the current track short file name.

- `readModuleDeviceId(uint16_t &moduleDeviceId, uint32_t timeoutMs = 300)`
  Reads the module device address.

- `readBaudRate(uint32_t &baudRate, uint32_t timeoutMs = 300)`
  Reads the current module baud rate.

### 6.7 Advanced Interfaces

- `sendCommand(uint8_t command, const uint8_t *payload = nullptr, size_t payloadLength = 0)`
  Sends any protocol command. Suitable for advanced users extending functionality.

- `queryFrame(uint8_t command, Frame &frame, uint32_t timeoutMs = 300)`
  Sends a query command and directly gets the full response frame.

## 7. Common Enumerations

### 7.1 Storage Device Enumeration

- `STORAGE_USB`
- `STORAGE_SD`
- `STORAGE_FLASH`

### 7.2 Playback State Enumeration

- `PLAY_STATE_STOPPED`
- `PLAY_STATE_PLAYING`
- `PLAY_STATE_PAUSED`

### 7.3 Playback Mode Enumeration

- `PLAY_MODE_LOOP_ALL`
- `PLAY_MODE_LOOP_SINGLE`
- `PLAY_MODE_LOOP_FOLDER`
- `PLAY_MODE_LOOP_RANDOM`
- `PLAY_MODE_STOP_AFTER_ALL`
- `PLAY_MODE_STOP_AFTER_SINGLE`
- `PLAY_MODE_STOP_AFTER_RANDOM`

### 7.4 EQ Enumeration

- `EQ_NORMAL`
- `EQ_ROCK`
- `EQ_POP`
- `EQ_CLASSIC`
- `EQ_JAZZ`
- `EQ_COUNTRY`
- `EQ_CUSTOM`

## 8. Example Sketches

- `01_Board_AVR_SoftwareSerial`
  Basic AVR software serial wiring example for UNO R3 / Mega2560.

- `02_Board_AVR_HardwareSerial`
  Basic AVR hardware serial wiring example for UNO R3 / Mega2560.

- `03_Board_ESP32_CustomPins`
  ESP32 / ESP32-S3 custom pin wiring example.

- `10_Playback_Control_Demo`
  Demonstrates playback, pause, stop, previous track, next track, and select-without-play operations.

- `20_Advert_Path_Demo`
  Demonstrates path playback, insert playback, stop insert playback, previous folder playback, and next folder playback.

- `30_Volume_EQ_Demo`
  Demonstrates direct volume setting, volume up/down, and EQ listening comparison.

- `40_PlayMode_Save_Demo`
  Demonstrates playback mode setting, repeat count setting, reading playback mode, saving settings, and restoring default settings.

- `50_Combination_Reboot_Demo`
  Demonstrates combination playback for `01 02`, `01 02 03`, and module reboot.

- `90_SerialMonitor_AllFunctions`
  Interactive control through the serial monitor, suitable for teaching, debugging, and product demo videos.

## 9. Default Paths Used in the Examples

- Long root track: `00010`
- Combination playback: `/ZH/01.mp3`, `/ZH/02.mp3`, `/ZH/03.mp3`
- Insert playback example: `/chabo/00001.*`
- Path playback example: `/chabo/00002.*`

## 10. License

MIT License. See `extras/LICENSE` for details.
