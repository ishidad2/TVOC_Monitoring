#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Adafruit_SGP30.h>
#include <Preferences.h>

class SensorManager {
public:
  SensorManager();

  // センサー初期化
  bool init(Adafruit_SGP30* sensor, Preferences* prefs);

  // センサー値の更新
  void update(bool sensor_connected);

  // ベースライン関連
  bool saveBaseline(Adafruit_SGP30* sensor, Preferences* prefs);
  bool loadBaseline(Adafruit_SGP30* sensor, Preferences* prefs);
  bool resetBaseline(Adafruit_SGP30* sensor);
  bool getBaseline(Adafruit_SGP30* sensor, uint16_t* eco2_base, uint16_t* tvoc_base);
  void checkAutoBaseline();
  void checkPeriodicBaselineSave();

  // センサー値取得
  uint16_t getTVOC() const { return tvoc_value; }
  uint16_t getECO2() const { return eco2_value; }

  // クリーンエア状態チェック
  bool isCleanAirCondition() const;
  bool isCleanAirDetected() const { return condition_flag; }
  unsigned long getCleanAirRemainingTime() const;

private:
  // 定数定義
  static const uint16_t ECO2_MIN = 400;      // eCO2最小値 (ppm)
  static const uint16_t ECO2_MAX = 500;      // eCO2最大値 (ppm)
  static const uint16_t TVOC_MAX = 100;      // TVOC最大値 (ppb)
  static const unsigned long STABLE_TIME = 600000; // 条件が満たされるべき時間 (ms) - 10分
  static const unsigned long AUTO_CHECK_INTERVAL = 10000; // 自動チェック間隔 (ms)
  static const unsigned long SENSOR_UPDATE_INTERVAL = 1000; // センサー更新間隔 (ms)
  static const unsigned long BASELINE_AUTO_SAVE_INTERVAL = 43200000; // ベースライン自動保存間隔 (ms) - 12時間

  // センサー関連
  Adafruit_SGP30* sgp;
  Preferences* preferences;

  // 測定値
  uint16_t tvoc_value;
  uint16_t eco2_value;

  // クリーンエア判定用
  bool condition_flag;
  unsigned long stable_condition_start;
  unsigned long last_auto_check_time;
  unsigned long last_read_time;
  unsigned long last_baseline_save_time;

  // デモ用
  float demo_phase;

  // 内部メソッド
  bool isGoodConditionForBaseline(uint16_t eco2_value, uint16_t tvoc_value);
  void generateDemoData();
};

#endif // SENSOR_MANAGER_H