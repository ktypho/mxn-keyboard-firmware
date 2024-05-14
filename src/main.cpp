//#define DEBUG
#define ENABLE_ROTARY_ENCODER

#ifdef ENABLE_ROTARY_ENCODER
#include <MsTimer2.h>
#endif

#include <Keyboard.h>
#include <Mouse.h>

#ifdef ENABLE_ROTARY_ENCODER
// ロータリーエンコーダ入力ピン
#define RE_PINA 10
#define RE_PINB 16
#endif

volatile uint8_t enc1c;  // ロータリーエンコーダ現在状態
volatile uint8_t enc1p;  // ロータリーエンコーダ前回状態
volatile int8_t counter1 = 0;  // 回転方向カウンタ

// ASCII CODE
#define KEY_A 0x61
#define KEY_B 0x62
#define KEY_C 0x63
#define KEY_D 0x64
#define KEY_E 0x65
#define KEY_F 0x66
#define KEY_G 0x67
#define KEY_H 0x68
#define KEY_I 0x69
#define KEY_J 0x6a
#define KEY_K 0x6b
#define KEY_L 0x6c
#define KEY_M 0x6d
#define KEY_N 0x6e
#define KEY_O 0x6f
#define KEY_P 0x70
#define KEY_Q 0x71
#define KEY_R 0x72
#define KEY_S 0x73
#define KEY_T 0x74
#define KEY_U 0x75
#define KEY_V 0x76
#define KEY_W 0x77
#define KEY_X 0x78
#define KEY_Y 0x79
#define KEY_Z 0x7a
#define KEY_EQUAL 0x3d // =
#define KEY_MINUS 0x2d // -

// 修飾キー定義
#define MOD_L_SHIFT 0x01
#define MOD_L_CTRL  0x02
#define MOD_L_ALT   0x04
#define MOD_FUNC1   0x08

// キーコマンド（修飾キー＋キー）
struct KeyCommand {
  uint8_t mod;  // 修飾キー
  uint8_t key;  // キー

  KeyCommand(uint8_t main_key) {
    this->mod = 0x00;
    this->key = main_key;
  }
  KeyCommand(uint8_t modifier_key, uint8_t main_key) {
    this->mod = modifier_key;
    this->key = main_key;
  }
};

// キーマトリクスの行列数
#define ROW_NUM 3
#define COL_NUM 4

// 入出力ピン
const uint8_t ROW_PIN[ROW_NUM] = {2, 3, 4};         // 入力
const uint8_t COL_PIN[COL_NUM] = {21, 20, 19, 18};  // 出力

// キーマップ（修飾キー対応）
using KC = KeyCommand;
const KeyCommand KEY_MAP2[ROW_NUM][COL_NUM] = {
  {KC(KEY_Q),             KC(KEY_W),             KC(KEY_E),             KC(MOD_L_CTRL, KEY_Y)},
  {KC(MOD_L_CTRL, KEY_A), KC(MOD_L_CTRL, KEY_S), KC(KEY_LEFT_CTRL),     KC(MOD_L_CTRL, KEY_V)},
  {KC(MOD_L_CTRL, KEY_Z), KC(MOD_L_CTRL, KEY_X), KC(MOD_L_CTRL, KEY_C), KC(KEY_F)            }
};
// キーマップ
/*
const uint8_t KEY_MAP[ROW_NUM][COL_NUM] = {
  {0x61, 0x62, 0x63, KEY_BACKSPACE},
  {KEY_LEFT_SHIFT, 0x66, 0x67, 0x68},
  {KEY_LEFT_CTRL, 0x6a, 0x6b, 0x6c}
};
*/

// 現在状態、前回状態
uint8_t current_state[ROW_NUM][COL_NUM];   // キーの現在状態
uint8_t previous_state[ROW_NUM][COL_NUM];  // キーの前回状態

uint8_t i, j;  // ループ用

#ifdef ENABLE_ROTARY_ENCODER
// ロータリーエンコーダ読み込み
void encoder() {
  enc1c = (digitalRead(RE_PINA)<<1) | (digitalRead(RE_PINB));  // ピン状態
  if(!(enc1c == enc1p)) {  // 前回状態から変化しているとき
    uint8_t enc1 = enc1p << 2 | enc1c;
    if( enc1==0b0001) {
      counter1++;
    }
    else if(enc1 == 0b0100) {
      counter1--;
    }
    enc1p=enc1c;
  }
}

// ロータリーエンコーダの回転方向に応じて操作を実行
void scanRotaryEncoder () {
  if (counter1 > 0) {      // CWなら
    //Mouse.move(0, 0, -1);  // スクロールダウン
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_SHIFT);
    Keyboard.press(KEY_EQUAL);
    delay(1);
    Keyboard.releaseAll();
    counter1--;
  }
  else if (counter1 < 0){  // CCWなら
    //Mouse.move(0, 0, 1);   // スクロールアップ
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_SHIFT);
    Keyboard.press(KEY_MINUS);
    Keyboard.releaseAll();
    counter1++;
  }
  delay(1);
}
#endif

void scanKeyMatrix() {
  for (i = 0; i < COL_NUM; i++) {   // 各行について：
    digitalWrite(COL_PIN[i], LOW);  //   COL_PIN の出力を LOW にする

    for (j = 0; j < ROW_NUM; j++) {                   // 各列について：
      current_state[j][i] = digitalRead(ROW_PIN[j]);  //   現在状態を確認
      if (current_state[j][i] != previous_state[j][i]) {   // 前回状態と異なるかつ、
        if (current_state[j][i] == LOW) {                  // 現在状態 = LOW（スイッチ押下）なら：
          if ((KEY_MAP2[j][i].mod & MOD_L_SHIFT) == MOD_L_SHIFT) {
              Keyboard.press(KEY_LEFT_SHIFT);  // LEFT SHIFT
          }
          if ((KEY_MAP2[j][i].mod & MOD_L_CTRL) == MOD_L_CTRL) {
              Keyboard.press(KEY_LEFT_CTRL);   // LEFT CTRL
          }
          if ((KEY_MAP2[j][i].mod & MOD_L_SHIFT) == MOD_L_ALT) {
              Keyboard.press(KEY_LEFT_ALT);    // LEFT ALT
          }
          Keyboard.press(KEY_MAP2[j][i].key);                   //   当該キーを press
        }
        else {                                            // 現在状態 = HIGH（スイッチオフ）なら： 
          if ((KEY_MAP2[j][i].mod & MOD_L_SHIFT) == MOD_L_SHIFT) {
              Keyboard.release(KEY_LEFT_SHIFT);  // LEFT SHIFT
          }
          if ((KEY_MAP2[j][i].mod & MOD_L_CTRL) == MOD_L_CTRL) {
              Keyboard.release(KEY_LEFT_CTRL);   // LEFT CTRL
          }
          if ((KEY_MAP2[j][i].mod & MOD_L_SHIFT) == MOD_L_ALT) {
              Keyboard.release(KEY_LEFT_ALT);    // LEFT ALT
          }
          Keyboard.release(KEY_MAP2[j][i].key);                 //   当該キーを release
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

#ifdef ENABLE_ROTARY_ENCODER
  // ロータリーエンコーダ入力ピン初期化
  pinMode(RE_PINA, INPUT_PULLUP);
  pinMode(RE_PINB, INPUT_PULLUP);

  //割り込み処理の設定
  MsTimer2::set(1, encoder);
  MsTimer2::start();

  Mouse.begin();
#endif
}

// メインループ
void loop() {
  // キーマトリクスのスキャン
  scanKeyMatrix();
#ifdef ENABLE_ROTARY_ENCODER
  // ロータリーエンコーダのスキャン
  scanRotaryEncoder();
#endif
}