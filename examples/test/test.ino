#include<IOXhop_BC95.h>

IOXhop_BC95 nb;
Socket *soc = NULL;

unsigned long timeSend = 0;

void setup() {
  Serial.begin(9600);

  // Setup module
  nb.easySetup(true);
  
  Serial.println("Create new socket");
  if (soc = nb.CreateUDPSocket(9687)) {
    Serial.print("Socket id is ");
    Serial.println(soc->getID());
  } else {
    Serial.println("Create fail !");
  }
  
  soc->onReceiver([](String ip, unsigned long port, byte* data, int len) {
    data[len] = 0;
    Serial.print("Receiver[" + ip + ":" + String(port) + "]: ");
    Serial.println((char*)data);
  });
}

void loop() {
  if (soc) {
    if ((millis() - timeSend) >= 1000) { // every 1 sec
      timeSend = millis();
      char text[] = "Hello, NB-IoT !";
      Serial.print("Send ");
      if (soc->send("45.77.40.88", 55567, text, strlen(text))) {
        Serial.println("OK");
      } else {
        Serial.println("fail !");
      }
    }
    soc->CheckReceiver();
  }
  delay(1);
  //nb.loop();
}