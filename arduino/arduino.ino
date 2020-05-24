#include "serial-command.h"

int button = 12;

Range address_range, data_range, response_range = {0,0};
Command button_pressed = {1, address_range, data_range, response_range};

void setup() {
  // put your setup code here, to run once:
  SerialComm.begin(250000);

  pinMode(button, INPUT_PULLUP);

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
  
}
