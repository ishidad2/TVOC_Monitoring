#include "GraphManager.h"

GraphManager::GraphManager() :
  tvoc_y_prev(0),
  eco2_y_prev(0),
  first_plot(true),
  last_update_time(0) {

  // TVOC用グラフ設定
  tvocGraph = {
    18, 40, 300, 80, 1000, 0, 500, MAGENTA, &graph_tvoc
  };

  // eCO2用グラフ設定
  eco2Graph = {
    18, 130, 300, 80, 5000, 400, 2700, CYAN, &graph_eco2
  };
}

GraphManager::~GraphManager() {
  // スプライトの解放
  graph_tvoc.deleteSprite();
  graph_eco2.deleteSprite();
}

void GraphManager::init() {
  // スプライトの初期化
  setupGraph(tvocGraph);
  setupGraph(eco2Graph);
}

void GraphManager::setupGraph(GraphConfig& graph) {
  graph.sprite->setColorDepth(8);
  graph.sprite->createSprite(graph.width, graph.height);
  graph.sprite->fillSprite(TFT_BLACK);
}

void GraphManager::drawFrames() {
  // グラフフレームの描画
  drawGraphFrame(tvocGraph);
  drawGraphFrame(eco2Graph);

  // 垂直グリッド線の描画
  drawVerticalGridLines();
}

void GraphManager::drawGraphFrame(const GraphConfig& graph) {
  // グラフ枠を描画
  M5.Lcd.drawRoundRect(graph.xPos - 1, graph.yPos, graph.width + 2, graph.height + 2, 2, WHITE);

  // Y軸ラベル背景をクリア
  M5.Lcd.fillRect(0, graph.yPos + 3, 16, 10, BLACK);
  M5.Lcd.fillRect(0, graph.yPos + graph.height/2, 16, 10, BLACK);
  M5.Lcd.fillRect(0, graph.yPos + graph.height - 11, 16, 10, BLACK);

  // Y軸ラベルの描画（drawString() の代わりに setCursor() と print() を使用）
  M5.Lcd.setTextSize(1.5);
  M5.Lcd.setTextColor(graph.color);
  M5.Lcd.setCursor(0, graph.yPos + 3);
  M5.Lcd.print(graph.maxValue);
  M5.Lcd.setCursor(0, graph.yPos + graph.height / 2);
  M5.Lcd.print(graph.midValue);
  M5.Lcd.setCursor(0, graph.yPos + graph.height - 11);
  M5.Lcd.print(graph.minValue);

  // 水平グリッド線
  M5.Lcd.drawLine(graph.xPos, graph.yPos + 13, graph.xPos + graph.width, graph.yPos + 13, DARKGREEN);
  M5.Lcd.drawLine(graph.xPos, graph.yPos + graph.height/2 + 10, graph.xPos + graph.width, graph.yPos + graph.height/2 + 10, DARKGREEN);
  M5.Lcd.drawLine(graph.xPos, graph.yPos + graph.height - 1, graph.xPos + graph.width, graph.yPos + graph.height - 1, DARKGREEN);
}

void GraphManager::update(uint16_t tvoc_value, uint16_t eco2_value) {
  // 更新間隔チェック
  if (millis() - last_update_time < GRAPH_UPDATE_INTERVAL) {
    return;
  }

  last_update_time = millis();

  // グラフの更新
  updateGraph(tvocGraph, tvoc_value, tvoc_y_prev);
  updateGraph(eco2Graph, eco2_value, eco2_y_prev);

  // 初回プロットフラグをリセット
  if (first_plot) {
    first_plot = false;
  }

  // グラフ枠と目盛りの再描画
  redrawGraphFrameAndLabels();
}

void GraphManager::updateGraph(const GraphConfig& graph, uint16_t value, uint16_t& y_prev) {
  // グラフスプライトをスクロール
  graph.sprite->scroll(-1, 0);

  // 右端の位置
  const uint16_t Xpos = graph.width - 1;

  // Y位置を計算
  uint16_t y_pos = calculateYPosition(value, graph);

  // 点をプロット
  if (first_plot) {
    graph.sprite->fillRect(Xpos, y_pos, 1, 1, graph.color);
  } else {
    graph.sprite->drawLine(Xpos-1, y_prev, Xpos, y_pos, graph.color);
  }

  // 現在のY位置を保存
  y_prev = y_pos;

  // スプライトを画面に描画
  graph.sprite->pushSprite(graph.xPos, graph.yPos);
}

uint16_t GraphManager::calculateYPosition(uint16_t value, const GraphConfig& graph) {
  uint16_t y_pos = 0;

  // 値をグラフの高さにマッピング（上下反転）
  if (value >= graph.minValue && value <= graph.maxValue) {
    float range = graph.maxValue - graph.minValue;
    float normalized = (value - graph.minValue) / range;
    y_pos = graph.height - int(normalized * graph.height);
  } else if (value < graph.minValue) {
    y_pos = graph.height;  // 下限値
  } else {
    y_pos = 0;  // 上限値
  }

  return y_pos;
}

void GraphManager::drawVerticalGridLines() {
  // グラフ共通の垂直グリッド線
  int grid_spacing = tvocGraph.width / 5;

  for (int i = 1; i < 5; i++) {
    int x = tvocGraph.xPos + i * grid_spacing;

    // TVOC用垂直グリッド線
    M5.Lcd.drawLine(x, tvocGraph.yPos + 1, x, tvocGraph.yPos + tvocGraph.height - 1, DARKGREEN);

    // eCO2用垂直グリッド線
    M5.Lcd.drawLine(x, eco2Graph.yPos + 1, x, eco2Graph.yPos + eco2Graph.height - 1, DARKGREEN);
  }
}

void GraphManager::redrawGraphFrameAndLabels() {
  drawGraphFrame(tvocGraph);
  drawGraphFrame(eco2Graph);
  drawVerticalGridLines();
}
