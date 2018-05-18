#include<IOXhop_BC95.h>

IOXhop_BC95 nb;
int socketId = -1;

unsigned long timeSend = 0;

void setup() {
  Serial.begin(9600);

  nb.begin();
  
  // Reboot module
  nb.reboot();
  delay(3000);
  
  while (!nb.connect()) {
    Serial.println("NB-IoT begin fail !");
    delay(1000);
  }

  Serial.print("Connect to network");
  while (!nb.isConnected()) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" Connected !");
}

void loop() {
  if (socketId < 0) {
    Serial.println("Create new socket");
    socketId = nb.CreateUDPSocket(9687);
    Serial.print("Socket id is ");
    Serial.println(socketId);
  } else {
    if ((millis() - timeSend) > 1000) { // every 1 sec
      timeSend = millis();
      char text[] = "Hello, NB-IoT !";
      bool ros = nb.SendUDP(socketId, "45.77.42.193", 55567, text, strlen(text));
      Serial.print("Send ");
      if (ros) {
        Serial.println("OK");
      } else {
        Serial.println("fail !");
      }
    }
    nb.UDPReceiver(socketId, [](int socket_id, String ip, unsigned long port, byte* data, int len) {
      data[len] = 0;
      Serial.print("Receiver[" + ip + ":" + String(port) + "]: ");
      Serial.println((char*)data);
    });
  }
}