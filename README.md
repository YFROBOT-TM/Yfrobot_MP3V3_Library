# Yfrobot MP3 V3 Arduino Library

[English](README.en.md)

适用于 YFROBOT MP3 音频播放模块 V3，芯片为 `CX1000A`，通过串口协议控制播放、查询状态、调节音量和插播语音。

## 特性

- 支持 `Arduino UNO R3`
- 支持 `Arduino Mega2560`
- 支持 `ESP32 / ESP32-S3`
- `UNO R3 / Mega2560` 支持软串口和硬串口
- `ESP32 / ESP32-S3` 支持硬串口自定义引脚
- 提供更直观的播放 API：`playTrack()`、`playPath()`、`insertAdvert()`、`readPlayState()`
- 提供教学和展示型示例，适合课堂、短视频和卖货演示

## 安装

将整个库文件夹放到：

`Documents/Arduino/libraries/Yfrobot_MP3V3_Library`

然后重启 Arduino IDE。

## 协议摘要

- 波特率：`9600`
- 数据位：`8`
- 停止位：`1`
- 校验位：`N`
- 帧头：`0x7E`
- 帧尾：`0xEF`
- 默认设备 ID：`0x0001`
- 推荐广播地址：`0xFFFF`

## 快速开始

### UNO R3 / Mega2560 软串口

```cpp
#include <Yfrobot_MP3V3.h>

YfrobotMP3V3 player;

void setup() {
  Serial.begin(115200);
  player.beginSoftwareSerial(11, 10);  // RX, TX
  player.setDeviceId(YfrobotMP3V3::kBroadcastDeviceId);
  player.setVolume(20);
  player.playTrack(1);
}

void loop() {
}
```

### Mega2560 硬串口

```cpp
#include <Yfrobot_MP3V3.h>

YfrobotMP3V3 player;

void setup() {
  player.beginHardwareSerial(Serial1);
  player.playPath(YfrobotMP3V3::STORAGE_FLASH, "/demo/00001.*");
}

void loop() {
}
```

### ESP32 / ESP32-S3 自定义引脚

```cpp
#include <Yfrobot_MP3V3.h>

YfrobotMP3V3 player;

void setup() {
  Serial.begin(115200);
  player.beginHardwareSerial(Serial2, 16, 17);  // RX, TX
  player.playTrack(1);
}

void loop() {
}
```

## 推荐 API

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

## 示例说明

- `BasicControl_SerialMonitor`
  通过串口监视器输入命令，适合教学和调试。
- `AutoShowcase`
  自动依次演示播放、切歌、插播、暂停与恢复，适合拍视频和卖货展示。
- `AVR_SoftwareSerial`
  演示 UNO / Mega2560 软串口接法。
- `AVR_HardwareSerial`
  演示 UNO / Mega2560 硬串口接法。
- `ESP32_CustomPins`
  演示 ESP32 / ESP32-S3 的自定义引脚硬串口。

## 路径格式

- 指定路径播放：`/demo/00001.*`
- 插播路径：`/ad/00001.*`
- 盘符：
  - `STORAGE_USB`
  - `STORAGE_SD`
  - `STORAGE_FLASH`

说明：

- 路径使用 ASCII 字符串
- 数据高字节在前，遵循模块协议
- 文档说明该 V3 模块默认支持 `FLASH`

## 兼容头文件

如果你希望按芯片名包含头文件，也可以这样写：

```cpp
#include <CX1000A.h>

CX1000A player;
```

## License

MIT License，见 `extras/LICENSE`。
