/*
  example IBUS message:
  50 04 68 32 11 1F (volume up button pressed on the steering wheel)
  |  |  |  |  |  |
  |  |  |  |  |  checksum (xorsum of all previous bytes)
  |  |  |  |  one or more data fields
  |  |  |  message type/command type
  |  |  destination address
  |  length of message (including destination address and checksum)
  source address
*/

// this example shows how to transmit a message over the IBUS

#include <IbusTrx.h> // include the IbusTrx library

IbusTrx ibusTrx; // create a new IbusTrx instance



uint8_t clear_text[6] = {0xC8, 0x05, 0x80, 0x23, 0x41, 0x20}; //no checksum

uint8_t  display_text[16] = {0xC8, 0x0E, 0x80, 0x23, 0x43, 0x20, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A};
//uint8_t display_text[7]= {0xC8,0x06,0x80,0x23,0x43,0x20,0x00};//
//uint8_t display_text[24]={0xC8,0x17, 0x80, 0x23, 0x43, 0x20, 0x54, 0x45, 0x4C, 0x20, 0x2B, 0x34, 0x39, 0x32, 0x32, 0x33, 0x33, 0x00, 0x35, 0x34, 0x31, 0x34, 0x33};


const int interval = 200;
unsigned long prevMillis = 0;
void setup() {
  ibusTrx.begin(Serial2); // begin listening for messages
  Serial.begin(115200);
  Serial.print("ibus checker");
  ibusTrx.write(clear_text);
}

int i = 0x00;
void loop() {
 
    if (millis() - prevMillis > interval) {
      display_text[3]=i;
      ibusTrx.write(display_text);
      prevMillis = millis();
      i = i+0x01;
    }
    
     if (ibusTrx.available()) {}

  








}
// remember to never use a blocking function like delay() in your program,
// always use millis() or micros() if you have to implement a delay somewhere