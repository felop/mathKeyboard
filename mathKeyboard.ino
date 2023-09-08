#include "Adafruit_Keypad.h"
#include <Keyboard.h>

#define osType 0 // macos: 0, windows: 1

#if (osType != 0) && (osType != 1)
#error Choose a valid ostype!
#endif

const byte ROWS = 4;
const byte COLS = 4;
byte rowPins[ROWS] = {2, 6, 7, 9};
byte colPins[COLS] = {8, 5, 27, 19};

char keys[ROWS][COLS] = {
  {'A', 'E', 'I', 'M'},
  {'B', 'F', 'J', 'N'},
  {'C', 'G', 'K', 'O'},
  {'D', 'H', 'L', 'P'}
};
unsigned long currentKeysState[4][4] = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};
String Symbols[2][4][4][3] =
{
  {
    {{"\\sum_{}^{}", "\\int_{}^{}", "\\prod{}^{}"}, {"\\sqrt{}", "ln()", "\\circ"}, {"\\frac{}{}", "\\lim_{ \\to }", "\\infty"}, {"\\cos", "\\sin", "\\tan"}},
    {{"\\vec{\\rm }", "|", "\\cdot"}, {"^", "\'", "\\bmatrix{ & @ & }"}, {"\\equiv", "\\geqslant", "\\leqslant"}, {"\\cosh", "\\sinh", "\\tanh"}},
    {{"\\pi", "\\exp()", ""}, {"\\R", "\\N", "\\forall"}, {"\\cap", "\\subset", "\\in"}, {"\\csc", "\\sec", "\\cot"}},
    {{"\\Delta", "\\lambda", "\\mu"}, {"\\theta", "\\phi", "\\omega"}, {"", "", ""}, {"", "", ""}}
  },
  {
    {{"\\sigma", "[]_a^b", "!"}, {"\\sqrt[]{}", "\\log_{}", "\\emptyset"}, {"\\frac{d}{dx}", "", ""}, {"\\arccos", "\\arcsin", "\\arctan"}},
    {{"\\widehat", "\\parallel", ""}, {"_", "\'\'", "\\binom{}{}"}, {"\\neq", "\\Longrightarrow", "\\Leftrightarrow"}, {"arcosh", "arsinh", "artanh"}},
    {{"", "", ""}, {"\\C", "\\Z", "\\exists"}, {"\\cup", "\\setminus", "\\notin"}, {"\\arccsc", "\\arcsec", "\\arccot"}},
    {{"\\Gamma", "\\Lambda", "\\epsilon"}, {"\\Theta", "\\Phi", "\\Omega"}, {"", "", ""}, {"", "", ""}}
  }
};

int holdTime = 500;
int hold, layer;

Adafruit_Keypad mathkeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
void setup() {
  Serial.begin(9600);
  mathkeypad.begin();
  switch (osType) {
    case 0:
      Keyboard.begin(KeyboardLayout_mc_FR);
      break;
    case 1:
      Keyboard.begin(KeyboardLayout_fr_FR);
      break;
  }
}

void loop() {
  mathkeypad.tick();
  while (mathkeypad.available()) {
    keypadEvent e = mathkeypad.read();
    //Serial.print((char)e.bit.KEY);

    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      uint8_t row = e.bit.ROW;
      uint8_t col = e.bit.COL;
      if (col == 3 && (row == 2 || row == 3)) {
        if (currentKeysState[2][3] != 0 || currentKeysState[3][3] != 0) {
          switch (osType) {
            case 0:
              Keyboard.press(KEY_LEFT_CTRL);delay(10);Keyboard.press(0xC0);
              break;
            case 1:
              Keyboard.press(KEY_LEFT_ALT);delay(10);Keyboard.press(0xb6);
              break;
          }
          delay(100);
          Keyboard.releaseAll();
        }
      }
      updateKeysState(true,e.bit.COL, e.bit.ROW);
    }
    else if (e.bit.EVENT == KEY_JUST_RELEASED) {
      uint8_t row = e.bit.ROW;
      uint8_t col = e.bit.COL;
      //Serial.print("Row: "); Serial.print(row);
      //Serial.print(" col: "); Serial.print(col);
      //Serial.println("");
      
      if (col != 3 || (row != 2 && row != 3)) {
        if (millis()-currentKeysState[row][col] < holdTime) {
          hold = 0;
        } else {
          hold = 1;
        }
        if (currentKeysState[2][3] != 0 && currentKeysState[3][3] == 0 ) {
          layer = 1;
        } else if (currentKeysState[3][3] != 0 && currentKeysState[2][3] == 0) {
          layer = 2;
        } else {
          layer = 0;
        }
        String str2print = Symbols[hold][col][row][layer];
        Keyboard.print(str2print);
      }
      updateKeysState(false,e.bit.COL, e.bit.ROW);
    }
  }
  delay(10);
}

void updateKeysState(bool pressed, int col , int row) {
  if (pressed) {
    currentKeysState[row][col] = millis();
  } else {
    currentKeysState[row][col] = 0;
  }
}
