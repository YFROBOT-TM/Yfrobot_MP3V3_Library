#include "Yfrobot_MP3V3.h"

namespace {
static const uint8_t CMD_QUERY_PLAY_STATE = 0x01;         // 查询当前播放状态
static const uint8_t CMD_PLAY = 0x02;                     // 播放当前曲目
static const uint8_t CMD_PAUSE = 0x03;                    // 暂停当前播放
static const uint8_t CMD_STOP = 0x04;                     // 停止当前播放
static const uint8_t CMD_PREVIOUS_TRACK = 0x05;           // 切换到上一曲
static const uint8_t CMD_NEXT_TRACK = 0x06;               // 切换到下一曲
static const uint8_t CMD_PLAY_TRACK = 0x07;               // 指定根目录曲目并播放
static const uint8_t CMD_PLAY_PATH = 0x08;                // 指定路径播放
static const uint8_t CMD_QUERY_TOTAL_TRACKS = 0x0C;       // 查询当前盘符总曲目数
static const uint8_t CMD_QUERY_CURRENT_TRACK_NAME = 0x0D; // 查询当前曲目名称
static const uint8_t CMD_PREVIOUS_FOLDER = 0x0E;          // 切换到上一文件夹播放
static const uint8_t CMD_NEXT_FOLDER = 0x0F;              // 切换到下一文件夹播放
static const uint8_t CMD_STOP_ADVERT = 0x10;              // 结束插播
static const uint8_t CMD_QUERY_FOLDER_TRACKS = 0x12;      // 查询当前文件夹曲目数
static const uint8_t CMD_SET_VOLUME = 0x13;               // 直接设置音量
static const uint8_t CMD_VOLUME_UP = 0x14;                // 音量加 1
static const uint8_t CMD_VOLUME_DOWN = 0x15;              // 音量减 1
static const uint8_t CMD_QUERY_VOLUME = 0x16;             // 查询当前音量
static const uint8_t CMD_INSERT_ADVERT = 0x17;            // 指定路径插播
static const uint8_t CMD_SET_PLAYBACK_MODE = 0x18;        // 设置播放模式和循环次数
static const uint8_t CMD_QUERY_PLAYBACK_MODE = 0x19;      // 查询播放模式和循环次数
static const uint8_t CMD_SET_EQ = 0x1A;                   // 设置 EQ 模式
static const uint8_t CMD_PLAY_COMBINATION = 0x1B;         // 组合播放
static const uint8_t CMD_SELECT_TRACK = 0x1F;             // 选曲但不立即播放
static const uint8_t CMD_SAVE_SETTINGS = 0x32;            // 保存当前参数
static const uint8_t CMD_REBOOT = 0x34;                   // 重启模块

/*======== 以下指令无用 暂保留 ========*/
static const uint8_t CMD_QUERY_ONLINE_STORAGE = 0x09;     // 查询当前在线盘符
static const uint8_t CMD_QUERY_CURRENT_STORAGE = 0x0A;    // 查询当前播放盘符
static const uint8_t CMD_SWITCH_STORAGE = 0x0B;           // 切换到指定盘符
static const uint8_t CMD_READ_DEVICE_ID = 0x30;           // 读取模块设备地址
static const uint8_t CMD_READ_BAUD_RATE = 0x31;           // 读取模块波特率
static const uint8_t CMD_RESTORE_DEFAULTS = 0x33;         // 恢复出厂默认参数
static const uint8_t CMD_QUERY_SHORT_FILE_NAME = 0x1E;    // 查询歌曲短文件名
}

/**
 * @brief 构造函数，初始化内部成员变量。
 * @param 无。
 * @return 无。
 */
YfrobotMP3V3::YfrobotMP3V3()
    : _stream(nullptr),
      _hardwareSerial(nullptr),
#if YFMP3V3_HAS_SOFTWARE_SERIAL
      _softwareSerial(nullptr),
#endif
      _transportMode(TRANSPORT_NONE),
      _deviceId(kBroadcastDeviceId),
      _baud(kDefaultBaud),
      _nextCommandReadyAt(0) {}

/**
 * @brief 析构函数，释放串口资源。
 * @param 无。
 * @return 无。
 */
YfrobotMP3V3::~YfrobotMP3V3() {
  end();
}

/**
 * @brief 使用硬件串口初始化模块。
 * @param serial 连接模块的硬件串口对象。
 * @param baud 串口波特率。
 * @return `true` 表示初始化成功；`false` 表示初始化失败。
 */
bool YfrobotMP3V3::beginHardwareSerial(HardwareSerial &serial, uint32_t baud) {
  end();
  _hardwareSerial = &serial;
  _stream = &serial;
  _transportMode = TRANSPORT_HARDWARE;
  _hardwareSerial->begin(baud);
  return beginWithActiveStream(baud);
}

#if defined(ESP32)
/**
 * @brief 使用 ESP32/ESP32-S3 硬件串口和自定义引脚初始化模块。
 * @param serial 连接模块的硬件串口对象。
 * @param rxPin 模块 TX 连接到开发板的 RX 引脚号。
 * @param txPin 模块 RX 连接到开发板的 TX 引脚号。
 * @param baud 串口波特率。
 * @return `true` 表示初始化成功；`false` 表示初始化失败。
 */
bool YfrobotMP3V3::beginHardwareSerial(HardwareSerial &serial,
                                       int8_t rxPin,
                                       int8_t txPin,
                                       uint32_t baud) {
  end();
  _hardwareSerial = &serial;
  _stream = &serial;
  _transportMode = TRANSPORT_HARDWARE;
  _hardwareSerial->begin(baud, SERIAL_8N1, rxPin, txPin);
  return beginWithActiveStream(baud);
}
#endif

#if YFMP3V3_HAS_SOFTWARE_SERIAL
/**
 * @brief 使用 AVR 软串口初始化模块。
 * @param rxPin 开发板接收引脚。
 * @param txPin 开发板发送引脚。
 * @param baud 串口波特率。
 * @return `true` 表示初始化成功；`false` 表示初始化失败。
 */
bool YfrobotMP3V3::beginSoftwareSerial(uint8_t rxPin, uint8_t txPin, uint32_t baud) {
  end();
  _softwareSerial = new SoftwareSerial(rxPin, txPin);
  if (_softwareSerial == nullptr) {
    return false;
  }

  _softwareSerial->begin(baud);
  _stream = _softwareSerial;
  _transportMode = TRANSPORT_SOFTWARE;
  return beginWithActiveStream(baud);
}
#endif

/**
 * @brief 结束当前串口连接并释放资源。
 * @param 无。
 * @return 无。
 */
void YfrobotMP3V3::end() {
#if YFMP3V3_HAS_SOFTWARE_SERIAL
  if (_softwareSerial != nullptr) {
    _softwareSerial->end();
    delete _softwareSerial;
    _softwareSerial = nullptr;
  }
#endif

  _stream = nullptr;
  _hardwareSerial = nullptr;
  _transportMode = TRANSPORT_NONE;
  _nextCommandReadyAt = 0;
}

/**
 * @brief 判断当前是否已经完成初始化。
 * @param 无。
 * @return `true` 表示已初始化；`false` 表示未初始化。
 */
bool YfrobotMP3V3::isConnected() const {
  return _stream != nullptr;
}

/**
 * @brief 判断当前是否使用软串口。
 * @param 无。
 * @return `true` 表示当前使用软串口；`false` 表示当前不是软串口。
 */
bool YfrobotMP3V3::isUsingSoftwareSerial() const {
  return _transportMode == TRANSPORT_SOFTWARE;
}

/**
 * @brief 获取当前波特率。
 * @param 无。
 * @return 当前波特率。
 */
uint32_t YfrobotMP3V3::baud() const {
  return _baud;
}

/**
 * @brief 设置目标设备地址,此函数无用，ID为厂家固化，无法修改
 * @param deviceId 设备地址。
 * @return 无。
 */
void YfrobotMP3V3::setDeviceId(uint16_t deviceId) {
  _deviceId = deviceId;
}

/**
 * @brief 获取当前目标设备地址。
 * @param 无。
 * @return 当前目标设备地址。
 */
uint16_t YfrobotMP3V3::deviceId() const {
  return _deviceId;
}

/**
 * @brief 播放当前曲目。
 * @param 无。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::play() {
  return sendCommand(CMD_PLAY);
}

/**
 * @brief 暂停当前曲目。
 * @param 无。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::pause() {
  return sendCommand(CMD_PAUSE);
}

/**
 * @brief 停止播放。
 * @param 无。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::stop() {
  return sendCommand(CMD_STOP);
}

/**
 * @brief 切换到上一曲。
 * @param 无。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::previousTrack() {
  return sendCommand(CMD_PREVIOUS_TRACK);
}

/**
 * @brief 切换到下一曲。
 * @param 无。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::nextTrack() {
  return sendCommand(CMD_NEXT_TRACK);
}

/**
 * @brief 指定根目录曲目编号并立即播放。
 * @param trackNumber 曲目编号。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::playTrack(uint16_t trackNumber) {
  const uint8_t payload[2] = {
      static_cast<uint8_t>(trackNumber >> 8),
      static_cast<uint8_t>(trackNumber & 0xFF)};
  return sendCommand(CMD_PLAY_TRACK, payload, sizeof(payload));
}

/**
 * @brief 指定根目录曲目编号但不立即播放。
 * @param trackNumber 曲目编号。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::selectTrack(uint16_t trackNumber) {
  const uint8_t payload[2] = {
      static_cast<uint8_t>(trackNumber >> 8),
      static_cast<uint8_t>(trackNumber & 0xFF)};
  return sendCommand(CMD_SELECT_TRACK, payload, sizeof(payload));
}

/**
 * @brief 指定盘符和路径播放音频。
 * @param storage 目标盘符。
 * @param path ASCII 路径字符串。
 * @return `true` 表示命令发送成功；`false` 表示参数非法或发送失败。
 */
bool YfrobotMP3V3::playPath(StorageDevice storage, const char *path) {
  if (path == nullptr) {
    return false;
  }

  const size_t pathLength = strlen(path);
  if (pathLength == 0 || pathLength > kMaxPayloadBytes - 1) {
    return false;
  }

  uint8_t payload[kMaxPayloadBytes];
  payload[0] = static_cast<uint8_t>(storage);
  memcpy(payload + 1, path, pathLength);
  return sendCommand(CMD_PLAY_PATH, payload, pathLength + 1);
}

bool YfrobotMP3V3::playPath(const char *path, StorageDevice storage) {
  return playPath(storage, path);
}

/**
 * @brief 指定盘符和路径进行插播。
 * @param storage 目标盘符。
 * @param path ASCII 路径字符串。
 * @return `true` 表示命令发送成功；`false` 表示参数非法或发送失败。
 */
bool YfrobotMP3V3::insertAdvert(StorageDevice storage, const char *path) {
  if (path == nullptr) {
    return false;
  }

  const size_t pathLength = strlen(path);
  if (pathLength == 0 || pathLength > kMaxPayloadBytes - 1) {
    return false;
  }

  uint8_t payload[kMaxPayloadBytes];
  payload[0] = static_cast<uint8_t>(storage);
  memcpy(payload + 1, path, pathLength);
  return sendCommand(CMD_INSERT_ADVERT, payload, pathLength + 1);
}

bool YfrobotMP3V3::insertAdvert(const char *path, StorageDevice storage) {
  return insertAdvert(storage, path);
}

/**
 * @brief 提前结束当前插播。
 * @param 无。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::stopAdvert() {
  return sendCommand(CMD_STOP_ADVERT);
}

/**
 * @brief 切换到上一个文件夹并播放。
 * @param 无。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::playPreviousFolder() {
  return sendCommand(CMD_PREVIOUS_FOLDER);
}

/**
 * @brief 切换到下一个文件夹并播放。
 * @param 无。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::playNextFolder() {
  return sendCommand(CMD_NEXT_FOLDER);
}

/**
 * @brief 组合播放 `/ZH` 文件夹中的多首曲目。
 * @param trackList 组合字符串，例如 `"0102"`、`"010203"`。
 * @return `true` 表示命令发送成功；`false` 表示参数非法或发送失败。
 */
bool YfrobotMP3V3::playCombination(const char *trackList) {
  if (trackList == nullptr) {
    return false;
  }

  const size_t payloadLength = strlen(trackList);
  if (payloadLength == 0 || payloadLength > kMaxPayloadBytes) {
    return false;
  }

  return sendCommand(CMD_PLAY_COMBINATION,
                     reinterpret_cast<const uint8_t *>(trackList),
                     payloadLength);
}

/**
 * @brief 直接设置音量。
 * @param volume 音量值。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::setVolume(uint8_t volume) {
  if (volume > 30) {
    volume = 30;
  }
  return sendCommand(CMD_SET_VOLUME, &volume, 1);
}

/**
 * @brief 音量加 1。
 * @param 无。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::volumeUp() {
  return sendCommand(CMD_VOLUME_UP);
}

/**
 * @brief 音量减 1。
 * @param 无。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::volumeDown() {
  return sendCommand(CMD_VOLUME_DOWN);
}

/**
 * @brief 查询当前音量。
 * @param volume 用于返回音量值的引用变量。
 * @param timeoutMs 查询超时时间，单位毫秒。
 * @return `true` 表示查询成功；`false` 表示超时或解析失败。
 */
bool YfrobotMP3V3::readVolume(uint8_t &volume, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(CMD_QUERY_VOLUME, frame, timeoutMs) || frame.dataLength < 1) {
    return false;
  }
  volume = frame.data[0];
  return true;
}

/**
 * @brief 设置播放模式和循环次数。
 * @param mode 播放模式。
 * @param repeatCount 循环次数。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::setPlaybackMode(PlayMode mode, uint16_t repeatCount) {
  const uint8_t payload[3] = {
      static_cast<uint8_t>(mode),
      static_cast<uint8_t>(repeatCount >> 8),
      static_cast<uint8_t>(repeatCount & 0xFF)};
  return sendCommand(CMD_SET_PLAYBACK_MODE, payload, sizeof(payload));
}

/**
 * @brief 查询当前播放模式和循环次数。
 * @param settings 用于保存查询结果的结构体。
 * @param timeoutMs 查询超时时间，单位毫秒。
 * @return `true` 表示查询成功；`false` 表示超时或解析失败。
 */
bool YfrobotMP3V3::readPlaybackMode(PlaybackModeSettings &settings, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(CMD_QUERY_PLAYBACK_MODE, frame, timeoutMs) || frame.dataLength < 3) {
    return false;
  }

  settings.mode = static_cast<PlayMode>(frame.data[0]);
  settings.repeatCount = static_cast<uint16_t>(frame.data[1] << 8) | frame.data[2];
  return true;
}

/**
 * @brief 设置 EQ 模式。
 * @param eq EQ 模式枚举值。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::setEq(EqPreset eq) {
  const uint8_t payload = static_cast<uint8_t>(eq);
  return sendCommand(CMD_SET_EQ, &payload, 1);
}

/**
 * @brief 保存当前参数。
 * @param 无。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::saveSettings() {
  return sendCommand(CMD_SAVE_SETTINGS);
}

/**
 * @brief 重启模块。
 * @param 无。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::reboot() {
  return sendCommand(CMD_REBOOT);
}

/**
 * @brief 查询当前播放状态。
 * @param state 用于返回播放状态的引用变量。
 * @param timeoutMs 查询超时时间，单位毫秒。
 * @return `true` 表示查询成功；`false` 表示超时或解析失败。
 */
bool YfrobotMP3V3::readPlayState(PlayState &state, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(CMD_QUERY_PLAY_STATE, frame, timeoutMs) || frame.dataLength < 1) {
    return false;
  }
  state = static_cast<PlayState>(frame.data[0]);
  return true;
}

/**
 * @brief 查询当前盘符总曲目数。
 * @param trackCount 用于返回曲目总数的引用变量。
 * @param timeoutMs 查询超时时间，单位毫秒。
 * @return `true` 表示查询成功；`false` 表示超时或解析失败。
 */
bool YfrobotMP3V3::readTotalTracks(uint16_t &trackCount, uint32_t timeoutMs) {
  return readU16Response(CMD_QUERY_TOTAL_TRACKS, trackCount, timeoutMs);
}

/**
 * @brief 查询当前文件夹曲目数。
 * @param trackCount 用于返回文件夹曲目数的引用变量。
 * @param timeoutMs 查询超时时间，单位毫秒。
 * @return `true` 表示查询成功；`false` 表示超时或解析失败。
 */
bool YfrobotMP3V3::readFolderTrackCount(uint16_t &trackCount, uint32_t timeoutMs) {
  return readU16Response(CMD_QUERY_FOLDER_TRACKS, trackCount, timeoutMs);
}

/**
 * @brief 查询当前曲目名称。
 * @param buffer 用于接收曲目名称的缓冲区。
 * @param bufferLength 缓冲区长度。
 * @param timeoutMs 查询超时时间，单位毫秒。
 * @return `true` 表示查询成功；`false` 表示超时、参数非法或当前无返回。
 */
bool YfrobotMP3V3::readCurrentTrackName(char *buffer, size_t bufferLength, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(CMD_QUERY_CURRENT_TRACK_NAME, frame, timeoutMs)) {
    return false;
  }
  return copyAsciiPayload(frame, buffer, bufferLength);
}


/*======================================= 以下函数无用 暂保留 =======================================*/
/**
 * @brief 查询当前在线盘符位掩码。
 * @param mask 用于返回在线盘符掩码的引用变量。
 * @param timeoutMs 查询超时时间，单位毫秒。
 * @return `true` 表示查询成功；`false` 表示超时或解析失败。
 */
bool YfrobotMP3V3::readOnlineStorageMask(uint8_t &mask, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(CMD_QUERY_ONLINE_STORAGE, frame, timeoutMs) || frame.dataLength < 1) {
    return false;
  }
  mask = frame.data[0];
  return true;
}

/**
 * @brief 查询当前播放盘符。
 * @param storage 用于返回盘符的引用变量。
 * @param timeoutMs 查询超时时间，单位毫秒。
 * @return `true` 表示查询成功；`false` 表示超时或解析失败。
 */
bool YfrobotMP3V3::readCurrentStorage(StorageDevice &storage, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(CMD_QUERY_CURRENT_STORAGE, frame, timeoutMs) || frame.dataLength < 1) {
    return false;
  }
  storage = static_cast<StorageDevice>(frame.data[0]);
  return true;
}

/**
 * @brief 切换到指定盘符。
 * @param storage 目标盘符。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::switchStorage(StorageDevice storage) {
  const uint8_t payload = static_cast<uint8_t>(storage);
  return sendCommand(CMD_SWITCH_STORAGE, &payload, 1);
}

/**
 * @brief 查询模块设备地址。
 * @param moduleDeviceId 用于返回设备地址的引用变量。
 * @param timeoutMs 查询超时时间，单位毫秒。
 * @return `true` 表示查询成功；`false` 表示超时或解析失败。
 */
bool YfrobotMP3V3::readModuleDeviceId(uint16_t &moduleDeviceId, uint32_t timeoutMs) {
  return readU16Response(CMD_READ_DEVICE_ID, moduleDeviceId, timeoutMs);
}

/**
 * @brief 查询当前波特率。
 * @param baudRate 用于返回波特率的引用变量。
 * @param timeoutMs 查询超时时间，单位毫秒。
 * @return `true` 表示查询成功；`false` 表示超时或解析失败。
 */
bool YfrobotMP3V3::readBaudRate(uint32_t &baudRate, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(CMD_READ_BAUD_RATE, frame, timeoutMs) || frame.dataLength < 4) {
    return false;
  }

  baudRate = (static_cast<uint32_t>(frame.data[0]) << 24) |
             (static_cast<uint32_t>(frame.data[1]) << 16) |
             (static_cast<uint32_t>(frame.data[2]) << 8) |
             static_cast<uint32_t>(frame.data[3]);
  return true;
}

/**
 * @brief 恢复默认参数。
 * @param 无。
 * @return `true` 表示命令发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::restoreFactorySettings() {
  return sendCommand(CMD_RESTORE_DEFAULTS);
}

/**
 * @brief 查询当前曲目的短文件名。
 * @param buffer 用于接收短文件名的缓冲区。
 * @param bufferLength 缓冲区长度。
 * @param timeoutMs 查询超时时间，单位毫秒。
 * @return `true` 表示查询成功；`false` 表示超时、参数非法或当前无返回。
 */
bool YfrobotMP3V3::readShortFileName(char *buffer, size_t bufferLength, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(CMD_QUERY_SHORT_FILE_NAME, frame, timeoutMs)) {
    return false;
  }
  return copyAsciiPayload(frame, buffer, bufferLength);
}
/*======================================= 以上函数无用 暂保留 =======================================*/

/**
 * @brief 发送任意协议命令。
 * @param command 指令码。
 * @param payload 数据区首地址，可为空。
 * @param payloadLength 数据区长度，单位字节。
 * @return `true` 表示命令发送成功；`false` 表示串口未初始化或参数非法。
 */
bool YfrobotMP3V3::sendCommand(uint8_t command, const uint8_t *payload, size_t payloadLength) {
  if (!isConnected()) {
    return false;
  }

  // 先等待上一条命令的最小处理间隔结束。
  waitCommandGap();
  clearInput();
  const bool sent = sendFrame(command, payload, payloadLength);
  if (sent) {
    // 发送成功后，记录下一条命令的可发送时刻。
    _nextCommandReadyAt = millis() + postCommandGapMs(command);
  }
  return sent;
}

/**
 * @brief 发送查询命令并等待完整返回帧。
 * @param command 查询指令码。
 * @param frame 用于保存返回帧的结构体。
 * @param timeoutMs 查询超时时间，单位毫秒。
 * @return `true` 表示查询成功；`false` 表示发送失败、超时或解析失败。
 */
bool YfrobotMP3V3::queryFrame(uint8_t command, Frame &frame, uint32_t timeoutMs) {
  if (!isConnected() || timeoutMs == 0) {
    return false;
  }

  const uint32_t startTime = millis();
  uint32_t nextSendTime = startTime;

  while (millis() - startTime < timeoutMs) {
    const uint32_t now = millis();
    if (now >= nextSendTime) {
      // 查询命令也要服从全局发送间隔，避免和前一条命令冲突。
      waitCommandGap();
      clearInput();
      if (!sendFrame(command, nullptr, 0)) {
        return false;
      }
      const uint32_t sentAt = millis();
      // 查询帧本身也给模块留出内部处理时间。
      _nextCommandReadyAt = sentAt + postCommandGapMs(command);
      nextSendTime = sentAt + 120;
    }

    const uint32_t elapsed = millis() - startTime;
    const uint32_t remaining = (elapsed < timeoutMs) ? (timeoutMs - elapsed) : 0;
    if (remaining == 0) {
      break;
    }

    const uint32_t sliceTimeout = (remaining > 120) ? 120 : remaining;
    if (!receiveFrame(frame, sliceTimeout)) {
      continue;
    }

    if (frame.command == command) {
      return true;
    }
  }

  return false;
}

/**
 * @brief 完成串口对象初始化后的公共收尾动作。
 * @param baud 当前波特率。
 * @return `true` 表示初始化成功；`false` 表示当前流对象为空。
 */
bool YfrobotMP3V3::beginWithActiveStream(uint32_t baud) {
  if (_stream == nullptr) {
    return false;
  }

  _baud = baud;
  // 新建连接后重置命令冷却状态。
  _nextCommandReadyAt = 0;
  delay(20);
  clearInput();
  return true;
}

/**
 * @brief 等待到当前命令间隔满足，再继续发送下一帧。
 */
void YfrobotMP3V3::waitCommandGap() {
  while (static_cast<int32_t>(_nextCommandReadyAt - millis()) > 0) {
    delay(1);
  }
}

/**
 * @brief 根据指令类型返回建议的后续间隔，
 *        配置/保存/重启类命令比普通播放控制命令更长。
 */
uint16_t YfrobotMP3V3::postCommandGapMs(uint8_t command) const {
  switch (command) {
    case CMD_SET_PLAYBACK_MODE:
    case CMD_SET_EQ:
    case CMD_SWITCH_STORAGE:
      return kConfigCommandGapMs;

    case CMD_SAVE_SETTINGS:
    case CMD_RESTORE_DEFAULTS:
      return kSaveCommandGapMs;

    case CMD_REBOOT:
      return kRebootCommandGapMs;

    default:
      return kNormalCommandGapMs;
  }
}

/**
 * @brief 按协议打包并发送完整数据帧。
 * @param command 指令码。
 * @param payload 数据区首地址，可为空。
 * @param payloadLength 数据区长度，单位字节。
 * @return `true` 表示发送成功；`false` 表示发送失败。
 */
bool YfrobotMP3V3::sendFrame(uint8_t command, const uint8_t *payload, size_t payloadLength) {
  if (_stream == nullptr || payloadLength > kMaxPayloadBytes) {
    return false;
  }

  const uint8_t frameLength = static_cast<uint8_t>(kFixedFrameBytes + payloadLength);
  const uint8_t highId = static_cast<uint8_t>(_deviceId >> 8);
  const uint8_t lowId = static_cast<uint8_t>(_deviceId & 0xFF);
  const uint8_t sum = checksumForFrame(_deviceId, frameLength, command, payload, payloadLength);

  _stream->write(kStartCode);
  _stream->write(frameLength);
  _stream->write(highId);
  _stream->write(lowId);
  _stream->write(command);

  for (size_t i = 0; i < payloadLength; ++i) {
    _stream->write(payload[i]);
  }

  _stream->write(sum);
  _stream->write(kEndCode);

  if (_hardwareSerial != nullptr) {
    _hardwareSerial->flush();
  }
#if YFMP3V3_HAS_SOFTWARE_SERIAL
  else if (_softwareSerial != nullptr) {
    _softwareSerial->flush();
  }
#endif

  return true;
}

/**
 * @brief 从串口接收并解析一帧返回数据。
 * @param frame 用于保存解析结果的结构体。
 * @param timeoutMs 接收超时时间，单位毫秒。
 * @return `true` 表示成功解析到一帧；`false` 表示超时或校验失败。
 */
bool YfrobotMP3V3::receiveFrame(Frame &frame, uint32_t timeoutMs) {
  if (_stream == nullptr) {
    return false;
  }

  const uint32_t startTime = millis();

  while (millis() - startTime < timeoutMs) {
    if (_stream->available() <= 0) {
      continue;
    }

    const int value = _stream->read();
    if (value < 0 || static_cast<uint8_t>(value) != kStartCode) {
      continue;
    }

    while (_stream->available() <= 0) {
      if (millis() - startTime >= timeoutMs) {
        return false;
      }
    }

    const uint8_t frameLength = static_cast<uint8_t>(_stream->read());
    if (frameLength < kFixedFrameBytes) {
      return false;
    }

    const size_t remainingBytes = frameLength - 2;
    if (remainingBytes > (kMaxPayloadBytes + 5)) {
      return false;
    }

    uint8_t buffer[kMaxPayloadBytes + 5];
    size_t received = 0;

    while (received < remainingBytes) {
      if (_stream->available() <= 0) {
        if (millis() - startTime >= timeoutMs) {
          return false;
        }
        continue;
      }

      const int nextByte = _stream->read();
      if (nextByte < 0) {
        continue;
      }
      buffer[received++] = static_cast<uint8_t>(nextByte);
    }

    if (buffer[remainingBytes - 1] != kEndCode) {
      return false;
    }

    const size_t payloadLength = frameLength - kFixedFrameBytes;
    const uint16_t deviceId = static_cast<uint16_t>(buffer[0] << 8) | buffer[1];
    const uint8_t command = buffer[2];
    const uint8_t actualChecksum = buffer[remainingBytes - 2];
    const uint8_t expectedChecksum =
        checksumForFrame(deviceId, frameLength, command, buffer + 3, payloadLength);

    if (actualChecksum != expectedChecksum) {
      return false;
    }

    frame.deviceId = deviceId;
    frame.command = command;
    frame.dataLength = payloadLength;

    if (payloadLength > 0) {
      memcpy(frame.data, buffer + 3, payloadLength);
    }

    return true;
  }

  return false;
}

/**
 * @brief 清空串口接收缓冲区。
 * @param 无。
 * @return 无。
 */
void YfrobotMP3V3::clearInput() {
  if (_stream == nullptr) {
    return;
  }

  while (_stream->available() > 0) {
    _stream->read();
  }
}

/**
 * @brief 计算一帧数据的校验和。
 * @param deviceId 设备地址。
 * @param frameLength 整帧长度。
 * @param command 指令码。
 * @param payload 数据区首地址，可为空。
 * @param payloadLength 数据区长度，单位字节。
 * @return 校验和低 8 位。
 */
uint8_t YfrobotMP3V3::checksumForFrame(uint16_t deviceId,
                                       uint8_t frameLength,
                                       uint8_t command,
                                       const uint8_t *payload,
                                       size_t payloadLength) const {
  uint32_t sum = kStartCode;
  sum += frameLength;
  sum += static_cast<uint8_t>(deviceId >> 8);
  sum += static_cast<uint8_t>(deviceId & 0xFF);
  sum += command;

  for (size_t i = 0; i < payloadLength; ++i) {
    sum += payload[i];
  }

  return static_cast<uint8_t>(sum & 0xFF);
}

/**
 * @brief 将返回帧中的 ASCII 数据复制到字符串缓冲区。
 * @param frame 已解析完成的返回帧。
 * @param buffer 目标缓冲区。
 * @param bufferLength 缓冲区长度。
 * @return `true` 表示复制成功；`false` 表示参数非法或数据为空。
 */
bool YfrobotMP3V3::copyAsciiPayload(const Frame &frame, char *buffer, size_t bufferLength) const {
  if (buffer == nullptr || bufferLength == 0 || frame.dataLength == 0) {
    return false;
  }

  const size_t copyLength = (frame.dataLength < (bufferLength - 1))
                                ? frame.dataLength
                                : (bufferLength - 1);
  memcpy(buffer, frame.data, copyLength);
  buffer[copyLength] = '\0';
  return true;
}

/**
 * @brief 发送查询命令并读取两字节无符号整数结果。
 * @param command 查询指令码。
 * @param value 用于返回结果的引用变量。
 * @param timeoutMs 查询超时时间，单位毫秒。
 * @return `true` 表示查询成功；`false` 表示超时或解析失败。
 */
bool YfrobotMP3V3::readU16Response(uint8_t command, uint16_t &value, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(command, frame, timeoutMs)) {
    return false;
  }

  if (frame.dataLength >= 2) {
    value = static_cast<uint16_t>(frame.data[0] << 8) | frame.data[1];
    return true;
  }

  if (frame.dataLength == 1) {
    value = frame.data[0];
    return true;
  }

  if (command == CMD_READ_DEVICE_ID) {
    value = frame.deviceId;
    return true;
  }

  return false;
}
