/*

  arduino-serial library
  
  A library for sending commands and data between an Arduino and another
  device over a serial connection.
  
  This is a singleton class as (most) Arduino boards only support one 
  hardware serial port, which we will be using.
  
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
  
    static SerialCommand& getInstance();
    void begin(long speed);
    bool open();
    bool close();
    bool sendCommand(Command command);
    bool sendCommand(Command command, byte address[],
                      int address_length, byte data[], int data_length);
    bool getResponse(byte data[], byte &data_length);
    
  private:
  
    SerialCommand() {};
    
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

//Define an externally accessible variable to hold the only instance of
//our class
extern SerialCommand SerialComm;

#endif
