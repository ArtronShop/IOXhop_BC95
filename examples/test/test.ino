/* File              : test.ino
   Codeing By IOXhop : www.ioxhop.com
   Sonthaya Nongnuch : www.fb.me/maxthai */

#include<IOXhop_BC95.h>

IOXhop_BC95 nb;
Socket *soc = NULL;

unsigned long timeSend = 0, timeSend2 = 0;

void setup() {
  Serial.begin(9600);

  // Setup module
  nb.easySetup(true);

  // Socket
  Serial.println("Create new socket");
  if (soc = nb.CreateUDPSocket(9687)) {
    Serial.print("Socket id is ");
    Serial.println(soc->getID());
  } else {
    Serial.println("Create fail !");
  }
  
  soc->onReceiver([](String ip, unsigned long port, byte* data, int len) {
    char message[len + 1];
    memcpy(message, data, len);
    message[len] = 0;
    Serial.print("Receiver[" + ip + ":" + String(port) + "]: ");
    Serial.println(message);
  });
}

void loop() {
  if (soc) {
    if ((millis() - timeSend) >= 5000) { // every 5 sec
      timeSend = millis();
      char text[] = "Hello, NB-IoT !";
      Serial.print("Send ");
      if (soc->send("149.28.51.94", 55567, text, strlen(text))) {
        Serial.println("OK");
      } else {
        Serial.println("fail !");
      }
    }
  }
  
  nb.loop();
}