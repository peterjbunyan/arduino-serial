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

The presence of the data bytes determined by the command that is being responded to.

## Command Definition

Commands are defined with the following fields:

- `command`: Command Friendly Name
- `min_addr_len`: Minimum Address Length, 0 to 8
- `max_addr_len`: Maximum Address Length, 0 to 8
- `min_data_len`: Minimum Data Length, 0 to 8
- `max_data_len`: Maximum Data Length, 0 to 8
- `min_response_len`: Minimum Response Length, 0 to 16
- `max_response_len`: Maximum Response Length, 0 to 16


The limits on the amount of address and data bytes you can send in one command is due to the Arduino's serial buffer size of 64 bytes. Each byte that we send when converted into ASCII encoded Hex takes up 2 bytes, limiting us to sending 32 actual bytes at a time. The number of fixed bytes sent for each command header is 10 (5 ASCII encoded bytes) leaving us with 54 bytes