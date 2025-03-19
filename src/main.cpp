#include <M5Stack.h>
#include "Adafruit_SGP30.h"
#include <Preferences.h>
#include "GraphManager.h"
#include "SensorManager.h"
#include "UIManager.h"

// 定数定義
#define INIT_COUNTDOWN 15     // 初期化カウントダウン秒数
#define BASELINE_CHECK_MS 10000  // ベースライン自動チェック間隔 (ms)

// グローバル変数
Adafruit_SGP30 sgp;
Preferences preferences;
long last_millis = 0;
bool sensor_connected = false;
SensorManager sensor_manager;
GraphManager graph_manager;
UIManager ui_manager;

void setup() {
  // M5Stackの初期化
  M5.begin(true, false, true, true);
  M5.Lcd.fillScreen(BLACK);

  // UIマネージャの初期化
  ui_manager.init();

  // グラフマネージャの初期化
  graph_manager.init();

  // センサーの初期化
  sensor_connected = sensor_manager.init(&sgp, &preferences);
  if (!sensor_connected) {
    ui_manager.showSensorError();
  }

  // ボタン操作ガイドを表示
  ui_manager.showButtonGuide();
}

// ボタン処理関数
void handleButtons() {
  M5.update();

  // Aボタン：ベースラインリセット
  if (M5.BtnA.wasPressed() && sensor_connected) {
    if (sensor_manager.resetBaseline(&sgp)) {
      ui_manager.showBaselineReset();
    }
  }

  // Bボタン：手動ベースライン保存
  if (M5.BtnB.wasPressed() && sensor_connected) {
    bool isCleanAir = sensor_manager.isCleanAirCondition();
    sensor_manager.saveBaseline(&sgp, &preferences);
    ui_manager.showBaselineSaved(isCleanAir);
  }

  // Cボタン：現在のベースライン値を表示
  if (M5.BtnC.wasPressed() && sensor_connected) {
    uint16_t eco2_base, tvoc_base;
    if (sensor_manager.getBaseline(&sgp, &eco2_base, &tvoc_base)) {
      ui_manager.showBaselineValues(eco2_base, tvoc_base);
    }
  }
}

void loop() {
  static int countdown = INIT_COUNTDOWN;
  static bool initialized = false;

  // 初期化カウントダウン処理
  if (countdown > 0) {
    if (millis() - last_millis > 1000) {
      last_millis = millis();
      countdown--;
      ui_manager.updateCountdown(countdown);
    }
    delay(10);
    return;
  }

  // 初期化完了後の一度だけの処理
  if (!initialized) {
    ui_manager.clearInitArea();
    graph_manager.drawFrames();
    initialized = true;
  }

  // センサーデータ更新（1秒ごと）
  sensor_manager.update(sensor_connected);

  // グラフ更新（1秒ごと）
  graph_manager.update(sensor_manager.getTVOC(), sensor_manager.getECO2());

  // 画面表示更新（1秒ごと）
  ui_manager.updateValues(
    sensor_manager.getTVOC(),
    sensor_manager.getECO2(),
    sensor_connected,
    sensor_manager.isCleanAirDetected(),
    sensor_manager.getCleanAirRemainingTime()
  );

  // ベースライン自動保存チェック
  sensor_manager.checkAutoBaseline();

  // ボタン処理
  handleButtons();

  delay(10);
}