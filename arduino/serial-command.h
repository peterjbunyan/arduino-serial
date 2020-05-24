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

class SerialCommand {
  
  public:
  
    static SerialCommand& getInstance();
    void begin(long speed);
    bool open();
    bool close();
    bool sendCommand(Command command);
    bool sendCommand(Command command, byte address[],
                      int address_length, byte data[], int data_length);
    void getResponse(byte data[], int &data_length);
    
  private:
  
    SerialCommand() {};
    
    void encodeBytes(byte bytes[], 
                      int bytes_length, byte encoded_bytes[]);
    void encodeByte(byte data, byte encoded_bytes[]);
    byte nibbleToASCII(byte data);
    void sendPacket(byte packet[], int packet_length);
    
};

//Define an externally accessible variable to hold the only instance of
//our class
extern SerialCommand SerialComm;

#endif
