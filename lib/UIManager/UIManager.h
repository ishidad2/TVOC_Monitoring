#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <M5Stack.h>

class UIManager {
private:
  static const unsigned long UI_UPDATE_INTERVAL = 1000;  // UI更新間隔（ミリ秒）
  unsigned long last_update_time;  // 最後の画面更新時間
  // 内部ヘルパーメソッド
  void setTextStyle(uint8_t size, uint16_t color);
  void drawWiFiStatus(bool connected, int x, int y);

public:
  UIManager();

  void init();
  void updateCountdown(int count);
  void clearInitArea();
  void updateValues(uint16_t tvoc, uint16_t eco2, bool sensor_connected, bool clean_air_detected, unsigned long remaining_time, bool wifi_connected);
  void showSensorError();
  void showButtonGuide();
  void clearStatusArea();
  void showBaselineReset();
  void showBaselineSaved(bool isCleanAir);
  void showBaselineValues(uint16_t eco2_base, uint16_t tvoc_base);
  void showMessage(const char* message, int delay_ms);
};

#endif