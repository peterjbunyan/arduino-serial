/*

  arduino-serial library
  
  A library for sending commands and data between an Arduino and another
  device over a serial connection.
  
  This is a singleton class as (most) Arduino boards only support one 
  hardware serial port, which we will be using.
  
  You can find more details on how this library works in the documentation
  in the GitHub repository https://github.com/peterjbunyan/arduino-serial
  
*/

#include "serial-command.h"

// Define and initialise our single class instance
SerialCommand SerialComm = SerialCommand::getInstance();

const Command SerialCommand::open_command = {253, {0,0}, {0,0}, {0,0}};
const Command SerialCommand::close_command = {254, {0,0}, {0,0}, {0,0}};
const Command SerialCommand::ack_command = {255, {0,0}, {0,0}, {0,0}};

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
  if ((command.address_length.min != 0) 
        || (command.data_length.min != 0)) {
    return false;
  }
  
  byte packet[3] = {command.ID, 0, 0};
  byte encoded_packet[6];
  encodeBytes(packet, 3, encoded_packet);
  
  sendPacket(encoded_packet, 6);
  
  //TODO: Implement ACK check
  
  
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
bool SerialCommand::getResponse(byte data[], byte &data_length) {
  if (receivePacket()) {
    decodeBytes(received_packet, received_packet_length, data);
    data_length = received_packet_length/2;
    packet_received = false;
    return true;
  }
  return false;
}

/*
  Encode an array of bytes into an array of ASCII hex numbers by first
  splitting each byte into two nibbles, then adding a value to convert
  them from their integer value to the ASCII value for that nibble.
*/
void SerialCommand::encodeBytes(byte bytes[], int bytes_length,
                                char encoded_bytes[]) {
  char single_byte[2] = {0};
  for (int i = 0; i < bytes_length; i++) {
    encodeByte(bytes[i], single_byte);
    encoded_bytes[i*2] = single_byte[0];
    encoded_bytes[(i*2)+1] = single_byte[1];
  }
}

/*
  Decode an array of ASCII hex numbers into bytes by converting each
  char back to a nibble, then combining each pair of nibbles in to a
  byte.
*/
void SerialCommand::decodeBytes(char characters[], int bytes_length,
                                byte decoded_characters[]) {

  for (int i = 0; i < (bytes_length / 2); i++) {
    decoded_characters[i] = (asciiToNibble(characters[i*2]) << 4);
    decoded_characters[i] |= asciiToNibble(characters[(i*2)+1]);
  }

}

/*
  Encode a single byte as an ASCII Hex number by first splitting the byte
  into two nibbles, then adding a value to convert the nibbles from their
  integer value to the ASCII value for that nibble.
*/

void SerialCommand::encodeByte(byte data, char encoded_byte[]) {
  auto high_nibble = [](byte x) {return (x >> 4) & 0xF; }; 
  auto low_nibble = [](byte x) {return x & 0xF; }; 

  encoded_byte[0] = nibbleToASCII(high_nibble(data));
  encoded_byte[1] = nibbleToASCII(low_nibble(data));

}

/*
  Converts a nibble (stored as a byte as C++ doesn't have a type for
  nibbles) into an ASCII character representing it's value in Hex
  Hex 0..9 becomes ASCII 48..57
  Hex A..F becomes ASCII 65..70
*/
char SerialCommand::nibbleToASCII(byte data) {
  if (data < 10) {
    return data += 48; 
  } else {
    return data  += 55;
  }
}

/*
  Converts a nibble (stored as a byte as C++ doesn't have a type for
  nibbles) into an ASCII character representing it's value in Hex
  ASCII 48..57 becomes Hex 0..9
  ASCII 65..70 becomes Hex A..F
*/
byte SerialCommand::asciiToNibble(char character) {
  if (character <= '9') {
    return character -= 48; 
  } else {
    return character  -= 55;
  }
}

/*
  Sends a packet with a header and footer added
*/
void SerialCommand::sendPacket(byte packet[], int packet_length) {
  Serial.write("++");
  Serial.write(packet, packet_length);
  Serial.write("--");
}

/*
  Receive and store a packet. This is a blocking function and won't return
  until a packet has been received. Returns true if the packet is 
  correctly formed and false if it is not. A correctly formed packets has
  between 6 and 60 bytes, and the number of bytes is even (Each data byte
  is encoded as two bytes in the packet).
*/
bool SerialCommand::receivePacket() {
  MessageState state = HEADER;
  byte header_count = 0;
  //Max data size is max packet length (64) minus two bytes each for the
  //header and footer
  byte data_buffer[60];
  byte data_length = 0;
  byte data_byte = 0;
  
  while (state == HEADER) {
    data_byte = getSerialByte();
    if (data_byte == '+') {
      header_count++;
      if (header_count >= 2) {
        state = DATA;
      }
    } else {
      header_count = 0;
    }
  }
  
  while (state == DATA) {
    data_byte = getSerialByte();
    if (isHexChar(data_byte)) {
      data_buffer[data_length++] = data_byte;
      if (data_length >= 60) {
        return false;
      }
    } else if (data_byte == '-') {
      state = FOOTER;
    } else {
      return false;
    }
  }
  
  if ((data_length < 6) || ((data_length % 2) != 0)) {
    return false;
  }
  
  data_byte = getSerialByte();
  if (data_byte == '-'){
    for (int i = 0; i < data_length; i++) {
      received_packet[i] = data_buffer[i];
    }
    received_packet_length = data_length;
    packet_received = true;
    return true;
  }

  return false;
  
}

/*
  A helper to wait for and return the next received byte
*/
byte SerialCommand::getSerialByte() {
  while (Serial.available() == false) {};
  return Serial.read();
}

/*
  A helper method to check if a byte is an ASCII character the represents
  a Hex digit (0..9 and A..F)
*/
bool SerialCommand::isHexChar(char character) {
  if (('0' <= character) && (character <= '9')) {
    return true;
  } else if (('A' <= character) && (character <= 'F')) {
    return true;
  } else {
    return false;
  }
}