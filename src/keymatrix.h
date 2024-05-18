#include <Keyboard.h>

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
#define KEY_SQUOTE 0x27 // '
#define KEY_PLUS 0x2b // +
#define KEY_MINUS 0x2d // -
#define KEY_SPACE ' '

// Modifier Keys
#define MOD_L_SHIFT 0x01
#define MOD_L_CTRL  0x02
#define MOD_L_ALT   0x04
#define MOD_FUNC1   0x08

// Key Command
struct KeyCommand {
  uint8_t mod;  // modifier key
  uint8_t key;  // key

  KeyCommand(uint8_t main_key);
  KeyCommand(uint8_t modifier_key, uint8_t main_key);
};

// Key Matrix
struct KeyMatrix {
    KeyCommand const *km; // key map km[banks_][rows_][cols_]
    uint8_t const *row_pin; // input pins
    uint8_t const *col_pin; // output pins
    const uint8_t rows; // number of rows
    const uint8_t cols; // number of rows

    uint8_t *cur_state; // current state (HIGH/LOW)
    uint8_t *pre_state; // purevious state (HIGH/LOW)
    uint8_t bank;

    KeyMatrix(const KeyCommand *key_matrix, const uint8_t *row_pin, const uint8_t *col_pin, uint8_t rows, uint8_t cols);
    void init();  // initialize
    void scan();  // scan and push/release keys

};