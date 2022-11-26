hw_timer_t *rpmTimer = NULL;

const int SENSOR_PIN = 25;

int pulseCount = 0;
int timerCount = 0;
bool timerFlag = false;

void IRAM_ATTR onTimer() {
  bool check = digitalRead(SENSOR_PIN) == HIGH;
  if (check && !timerFlag) {
    timerCount++;
    timerFlag = true;
  }
  if (!check)
    timerFlag = false;
}

void IRAM_ATTR intCounter() {
  pulseCount++;
}

void setup() {
  Serial.begin(115200);
  
  rpmTimer = timerBegin(0, 80, true);
  timerAttachInterrupt(rpmTimer, &onTimer, true);
  timerAlarmWrite(rpmTimer, 185, true);
  timerAlarmEnable(rpmTimer);

  pinMode(SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(SENSOR_PIN, &intCounter, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.printf("PulseCount: %d\n", pulseCount);
  Serial.printf("TimerCount: %d\n", timerCount);
  Serial.println("----------------");
  pulseCount = 0;
  timerCount = 0;
  delay(1000);
}
