#define TCCR1B_CAPTURE_RISING 0x07
#define TCCR1B_CAPTURE_FALLING 0x06
#define TCCR1B_OFF 0x00

#define TCNT1_MULTIPLIER 60

void setup() {
  Serial.begin(115200);

  // Set TIMER1 normal mode operation
  TCCR1A = 0x00;
  TCCR1B = 0x00;

  pinMode(5, INPUT);
}

void loop() {
  long speed = 0;
  
  // Reset TIMER1 count
  TCCR1B = TCCR1B_OFF;
  TCNT1 = 0;

  // Enable TIMER1 external clock input (D5)
  TCCR1B = TCCR1B_CAPTURE_RISING;

  // Wait 1 second
  delay(1000);
  // Disable TIMER1 external clock input
  TCCR1B = TCCR1B_OFF;

  // Calculate speed
  speed = ( TCNT1 * TCNT1_MULTIPLIER );

  Serial.println(TCNT1);
}