#include "UIManager.h"

UIManager::UIManager() : last_update_time(0) {
  // 初期化
}

void UIManager::init() {
  // タイトル表示
  setTextStyle(2, WHITE);
  M5.Lcd.setCursor(80, 0);
  M5.Lcd.print("TVOC TEST");

  // カウントダウン表示の準備
  M5.Lcd.setCursor(30, 80);
  M5.Lcd.print("Initialization...");
}

void UIManager::setTextStyle(uint8_t size, uint16_t color) {
  M5.Lcd.setTextSize(size);
  M5.Lcd.setTextColor(color);
}

void UIManager::updateCountdown(int count) {
  M5.Lcd.fillRect(20, 120, 60, 30, BLACK);
  M5.Lcd.drawNumber(count, 20, 120, 2);
}

void UIManager::clearInitArea() {
  M5.Lcd.fillRect(0, 40, 320, 200, BLACK);
}

void UIManager::updateValues(uint16_t tvoc, uint16_t eco2, bool sensor_connected, bool clean_air_detected, unsigned long remaining_time, bool wifi_connected) {
  // 更新間隔チェック（1秒ごとに更新）
  if (millis() - last_update_time < UI_UPDATE_INTERVAL) {
    return;
  }

  last_update_time = millis();

  // 値の表示エリアをクリア
  M5.Lcd.fillRect(0, 0, 319, 25, TFT_BLACK);

  // TVOC値とeCO2値を表示
  char buffer[60];
  sprintf(buffer, "TVOC:%dppb eCO2:%dppm", tvoc, eco2);
  setTextStyle(2, WHITE);
  M5.Lcd.setCursor(5, 5);
  M5.Lcd.print(buffer);

  // WiFi接続状態を表示
  drawWiFiStatus(wifi_connected, 300, 5);

  // デモモード表示
  if (!sensor_connected) {
    setTextStyle(1, YELLOW);
    M5.Lcd.setCursor(240, 25);
    M5.Lcd.print("DEMO");
  }

  // クリーンエア検出中の表示（必要に応じてコメント解除）
  /*
  if (clean_air_detected && sensor_connected) {
    setTextStyle(1, GREEN);
    char clean_air_msg[50];
    sprintf(clean_air_msg, "Clean air detected: %lu s", remaining_time);
    M5.Lcd.setCursor(5, 25);
    M5.Lcd.print(clean_air_msg);
  }
  */
}

void UIManager::showMessage(const char* message, int delay_ms) {
  clearStatusArea();
  setTextStyle(1, WHITE);
  M5.Lcd.setCursor(5, 25);
  M5.Lcd.print(message);
  if (delay_ms > 0) {
    delay(delay_ms);
    clearStatusArea();
  }
}

void UIManager::showSensorError() {
  M5.Lcd.fillRect(0, 40, 320, 60, BLACK);
  setTextStyle(2, RED);
  M5.Lcd.setCursor(30, 50);
  M5.Lcd.print("Sensor not found");
  M5.Lcd.setCursor(30, 80);
  M5.Lcd.print("Running in DEMO mode");
  delay(2000);
  M5.Lcd.fillRect(0, 40, 320, 100, BLACK);
}

void UIManager::showButtonGuide() {
  setTextStyle(1, WHITE);
  M5.Lcd.setCursor(10, 220);
  M5.Lcd.print("BtnA: Reset  BtnB: Save Baseline  BtnC: Show Baseline");
}

void UIManager::clearStatusArea() {
  M5.Lcd.fillRect(5, 25, 315, 15, BLACK);
}

void UIManager::showBaselineReset() {
  clearStatusArea();
  setTextStyle(1, RED);
  M5.Lcd.setCursor(5, 25);
  M5.Lcd.print("Baseline Reset - New calibration needed");
}

void UIManager::showBaselineSaved(bool isCleanAir) {
  clearStatusArea();
  setTextStyle(1, isCleanAir ? GREEN : YELLOW);
  M5.Lcd.setCursor(5, 25);

  if (isCleanAir) {
    M5.Lcd.print("Baseline Saved in Good Condition");
  } else {
    M5.Lcd.print("Warning: Not Clean Air but Baseline Saved");
  }
}

void UIManager::showBaselineValues(uint16_t eco2_base, uint16_t tvoc_base) {
  clearStatusArea();
  setTextStyle(1, YELLOW);
  char baseline_info[60];
  sprintf(baseline_info, "Baseline:eCO2=%uTVOC=%u", eco2_base, tvoc_base);
  M5.Lcd.setCursor(5, 25);
  M5.Lcd.print(baseline_info);
}

// WiFi接続状態を表示するメソッド
void UIManager::drawWiFiStatus(bool connected, int x, int y) {
  // WiFiアイコンを描画
  int iconSize = 15;
  int color = connected ? GREEN : RED;

  // 基本のアイコン枠
  M5.Lcd.drawRoundRect(x, y, iconSize, iconSize, 2, color);

  if (connected) {
    // 接続時はWiFiシグナルを表示（3本線）
    int barWidth = 2;
    int barSpace = 1;
    int startX = x + 3;

    for (int i = 0; i < 3; i++) {
      int barHeight = (i + 1) * 3;
      int barY = y + iconSize - barHeight - 2;
      M5.Lcd.fillRect(startX + (i * (barWidth + barSpace)), barY, barWidth, barHeight, color);
    }
  } else {
    // 未接続時はX印を表示
    M5.Lcd.drawLine(x+3, y+3, x+iconSize-3, y+iconSize-3, color);
    M5.Lcd.drawLine(x+iconSize-3, y+3, x+3, y+iconSize-3, color);
  }
}