#include "serial-command.h"
#include <LiquidCrystal.h>

int button = 12;

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

Range address_range, data_range, response_range = {0,0};
Command button_pressed = {1, address_range, data_range, response_range};

void setup() {
  // put your setup code here, to run once:
  SerialComm.begin(250000);

  pinMode(button, INPUT_PULLUP);
  
  lcd.begin(16, 4);

}

void loop() {
  // put your main code here, to run repeatedly:
  static bool last_state = HIGH;

  //We're using a pull-up resistor so LOW means the button is pressed
  if (digitalRead(button) == LOW) {
    if (last_state == HIGH) {
      SerialComm.sendCommand(button_pressed);  
    }
    last_state = LOW;
  } else {
    last_state = HIGH;
  }
  
  byte response[64] = {0};
  byte response_length = 0;
  if (SerialComm.getResponse(response, response_length) == true) {
    lcd.clear();
    lcd.print("Got a packet!");
    for (byte i = 0; i < response_length; i++) {
      lcd.print(response[i]);
      lcd.print(", ");
    }
  } else {
    lcd.clear();
    lcd.print("Got a malformed packet!");
  }
  
}
