#include <Keyboard.h>
#include <Keypad.h>

const byte ROWS = 7;
const byte COLS = 8;

// The wiring on this side was done backwards, fun!
int layout[ROWS][COLS] = {
  { KEY_0, KEY_9, KEY_8, KEY_7, KEY_6, 0, 0, 0},
  { KEY_P, KEY_O, KEY_I, KEY_U, KEY_Y, 0, 0, 0},
  { KEY_SEMICOLON, KEY_L, KEY_K, KEY_J, KEY_H, 0, 0, 0},
  { KEY_SLASH, KEY_PERIOD, KEY_COMMA, KEY_M, KEY_N, 0, 0, 0},
  { 0, 0, 0, 0, 0, KEY_TAB, 0, 0 },
  { 0, 0, 0, 0, 0, 0, KEY_ENTER, 0 },
  { 0, 0, 0, 0, 0, 0, 0, KEY_BACKSPACE }
};

// s/rows/cols/, but it still works.
byte rows[ROWS] = { 8, 9, 10, 11, 12, 13, 14};
byte cols[COLS] = { 0, 1, 2, 3, 4, 5, 6, 7 };
Keypad kpd = Keypad( makeKeymap(layout), cols, rows, COLS, ROWS );

// Hold the state of our mod+tap keys
int maybeenter = 0;
int maybetab = 0;

int key;

void setup() {
  kpd.setHoldTime(100);
}

void loop() {
  if (kpd.getKeys()) {
    for (int i = 0; i < 10; i++) {
      if ( kpd.key[i].stateChanged ) {
        key = layout[kpd.key[i].kcode % 7][kpd.key[i].kcode / 7];
        switch (kpd.key[i].kstate) {
          case PRESSED:
            // Ideally, we'd check state of shiftmask here as well
            // so we could get the earlier-than-hold events
            // As the keyboard this is running on is done in many pieces
            // and it's impossible to inspect the mask state from the underlying system
            // bothering to have it active for one side is only going to cause
            // behavioral inconsistency, which at scale and over time will
            // lead to pathologies and odd strain proliferations
            switch (key) {
              case KEY_ENTER:
                maybeenter = 1;
                break;
              case KEY_TAB:
                maybetab = 1;
                break;
              default:
                Keyboard.press(key);
            }
            break;
          case HOLD:
            switch (key) {
              case KEY_ENTER:
                maybeenter = 0;
                Keyboard.press(MODIFIERKEY_LEFT_SHIFT);
                break;
              case KEY_TAB:
                maybetab = 0;
                Keyboard.press(MODIFIERKEY_LEFT_ALT);
                break;
              default:
                Keyboard.press(key);
            }
            return;
          case RELEASED:
            switch (key) {
              // Actual press of enter key (TAP)
              case KEY_ENTER:
                if (maybeenter > 0){
                  Keyboard.press(key);
                  Keyboard.release(key);
                }
                Keyboard.release(MODIFIERKEY_LEFT_SHIFT);
                maybeenter = 0;
                break;
              case KEY_TAB:
                if (maybetab > 0) {
                  Keyboard.press(key);
                  Keyboard.release(key);
                }
                Keyboard.release(MODIFIERKEY_LEFT_ALT);
                maybetab = 0;
                break;
              default:
                Keyboard.release(key);
            }
            break;
          case IDLE:
            break;
        }
      }
    }
  }
}
