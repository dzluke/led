// Luke Dzwonczyk, 2021
// includes code from WS2812Serial examples 
// includes OSC example code written by Adrian Freed

#include <WS2812Serial.h>
#define USE_WS2812SERIAL
//#define FASTLED_INTERNAL
#include <FastLED.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <SPI.h>  
#include <OSCBundle.h>
#include <OSCBoards.h>


#define NUM_LEDS 128
#define DATA_PIN 1

// Define the array of leds
CRGB leds[NUM_LEDS];

EthernetUDP Udp;

byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // you can find this written on the board of some Arduino Ethernets or shields

//the Arduino's IP
//IPAddress ip(128, 32, 122, 252);

unsigned int localPort = 7000;
unsigned int remotePort = 7001;


void setup() {
  Serial.begin(57600);

  Ethernet.begin(mac);
  Udp.begin(localPort);
  
  Serial.print("Local IP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("Local port: ");
  Serial.println(localPort);

  LEDS.addLeds<WS2812SERIAL, DATA_PIN, BRG>(leds,NUM_LEDS);
  LEDS.setBrightness(40);

  FastLED.clear();
  FastLED.show();
}

void loop() {
  OSCBundle bundleIN;
  int packetSize = Udp.parsePacket();
  if(packetSize) {
//    Serial.print("Received packet of size ");
//    Serial.println(packetSize);
    
    while(packetSize--) {
      bundleIN.fill(Udp.read());
    }
    
    if(!bundleIN.hasError()) {
      bundleIN.route("/led", set_led);
    }
   }
}

void set_led(OSCMessage &msg) {
//  char* address;
//  msg.getAddress(address);
//  Serial.print(address);
  for (int i = 0; i < NUM_LEDS * 3; i+=3) {
    int index = i / 3;
    int rgb[] = {msg.getInt(i), msg.getInt(i+1), msg.getInt(i+2)};
    leds[index].setRGB(rgb[0], rgb[1], rgb[2]);
  }
  FastLED.show();
  for (int i = 0; i < NUM_LEDS; i++) {
      Serial.print(i);
      Serial.print(": ");
      Serial.print(leds[i].r);
      Serial.print(" ");
      Serial.print(leds[i].g);
      Serial.print(" ");
      Serial.print(leds[i].b);
      Serial.println();
   }
}
