/*

  arduino-serial library
  
  A library for sending commands and data between an Arduino and another
  device over a serial connection.
  
  While most Arduino boards only have one serial port, boards such as the
  Mega have multiple hardware UARTs, and more can be set-up using the
  SoftwareSerial library
  
  You can find more details on how this library works in the documentation
  in the GitHub repository https://github.com/peterjbunyan/arduino-serial
  
*/

#include "serial-command.h"

const Command SerialCommand::open_command = {253, {0,0}, {0,0}, {0,0}};
const Command SerialCommand::close_command = {254, {0,0}, {0,0}, {0,0}};
const Command SerialCommand::ack_command = {255, {0,0}, {0,0}, {0,0}};

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
  
  return sendPacket(packet, 3);
  
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
  Check to see if there is a packet we haven't read yet. If there is,
  decode it and copy the decoded data to the passed-in array then return
  true. If not, return false.
*/
bool SerialCommand::getResponse(byte data[], byte &data_length) {
  if (packet_received == true) {
    for (byte i = 0; i < received_bytes_length; i++) {
      data[i] = received_bytes[i];
    }
    data_length = received_bytes_length;
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
  Sends a packet with a header and footer added, and waits for an ACK
  response. Tried 3 times and returns if we got a successful ACK or not.
*/
bool SerialCommand::sendPacket(byte data[], int data_length) {
  
  byte packet_length = data_length*2;
  byte packet[packet_length];
  encodeBytes(data, data_length, packet);
  
  byte tries = 0;
  
  while (tries < 3) {
    tries++;
    serial.write("++");
    serial.write(packet, packet_length);
    serial.write("--");
    if (receivePacket() && (received_bytes[0] == ack_command.ID)) {
      return true;
    }
  }

  return false;

}

/*
  Receive and store a packet. This is a blocking function and won't return
  until a packet has been received or the timeout has expired. Returns
  true if the packet is correctly formed and false if it is not or if no
  packet is received. A  formed packets has between 6 and 60 bytes, and
  the number of bytes is even (Each data byte is encoded as two bytes in
  the packet).
*/
bool SerialCommand::receivePacket(unsigned long timeout = 1000) {
  unsigned long start_time = millis();
  unsigned long elapsed_time = 0;
  
  MessageState state = HEADER;
  byte header_count = 0;
  //Max data size is max packet length (64) minus two bytes each for the
  //header and footer
  byte data_buffer[60];
  byte data_length = 0;
  byte data_byte = 0;
  
  while ((state == HEADER) && (elapsed_time < timeout)) {
    if (serial.available()) {
      if (serial.read() == '+') {
        header_count++;
        if (header_count >= 2) {
          state = DATA;
        }
      } else {
        header_count = 0;
      }
    }
    elapsed_time = millis() - start_time;
  }
  
  while ((state == DATA) && (elapsed_time < timeout)) {
    if (serial.available()) {
      data_byte = serial.read();
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
    elapsed_time = millis() - start_time;
  }
  
  if ((data_length < 6) || ((data_length % 2) != 0)) {
    return false;
  }
  
  while (elapsed_time < timeout) {
    if (serial.available()) {
      if (serial.read() == '-'){
        decodeBytes(data_buffer, data_length, received_bytes);
        received_bytes_length = data_length/2;
        packet_received = true;
        return true;
      }
    }
    elapsed_time = millis() - start_time;
  }

  return false;
  
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