#include <EEPROM.h>

const int EEPROM_SIZE = 10;

void setup() {
  Serial.begin(115200);
  
  EEPROM.begin(EEPROM_SIZE);

  for (int i = 0; i < EEPROM_SIZE; i++)
    EEPROM.write(i, i);
  
  EEPROM.commit();
}

void loop() {
  Serial.println(EEPROM_SIZE);

  for (int i = 0; i < EEPROM_SIZE; i++)
    Serial.println(EEPROM.read(i));
  delay(2000);
}
