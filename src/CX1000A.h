// CX1000A.h
#ifndef CX1000A_H
#define CX1000A_H

#include <Arduino.h>

#if defined(__AVR__) || defined(ESP8266) || defined(NRF52) || defined(NRF5) 
#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#elif defined(ESP32)
#include <HardwareSerial.h>
#endif

// 抽象串口基类
class SerialBase {
public:
    virtual ~SerialBase() {}
    virtual void begin(unsigned long baud) = 0;
    virtual int available() = 0;
    virtual int read() = 0;
    virtual void write(uint8_t data) = 0;
    virtual void flush() = 0;
};

// 硬件串口适配器
class HardwareSerialAdapter : public SerialBase {
private:
    HardwareSerial* _serial;
    int8_t _rxPin;
    int8_t _txPin;
public:
    HardwareSerialAdapter(HardwareSerial* serial, int8_t rxPin = -1, int8_t txPin = -1) 
        : _serial(serial), _rxPin(rxPin), _txPin(txPin) {}
    void begin(unsigned long baud) override {
#if defined(ESP32)
        if (_rxPin >= 0 && _txPin >= 0) {
            _serial->begin(baud, SERIAL_8N1, _rxPin, _txPin);
        } else {
            _serial->begin(baud);
        }
#else
        _serial->begin(baud);
#endif
    }
    int available() override {
        return _serial->available();
    }
    int read() override {
        return _serial->read();
    }
    void write(uint8_t data) override {
        _serial->write(data);
    }
    void flush() override {
        _serial->flush();
    }
};

#if defined(__AVR__) || defined(ESP8266) || defined(NRF52) || defined(NRF5)
// 软件串口适配器
class SoftwareSerialAdapter : public SerialBase {
private:
    SoftwareSerial* _serial;
public:
    SoftwareSerialAdapter(SoftwareSerial* serial) : _serial(serial) {}
    void begin(unsigned long baud) override {
        _serial->begin(baud);
    }
    int available() override {
        return _serial->available();
    }
    int read() override {
        return _serial->read();
    }
    void write(uint8_t data) override {
        _serial->write(data);
    }
    void flush() override {
        _serial->flush();
    }
};
#endif

enum SerialType {
  SERIALTYPE_SW,
  SERIALTYPE_HW
};

// CX1000A 协议常量
#define CX1000A_START_CODE    0x7E
#define CX1000A_END_CODE      0xEF
#define CX1000A_BROADCAST_ID  0xFFFF
#define CX1000A_DEFAULT_ID    0x0001
#define CX1000A_DEFAULT_BAUD  9600

// CX1000A 指令码定义
#define CMD_PLAY              0x01
#define CMD_PAUSE             0x02
#define CMD_STOP              0x03
#define CMD_PREV              0x04
#define CMD_NEXT              0x05
#define CMD_VOLUME_SET        0x06
#define CMD_VOLUME_UP         0x07
#define CMD_VOLUME_DOWN       0x08
#define CMD_PLAY_INDEX        0x09
#define CMD_PLAY_FOLDER       0x0A
#define CMD_SET_EQ            0x0B
#define CMD_SET_LOOP          0x0C
#define CMD_SET_DEVICE        0x0D
#define CMD_QUERY_STATUS      0x0E
#define CMD_QUERY_VOLUME      0x0F
#define CMD_QUERY_TRACK       0x10
#define CMD_RESET             0x11

// EQ 模式
#define EQ_NORMAL             0x00
#define EQ_POP                0x01
#define EQ_ROCK               0x02
#define EQ_JAZZ               0x03
#define EQ_CLASSIC            0x04
#define EQ_BASS               0x05

// 循环模式
#define LOOP_ALL              0x00
#define LOOP_FOLDER           0x01
#define LOOP_ONE              0x02
#define LOOP_RANDOM           0x03

// 设备类型
#define DEVICE_U_DISK         0x00
#define DEVICE_SD_CARD        0x01
#define DEVICE_FLASH          0x02

class CX1000A {
public:
    // Constructor
#if defined(__AVR__) || defined(ESP8266) || defined(NRF52) || defined(NRF5) 
    CX1000A(SerialType serialType = SERIALTYPE_SW, uint8_t rxPin = 11, uint8_t txPin = 10, HardwareSerial* hwSerial = &Serial);
#elif defined(ESP32) 
    CX1000A(uint8_t rxPin = 16, uint8_t txPin = 17, HardwareSerial* hwSerial = &Serial2);
#endif
    ~CX1000A();

    // Public methods
    void begin(unsigned long baud = CX1000A_DEFAULT_BAUD);
    void setDeviceID(uint16_t deviceID);
    uint16_t getDeviceID();

    // 播放控制
    bool play();
    bool pause();
    bool stop();
    bool prev();
    bool next();
    bool playIndex(uint16_t index);
    bool playFolder(uint8_t folder, uint8_t file);

    // 音量控制
    bool setVolume(uint8_t volume);
    bool volumeUp();
    bool volumeDown();

    // EQ 设置
    bool setEQ(uint8_t eqMode);

    // 循环模式
    bool setLoopMode(uint8_t loopMode);

    // 设备选择
    bool setDevice(uint8_t device);

    // 查询指令
    bool queryStatus(char* respBuf, size_t bufLen, uint32_t timeoutMs = 500);
    bool queryVolume(uint8_t& volume, uint32_t timeoutMs = 500);
    bool queryTrack(uint16_t& track, uint32_t timeoutMs = 500);

    // 复位
    bool reset();

    // 发送自定义指令
    bool sendCommand(uint8_t cmd, const uint8_t* data = nullptr, uint8_t dataLen = 0);

    // 等待响应
    bool waitForResponse(char* respBuf, size_t bufLen, uint32_t timeoutMs = 500);

    // 检查是否有数据
    bool available();

private:
    SerialBase* _serial;
#if defined(__AVR__) || defined(ESP8266) || defined(NRF52) || defined(NRF5) 
    SoftwareSerial* _sw;
    HardwareSerial* _hw;
    SerialType _serialType;
    uint8_t _rxPin, _txPin;
#elif defined(ESP32)
    HardwareSerial* _hw;
    uint8_t _rxPin, _txPin;
    SerialType _serialType;
#endif
    uint16_t _deviceID;

    void sendFrame(uint8_t cmd, const uint8_t* data = nullptr, uint8_t dataLen = 0);
    uint8_t calculateChecksum(const uint8_t* data, uint8_t len);
    void flushInput();
};

#endif // CX1000A_H