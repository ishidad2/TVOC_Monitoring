#include <M5Stack.h>
#include "Adafruit_SGP30.h"
#include <Preferences.h>
#include "GraphManager.h"
#include "SensorManager.h"
#include "UIManager.h"
#include <WiFi.h>
#include <SD.h>

// 定数定義
#define INIT_COUNTDOWN 15     // 初期化カウントダウン秒数
#define WIFI_CONFIG_FILE "/wifi_config.txt"  // SDカード上の設定ファイル
#define WIFI_CONNECT_TIMEOUT 10000  // WiFi接続タイムアウト（ミリ秒）
#define LOOP_DELAY 10         // メインループの遅延（ミリ秒）
#define SD_CS_PIN 4           // M5Stack標準のSDカードCSピン

// グローバル変数
Adafruit_SGP30 sgp;
Preferences preferences;
SensorManager sensor_manager;
GraphManager graph_manager;
UIManager ui_manager;
bool sensor_connected = false;
bool wifi_connected = false;
unsigned long last_millis = 0;

// SDカード初期化関数 - 診断テストで成功した方法を使用
bool initSDCard() {
  // 方法1: 直接SPI
  if (SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card initialized with direct SPI method");
    return true;
  }

  // 方法2: 低速SPI
  Serial.println("Trying low-speed SPI method...");
  SD.end(); // 前回の初期化をリセット
  delay(200);

  SPIClass spi = SPIClass(VSPI);
  spi.begin();
  if (SD.begin(SD_CS_PIN, spi, 400000)) { // 400kHzの低速で試行
    Serial.println("SD Card initialized with low-speed SPI method");
    return true;
  }

  Serial.println("All SD initialization methods failed");
  return false;
}

// WiFi設定を読み込む関数
bool loadWifiConfig(String &ssid, String &password) {
  // SDカードはすでに初期化されているはず

  File configFile = SD.open(WIFI_CONFIG_FILE, FILE_READ);
  if (!configFile) {
    Serial.println("Config file not found");
    return false;
  }

  ssid = configFile.readStringUntil('\n');
  ssid.trim();
  password = configFile.readStringUntil('\n');
  password.trim();

  configFile.close();
  return (ssid.length() > 0);
}

// WiFiに接続する関数
bool connectToWifi() {
  String ssid, password;

  if (!loadWifiConfig(ssid, password)) {
    Serial.println("WiFi config load failed");
    return false;
  }

  Serial.printf("WiFi connecting: %s\n", ssid.c_str());
  WiFi.begin(ssid.c_str(), password.c_str());

  // 接続待機（タイムアウト付き）
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startTime > WIFI_CONNECT_TIMEOUT) {
      Serial.println("\nWiFi connection timeout");
      return false;
    }
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

// WiFi接続状態をチェックする関数
void checkWiFiStatus() {
  // 接続が切れていれば再接続を試みる
  if (wifi_connected && WiFi.status() != WL_CONNECTED) {
    wifi_connected = false;
    Serial.println("WiFi connection lost");
  } else if (!wifi_connected && WiFi.status() == WL_CONNECTED) {
    wifi_connected = true;
    Serial.println("WiFi connected");
  }
}

void setup() {
  Serial.begin(115200); // 通信速度を115200bpsに変更
  Serial.println("\n=== Air Quality Monitor Starting ===");

  // M5Stackの初期化
  M5.begin(true, false, true, true);
  M5.Lcd.fillScreen(BLACK);

  // UIマネージャの初期化
  ui_manager.init();

  // グラフマネージャの初期化
  graph_manager.init();

  // SDカードの初期化 - 改良版を使用
  Serial.println("Initializing SD card...");
  bool sdCardOK = initSDCard();
  if (!sdCardOK) {
    Serial.println("SD card initialization failed");
    ui_manager.showMessage("SD Card Error!", 2000);
    // SDカードエラーでも続行
  }

  // WiFi接続（SDカード初期化成功時のみ）
  if (sdCardOK) {
    wifi_connected = connectToWifi();
    ui_manager.showMessage(wifi_connected ? "WiFi connected!" : "WiFi connection failed", 2000);
  } else {
    wifi_connected = false;
  }

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
    delay(LOOP_DELAY);
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
    sensor_manager.getCleanAirRemainingTime(),
    wifi_connected
  );

  // WiFi接続状態をチェック
  checkWiFiStatus();

  // ボタン処理
  handleButtons();

  delay(LOOP_DELAY);
}