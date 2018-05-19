/* File              : ntp.ino
   Codeing By IOXhop : www.ioxhop.com
   Sonthaya Nongnuch : www.fb.me/maxthai */
   
/*
  Needs: https://github.com/PaulStoffregen/Time
  Thanks: https://www.arduino.cc/en/Tutorial/UdpNtpClient
*/

#include <IOXhop_BC95.h>
#include <TimeLib.h>

IOXhop_BC95 nb;
Socket *soc = NULL;

String timeServer = "158.108.212.149"; // NTP server (IP only)
#define NTP_PACKET_SIZE 48 // NTP time stamp is in the first 48 bytes of the message
#define TIME_ZONE 7

long unsigned timeShow = 0;

void setup() {
  Serial.begin(9600);

  // Setup module
  nb.easySetup(true);

  // Socket
  Serial.println("Create new socket");
  if (soc = nb.CreateUDPSocket(8888)) {
    Serial.print("Socket id is ");
    Serial.println(soc->getID());
  } else {
    Serial.println("Create fail !");
  }
  
  soc->onReceiver([](String ip, unsigned long port, byte* packetBuffer, int len) {
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    
    // now convert NTP time into everyday time:
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long timestamp = secsSince1900 - seventyYears;
    
    // print Unix time:
    Serial.print("Timestamp = ");
    Serial.println(timestamp);

    setTime(timestamp + (TIME_ZONE * 60 * 60));
    timeShow = millis();
  });

  sendNTPpacket(timeServer);
}

void loop() {
  if (timeShow > 0 && (millis() - timeShow) >= 1000) { // every 1 sec
    timeShow = millis();
    digitalClockDisplay();
  }
  nb.loop();
}

void digitalClockDisplay() {
  Serial.print(day());
  Serial.print("/");
  Serial.print(month());
  Serial.print("/");
  Serial.print(year()); 
  Serial.print(" ");
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println();
  
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

// send an NTP request to the time server at the given address
void sendNTPpacket(String address) {
  byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold outgoing packets
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  if (!soc->send(address, 123, packetBuffer, NTP_PACKET_SIZE)) {
    Serial.println("Send fail !");
  }
}