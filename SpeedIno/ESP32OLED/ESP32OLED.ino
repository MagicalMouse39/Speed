#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Dimensions in pixels
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1 /* Reset: -1 = share arduino reset */);

// Ylw: 0~7
// Blu: 8~31

void setup() {
  Serial.begin(9600);

  pinMode(2, OUTPUT);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  // Draw yellow rectangle
  // display.drawRect(0, 0, 128, 8, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("PRNMD"));

  display.drawRect(0, 8, 128, 32 - 8, SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(4, 12);
  display.println(F("TEST"));

  // display.startscrollright(8, 16);

  display.display();
}

void loop() {
}
