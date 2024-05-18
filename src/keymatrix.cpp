#include <keymatrix.h>

KeyCommand::KeyCommand(uint8_t main_key) {
    this->mod = 0x00;
    this->key = main_key;
};

KeyCommand::KeyCommand(uint8_t modifier_key, uint8_t main_key) {
    this->mod = modifier_key;
    this->key = main_key;
};

KeyMatrix::KeyMatrix(const KeyCommand *key_matrix, const uint8_t *row_pin, const uint8_t *col_pin, uint8_t nrows, uint8_t ncols)
    : km(key_matrix), row_pin(row_pin), col_pin(col_pin), rows(nrows), cols(ncols)
{
    cur_state = new uint8_t[rows * cols];
    pre_state = new uint8_t[rows * cols];
    
    bank = 0;
};

void KeyMatrix::init() {
    for (uint8_t i = 0; i < rows; i++) pinMode(row_pin[i], INPUT_PULLUP);
    for (uint8_t i = 0; i < cols; i++) pinMode(col_pin[i], OUTPUT);

    memset(cur_state, HIGH, sizeof(*cur_state));
    memset(pre_state, HIGH, sizeof(*pre_state));
    
  Keyboard.begin();
}

// km[bank][j][i] = km[(bank * rows * cols) + (j * cols) + i]

void KeyMatrix::scan() {
    for (uint8_t i = 0; i < cols; i++) {
        // for each colmns:
        digitalWrite(col_pin[i], LOW);  // set output pin LOW

        for (uint8_t j = 0; j < rows; j++) {
            // for each rows:
            cur_state[j * cols + i] = digitalRead(row_pin[j]);  // read input pin
        
            if (cur_state[j * cols + i] != pre_state[j * cols + i]) {
                if (cur_state[j * cols + i] == LOW) {  // if HIGH -> LOW (released -> pushed)
                    // push modifier keys
                    if ((km[(bank * rows * cols) + (j * cols) + i].mod & MOD_L_SHIFT) == MOD_L_SHIFT) {
                        Keyboard.press(KEY_LEFT_SHIFT);  // LEFT SHIFT
                    }
                    if ((km[(bank * rows * cols) + (j * cols) + i].mod & MOD_L_CTRL) == MOD_L_CTRL) {
                        Keyboard.press(KEY_LEFT_CTRL);   // LEFT CTRL
                    }
                    if ((km[(bank * rows * cols) + (j * cols) + i].mod & MOD_L_SHIFT) == MOD_L_ALT) {
                        Keyboard.press(KEY_LEFT_ALT);    // LEFT ALT
                    }

                    // push main key
                    if ((km[(bank * rows * cols) + (j * cols) + i].mod & MOD_FUNC1) == MOD_FUNC1) {
                        // if function key pushed, just switch bank
                        bank = 1;
                    }
                    else if (km[(bank * rows * cols) + (j * cols) + i].key != 0x00) {
                        // push main key
                        Keyboard.press(km[(bank * rows * cols) + (j * cols) + i].key);
                    }
                }
                else {  // if LOW -> HIGH (pushed -> released)
                    // release modifier keys
                    if ((km[(bank * rows * cols) + (j * cols) + i].mod & MOD_L_SHIFT) == MOD_L_SHIFT) {
                        Keyboard.release(KEY_LEFT_SHIFT);  // LEFT SHIFT
                    }
                    if ((km[(bank * rows * cols) + (j * cols) + i].mod & MOD_L_CTRL) == MOD_L_CTRL) {
                        Keyboard.release(KEY_LEFT_CTRL);   // LEFT CTRL
                    }
                    if ((km[(bank * rows * cols) + (j * cols) + i].mod & MOD_L_SHIFT) == MOD_L_ALT) {
                        Keyboard.release(KEY_LEFT_ALT);    // LEFT ALT
                    }

                    // release main key
                    if ((km[(bank * rows * cols) + (j * cols) + i].mod & MOD_FUNC1) == MOD_FUNC1) {
                        // if function key pushed, just RESET bank
                        bank = 0;
                    }
                    else {
                        // release main key
                        Keyboard.release(km[(bank * rows * cols) + (j * cols) + i].key);
                    }
                }
            }

            pre_state[j * cols + i] = cur_state[j * cols + i];  // update previous state
        }
        
        digitalWrite(col_pin[i], HIGH);  // set output pin HIGH
    }

};