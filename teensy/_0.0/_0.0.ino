void setup() {
  // put your setup code here, to run once:
  pinMode(21, INPUT);
  pinMode(11, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
if (digitalRead(21)){
    digitalWrite(11,HIGH);
    Keyboard.set_key1(KEY_G);
  }
  else{
    digitalWrite(11,LOW);
    }
  Keyboard.send_now();
  Keyboard.set_key1(0);
  delay(50);
}
