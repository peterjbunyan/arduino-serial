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
SerialCommand SerComm = SerialCommand::getInstance();

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
  //
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
bool SerialCommand::sendCommand(Command command, byte address[], 
                                int address_length,
                                byte data[], int data_length) {
  //TODO
}

/*
  Send a command to the other party and wait for the acknowledgement
*/
void SerialCommand::getResponse(byte data[], int &data_length) {
  //TODO
}

/*
  Encode an array of bytes into an array of ASCII hex numbers by first
  splitting each byte into two nibbles, then adding a value to convert
  them from their integer value to the ASCII value for that nibble.
*/
void SerialCommand::encodeBytes(byte bytes[],
                                int bytes_length, byte encoded_bytes[]) {
  auto high_nibble = [](byte x) {return (x >> 4) & 0xF; }; 
  auto low_nibble = [](byte x) {return x & 0xF; }; 
  for (int i = 0; i < bytes_length; i++) {
    encoded_bytes[i*2] = high_nibble(bytes[i]);
    encoded_bytes[(i*2)+1] = low_nibble(bytes[i]);
  }
  for (int i = 0; i < (bytes_length * 2); i++) {
    encoded_bytes[i] = nibbleToASCII(encoded_bytes[i]);
  }
}

/*
  Encode a single byte as an ASCII Hex number by first splitting the byte
  into two nibbles, then adding a value to convert the nibbles from their
  integer value to the ASCII value for that nibble.
*/

void SerialCommand::encodeByte(byte data, byte encoded_bytes[]) {
  auto high_nibble = [](byte x) {return (x >> 4) & 0xF; }; 
  auto low_nibble = [](byte x) {return x & 0xF; }; 

  encoded_bytes[0] = high_nibble(data);
  encoded_bytes[1] = low_nibble(data);
  
  encoded_bytes[0] = nibbleToASCII(encoded_bytes[0]);
  encoded_bytes[1] = nibbleToASCII(encoded_bytes[1]);

}

/*
  Converts a nibble (stored as a byte as C++ doesn't have a typo for
  nibbles) into an ASCII character representing it's value in Hex
  Hex 0..9 becomes ASCII 48..57
  Hex A..F becomes ASCII 65..70
*/
byte SerialCommand::nibbleToASCII(byte data) {
  if (data < 10) {
    return data += 48; 
  } else {
    return data  += 55;
  }
}
