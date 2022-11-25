void setup() {
  Serial.begin(9600);

  pinMode(A0, INPUT);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

}

void loop() {
  // Print A0 value in Volts
  // Serial.println(analogRead(A0) * (5.0 / 1023.0));

  digitalWrite(2, LOW);
  delay(3000);
  digitalWrite(2, HIGH);
  digitalWrite(3, LOW);
  delay(3000);
  digitalWrite(3, HIGH);

  delay(3000);
}
