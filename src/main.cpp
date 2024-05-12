#define DEBUG

//#include <Arduino.h>
#include <Keyboard.h>
#include <Mouse.h>

// キーマトリクスの行列数
#define ROW_NUM 3
#define COL_NUM 4

// 入出力ピン
const uint8_t ROW_PIN[ROW_NUM] = {2, 3, 4};         // 入力
const uint8_t COL_PIN[COL_NUM] = {21, 20, 19, 18};  // 出力

// キーマップ
const uint8_t KEY_MAP[ROW_NUM][COL_NUM] = {
  {0x61, 0x62, 0x63, KEY_BACKSPACE},
  {KEY_LEFT_SHIFT, 0x66, 0x67, 0x68},
  {KEY_LEFT_CTRL, 0x6a, 0x6b, 0x6c}
};

// 現在状態、前回状態
uint8_t current_state[ROW_NUM][COL_NUM];   // キーの現在状態
uint8_t previous_state[ROW_NUM][COL_NUM];  // キーの前回状態

uint8_t i, j;  // ループ用

void scanKeyMatrix() {
  for (i = 0; i < COL_NUM; i++) {   // 各行について：
    digitalWrite(COL_PIN[i], LOW);  //   COL_PIN の出力を LOW にする

    for (j = 0; j < ROW_NUM; j++) {                   // 各列について：
      current_state[j][i] = digitalRead(ROW_PIN[j]);  //   現在状態を確認
      if (current_state[j][i] != previous_state[j][i]) {   // 前回状態と異なるかつ、
        if (current_state[j][i] == LOW) {                  // 現在状態 = LOW（スイッチ押下）なら：
          Keyboard.press(KEY_MAP[j][i]);                   //   当該キーを press
        }
        else {                                             // 現在状態 = HIGH（スイッチオフ）なら：
          Keyboard.release(KEY_MAP[j][i]);                 //   当該キーを release
        }

        previous_state[j][i] = current_state[j][i];  // 前回状態を更新
      }
    }

    digitalWrite(COL_PIN[i], HIGH);  // COL_PINの出力を HIGH にする。
  }
}

// 初期化処理
void setup() {
#ifdef DEBUG
  // デバッグ用シリアル通信
  Serial.begin(9600);
#endif

  // 入出力ピン初期化
  for (i = 0; i < ROW_NUM; i++) pinMode(ROW_PIN[i], INPUT_PULLUP);
  for (i = 0; i < COL_NUM; i++) pinMode(COL_PIN[i], OUTPUT);

  // 現在状態、前回状態の初期化
  memset(current_state, HIGH, sizeof(current_state));
  memset(previous_state, HIGH, sizeof(previous_state));

  // キーボードのエミュレーション開始
  Keyboard.begin();
}

// メインループ
void loop() {
  // キーマトリクスのスキャン
  scanKeyMatrix();
}