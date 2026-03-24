/*
 * 示例名称：90_SerialMonitor_AllFunctions
 * 示例功能：通过串口监视器交互式演示模块的主要功能。
 * 支持命令：
 *   1  播放根目录 00001
 *   2  播放根目录 00010
 *   p  播放
 *   a  暂停
 *   s  停止
 *   n  下一曲
 *   b  上一曲
 *   c  选中根目录 00003，不立即播放
 *   f  指定路径播放 /chabo/00002.*
 *   i  指定路径插播 /chabo/00001.*
 *   e  结束插播
 *   +  音量加
 *   -  音量减
 *   v  直接设置音量为 20
 *   r  读取音量
 *   m  设置单曲循环
 *   g  设置随机循环
 *   o  读取当前播放模式
 *   q  读取播放状态和曲目名称
 *   z  组合播放 01 + 02 + 03
 *   x  保存参数
 *   d  恢复默认参数
 *   t  重启模块
 *   h  打印帮助菜单
 * 作者：YFROBOT
 * 日期：2026-03-24
 */

#include <Yfrobot_MP3V3.h>

YfrobotMP3V3 player;

/**
 * @brief 根据当前板卡自动初始化模块。
 * @return `true` 表示初始化成功；`false` 表示初始化失败。
 */
static bool beginModule() {
#if defined(ESP32)
  return player.beginHardwareSerial(Serial2, 16, 17);
#elif defined(ARDUINO_AVR_MEGA2560)
  return player.beginHardwareSerial(Serial1);
#elif defined(ARDUINO_ARCH_AVR)
  return player.beginSoftwareSerial(11, 10);
#else
  return false;
#endif
}

/**
 * @brief 打印串口交互菜单。
 */
static void printMenu() {
  Serial.println(F("==== YFROBOT MP3 V3 串口交互菜单 ===="));
  Serial.println(F("1 : 播放根目录 00001"));
  Serial.println(F("2 : 播放根目录 00010"));
  Serial.println(F("p : 播放"));
  Serial.println(F("a : 暂停"));
  Serial.println(F("s : 停止"));
  Serial.println(F("n : 下一曲"));
  Serial.println(F("b : 上一曲"));
  Serial.println(F("c : 选中根目录 00003，但不播放"));
  Serial.println(F("f : 指定路径播放 /chabo/00002.*"));
  Serial.println(F("i : 指定路径插播 /chabo/00001.*"));
  Serial.println(F("e : 结束插播"));
  Serial.println(F("+ : 音量加"));
  Serial.println(F("- : 音量减"));
  Serial.println(F("v : 直接设置音量为 20"));
  Serial.println(F("r : 读取当前音量"));
  Serial.println(F("m : 设置单曲循环"));
  Serial.println(F("g : 设置随机循环"));
  Serial.println(F("o : 读取当前播放模式"));
  Serial.println(F("q : 读取播放状态和曲目名称"));
  Serial.println(F("z : 组合播放 01 + 02 + 03"));
  Serial.println(F("x : 保存参数"));
  Serial.println(F("d : 恢复默认参数"));
  Serial.println(F("t : 重启模块"));
  Serial.println(F("h : 打印帮助菜单"));
}

/**
 * @brief 打印当前播放模式与循环次数。
 */
static void printPlaybackMode() {
  YfrobotMP3V3::PlaybackModeSettings settings;
  if (player.readPlaybackMode(settings)) {
    Serial.print(F("播放模式："));
    Serial.println(static_cast<uint8_t>(settings.mode));
    Serial.print(F("循环次数："));
    Serial.println(settings.repeatCount);
  } else {
    Serial.println(F("读取播放模式失败。"));
  }
}

/**
 * @brief 打印当前播放状态与曲目名称。
 */
static void printPlayInfo() {
  YfrobotMP3V3::PlayState state;
  char trackName[32];

  if (player.readPlayState(state)) {
    Serial.print(F("播放状态："));
    Serial.println(static_cast<uint8_t>(state));
  } else {
    Serial.println(F("读取播放状态失败。"));
  }

  if (player.readCurrentTrackName(trackName, sizeof(trackName))) {
    Serial.print(F("当前曲目："));
    Serial.println(trackName);
  } else {
    Serial.println(F("当前曲目名称暂无返回。"));
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);

  if (!beginModule()) {
    Serial.println(F("模块初始化失败。"));
    while (true) {
      delay(1000);
    }
  }

  player.setDeviceId(YfrobotMP3V3::kBroadcastDeviceId);
  printMenu();
}

void loop() {
  if (!Serial.available()) {
    return;
  }

  const char cmd = static_cast<char>(Serial.read());

  if (cmd == '1') player.playTrack(1);
  if (cmd == '2') player.playTrack(10);
  if (cmd == 'p') player.play();
  if (cmd == 'a') player.pause();
  if (cmd == 's') player.stop();
  if (cmd == 'n') player.nextTrack();
  if (cmd == 'b') player.previousTrack();
  if (cmd == 'c') player.selectTrack(3);
  if (cmd == 'f') player.playPath(YfrobotMP3V3::STORAGE_FLASH, "/chabo/00002.*");
  if (cmd == 'i') player.insertAdvert(YfrobotMP3V3::STORAGE_FLASH, "/chabo/00001.*");
  if (cmd == 'e') player.stopAdvert();
  if (cmd == '+') player.volumeUp();
  if (cmd == '-') player.volumeDown();
  if (cmd == 'v') player.setVolume(20);
  if (cmd == 'm') player.setPlaybackMode(YfrobotMP3V3::PLAY_MODE_LOOP_SINGLE, 0);
  if (cmd == 'g') player.setPlaybackMode(YfrobotMP3V3::PLAY_MODE_LOOP_RANDOM, 0);
  if (cmd == 'z') player.playCombination("010203");
  if (cmd == 'x') player.saveSettings();
  if (cmd == 'd') player.restoreFactorySettings();
  if (cmd == 't') player.reboot();
  if (cmd == 'h') printMenu();

  if (cmd == 'r') {
    uint8_t volume = 0;
    if (player.readVolume(volume)) {
      Serial.print(F("当前音量："));
      Serial.println(volume);
    } else {
      Serial.println(F("读取音量失败。"));
    }
  }

  if (cmd == 'o') {
    printPlaybackMode();
  }

  if (cmd == 'q') {
    printPlayInfo();
  }
}
