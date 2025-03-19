#include "UIManager.h"

UIManager::UIManager() : last_update_time(0) {
}

void UIManager::init() {
  // タイトル表示（println() は許容）
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(80, 0);
  M5.Lcd.println("TVOC TEST");

  // カウントダウン表示の準備
  M5.Lcd.setCursor(30, 80);
  M5.Lcd.println("Initialization...");
}

void UIManager::updateCountdown(int count) {
  M5.Lcd.fillRect(20, 120, 60, 30, BLACK);
  M5.Lcd.drawNumber(count, 20, 120, 2);
}

void UIManager::clearInitArea() {
  M5.Lcd.fillRect(0, 40, 320, 200, BLACK);
}

void UIManager::updateValues(uint16_t tvoc, uint16_t eco2, bool sensor_connected, bool clean_air_detected, unsigned long remaining_time) {
  // 更新間隔チェック（1秒ごとに更新）
  if (millis() - last_update_time < 1000) {
    return;
  }

  last_update_time = millis();

  // 値の表示エリアをクリア
  M5.Lcd.fillRect(0, 0, 319, 25, TFT_BLACK);

  // TVOC値とeCO2値を表示
  char buffer[60];
  sprintf(buffer, "TVOC:%dppb eCO2:%dppm", tvoc, eco2);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(5, 5);
  M5.Lcd.print(buffer);

  // デモモード表示
  if (!sensor_connected) {
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setCursor(240, 25);
    M5.Lcd.print("DEMO");
  }

  // // クリーンエア検出中の表示
  // if (clean_air_detected && sensor_connected) {
  //   M5.Lcd.setTextColor(GREEN);
  //   char clean_air_msg[50];
  //   sprintf(clean_air_msg, "Clean air detected: %lu s", remaining_time);
  //   M5.Lcd.setCursor(5, 25);
  //   M5.Lcd.print(clean_air_msg);
  // }
}

void UIManager::showSensorError() {
  M5.Lcd.fillRect(0, 40, 320, 60, BLACK);
  M5.Lcd.setCursor(30, 50);
  M5.Lcd.println("Sensor not found");
  M5.Lcd.setCursor(30, 80);
  M5.Lcd.println("Running in DEMO mode");
  delay(2000);
  M5.Lcd.fillRect(0, 40, 320, 100, BLACK);
}

void UIManager::showButtonGuide() {
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(10, 220);
  M5.Lcd.println("BtnA: Reset  BtnB: Save Baseline  BtnC: Show Baseline");
  M5.Lcd.setTextSize(2);
}

void UIManager::clearStatusArea() {
  M5.Lcd.fillRect(5, 25, 315, 15, BLACK);
}

void UIManager::showBaselineReset() {
  clearStatusArea();
  M5.Lcd.setTextColor(RED);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(5, 25);
  M5.Lcd.print("Baseline Reset - New calibration needed");
}

void UIManager::showBaselineSaved(bool isCleanAir) {
  clearStatusArea();

  if (isCleanAir) {
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(5, 25);
    M5.Lcd.print("Baseline Saved in Good Condition");
  } else {
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(5, 25);
    M5.Lcd.print("Warning: Not Clean Air but Baseline Saved");
  }
}

void UIManager::showBaselineValues(uint16_t eco2_base, uint16_t tvoc_base) {
  clearStatusArea();
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(1);
  char baseline_info[60];
  sprintf(baseline_info, "Baseline:eCO2=%uTVOC=%u", eco2_base, tvoc_base);
  M5.Lcd.setCursor(5, 25);
  M5.Lcd.print(baseline_info);
}