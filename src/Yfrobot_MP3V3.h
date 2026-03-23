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

class YfrobotMP3V3 {
public:
  static const uint8_t kStartCode = 0x7E;
  static const uint8_t kEndCode = 0xEF;
  static const uint16_t kBroadcastDeviceId = 0xFFFF;
  static const uint16_t kDefaultDeviceId = 0x0001;
  static const uint32_t kDefaultBaud = 9600;

  enum StorageDevice : uint8_t {
    STORAGE_USB = 0x00,
    STORAGE_SD = 0x01,
    STORAGE_FLASH = 0x02,
    STORAGE_NONE = 0xFF
  };

  enum PlayState : uint8_t {
    PLAY_STATE_STOPPED = 0x00,
    PLAY_STATE_PLAYING = 0x01,
    PLAY_STATE_PAUSED = 0x02,
    PLAY_STATE_UNKNOWN = 0xFF
  };

  enum PlayMode : uint8_t {
    PLAY_MODE_LOOP_ALL = 0x01,
    PLAY_MODE_LOOP_SINGLE = 0x02,
    PLAY_MODE_LOOP_FOLDER = 0x03,
    PLAY_MODE_LOOP_RANDOM = 0x04,
    PLAY_MODE_STOP_AFTER_ALL = 0x05,
    PLAY_MODE_STOP_AFTER_SINGLE = 0x06,
    PLAY_MODE_STOP_AFTER_RANDOM = 0x07
  };

  enum EqPreset : uint8_t {
    EQ_NORMAL = 0x00,
    EQ_ROCK = 0x01,
    EQ_POP = 0x02,
    EQ_CLASSIC = 0x03,
    EQ_JAZZ = 0x04,
    EQ_COUNTRY = 0x05,
    EQ_CUSTOM = 0x06
  };

  struct PlaybackModeSettings {
    PlayMode mode;
    uint16_t repeatCount;
  };

  struct Frame {
    uint16_t deviceId;
    uint8_t command;
    uint8_t data[96];
    size_t dataLength;
  };

  YfrobotMP3V3();
  ~YfrobotMP3V3();

  bool beginHardwareSerial(HardwareSerial &serial, uint32_t baud = kDefaultBaud);

#if defined(ESP32)
  bool beginHardwareSerial(HardwareSerial &serial,
                           int8_t rxPin,
                           int8_t txPin,
                           uint32_t baud = kDefaultBaud);
#endif

#if YFMP3V3_HAS_SOFTWARE_SERIAL
  bool beginSoftwareSerial(uint8_t rxPin, uint8_t txPin, uint32_t baud = kDefaultBaud);
#endif

  void end();
  bool isConnected() const;
  bool isUsingSoftwareSerial() const;
  uint32_t baud() const;

  void setDeviceId(uint16_t deviceId);
  uint16_t deviceId() const;

  bool play();
  bool pause();
  bool stop();
  bool previousTrack();
  bool nextTrack();
  bool playTrack(uint16_t trackNumber);
  bool selectTrack(uint16_t trackNumber);
  bool playPath(StorageDevice storage, const char *path);
  bool insertAdvert(StorageDevice storage, const char *path);
  bool stopAdvert();
  bool playPreviousFolder();
  bool playNextFolder();
  bool playCombination(const char *trackList);

  bool setVolume(uint8_t volume);
  bool volumeUp();
  bool volumeDown();
  bool readVolume(uint8_t &volume, uint32_t timeoutMs = 300);

  bool setPlaybackMode(PlayMode mode, uint16_t repeatCount = 0);
  bool readPlaybackMode(PlaybackModeSettings &settings, uint32_t timeoutMs = 300);

  bool setEq(EqPreset eq);
  bool saveSettings();
  bool restoreFactorySettings();
  bool reboot();

  bool readPlayState(PlayState &state, uint32_t timeoutMs = 300);
  bool readOnlineStorageMask(uint8_t &mask, uint32_t timeoutMs = 300);
  bool readCurrentStorage(StorageDevice &storage, uint32_t timeoutMs = 300);
  bool switchStorage(StorageDevice storage);
  bool readTotalTracks(uint16_t &trackCount, uint32_t timeoutMs = 300);
  bool readFolderTrackCount(uint16_t &trackCount, uint32_t timeoutMs = 300);
  bool readCurrentTrackName(char *buffer, size_t bufferLength, uint32_t timeoutMs = 300);
  bool readShortFileName(char *buffer, size_t bufferLength, uint32_t timeoutMs = 300);
  bool readModuleDeviceId(uint16_t &moduleDeviceId, uint32_t timeoutMs = 300);
  bool readBaudRate(uint32_t &baudRate, uint32_t timeoutMs = 300);

  bool sendCommand(uint8_t command, const uint8_t *payload = nullptr, size_t payloadLength = 0);
  bool queryFrame(uint8_t command, Frame &frame, uint32_t timeoutMs = 300);

private:
  enum TransportMode : uint8_t {
    TRANSPORT_NONE = 0,
    TRANSPORT_HARDWARE = 1,
    TRANSPORT_SOFTWARE = 2
  };

  static const size_t kFixedFrameBytes = 7;
  static const size_t kMaxPayloadBytes = 96;

  Stream *_stream;
  HardwareSerial *_hardwareSerial;
#if YFMP3V3_HAS_SOFTWARE_SERIAL
  SoftwareSerial *_softwareSerial;
#endif
  TransportMode _transportMode;
  uint16_t _deviceId;
  uint32_t _baud;

  bool beginWithActiveStream(uint32_t baud);
  bool sendFrame(uint8_t command, const uint8_t *payload, size_t payloadLength);
  bool receiveFrame(Frame &frame, uint32_t timeoutMs);
  void clearInput();
  uint8_t checksumForFrame(uint16_t deviceId,
                           uint8_t frameLength,
                           uint8_t command,
                           const uint8_t *payload,
                           size_t payloadLength) const;
  bool copyAsciiPayload(const Frame &frame, char *buffer, size_t bufferLength) const;
  bool readU16Response(uint8_t command, uint16_t &value, uint32_t timeoutMs);
};

#endif
