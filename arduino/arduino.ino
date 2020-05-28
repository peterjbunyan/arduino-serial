#include "serial-command.h"
#include <LiquidCrystal.h>

int button = 12;

SerialCommand serial(Serial);

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

Range address_range, data_range, response_range = {0,0};
Command button_pressed = {1, address_range, data_range, response_range};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(250000);

  pinMode(button, INPUT_PULLUP);
  
  lcd.begin(16, 4);

}

void loop() {
  // put your main code here, to run repeatedly:
  static bool last_state = HIGH;

  //We're using a pull-up resistor so LOW means the button is pressed
  if (digitalRead(button) == LOW) {
    if (last_state == HIGH) {
      lcd.clear();
      if (serial.sendCommand(button_pressed)) {
        lcd.print("ACK Received!");
      } else {
        lcd.print("No ACK!");
      }
    }
    last_state = LOW;
  } else {
    last_state = HIGH;
  }
  
  /*
  byte response[64] = {0};
  byte response_length = 0;
  if (serial.getResponse(response, response_length) == true) {
    lcd.clear();
    lcd.print("Got a packet!");
    for (byte i = 0; i < response_length; i++) {
      lcd.print(response[i]);
      lcd.print(", ");
    }
  }
  */
  
}
