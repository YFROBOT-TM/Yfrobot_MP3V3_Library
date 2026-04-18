#ifndef YFROBOT_MP3V3_H
#define YFROBOT_MP3V3_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <Stream.h>

#if defined(ARDUINO_ARCH_AVR)
#include <SoftwareSerial.h>
#define YFMP3V3_HAS_SOFTWARE_SERIAL 1
#else
#define YFMP3V3_HAS_SOFTWARE_SERIAL 0
#endif

/**
 * @brief YFROBOT MP3 V3 串口控制类。
 *
 * 该类用于通过串口协议控制模块播放音频、查询状态、调节音量、
 * 设置播放模式、切换 EQ、组合播放、插播以及保存参数等功能。
 */
class YfrobotMP3V3 {
public:
  /** @brief 协议帧起始字节。 */
  static const uint8_t kStartCode = 0x7E;

  /** @brief 协议帧结束字节。 */
  static const uint8_t kEndCode = 0xEF;

  /** @brief 广播设备地址，默认使用。 */
  static const uint16_t kBroadcastDeviceId = 0xFFFF;

  // /** @brief 默认设备地址。 */
  // static const uint16_t kDefaultDeviceId = 0x0001;

  /** @brief 默认波特率。 */
  static const uint32_t kDefaultBaud = 9600;

  /**
   * @brief 存储设备枚举。
   */
  enum StorageDevice : uint8_t {
    STORAGE_USB = 0x00,
    STORAGE_SD = 0x01,
    STORAGE_FLASH = 0x02,
    STORAGE_NONE = 0xFF
  };

  /**
   * @brief 播放状态枚举。
   */
  enum PlayState : uint8_t {
    PLAY_STATE_STOPPED = 0x00,
    PLAY_STATE_PLAYING = 0x01,
    PLAY_STATE_PAUSED = 0x02,
    PLAY_STATE_UNKNOWN = 0xFF
  };

  /**
   * @brief 播放模式枚举。
   */
  enum PlayMode : uint8_t {
    PLAY_MODE_LOOP_ALL = 0x01,
    PLAY_MODE_LOOP_SINGLE = 0x02,
    PLAY_MODE_LOOP_FOLDER = 0x03,
    PLAY_MODE_LOOP_RANDOM = 0x04,
    PLAY_MODE_STOP_AFTER_ALL = 0x05,
    PLAY_MODE_STOP_AFTER_SINGLE = 0x06,
    PLAY_MODE_STOP_AFTER_RANDOM = 0x07
  };

  /**
   * @brief EQ 模式枚举。
   */
  enum EqPreset : uint8_t {
    EQ_NORMAL = 0x00,
    EQ_ROCK = 0x01,
    EQ_POP = 0x02,
    EQ_CLASSIC = 0x03,
    EQ_JAZZ = 0x04,
    EQ_COUNTRY = 0x05,
    EQ_CUSTOM = 0x06
  };

  /**
   * @brief 播放模式查询结果。
   */
  struct PlaybackModeSettings {
    PlayMode mode;
    uint16_t repeatCount;
  };

  /**
   * @brief 协议帧解析结果。
   */
  struct Frame {
    uint16_t deviceId;
    uint8_t command;
    uint8_t data[96];
    size_t dataLength;
  };

  /**
   * @brief 构造函数。
   * @param 无。
   * @return 无。
   */
  YfrobotMP3V3();

  /**
   * @brief 析构函数。
   * @param 无。
   * @return 无。
   */
  ~YfrobotMP3V3();

  /**
   * @brief 使用硬件串口初始化模块。
   * @param serial 连接模块的硬件串口对象。
   * @param baud 串口波特率，默认 9600。
   * @return `true` 表示初始化成功；`false` 表示初始化失败。
   */
  bool beginHardwareSerial(HardwareSerial &serial, uint32_t baud = kDefaultBaud);

#if defined(ESP32)
  /**
   * @brief 使用 ESP32/ESP32-S3 硬件串口和自定义引脚初始化模块。
   * @param serial 连接模块的硬件串口对象。
   * @param rxPin 模块 TX 连接到开发板的 RX 引脚号。
   * @param txPin 模块 RX 连接到开发板的 TX 引脚号。
   * @param baud 串口波特率，默认 9600。
   * @return `true` 表示初始化成功；`false` 表示初始化失败。
   */
  bool beginHardwareSerial(HardwareSerial &serial,
                           int8_t rxPin,
                           int8_t txPin,
                           uint32_t baud = kDefaultBaud);
#endif

#if YFMP3V3_HAS_SOFTWARE_SERIAL
  /**
   * @brief 使用 AVR 软串口初始化模块。
   * @param rxPin 开发板接收引脚，连接模块 TX。
   * @param txPin 开发板发送引脚，连接模块 RX。
   * @param baud 串口波特率，默认 9600。
   * @return `true` 表示初始化成功；`false` 表示初始化失败。
   */
  bool beginSoftwareSerial(uint8_t rxPin, uint8_t txPin, uint32_t baud = kDefaultBaud);
#endif

  /**
   * @brief 结束当前串口连接并释放资源。
   * @param 无。
   * @return 无。
   */
  void end();

  /**
   * @brief 判断当前是否已经完成串口初始化。
   * @param 无。
   * @return `true` 表示已初始化；`false` 表示未初始化。
   */
  bool isConnected() const;

  /**
   * @brief 判断当前是否使用软串口。
   * @param 无。
   * @return `true` 表示当前使用软串口；`false` 表示当前不是软串口。
   */
  bool isUsingSoftwareSerial() const;

  /**
   * @brief 获取当前串口波特率。
   * @param 无。
   * @return 当前波特率。
   */
  uint32_t baud() const;

  /**
   * @brief 设置目标设备地址。
   * @param deviceId 设备地址，单模块项目建议使用广播地址。
   * @return 无。
   */
  void setDeviceId(uint16_t deviceId);

  /**
   * @brief 获取当前目标设备地址。
   * @param 无。
   * @return 当前目标设备地址。
   */
  uint16_t deviceId() const;

  /**
   * @brief 播放当前曲目。
   * @param 无。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool play();

  /**
   * @brief 暂停当前曲目。
   * @param 无。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool pause();

  /**
   * @brief 停止播放。
   * @param 无。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool stop();

  /**
   * @brief 切换到上一曲。
   * @param 无。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool previousTrack();

  /**
   * @brief 切换到下一曲。
   * @param 无。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool nextTrack();

  /**
   * @brief 指定根目录曲目编号并立即播放。
   * @param trackNumber 曲目编号。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool playTrack(uint16_t trackNumber);

  /**
   * @brief 指定根目录曲目编号但不立即播放。
   * @param trackNumber 曲目编号。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool selectTrack(uint16_t trackNumber);

  /**
   * @brief 指定盘符和路径播放音频。
   * @param storage 目标盘符。
   * @param path ASCII 路径字符串，例如 `/chabo/00001.*`。
   * @return `true` 表示命令发送成功；`false` 表示参数非法或发送失败。
   */
  bool playPath(StorageDevice storage, const char *path);
  bool playPath(const char *path, StorageDevice storage = YfrobotMP3V3::STORAGE_FLASH);

  /**
   * @brief 指定盘符和路径进行插播。
   * @param storage 目标盘符。
   * @param path ASCII 路径字符串，例如 `/chabo/00001.*`。
   * @return `true` 表示命令发送成功；`false` 表示参数非法或发送失败。
   */
  bool insertAdvert(StorageDevice storage, const char *path);
  bool insertAdvert(const char *path, StorageDevice storage = YfrobotMP3V3::STORAGE_FLASH);

  /**
   * @brief 提前结束当前插播。
   * @param 无。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool stopAdvert();

  /**
   * @brief 切换到上一个文件夹并播放。
   * @param 无。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool playPreviousFolder();

  /**
   * @brief 切换到下一个文件夹并播放。
   * @param 无。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool playNextFolder();

  /**
   * @brief 组合播放 `/ZH` 文件夹中的多首两位编号曲目。
   * @param trackList 组合字符串，例如 `"0102"`、`"010203"`。
   * @return `true` 表示命令发送成功；`false` 表示参数非法或发送失败。
   */
  bool playCombination(const char *trackList);

  /**
   * @brief 直接设置音量。
   * @param volume 音量值，范围 0~30，超出范围会自动限制为 30。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool setVolume(uint8_t volume);

  /**
   * @brief 音量加 1。
   * @param 无。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool volumeUp();

  /**
   * @brief 音量减 1。
   * @param 无。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool volumeDown();

  /**
   * @brief 查询当前音量。
   * @param volume 用于返回当前音量的引用变量。
   * @param timeoutMs 查询超时时间，单位毫秒。
   * @return `true` 表示查询成功；`false` 表示超时或解析失败。
   */
  bool readVolume(uint8_t &volume, uint32_t timeoutMs = 300);

  /**
   * @brief 设置播放模式和循环次数。
   * @param mode 播放模式。
   * @param repeatCount 循环次数，0 表示无限循环。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool setPlaybackMode(PlayMode mode, uint16_t repeatCount = 0);

  /**
   * @brief 查询当前播放模式和循环次数。
   * @param settings 用于返回查询结果的结构体。
   * @param timeoutMs 查询超时时间，单位毫秒。
   * @return `true` 表示查询成功；`false` 表示超时或解析失败。
   */
  bool readPlaybackMode(PlaybackModeSettings &settings, uint32_t timeoutMs = 300);

  /**
   * @brief 设置 EQ 模式。
   * @param eq EQ 模式枚举值。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool setEq(EqPreset eq);

  /**
   * @brief 保存当前参数到模块内部。
   * @param 无。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool saveSettings();

  /**
   * @brief 恢复出厂默认参数。
   * @param 无。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool restoreFactorySettings();

  /**
   * @brief 重启模块。
   * @param 无。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool reboot();

  /**
   * @brief 查询当前播放状态。
   * @param state 用于返回播放状态的引用变量。
   * @param timeoutMs 查询超时时间，单位毫秒。
   * @return `true` 表示查询成功；`false` 表示超时或解析失败。
   */
  bool readPlayState(PlayState &state, uint32_t timeoutMs = 300);

  /**
   * @brief 查询当前在线盘符位掩码。
   * @param mask 用于返回在线盘符掩码的引用变量。
   * @param timeoutMs 查询超时时间，单位毫秒。
   * @return `true` 表示查询成功；`false` 表示超时或解析失败。
   */
  bool readOnlineStorageMask(uint8_t &mask, uint32_t timeoutMs = 300);

  /**
   * @brief 查询当前播放盘符。
   * @param storage 用于返回盘符的引用变量。
   * @param timeoutMs 查询超时时间，单位毫秒。
   * @return `true` 表示查询成功；`false` 表示超时或解析失败。
   */
  bool readCurrentStorage(StorageDevice &storage, uint32_t timeoutMs = 300);

  /**
   * @brief 切换当前播放盘符。
   * @param storage 目标盘符。
   * @return `true` 表示命令发送成功；`false` 表示发送失败。
   */
  bool switchStorage(StorageDevice storage);

  /**
   * @brief 查询当前盘符总曲目数。
   * @param trackCount 用于返回总曲目数的引用变量。
   * @param timeoutMs 查询超时时间，单位毫秒。
   * @return `true` 表示查询成功；`false` 表示超时或解析失败。
   */
  bool readTotalTracks(uint16_t &trackCount, uint32_t timeoutMs = 300);

  /**
   * @brief 查询当前文件夹曲目数。
   * @param trackCount 用于返回文件夹曲目数的引用变量。
   * @param timeoutMs 查询超时时间，单位毫秒。
   * @return `true` 表示查询成功；`false` 表示超时或解析失败。
   */
  bool readFolderTrackCount(uint16_t &trackCount, uint32_t timeoutMs = 300);

  /**
   * @brief 查询当前曲目名称。
   * @param buffer 用于接收曲目名称的缓冲区。
   * @param bufferLength 缓冲区长度。
   * @param timeoutMs 查询超时时间，单位毫秒。
   * @return `true` 表示查询成功；`false` 表示超时、参数非法或当前无返回。
   */
  bool readCurrentTrackName(char *buffer, size_t bufferLength, uint32_t timeoutMs = 300);

  /**
   * @brief 查询当前曲目的短文件名。
   * @param buffer 用于接收短文件名的缓冲区。
   * @param bufferLength 缓冲区长度。
   * @param timeoutMs 查询超时时间，单位毫秒。
   * @return `true` 表示查询成功；`false` 表示超时、参数非法或当前无返回。
   */
  bool readShortFileName(char *buffer, size_t bufferLength, uint32_t timeoutMs = 300);

  /**
   * @brief 查询模块设备地址。
   * @param moduleDeviceId 用于返回设备地址的引用变量。
   * @param timeoutMs 查询超时时间，单位毫秒。
   * @return `true` 表示查询成功；`false` 表示超时或解析失败。
   */
  bool readModuleDeviceId(uint16_t &moduleDeviceId, uint32_t timeoutMs = 300);

  /**
   * @brief 查询当前波特率。
   * @param baudRate 用于返回波特率的引用变量。
   * @param timeoutMs 查询超时时间，单位毫秒。
   * @return `true` 表示查询成功；`false` 表示超时或解析失败。
   */
  bool readBaudRate(uint32_t &baudRate, uint32_t timeoutMs = 300);

  /**
   * @brief 发送任意协议命令。
   * @param command 指令码。
   * @param payload 数据区首地址，可为空。
   * @param payloadLength 数据区长度，单位字节。
   * @return `true` 表示命令发送成功；`false` 表示串口未初始化或参数非法。
   */
  bool sendCommand(uint8_t command, const uint8_t *payload = nullptr, size_t payloadLength = 0);

  /**
   * @brief 发送查询命令并等待完整返回帧。
   * @param command 查询指令码。
   * @param frame 用于保存返回帧的结构体。
   * @param timeoutMs 查询超时时间，单位毫秒。
   * @return `true` 表示查询成功；`false` 表示发送失败、超时或解析失败。
   */
  bool queryFrame(uint8_t command, Frame &frame, uint32_t timeoutMs = 300);

private:
  /**
   * @brief 串口传输类型。
   */
  enum TransportMode : uint8_t {
    TRANSPORT_NONE = 0,
    TRANSPORT_HARDWARE = 1,
    TRANSPORT_SOFTWARE = 2
  };

  /** @brief 无数据时的固定帧长度。 */
  static const size_t kFixedFrameBytes = 7;

  /** @brief 最大数据区长度。 */
  static const size_t kMaxPayloadBytes = 96;

  // 不同类型命令在模块内部生效所需的最小间隔，
  // 避免连续发包过快导致部分无返回命令被忽略。
  static const uint16_t kNormalCommandGapMs = 20;
  static const uint16_t kConfigCommandGapMs = 50;
  static const uint16_t kSaveCommandGapMs = 200;
  static const uint16_t kRebootCommandGapMs = 2000;

  Stream *_stream;
  HardwareSerial *_hardwareSerial;
#if YFMP3V3_HAS_SOFTWARE_SERIAL
  SoftwareSerial *_softwareSerial;
#endif
  TransportMode _transportMode;
  uint16_t _deviceId;
  uint32_t _baud;
  // 记录下一条命令允许发送的最早时刻。
  uint32_t _nextCommandReadyAt;

  /**
   * @brief 完成串口对象初始化后的公共收尾动作。
   * @param baud 当前波特率。
   * @return `true` 表示初始化成功；`false` 表示当前流对象为空。
   */
  bool beginWithActiveStream(uint32_t baud);

  // 如果上一条命令仍在冷却期，则等待到可以继续发包。
  void waitCommandGap();
  // 根据命令类型返回发送后的最小间隔。
  uint16_t postCommandGapMs(uint8_t command) const;

  /**
   * @brief 按协议打包并发送完整数据帧。
   * @param command 指令码。
   * @param payload 数据区首地址，可为空。
   * @param payloadLength 数据区长度，单位字节。
   * @return `true` 表示发送成功；`false` 表示发送失败。
   */
  bool sendFrame(uint8_t command, const uint8_t *payload, size_t payloadLength);

  /**
   * @brief 从串口接收并解析一帧返回数据。
   * @param frame 用于保存解析结果的结构体。
   * @param timeoutMs 接收超时时间，单位毫秒。
   * @return `true` 表示成功解析到一帧；`false` 表示超时或校验失败。
   */
  bool receiveFrame(Frame &frame, uint32_t timeoutMs);

  /**
   * @brief 清空串口接收缓冲区。
   * @param 无。
   * @return 无。
   */
  void clearInput();

  /**
   * @brief 计算一帧数据的校验和。
   * @param deviceId 设备地址。
   * @param frameLength 整帧长度。
   * @param command 指令码。
   * @param payload 数据区首地址，可为空。
   * @param payloadLength 数据区长度，单位字节。
   * @return 校验和低 8 位。
   */
  uint8_t checksumForFrame(uint16_t deviceId,
                           uint8_t frameLength,
                           uint8_t command,
                           const uint8_t *payload,
                           size_t payloadLength) const;

  /**
   * @brief 将返回帧中的 ASCII 数据复制到字符串缓冲区。
   * @param frame 已解析完成的返回帧。
   * @param buffer 目标缓冲区。
   * @param bufferLength 缓冲区长度。
   * @return `true` 表示复制成功；`false` 表示参数非法或数据为空。
   */
  bool copyAsciiPayload(const Frame &frame, char *buffer, size_t bufferLength) const;

  /**
   * @brief 发送查询命令并读取两字节无符号整数结果。
   * @param command 查询指令码。
   * @param value 用于返回结果的引用变量。
   * @param timeoutMs 查询超时时间，单位毫秒。
   * @return `true` 表示查询成功；`false` 表示超时或解析失败。
   */
  bool readU16Response(uint8_t command, uint16_t &value, uint32_t timeoutMs);
};

#endif
