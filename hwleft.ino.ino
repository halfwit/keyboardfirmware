#include <Keyboard.h>
#include <Keypad.h>

const byte ROWS = 7;
const byte COLS = 8;
int layout[ROWS][COLS] = {
  { KEY_0, KEY_9, KEY_8, KEY_7, KEY_6, 0, 0, 0},
  { KEY_P, KEY_O, KEY_I, KEY_U, KEY_Y, 0, 0, 0},
  { KEY_SEMICOLON, KEY_L, KEY_K, KEY_J, KEY_H, 0, 0, 0},
  { KEY_SLASH, KEY_PERIOD, KEY_COMMA, KEY_M, KEY_N, 0, 0, 0},
  { 0, 0, 0, 0, 0, KEY_TAB, 0, 0 },
  { 0, 0, 0, 0, 0, 0, KEY_ENTER, 0 },
  { 0, 0, 0, 0, 0, 0, 0, KEY_BACKSPACE }
};
byte rows[ROWS] = { 8, 9, 10, 11, 12, 13, 14};
byte cols[COLS] = { 0, 1, 2, 3, 4, 5, 6, 7 };
Keypad kpd = Keypad( makeKeymap(layout), cols, rows, COLS, ROWS );
//TODO: Load all 6 possible keys
int key, key1, key2, key3, key4, key5, key6;
int altmask = 0;
int shiftmask = 0;

void clear_keys() {
  key1 = key2 = key3 = key4 = key5 = key6 = 0;
}

void setup() {
  kpd.setHoldTime(100);
  clear_keys();
}
// 6 key NKRO
void load_keys(int key) {
  if (!key1) {
    key1 = key;
  } else if (!key2) {
    key2 = key;
  } else if (!key3) {
    key3 = key;
  } else if (!key4) {
    key4 = key;
  } else if (!key5) {
    key5 = key;
  } else if (!key6) {
    key6 = key;
  }
}

void send_keys() {
  Keyboard.set_key1(key1);
  Keyboard.set_key2(key2);
  Keyboard.set_key3(key3);
  Keyboard.set_key4(key4);
  Keyboard.set_key5(key5);
  Keyboard.set_key6(key6);
  Keyboard.send_now();
}
void loop() {
  if (kpd.getKeys()) {
    for (int i = 0; i < 6; i++) {
      if ( kpd.key[i].stateChanged ) {
        key = layout[kpd.key[i].kcode % 7][kpd.key[i].kcode / 7];
        switch (kpd.key[i].kstate) {
          case PRESSED:
            if (key == KEY_ENTER) {
              shiftmask = 1;
              break;
            }
            if (key == KEY_TAB) {
              altmask = 1;
              break;
            }
            load_keys(key);
            break;
          case HOLD:
            clear_keys();
            if (key == KEY_ENTER) {
              shiftmask = 2;
              Keyboard.set_modifier(MODIFIERKEY_LEFT_SHIFT);
              break;
            }
            if (key == KEY_TAB) {
              altmask = 2;
              Keyboard.set_modifier(MODIFIERKEY_LEFT_ALT);
              break;
            }
            // BUG(halfwit): Keys will stick on if we don't return here
            load_keys(key);
            send_keys();
            break;
          case RELEASED:
            if (key == KEY_ENTER) {
              if (shiftmask != 2)
                load_keys(KEY_ENTER);
              Keyboard.set_modifier(0);
              shiftmask = 0;
            }
            if (key == KEY_TAB) {
              if (altmask != 2)
                load_keys(KEY_TAB);
              Keyboard.set_modifier(0);
              altmask = 0;
            }
            break;
          case IDLE:
            clear_keys();
            Keyboard.set_modifier(0);
            return;
        }
      }
    }
    send_keys();
    clear_keys();
  }
}
