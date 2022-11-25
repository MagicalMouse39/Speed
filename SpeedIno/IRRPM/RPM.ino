const int RPM_DO = 25;

int delaysToSec = 0;
int motorRounds = 0;

void setup() {
  Serial.begin(115200);

  pinMode(RPM_DO, INPUT);
}

void loop() {
  if (delaysToSec == 1000) {
    delaysToSec = 0;
    Serial.println(motorRounds * 60);
    motorRounds = 0;
  }

  motorRounds += !digitalRead(RPM_DO);
  delaysToSec++;
  delay(1);
}
