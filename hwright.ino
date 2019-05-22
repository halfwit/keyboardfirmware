#include <Keyboard.h>
#include <Keypad.h>

const byte ROWS = 6;
const byte COLS = 7;

int layout[ROWS][COLS] = {
  { KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, 0, 0},
  { KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, 0, 0},
  { KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, 0, 0},
  { KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, 0, 0},
  { 0, 0, 0, 0, 0, KEY_ESC, 0},
  { 0, 0, 0, 0, 0, 0, KEY_SPACE},
};

byte rows[ROWS] = { 7, 8, 9, 10, 11, 12};
byte cols[COLS] = { 0, 1, 2, 3, 4, 5, 6};
Keypad kpd = Keypad( makeKeymap(layout), cols, rows, COLS, ROWS );

// Hold the state of our mod+tap keys
int maybeesc = 0;
int maybespace = 0;

int key;

void setup() {
  kpd.setHoldTime(120);
}

void loop() {
  if (kpd.getKeys()) {
    for (int i = 0; i < 10; i++) {
      if ( kpd.key[i].stateChanged ) {
        key = layout[kpd.key[i].kcode % 6][kpd.key[i].kcode / 6];
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
              case KEY_ESC:
                maybeesc = 1;
                break;
              case KEY_SPACE:
                maybespace = 1;
                break;
              default:
                Keyboard.press(key);
            }
            break;
          case HOLD:
            switch (key) {
              case KEY_ESC:
                maybeesc = 0;
                Keyboard.press(MODIFIERKEY_GUI);
                break;
              case KEY_SPACE:
                maybespace = 0;
                Keyboard.press(MODIFIERKEY_CTRL);
                break;
              default:
                Keyboard.press(key);
            }
            return;
          case RELEASED:
            switch (key) {
              // Actual press of enter key (TAP)
              case KEY_ESC:
                if (maybeesc > 0){
                  Keyboard.press(key);
                  Keyboard.release(key);
                }
                Keyboard.release(MODIFIERKEY_GUI);
                maybeesc = 0;
                break;
              case KEY_SPACE:
                if (maybespace > 0) {
                  Keyboard.press(key);
                  Keyboard.release(key);
                }
                Keyboard.release(MODIFIERKEY_CTRL);
                maybespace = 0;
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
