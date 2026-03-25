# Yfrobot MP3 V3 Arduino Library [![English Version](https://img.shields.io/badge/-English-blue)](README.en.md)

## Language Switch
- [English](README.en.md)
- [中文 (Chinese)](README.md)

## 1. 库简介

本库用于通过串口协议控制 YFROBOT MP3 V3 模块，适用于：

- Arduino UNO R3
- Arduino Mega2560
- ESP32
- ESP32-S3

兼容方式如下：

- UNO R3 / Mega2560：支持软串口、硬件串口
- ESP32 / ESP32-S3：支持硬件串口，支持自定义引脚

本库适合以下场景：

- 教学演示
- 产品卖货演示
- 拍视频展示音频效果
- 项目中快速接入串口控制音频播放

## 2. 模块预存音频树

出厂预存音频资源如下：

- 根目录：`00001` ~ `00010`
- `ZH` 组合播放文件夹：`01.mp3` ~ `03.mp3`
- `chabo` 文件夹：`00001.mp3` ~ `00008.mp3`

示例程序已尽量基于这套默认音频树设计，下载后即可直接演示。

## 3. 串口协议基础参数

- 波特率：`9600`
- 数据位：`8`
- 停止位：`1`
- 校验位：`N`
- 帧头：`0x7E`
- 帧尾：`0xEF`
- 默认设备地址：`0x0001`
- 推荐广播地址：`0xFFFF`

## 4. 安装方法

将整个库文件夹放入：

`Documents/Arduino/libraries/Yfrobot_MP3V3_Library`

然后重新打开 Arduino IDE。

## 5. 快速开始

### 5.1 UNO R3 / Mega2560 软串口

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

### 5.2 Mega2560 硬件串口

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

### 5.3 ESP32 / ESP32-S3 自定义引脚

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

## 6. API 中文说明

### 6.1 初始化与连接

- `beginHardwareSerial(HardwareSerial &serial, uint32_t baud = 9600)`
  使用硬件串口初始化模块，适合 Mega2560、ESP32、ESP32-S3，也可用于 UNO R3 的硬件串口方案。

- `beginHardwareSerial(HardwareSerial &serial, int8_t rxPin, int8_t txPin, uint32_t baud = 9600)`
  仅用于 ESP32 / ESP32-S3，支持自定义串口引脚。

- `beginSoftwareSerial(uint8_t rxPin, uint8_t txPin, uint32_t baud = 9600)`
  仅用于 AVR 平台，使用软串口初始化模块，适合 UNO R3 和 Mega2560。

- `end()`
  关闭当前串口连接并释放内部资源。

- `isConnected()`
  判断库是否已经完成串口初始化。

- `isUsingSoftwareSerial()`
  判断当前是否处于软串口模式。

- `baud()`
  获取当前串口波特率。

### 6.2 设备地址
- `deviceId()`
  读取当前设置的目标设备地址。

### 6.3 播放控制

- `play()`
  播放当前曲目。

- `pause()`
  暂停当前曲目。

- `stop()`
  停止当前播放。

- `previousTrack()`
  切换到上一曲。

- `nextTrack()`
  切换到下一曲。

- `playTrack(uint16_t trackNumber)`
  指定根目录曲目编号并立即播放，例如播放 `00010`。

- `selectTrack(uint16_t trackNumber)`
  指定根目录曲目编号但不立即播放，随后可再调用 `play()`。

- `playPath(const char *path, StorageDevice storage = STORAGE_FLASH)`
  指定盘符和路径播放，例如：
  `playPath(STORAGE_FLASH, "/chabo/00001.*")`

- `insertAdvert(const char *path, StorageDevice storage = STORAGE_FLASH)`
  指定盘符和路径进行插播，插播完成后会回到原来的播放状态。

- `stopAdvert()`
  提前结束当前插播。

- `playPreviousFolder()`
  切换到上一文件夹并播放。

- `playNextFolder()`
  切换到下一文件夹并播放。

- `playCombination(const char *trackList)`
  组合播放 `/ZH` 文件夹中的两位编号曲目，例如：
  `"0102"` 表示播放 01 + 02，
  `"010203"` 表示播放 01 + 02 + 03。

### 6.4 音量与 EQ

- `setVolume(uint8_t volume)`
  直接设置音量，范围 `0 ~ 30`。

- `volumeUp()`
  音量加 1。

- `volumeDown()`
  音量减 1。

- `readVolume(uint8_t &volume, uint32_t timeoutMs = 300)`
  查询当前音量。

- `setEq(EqPreset eq)`
  设置 EQ 模式，可选：
  `EQ_NORMAL`、`EQ_ROCK`、`EQ_POP`、`EQ_CLASSIC`、`EQ_JAZZ`、`EQ_COUNTRY`、`EQ_CUSTOM`

### 6.5 播放模式与参数保存

- `setPlaybackMode(PlayMode mode, uint16_t repeatCount = 0)`
  设置播放模式与循环次数。`repeatCount = 0` 表示无限循环。

- `readPlaybackMode(PlaybackModeSettings &settings, uint32_t timeoutMs = 300)`
  读取当前播放模式与循环次数。

- `saveSettings()`
  保存当前参数到模块内部。

- `restoreFactorySettings()`
  恢复模块出厂默认参数。

- `reboot()`
  重启模块。

### 6.6 查询接口

- `readPlayState(PlayState &state, uint32_t timeoutMs = 300)`
  查询当前播放状态。

- `readOnlineStorageMask(uint8_t &mask, uint32_t timeoutMs = 300)`
  查询当前在线盘符位掩码。

- `readCurrentStorage(StorageDevice &storage, uint32_t timeoutMs = 300)`
  查询当前播放盘符。

- `switchStorage(StorageDevice storage)`
  切换当前播放盘符。

- `readTotalTracks(uint16_t &trackCount, uint32_t timeoutMs = 300)`
  读取当前盘符总曲目数。

- `readFolderTrackCount(uint16_t &trackCount, uint32_t timeoutMs = 300)`
  读取当前文件夹曲目数。

- `readCurrentTrackName(char *buffer, size_t bufferLength, uint32_t timeoutMs = 300)`
  读取当前曲目文件名。

- `readShortFileName(char *buffer, size_t bufferLength, uint32_t timeoutMs = 300)`
  读取当前曲目短文件名。

- `readModuleDeviceId(uint16_t &moduleDeviceId, uint32_t timeoutMs = 300)`
  读取模块设备地址。

- `readBaudRate(uint32_t &baudRate, uint32_t timeoutMs = 300)`
  读取模块当前波特率。

### 6.7 高级接口

- `sendCommand(uint8_t command, const uint8_t *payload = nullptr, size_t payloadLength = 0)`
  发送任意协议命令，适合高级用户扩展使用。

- `queryFrame(uint8_t command, Frame &frame, uint32_t timeoutMs = 300)`
  发送查询命令并直接获取完整返回帧。

## 7. 常用枚举说明

### 7.1 盘符枚举

- `STORAGE_USB`
- `STORAGE_SD`
- `STORAGE_FLASH`

### 7.2 播放状态枚举

- `PLAY_STATE_STOPPED`
- `PLAY_STATE_PLAYING`
- `PLAY_STATE_PAUSED`

### 7.3 播放模式枚举

- `PLAY_MODE_LOOP_ALL`
- `PLAY_MODE_LOOP_SINGLE`
- `PLAY_MODE_LOOP_FOLDER`
- `PLAY_MODE_LOOP_RANDOM`
- `PLAY_MODE_STOP_AFTER_ALL`
- `PLAY_MODE_STOP_AFTER_SINGLE`
- `PLAY_MODE_STOP_AFTER_RANDOM`

### 7.4 EQ 枚举

- `EQ_NORMAL`
- `EQ_ROCK`
- `EQ_POP`
- `EQ_CLASSIC`
- `EQ_JAZZ`
- `EQ_COUNTRY`
- `EQ_CUSTOM`

## 8. 示例程序说明

- `01_Board_AVR_SoftwareSerial`
  AVR 软串口基础接线示例，适合 UNO R3 / Mega2560。

- `02_Board_AVR_HardwareSerial`
  AVR 硬件串口基础接线示例，适合 UNO R3 / Mega2560。

- `03_Board_ESP32_CustomPins`
  ESP32 / ESP32-S3 自定义引脚接线示例。

- `10_Playback_Control_Demo`
  演示播放、暂停、停止、上一曲、下一曲、选曲不播放等核心播放控制。

- `20_Advert_Path_Demo`
  演示指定路径播放、指定路径插播、结束插播、上一文件夹播放、下一文件夹播放。

- `30_Volume_EQ_Demo`
  演示设置音量、音量加减、不同 EQ 模式试听。

- `40_PlayMode_Save_Demo`
  演示播放模式设置、循环次数设置、读取播放模式、保存参数、恢复默认参数。

- `50_Combination_Reboot_Demo`
  演示组合播放 `01 02`、`01 02 03` 以及重启模块。

- `90_SerialMonitor_AllFunctions`
  通过串口监视器交互式控制，适合教学、调试和拍演示视频。

## 9. 示例中用到的默认路径

- 根目录长音频：`00010`
- 组合播放：`/ZH/01.mp3`、`/ZH/02.mp3`、`/ZH/03.mp3`
- 插播示例：`/chabo/00001.*`
- 路径播放示例：`/chabo/00002.*`

## 10. License

MIT License，详见 `extras/LICENSE`。
