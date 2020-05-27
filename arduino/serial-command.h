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

#include <arduino.h>

#ifndef SERIAL_COMMAND_h
#define  SERIAL_COMMAND_h

struct Range {
  byte min;
  byte max;
};

struct Command {
  int ID;
  Range address_length;
  Range data_length;
  Range response_length;
};

enum MessageState {
  HEADER = 0,
  DATA = 1,
  FOOTER = 2
};

class SerialCommand {
  
  public:
  
    SerialCommand(Stream& stream = Serial) : serial(stream) {};
    bool open();
    bool close();
    bool sendCommand(Command command);
    bool sendCommand(Command command, byte address[],
                      int address_length, byte data[], int data_length);
    bool getResponse(byte data[], byte &data_length);
    
  private:
  
    Stream& serial;
    
    //Command variables
    static const Command open_command, close_command, ack_command;
    volatile byte received_packet[60] = {0};
    volatile byte received_packet_length = 0;
    volatile bool packet_received = false;
    
    //Helper methods
    void encodeBytes(byte bytes[], 
                      int bytes_length, char encoded_bytes[]);
    void decodeBytes(char characters[], 
                      int bytes_length, byte decoded_characters[]);
    void encodeByte(byte data, char encoded_byte[]);
    char nibbleToASCII(byte data);
    byte asciiToNibble(char character);
    void sendPacket(byte packet[], int packet_length);
    bool receivePacket();
    byte getSerialByte();
    bool isHexChar(char character);
    
};

#endif
