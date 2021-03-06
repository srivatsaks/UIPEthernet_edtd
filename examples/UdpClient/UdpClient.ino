/*
 * UIPEthernet UdpClient example.
 *
 * UIPEthernet is a TCP/IP stack that can be used with a enc28j60 based
 * Ethernet-shield.
 *
 * UIPEthernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 *      -----------------
 *
 * This UdpClient example tries to send a packet via udp to 192.168.0.1
 * on port 5000 every 5 seconds. After successfully sending the packet it
 * waits for up to 5 seconds for a response on the local port that has been
 * implicitly opened when sending the packet.
 *
 * Copyright (C) 2013 by Norbert Truchsess (norbert.truchsess@t-online.de)
 * 
 */

#define MACADDRESS 0x05,0x04,0x03,0x02,0x01,0x05
#define MYIPADDR 192,168,1,200
#define MYIPMASK 255,255,255,0
#define MYDNS 192,168,1,1
#define MYGW 192,168,1,1

#if defined(__MBED__)
  #include <mbed.h>
  #include "mbed/millis.h"
  #define delay(x) wait_ms(x)
  #define PROGMEM
  #include "mbed/Print.h"
#endif

#include <UIPEthernet_edtd.h>
#include "utility/logging.h"

EthernetUDP udp;
unsigned long next;

#if defined(ARDUINO)
void setup() {
#endif  
#if defined(__MBED__)
int main() {
#endif
  #if ACTLOGLEVEL>LOG_NONE
    #if defined(ARDUINO)
      LogObject.begin(9600);
    #endif
    #if defined(__MBED__)
      Serial LogObject(SERIAL_TX,SERIAL_RX);
    #endif
  #endif

  uint8_t mac[6] = {MACADDRESS};
  uint8_t myIP[4] = {MYIPADDR};
  uint8_t myMASK[4] = {MYIPMASK};
  uint8_t myDNS[4] = {MYDNS};
  uint8_t myGW[4] = {MYGW};

  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  delay(100);
  Serial.begin(115200); // for debugging
  digitalWrite(5, HIGH);

//  Ethernet.begin(mac,IPAddress(192,168,0,6));
  Ethernet.begin(mac,myIP,myDNS,myGW,myMASK);
  Serial.print("UDP client is at: ");
  Serial.println(Ethernet.localIP());
  next = millis()+5000;
#if defined(ARDUINO)
}

void loop() {
#endif  

#if defined(__MBED__)
while(true) {
#endif
  int success;
  int len = 0;

  if (millis()>next)
    {
      do
        {
          success = udp.beginPacket(IPAddress(192,168,0,1),5000);
          #if ACTLOGLEVEL>=LOG_INFO
            LogObject.uart_send_str(F("beginPacket: "));
            LogObject.uart_send_strln(success ? "success" : "failed");
          #endif
          //beginPacket fails if remote ethaddr is unknown. In this case an
          //arp-request is send out first and beginPacket succeeds as soon
          //the arp-response is received.
          #if defined(ESP8266)
            wdt_reset();
          #endif
        }
      while (!success && (millis()<next));
      if (success)
        {
        success = udp.write("hello world from arduino");
        #if ACTLOGLEVEL>=LOG_INFO
          LogObject.uart_send_str(F("bytes written: "));
          LogObject.uart_send_decln(success);
        #endif
        success = udp.endPacket();
        #if ACTLOGLEVEL>=LOG_INFO
          LogObject.uart_send_str(F("endPacket: "));
          LogObject.uart_send_strln(success ? "success" : "failed");
        #endif
        do
          {
          //check for new udp-packet:
          success = udp.parsePacket();
          #if defined(ESP8266)
            wdt_reset();
          #endif
          }
        while (!success && (millis()<next));
        if (success)
          {
          #if ACTLOGLEVEL>=LOG_INFO
            LogObject.uart_send_str(F("received: '"));
          #endif
          do
            {
            char c = udp.read();
            #if ACTLOGLEVEL>=LOG_INFO
              #if defined(ARDUINO)
                LogObject.write(c);
              #endif
              #if defined(__MBED__)
                LogObject.printf("%c",&c);
              #endif
            #endif
            len++;
            }
          while ((success = udp.available())>0);
          #if ACTLOGLEVEL>=LOG_INFO
            LogObject.uart_send_str(F("', "));
            LogObject.uart_send_dec(len);
            LogObject.uart_send_strln(F(" bytes"));
          #endif
          //finish reading this packet:
          udp.flush();
          }
        }
      udp.stop();
      next = millis()+5000;
    }
}
#if defined(__MBED__)
}
#endif
