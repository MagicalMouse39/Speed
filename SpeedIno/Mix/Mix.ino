#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Dimensions in pixels
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Screen boxes
// Ylw: 0~7
// Blu: 8~31
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1 /* Reset: -1 = share arduino reset */);

BluetoothSerial SerialBT;

const int GEAR_SHIFT_DOWN = 16;
const int GEAR_SHIFT_UP = 17;

const int SPEED_FIRST = 18;
const int SPEED_SECOND = 19;

const int SPEED_MANUAL = 4;

const int RPM_DO = 25;

const int REVERSE_TST = 23;

const int THROTTLE_TST = 13;

const int EEPROM_SIZE = 10;

const String PWD = "7C9949957AD20F347F8412807DB24E08FBE07A3AA9DA6F10EC72860A7E7C158C";

enum Gear {
  PARKING = 'P',
  REVERSE = 'R',
  NEUTRAL = 'N',
  MANUAL = 'M',
  DRIVE = 'D'
};

Gear currentGear;

// 0-1-2
int currentSpeed;

bool gearShiftUpToggle;
bool gearShiftDownToggle;

bool btAuthed;

bool autoReverse;

void resetDisplay() {
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  String gears = "P R N M D";
  gears.replace((char) currentGear, ' ');
  display.println(gears);

  display.drawRect(0, 8, 128, 32 - 8, SSD1306_WHITE);
}

void setup() {
  // Serial setup
  Serial.begin(115200);

  SerialBT.begin("Speed");

  // EEPROM setup
  EEPROM.begin(EEPROM_SIZE);

  // Pin setup
  pinMode(GEAR_SHIFT_DOWN, INPUT);
  pinMode(GEAR_SHIFT_UP, INPUT);

  pinMode(SPEED_FIRST, OUTPUT);
  pinMode(SPEED_SECOND, OUTPUT);
  pinMode(SPEED_MANUAL, OUTPUT);

  pinMode(THROTTLE_TST, OUTPUT);
  pinMode(REVERSE_TST, OUTPUT);

  pinMode(RPM_DO, INPUT);
  
  // Variables setup
  gearShiftUpToggle = false;
  gearShiftDownToggle = false;

  // TODO: Change to false
  btAuthed = true;

  EEPROM.get(1, currentGear);
  autoReverse = EEPROM.readBool(0);

  // Gear setup
  activateGear();
  if (currentGear != NEUTRAL)
    digitalWrite(THROTTLE_TST, HIGH);

  // Display setup
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  displayGear();
  display.display();

  // Draw yellow rectangle
  // display.drawRect(0, 0, 128, 8, SSD1306_WHITE);

  // display.startscrollright(8, 16);
}

void gearShiftUp() {
  switch (currentGear) {
    case PARKING:
      currentGear = REVERSE;
      break;
    case REVERSE:
      currentGear = NEUTRAL;
      break;
    case NEUTRAL:
      currentGear = MANUAL;
      break;
    case MANUAL:
      currentGear = DRIVE;
      break;
    default:
      break;
  }

  EEPROM.put(1, currentGear);
  EEPROM.commit();
}

void gearShiftDown() {
  switch (currentGear) {
    case DRIVE:
      currentGear = MANUAL;
      break;
    case MANUAL:
      currentGear = NEUTRAL;
      break;
    case NEUTRAL:
      currentGear = REVERSE;
      break;
    case REVERSE:
      currentGear = PARKING;
      break;
    default:
      break;
  }

  EEPROM.put(1, currentGear);
  EEPROM.commit();
}

void displayGear() {
  resetDisplay();
  display.setTextSize(2);
  display.setCursor(4, 12);
  display.println((char) currentGear);
  display.display();
}

void deactivateGear(Gear g) {
  switch (g) {
    case REVERSE:
      digitalWrite(REVERSE_TST, LOW);
      break;
    case PARKING:
      // TODO: Deactivate parking brake
    case NEUTRAL:
      digitalWrite(THROTTLE_TST, HIGH);
      break;
    case MANUAL:
      digitalWrite(SPEED_MANUAL, LOW);
      break;
    case DRIVE:
      break;
  }
}

void activateGear() {
  switch (currentGear) {
    case REVERSE:
        digitalWrite(REVERSE_TST, HIGH);
        if (autoReverse) {
          Serial.println("AUTOREVERSE");
          digitalWrite(THROTTLE_TST, LOW);
          delay(1000);
          digitalWrite(THROTTLE_TST, HIGH);
        }
      break;
    case PARKING:
      // TODO: Activate parking brake
    case NEUTRAL:
      digitalWrite(THROTTLE_TST, LOW);
      break;
    case MANUAL:
      digitalWrite(SPEED_MANUAL, HIGH);
      break;
    case DRIVE:
      break;
  }
}

void handleGearShift() {
  bool up = digitalRead(GEAR_SHIFT_UP);
  bool down = digitalRead(GEAR_SHIFT_DOWN);

  if (!up)
    gearShiftUpToggle = false;

  if (!down)
    gearShiftDownToggle = false;

  if (!(up ^ down))
    return;

  Gear pre = currentGear;

  if (up && !gearShiftUpToggle) {
    gearShiftUp();
    gearShiftUpToggle = true;
  }

  if (down && !gearShiftDownToggle) {
    gearShiftDown();
    gearShiftDownToggle = true;
  }

  if (pre == currentGear)
    return;

  displayGear();
  deactivateGear(pre);
  activateGear();
}

bool contains(String a, String b) {
  return a.indexOf(b) >= 0;
}

void handleAutoSpeedShift(int rpm) {
  // TODO: Implement
}

void handleMotorSpeed() {
  int rpm = 0;

  if (currentGear == DRIVE)
    handleAutoSpeedShift(rpm);
}

void handleBluetooth() {
    if (SerialBT.available()) {
    String data = SerialBT.readString();
    Serial.println("BT recv: " + data);

    if (!btAuthed && !contains(data, "login")) {
      SerialBT.disconnect();
      return;
    }

    if (contains(data, "login")) {
      String pwd = data.substring(6);
      Serial.println("Login attempt, pwd: " + pwd);
      
      if (!pwd.equals(PWD)) {
        SerialBT.println("Bleah");
        SerialBT.disconnect();
        return;
      }

      SerialBT.println(":3");

      btAuthed = true;
    } else if (data.indexOf("setAutoReverse")) {
      autoReverse = !data.substring(15).equals(0);
      EEPROM.writeBool(0, autoReverse);
      EEPROM.commit();
      SerialBT.println("Done");
    }

    resetDisplay();
    display.setTextSize(2);
    display.setCursor(4, 12);
    display.println(data);
    display.display();
  }
}

void loop() {
  handleBluetooth();

  handleGearShift();

  handleMotorSpeed();

  delay(20);
}
