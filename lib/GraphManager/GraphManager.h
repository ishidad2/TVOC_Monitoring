#ifndef GRAPH_MANAGER_H
#define GRAPH_MANAGER_H

#include <M5Stack.h>

// グラフ描画用の構造体
struct GraphConfig {
  int xPos;                // X座標位置
  int yPos;                // Y座標位置
  int width;               // 幅
  int height;              // 高さ
  int maxValue;            // 最大値
  int minValue;            // 最小値
  int midValue;            // 中間値
  uint16_t color;          // グラフの色
  TFT_eSprite* sprite;     // グラフスプライト
};

class GraphManager {
public:
  GraphManager();
  ~GraphManager();

  // 初期化
  void init();

  // グラフフレーム描画
  void drawFrames();

  // グラフ更新
  void update(uint16_t tvoc_value, uint16_t eco2_value);

private:
  // 定数定義
  static const int GRAPH_UPDATE_INTERVAL = 1000;  // グラフ更新間隔（ミリ秒）

  // グラフスプライト
  TFT_eSprite graph_tvoc = TFT_eSprite(&M5.Lcd);
  TFT_eSprite graph_eco2 = TFT_eSprite(&M5.Lcd);

  // グラフ設定
  GraphConfig tvocGraph;
  GraphConfig eco2Graph;

  // 内部変数
  uint16_t tvoc_y_prev;
  uint16_t eco2_y_prev;
  bool first_plot;
  unsigned long last_update_time;

  // 内部メソッド
  void setupGraph(GraphConfig& graph);
  void drawGraphFrame(const GraphConfig& graph);
  void updateGraph(const GraphConfig& graph, uint16_t value, uint16_t& y_prev);
  uint16_t calculateYPosition(uint16_t value, const GraphConfig& graph);
  void drawVerticalGridLines();
  void redrawGraphFrameAndLabels();
};

#endif // GRAPH_MANAGER_H