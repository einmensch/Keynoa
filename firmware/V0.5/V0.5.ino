#include <Keyboard.h>

bool state = 0;
int input[4] = {10, 9, 8, 7};
int output[5] = {2, 3, 4, 5, 6};
char matrix[5][4] = {
  {'0', '&', '}', '/'}, //Esc /   *   -
  {'7' , '8', '9', ']'}, //7   8   9   +
  {'4' , '5', '6',}, //4   5   6
  {'1' , '2', '3', '\n'}, //1   2   3   Enter
  {'0', '0', '.', '0'}   //0       Del
};
int z = 0;
char lkeystate = NULL;
int countdown = 0;
int countdownreset = 50;
void setup() {
  //Serial.begin(9600)
  Keyboard.begin();
  pinMode(output[0], OUTPUT);
  pinMode(output[1], OUTPUT);
  pinMode(output[2], OUTPUT);
  pinMode(output[3], OUTPUT);
  pinMode(output[4], OUTPUT);
  pinMode(input[0], INPUT_PULLUP);
  pinMode(input[1], INPUT_PULLUP);
  pinMode(input[2], INPUT_PULLUP);
  pinMode(input[3], INPUT_PULLUP);
  //        pinMode(input[4],INPUT_PULLUP);
  digitalWrite(output[0], HIGH);
  digitalWrite(output[1], HIGH);
  digitalWrite(output[2], HIGH);
  digitalWrite(output[3], HIGH);
  digitalWrite(output[4], HIGH);
}

void loop() {
  for (int i = 0; i < sizeof(output) / sizeof(int); i++) {
    //          Serial.print("Micro");
    digitalWrite(output[i], LOW);
    for (int j = 0; j < sizeof(input) / sizeof(int); j++) {
      state = digitalRead(input[j]);
      if (state == LOW) {
        if (matrix[i][j] != lkeystate) {
          //                Serial.print("g");
          Serial.print(matrix[i][j]);
          Keyboard.press(matrix[i][j]);
        }
      }
      else{
        Keyboard.release(matrix[i][j]);
        }
      }
    digitalWrite(output[i], HIGH);
  }
  delay(50);
}
