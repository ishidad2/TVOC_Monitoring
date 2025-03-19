#ifndef CONFIG_H
#define CONFIG_H

// センサー関連の定数
#define ECO2_MIN 400       // eCO2最小値 (ppm)
#define ECO2_MAX 500       // eCO2最大値 (ppm)
#define TVOC_MAX 100       // TVOC最大値 (ppb)
#define STABLE_TIME 600000 // 条件が満たされるべき時間 (ms) - 10分

// グラフ描画関連の定数
#define GRAPH_UPDATE_INTERVAL 1000 // グラフ更新間隔（ミリ秒）

// UIに関する定数
#define STATUS_X 5         // ステータス表示X座標
#define STATUS_Y 25        // ステータス表示Y座標
#define STATUS_TEXT_SIZE 1 // ステータステキストサイズ

// TVOC用のグラフ設定
#define TVOC_GRAPH_X 18    // TVOCグラフX座標
#define TVOC_GRAPH_Y 40    // TVOCグラフY座標
#define TVOC_GRAPH_W 300   // TVOCグラフ幅
#define TVOC_GRAPH_H 80    // TVOCグラフ高さ
#define TVOC_MAX_VAL 1000  // TVOCグラフ最大値
#define TVOC_MIN_VAL 0     // TVOCグラフ最小値
#define TVOC_MID_VAL 500   // TVOCグラフ中間値

// eCO2用のグラフ設定
#define ECO2_GRAPH_X 18    // eCO2グラフX座標
#define ECO2_GRAPH_Y 130   // eCO2グラフY座標
#define ECO2_GRAPH_W 300   // eCO2グラフ幅
#define ECO2_GRAPH_H 80    // eCO2グラフ高さ
#define ECO2_MAX_VAL 5000  // eCO2グラフ最大値
#define ECO2_MIN_VAL 400   // eCO2グラフ最小値
#define ECO2_MID_VAL 2700  // eCO2グラフ中間値

// その他の設定
#define INIT_COUNTDOWN 15  // 初期化カウントダウン秒数
#define AUTO_CHECK_MS 10000 // ベースライン自動チェック間隔 (ms)

#endif // CONFIG_H