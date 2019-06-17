#include<LiquidCrystal.h>


void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);
  lcd.clear();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

lcd.setCursor(0,0);

}
