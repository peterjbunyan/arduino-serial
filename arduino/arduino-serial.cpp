/*

  arduino-serial library
  
  A library for sending commands and data between an Arduino and another
  device over a serial connection.
  
  This is a singleton class as (most) Arduino boards only support one 
  hardware serial port, which we will be using.
  
  You can find more details on how this library works in the documentation
  in the GitHub repository https://github.com/peterjbunyan/arduino-serial
  
*/

#include "arduino-serial.h"

// Define and initialise our single class instance
SerialCommand SerialComm = SerialCommand.getInstance()

/*
  Returns the single instance of this class, instantiating it if this 
  hasn't been done already.
*/
SerialCommand& SerialCommand::getInstance() {
  static SerialCommand instance;
  return instance;
}

/*
  Set up the serial port ready to open the connection
*/
void SerialCommand::begin(long speed) {
  Serial.begin(speed);
}

/*
  Open the connection with the other party. As we opened the connection
  we will be the ones controlling it
*/
bool SerialCommand::open() {
  //TODO
}

/*
  Close the connection with the other party.
*/
bool SerialCommand::close() {
  //TODO
}

/*
  Send a command to the other party and wait for the acknowledgement
*/
bool SerialCommand::sendCommand(Command command) {
  //TODO
}

/*
  Send a command to the other party and wait for the acknowledgement
*/
bool SerialCommand::sendCommand(Command command, byte[] address, 
                                int address_len, byte[] data,
                                int data_len)) {
  //TODO
}

/*
  Send a command to the other party and wait for the acknowledgement
*/
void SerialCommand::getResponse(byte[] data, int &byte_count) {
  //TODO
}

/*
  Encode an array of bytes into an array of ASCII hex numbers by first
  splitting each byte into two nibbles, then adding a value to convert
  them from their integer value to the ASCII value for that nibble.
  Hex 0..9 = ASCII 48..57
  Hex A..F = ASCII 65..70
*/
void SerialCommand::encodeBytes(byte bytes[], int byte_count, byte encoded_bytes[]) {
  auto high_nibble = [](byte x) {return (x >> 4) & 0xF; }; 
  auto low_nibble = [](byte x) {return x & 0xF; }; 
  for (int i = 0; i < byte_count; i++) {
    encoded_bytes[i*2] = high_nibble(bytes[i]);
    encoded_bytes[(i*2)+1] = low_nibble(bytes[i]);
  }
  for (int i = 0; i < (byte_count * 2); i++) {
    encoded_bytes[i] = nibbleToASCII(encoded_bytes[i]);
  }
}

void SerialCommand::encodeByte(byte data, byte encoded_bytes[]) {
  auto high_nibble = [](byte x) {return (x >> 4) & 0xF; }; 
  auto low_nibble = [](byte x) {return x & 0xF; }; 

  encoded_bytes[0] = high_nibble(bytes[i]);
  encoded_bytes[1] = low_nibble(bytes[i]);
  
  encoded_bytes[0] = nibbleToASCII(encoded_bytes[0]);
  encoded_bytes[1] = nibbleToASCII(encoded_bytes[1]);

}

byte SerialCommand::nibbleToASCII(byte data) {
  if (data < 10) {
    return data += 48; 
  } else {
    return data  += 55;
  }
}