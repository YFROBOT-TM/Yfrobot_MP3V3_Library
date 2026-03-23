#include "Yfrobot_MP3V3.h"

namespace {
static const uint8_t CMD_QUERY_PLAY_STATE = 0x01;
static const uint8_t CMD_PLAY = 0x02;
static const uint8_t CMD_PAUSE = 0x03;
static const uint8_t CMD_STOP = 0x04;
static const uint8_t CMD_PREVIOUS_TRACK = 0x05;
static const uint8_t CMD_NEXT_TRACK = 0x06;
static const uint8_t CMD_PLAY_TRACK = 0x07;
static const uint8_t CMD_PLAY_PATH = 0x08;
static const uint8_t CMD_QUERY_ONLINE_STORAGE = 0x09;
static const uint8_t CMD_QUERY_CURRENT_STORAGE = 0x0A;
static const uint8_t CMD_SWITCH_STORAGE = 0x0B;
static const uint8_t CMD_QUERY_TOTAL_TRACKS = 0x0C;
static const uint8_t CMD_QUERY_CURRENT_TRACK_NAME = 0x0D;
static const uint8_t CMD_PREVIOUS_FOLDER = 0x0E;
static const uint8_t CMD_NEXT_FOLDER = 0x0F;
static const uint8_t CMD_STOP_ADVERT = 0x10;
static const uint8_t CMD_QUERY_FOLDER_TRACKS = 0x12;
static const uint8_t CMD_SET_VOLUME = 0x13;
static const uint8_t CMD_VOLUME_UP = 0x14;
static const uint8_t CMD_VOLUME_DOWN = 0x15;
static const uint8_t CMD_QUERY_VOLUME = 0x16;
static const uint8_t CMD_INSERT_ADVERT = 0x17;
static const uint8_t CMD_SET_PLAYBACK_MODE = 0x18;
static const uint8_t CMD_QUERY_PLAYBACK_MODE = 0x19;
static const uint8_t CMD_SET_EQ = 0x1A;
static const uint8_t CMD_PLAY_COMBINATION = 0x1B;
static const uint8_t CMD_QUERY_SHORT_FILE_NAME = 0x1E;
static const uint8_t CMD_SELECT_TRACK = 0x1F;
static const uint8_t CMD_READ_DEVICE_ID = 0x30;
static const uint8_t CMD_READ_BAUD_RATE = 0x31;
static const uint8_t CMD_SAVE_SETTINGS = 0x32;
static const uint8_t CMD_RESTORE_DEFAULTS = 0x33;
static const uint8_t CMD_REBOOT = 0x34;
}

YfrobotMP3V3::YfrobotMP3V3()
    : _stream(nullptr),
      _hardwareSerial(nullptr),
#if YFMP3V3_HAS_SOFTWARE_SERIAL
      _softwareSerial(nullptr),
#endif
      _transportMode(TRANSPORT_NONE),
      _deviceId(kBroadcastDeviceId),
      _baud(kDefaultBaud) {}

YfrobotMP3V3::~YfrobotMP3V3() {
  end();
}

bool YfrobotMP3V3::beginHardwareSerial(HardwareSerial &serial, uint32_t baud) {
  end();
  _hardwareSerial = &serial;
  _stream = &serial;
  _transportMode = TRANSPORT_HARDWARE;
  _hardwareSerial->begin(baud);
  return beginWithActiveStream(baud);
}

#if defined(ESP32)
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
}

bool YfrobotMP3V3::isConnected() const {
  return _stream != nullptr;
}

bool YfrobotMP3V3::isUsingSoftwareSerial() const {
  return _transportMode == TRANSPORT_SOFTWARE;
}

uint32_t YfrobotMP3V3::baud() const {
  return _baud;
}

void YfrobotMP3V3::setDeviceId(uint16_t deviceId) {
  _deviceId = deviceId;
}

uint16_t YfrobotMP3V3::deviceId() const {
  return _deviceId;
}

bool YfrobotMP3V3::play() {
  return sendCommand(CMD_PLAY);
}

bool YfrobotMP3V3::pause() {
  return sendCommand(CMD_PAUSE);
}

bool YfrobotMP3V3::stop() {
  return sendCommand(CMD_STOP);
}

bool YfrobotMP3V3::previousTrack() {
  return sendCommand(CMD_PREVIOUS_TRACK);
}

bool YfrobotMP3V3::nextTrack() {
  return sendCommand(CMD_NEXT_TRACK);
}

bool YfrobotMP3V3::playTrack(uint16_t trackNumber) {
  const uint8_t payload[2] = {
      static_cast<uint8_t>(trackNumber >> 8),
      static_cast<uint8_t>(trackNumber & 0xFF)};
  return sendCommand(CMD_PLAY_TRACK, payload, sizeof(payload));
}

bool YfrobotMP3V3::selectTrack(uint16_t trackNumber) {
  const uint8_t payload[2] = {
      static_cast<uint8_t>(trackNumber >> 8),
      static_cast<uint8_t>(trackNumber & 0xFF)};
  return sendCommand(CMD_SELECT_TRACK, payload, sizeof(payload));
}

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

bool YfrobotMP3V3::stopAdvert() {
  return sendCommand(CMD_STOP_ADVERT);
}

bool YfrobotMP3V3::playPreviousFolder() {
  return sendCommand(CMD_PREVIOUS_FOLDER);
}

bool YfrobotMP3V3::playNextFolder() {
  return sendCommand(CMD_NEXT_FOLDER);
}

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

bool YfrobotMP3V3::setVolume(uint8_t volume) {
  if (volume > 30) {
    volume = 30;
  }
  return sendCommand(CMD_SET_VOLUME, &volume, 1);
}

bool YfrobotMP3V3::volumeUp() {
  return sendCommand(CMD_VOLUME_UP);
}

bool YfrobotMP3V3::volumeDown() {
  return sendCommand(CMD_VOLUME_DOWN);
}

bool YfrobotMP3V3::readVolume(uint8_t &volume, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(CMD_QUERY_VOLUME, frame, timeoutMs) || frame.dataLength < 1) {
    return false;
  }
  volume = frame.data[0];
  return true;
}

bool YfrobotMP3V3::setPlaybackMode(PlayMode mode, uint16_t repeatCount) {
  const uint8_t payload[3] = {
      static_cast<uint8_t>(mode),
      static_cast<uint8_t>(repeatCount >> 8),
      static_cast<uint8_t>(repeatCount & 0xFF)};
  return sendCommand(CMD_SET_PLAYBACK_MODE, payload, sizeof(payload));
}

bool YfrobotMP3V3::readPlaybackMode(PlaybackModeSettings &settings, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(CMD_QUERY_PLAYBACK_MODE, frame, timeoutMs) || frame.dataLength < 3) {
    return false;
  }

  settings.mode = static_cast<PlayMode>(frame.data[0]);
  settings.repeatCount = static_cast<uint16_t>(frame.data[1] << 8) | frame.data[2];
  return true;
}

bool YfrobotMP3V3::setEq(EqPreset eq) {
  const uint8_t payload = static_cast<uint8_t>(eq);
  return sendCommand(CMD_SET_EQ, &payload, 1);
}

bool YfrobotMP3V3::saveSettings() {
  return sendCommand(CMD_SAVE_SETTINGS);
}

bool YfrobotMP3V3::restoreFactorySettings() {
  return sendCommand(CMD_RESTORE_DEFAULTS);
}

bool YfrobotMP3V3::reboot() {
  return sendCommand(CMD_REBOOT);
}

bool YfrobotMP3V3::readPlayState(PlayState &state, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(CMD_QUERY_PLAY_STATE, frame, timeoutMs) || frame.dataLength < 1) {
    return false;
  }
  state = static_cast<PlayState>(frame.data[0]);
  return true;
}

bool YfrobotMP3V3::readOnlineStorageMask(uint8_t &mask, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(CMD_QUERY_ONLINE_STORAGE, frame, timeoutMs) || frame.dataLength < 1) {
    return false;
  }
  mask = frame.data[0];
  return true;
}

bool YfrobotMP3V3::readCurrentStorage(StorageDevice &storage, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(CMD_QUERY_CURRENT_STORAGE, frame, timeoutMs) || frame.dataLength < 1) {
    return false;
  }
  storage = static_cast<StorageDevice>(frame.data[0]);
  return true;
}

bool YfrobotMP3V3::switchStorage(StorageDevice storage) {
  const uint8_t payload = static_cast<uint8_t>(storage);
  return sendCommand(CMD_SWITCH_STORAGE, &payload, 1);
}

bool YfrobotMP3V3::readTotalTracks(uint16_t &trackCount, uint32_t timeoutMs) {
  return readU16Response(CMD_QUERY_TOTAL_TRACKS, trackCount, timeoutMs);
}

bool YfrobotMP3V3::readFolderTrackCount(uint16_t &trackCount, uint32_t timeoutMs) {
  return readU16Response(CMD_QUERY_FOLDER_TRACKS, trackCount, timeoutMs);
}

bool YfrobotMP3V3::readCurrentTrackName(char *buffer, size_t bufferLength, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(CMD_QUERY_CURRENT_TRACK_NAME, frame, timeoutMs)) {
    return false;
  }
  return copyAsciiPayload(frame, buffer, bufferLength);
}

bool YfrobotMP3V3::readShortFileName(char *buffer, size_t bufferLength, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(CMD_QUERY_SHORT_FILE_NAME, frame, timeoutMs)) {
    return false;
  }
  return copyAsciiPayload(frame, buffer, bufferLength);
}

bool YfrobotMP3V3::readModuleDeviceId(uint16_t &moduleDeviceId, uint32_t timeoutMs) {
  return readU16Response(CMD_READ_DEVICE_ID, moduleDeviceId, timeoutMs);
}

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

bool YfrobotMP3V3::sendCommand(uint8_t command, const uint8_t *payload, size_t payloadLength) {
  if (!isConnected()) {
    return false;
  }

  clearInput();
  return sendFrame(command, payload, payloadLength);
}

bool YfrobotMP3V3::queryFrame(uint8_t command, Frame &frame, uint32_t timeoutMs) {
  if (!sendCommand(command)) {
    return false;
  }

  const uint32_t startTime = millis();
  while (millis() - startTime < timeoutMs) {
    const uint32_t elapsed = millis() - startTime;
    const uint32_t remaining = (elapsed < timeoutMs) ? (timeoutMs - elapsed) : 0;
    if (remaining == 0) {
      break;
    }

    if (!receiveFrame(frame, remaining)) {
      continue;
    }

    if (frame.command == command) {
      return true;
    }
  }

  return false;
}

bool YfrobotMP3V3::beginWithActiveStream(uint32_t baud) {
  if (_stream == nullptr) {
    return false;
  }

  _baud = baud;
  delay(20);
  clearInput();
  return true;
}

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

void YfrobotMP3V3::clearInput() {
  if (_stream == nullptr) {
    return;
  }

  while (_stream->available() > 0) {
    _stream->read();
  }
}

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

bool YfrobotMP3V3::readU16Response(uint8_t command, uint16_t &value, uint32_t timeoutMs) {
  Frame frame;
  if (!queryFrame(command, frame, timeoutMs) || frame.dataLength < 2) {
    return false;
  }

  value = static_cast<uint16_t>(frame.data[0] << 8) | frame.data[1];
  return true;
}
