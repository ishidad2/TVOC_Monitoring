#include "SensorManager.h"
#include <math.h>

SensorManager::SensorManager() :
  sgp(nullptr),
  preferences(nullptr),
  tvoc_value(0),
  eco2_value(0),
  condition_flag(false),
  stable_condition_start(0),
  last_auto_check_time(0),
  last_read_time(0),
  last_baseline_save_time(0),
  demo_phase(0.0) {
}

bool SensorManager::init(Adafruit_SGP30* sensor, Preferences* prefs) {
  sgp = sensor;
  preferences = prefs;

  if (!sgp->begin()) {
    return false;
  }

  // ベースラインの読み込みを試みる
  loadBaseline(sgp, preferences);
  return true;
}

void SensorManager::update(bool sensor_connected) {
  // 1秒ごとにセンサーを更新
  if (millis() - last_read_time < SENSOR_UPDATE_INTERVAL) {
    return;
  }

  last_read_time = millis();

  if (sensor_connected) {
    // 実際のセンサーから読み取り
    if (!sgp->IAQmeasure()) {
      Serial.println("Measurement failed");
      return;
    }
    tvoc_value = sgp->TVOC;
    eco2_value = sgp->eCO2;

    // 定期的なベースライン保存処理
    checkPeriodicBaselineSave();
  } else {
    // デモデータの生成
    generateDemoData();
  }
}

void SensorManager::generateDemoData() {
  // デモデータの生成（サイン波を使って自然なデータ変動を模倣）
  demo_phase += 0.05;  // 位相を徐々に変化

  // TVOC: 200〜800の範囲でサイン波変動
  tvoc_value = 500 + (int)(300 * sin(demo_phase));

  // eCO2: 800〜1600の範囲でコサイン波変動（TVOCとは少し位相をずらす）
  eco2_value = 1200 + (int)(400 * cos(demo_phase * 0.7));
}

bool SensorManager::saveBaseline(Adafruit_SGP30* sensor, Preferences* prefs) {
  uint16_t eco2_base, tvoc_base;

  if (!sensor->getIAQBaseline(&eco2_base, &tvoc_base)) {
    Serial.println("Failed to get baseline readings");
    return false;
  }

  prefs->begin("sgp30", false);
  prefs->putUShort("eco2_base", eco2_base);
  prefs->putUShort("tvoc_base", tvoc_base);
  prefs->end();

  Serial.println("Baseline saved: eCO2=" + String(eco2_base) + ", TVOC=" + String(tvoc_base));
  return true;
}

bool SensorManager::loadBaseline(Adafruit_SGP30* sensor, Preferences* prefs) {
  prefs->begin("sgp30", false);
  uint16_t eco2_base = prefs->getUShort("eco2_base", 0);
  uint16_t tvoc_base = prefs->getUShort("tvoc_base", 0);
  prefs->end();

  if (eco2_base == 0 || tvoc_base == 0) {
    Serial.println("No valid baseline saved");
    return false;
  }

  if (!sensor->setIAQBaseline(eco2_base, tvoc_base)) {
    Serial.println("Failed to set baseline values");
    return false;
  }

  Serial.println("Baseline loaded: eCO2=" + String(eco2_base) + ", TVOC=" + String(tvoc_base));
  return true;
}

bool SensorManager::resetBaseline(Adafruit_SGP30* sensor) {
  if (!sensor->IAQinit()) {
    return false;
  }

  condition_flag = false;
  Serial.println("Baseline reset");
  return true;
}

bool SensorManager::getBaseline(Adafruit_SGP30* sensor, uint16_t* eco2_base, uint16_t* tvoc_base) {
  return sensor->getIAQBaseline(eco2_base, tvoc_base);
}

void SensorManager::checkAutoBaseline() {
  // AUTO_CHECK_INTERVALごとに自動ベースラインチェック
  if (millis() - last_auto_check_time < AUTO_CHECK_INTERVAL) {
    return;
  }

  last_auto_check_time = millis();

  // クリーンエアの条件をチェック
  if (isGoodConditionForBaseline(eco2_value, tvoc_value)) {
    saveBaseline(sgp, preferences);
  }
}

void SensorManager::checkPeriodicBaselineSave() {
  // 12時間ごとに定期保存
  if (millis() - last_baseline_save_time >= BASELINE_AUTO_SAVE_INTERVAL) {
    last_baseline_save_time = millis();
    
    // ベースラインを保存
    if (saveBaseline(sgp, preferences)) {
      Serial.println("Periodic baseline save completed");
    } else {
      Serial.println("Periodic baseline save failed");
    }
  }
}

bool SensorManager::isGoodConditionForBaseline(uint16_t eco2_value, uint16_t tvoc_value) {
  // eCO2が400-500ppmの範囲内かつTVOCが100ppb以下
  bool isCleanNow = (eco2_value >= ECO2_MIN && eco2_value <= ECO2_MAX && tvoc_value <= TVOC_MAX);

  if (isCleanNow) {
    if (!condition_flag) {
      stable_condition_start = millis();
      condition_flag = true;
      Serial.println("Clean air condition detected, monitoring stability...");
    } else if (millis() - stable_condition_start >= STABLE_TIME) {
      condition_flag = false; // リセット
      Serial.println("Clean air condition stable for required time");
      return true;
    }
  } else {
    if (condition_flag) {
      Serial.println("Clean air condition lost");
      condition_flag = false;
    }
  }

  return false;
}

bool SensorManager::isCleanAirCondition() const {
  return (eco2_value >= ECO2_MIN && eco2_value <= ECO2_MAX && tvoc_value <= TVOC_MAX);
}

unsigned long SensorManager::getCleanAirRemainingTime() const {
  if (!condition_flag) {
    return 0;
  }

  unsigned long elapsed = millis() - stable_condition_start;
  if (elapsed >= STABLE_TIME) {
    return 0;
  }

  return (STABLE_TIME - elapsed) / 1000; // 秒単位
}