# arduino-serial
A library for sending serial data between an Arduino and another serial device

## Description
This library is used for communicating with an Arduino via serial in a connection oriented way. This can be between an Arduino and a serial host or between two Arduino devices. The communication can be be opened from either end and is controlled by the device that initiates the connection.

Data is sent between the two devices as Hex encoded in ASCII. For example, to send the byte value `175` we first convert it into Hex notation and send it as two the ASCII characters `AF`.

This is done for a number of reasons:

- It makes the data easier to read and understand if we want to intercept it.
- We can easily use any serial terminal to manually interact with either end.
- We can use any ASCII characters other then `[0-9,A-F]` for flow control (Headers and Footers).

There are also downsides to this decision:

- It takes twice as many bytes to send data.
- Conversion to and from Hex encoded ASCII takes additional time.

For this library the improved ease of use is more important that the efficiency overheads caused by this conversion.

## Command Packets

![Command Packet Structure](/docs/command_packet_structure.png)

The presence of the address and/data bytes is determined by the command being sent. This is discussed in more detail under the [Command Definition](#command-definition) section.

## Response Packets

![Response Packet Structure](/docs/response_packet_structure.png)

The presence of the data bytes determined by the command that is being responded to. This is discussed in more detail under the [Command Definition](#command-definition) section.

## Command Definition

### Built-in Commands

There are three built-in commands that are used for managing the connection between devices:

- Open
- Close
- Acknowledge

The open and close commands must be sent with address and data lengths of 0.

The Acknowledge command must only be used in response packets. The number of data bytes returned is determined by command packet that is being responded to.

These commands are used by the library itself and can only be sent by using the built in methods `open()`, `close()` and `acknowledge(response_data)'

### Custom Commands

A custom command list must be provided when creating the connection object. This list must contain 1 or more custom command definitions, up to a maximum of 252.

Commands are defined with the following fields:

- `command`: Command Friendly Name
- `min_addr_len`: Minimum Address Length, 0 to 8
- `max_addr_len`: Maximum Address Length, 0 to 8
- `min_data_len`: Minimum Data Length, 0 to 8
- `max_data_len`: Maximum Data Length, 0 to 8
- `min_response_len`: Minimum Response Length, 0 to 16
- `max_response_len`: Maximum Response Length, 0 to 16


The limits on the amount of address and data bytes you can send in one command is due to the Arduino's serial buffer size of 64 bytes. Each byte that we send when converted into ASCII encoded Hex takes up 2 bytes, limiting us to sending 32 actual bytes at a time. The number of fixed bytes sent for each command header is 10 (5 ASCII encoded bytes) leaving us with 54 bytes

For [command packets](#command-packets) the remaining 54 bytes are split evenly between address and data, giving them 27 bytes each. This equates to 13.5 bytes of ASCII encoded Hex. This is reduced to 8 as the largest power of two to make data manipulation easier

For [response packet](#response-packets) all of the remaining bytes can be used for the response data. Reducing 27 tot he nearest power of 2 gives us 16 bytes of ASCII encoded Hex that can be send per response.