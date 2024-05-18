#define ENABLE_ROTARY_ENCODER

#ifdef ENABLE_ROTARY_ENCODER
#include <MsTimer2.h>
#endif

#include <keymatrix.h>
#include <Mouse.h>

// キーマトリクスの行列数
#define ROW_NUM 3
#define COL_NUM 4

#define BANK_NUM 2

// 入出力ピン
const uint8_t ROW_PIN[ROW_NUM] = {2, 3, 4};         // 入力
const uint8_t COL_PIN[COL_NUM] = {21, 20, 19, 18};  // 出力

// キーマップ（バンク切り替え対応）
using KC = KeyCommand;
const KeyCommand KEY_MAP3[BANK_NUM * ROW_NUM * COL_NUM] = {
    // default map (bank0)
    KC(KEY_Q),             KC(KEY_W),                  KC(KEY_E),                 KC(MOD_L_CTRL, KEY_Y),
    KC(MOD_FUNC1, 0x00),   KC(KEY_LEFT_ALT),           KC(MOD_L_CTRL, KEY_S),     KC(KEY_SPACE),
    KC(MOD_L_CTRL, KEY_Z), KC(MOD_L_SHIFT, KEY_SPACE), KC(MOD_L_CTRL, KEY_SPACE), KC(KEY_F),

    // map with FUNC1 (bank1)
    KC(KEY_Q),             KC(KEY_W),             KC(KEY_MINUS),             KC(KEY_SQUOTE),
    KC(MOD_FUNC1, 0x00),   KC(MOD_L_CTRL, KEY_A), KC(MOD_L_CTRL, KEY_MINUS), KC(MOD_L_CTRL, KEY_V),
    KC(MOD_L_CTRL, KEY_Z), KC(MOD_L_CTRL, KEY_X), KC(MOD_L_CTRL, KEY_C),     KC(MOD_L_CTRL, KEY_EQUAL)
};

KeyMatrix km(KEY_MAP3, ROW_PIN, COL_PIN, ROW_NUM, COL_NUM);

#ifdef ENABLE_ROTARY_ENCODER
// ロータリーエンコーダ入力ピン
#define RE_PINA 10
#define RE_PINB 16

volatile uint8_t enc1c;  // ロータリーエンコーダ現在状態
volatile uint8_t enc1p;  // ロータリーエンコーダ前回状態
volatile int8_t counter1 = 0;  // 回転方向カウンタ

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

// 初期化処理
void setup() {
#ifdef DEBUG_SERIAL
  // デバッグ用シリアル通信
  Serial.begin(9600);
#endif

  // キーマトリクス初期化
  km.init();

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
  km.scan();
  //scanKeyMatrix();
#ifdef ENABLE_ROTARY_ENCODER
  // ロータリーエンコーダのスキャン
  scanRotaryEncoder();
#endif
}