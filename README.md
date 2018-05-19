# ไลบารี่สำหรับ AIS NB-IoT Shield

ไลบารี่นี้จัดทำขึ้นโดยมุ่นเน้นทำงานรวดเร็วเป็นหลัก พยามตัดทุกส่วนที่ทำให้ช้าออกไป เมื่อส่วนใดของไลบารี่ติดต่อกับ BC95 ผิดพลาด จะพยามติดต่อใหม่ซ้ำ ๆ จนสามารถติดต่อได้

## ไลบารี่ที่ต้องติดตั้งเพิ่ม

* [AltSoftSerial](https://github.com/PaulStoffregen/AltSoftSerial)

## การใช้งาน

จะแบ่งเป็น 2 ช่วง คือ การเชื่อมต่อเน็คเวิค และการสื่อสารผ่าน UDP

### เชื่อมต่อไปยังเน็ตเวิค

การเริ่มต้นใช้งาน ใน `void setup()` ก่อนทำอะไรก็ตาม ต้องเรียก `IOXhop_BC95::begin();` ก่อน เพื่อเริ่มต้นใช้การสื่อสารผ่าน Serial

หากอยู่ในขั้นตอนพัฒนาควร Reboot ตัวโมดูลก่อน เพื่อให้ UDP Socket ที่สร้างไว้ทั้งหมดถูกลบทิ้ง แล้วไม่ทำให้โปรแกรมทำงานผิดพลาด การ Reboot โมดูลทำได้โดยใช้คำสั่ง `IOXhop_BC95::reboot();`

หลังจาก Reboot โมดูลแล้ว ต้องเรียกฟังก์ชั่น `bool IOXhop_BC95::connect();` เพื่อตั้งค่าการเชื่อมต่อ แต่เนื่องจากหลังบูตโมดูล จำเป็นจะต้องรอให้โมดูลพร้อมก่อน ดังนั้นคำสั่งนี้เมื่อใช้แล้วอาจจะได้ false ในครั้งแรก จึงจำเป็นต้องลูปคำสั่งนี้ซ้ำจนกว่าจะตั้งค่าการเชื่อมต่อได้

การเชื่อมต่อจะไม่สามารถเชื่อมต่อได้ทันที จำเป็นจะต้องรอประมาณ 30 วินาที จึงจะเชื่อมต่อกับเครือข่ายได้ ให้ใช้ฟังก์ชั่น `bool IOXhop_BC95::isConnected();` ในการเช็คว่าเชื่อมต่ได้แล้วหรือยัง

จากหลักการที่กล่าวมข้างต้น จึงได้โค้ดโปรแกรมดังนี้

```c++
nb.begin();
  
Serial.println("Reboot module");
nb.reboot();

Serial.print("Try to setup");
while (!nb.connect()) {
  Serial.print(".");
}
Serial.println(" OK !");
   
Serial.print("Connect to network");
while (!nb.isConnected()) {
  Serial.print(".");
  delay(500);
}
Serial.println(" Connected !");
```

โดยโค้ดโปรแกรมเหล่านี้ถูกรวมอยู่ในฟังก์ชั่น `void IOXhop_BC95::easySetup(bool showProcess = true);` ในความเป็นจริงจึงสามารถเรียกใช้ฟังก์ชั่นนี้ได้เลย

เมื่อเชื่อมต่อเน็ตเวิคได้แล้ว จะสามารถเรียกใช้ฟังก์ชั่นที่เกี่ยวกับการเชื่อมต่อได้ทันที

### การสื่อสารผ่าน UDP

การเริ่มต้นการสื่อสาร จะต้องสร้าง Socket ขึ้นมาก่อน เพื่อเป็นการสร้าง UDP Server ให้สามารถรับข้อมูลเข้ามาผ่านพอร์ตที่ตั้งไว้ได้

การสร้าง Socket ทำได้โดยเรียกใช้ฟังก์ชั่น `Socket* IOXhop_BC95::CreateUDPSocket(int listen_port);` โดยฟังชั่นนี้จะคืนค่าออกมาในรูปของออปเจคของคลาส Socket แบบ Pointer

หากต้องการให้สามารถรับข้อความเข้ามาได้ จะต้องเรียกฟังก์ชั่น `void Socket::onReceiver(UDPReceiverCallback callback);` เพื่อกำหนดฟังก์ชั่น callback ที่จะถูกเรียกเมื่อมีข้อมูลเข้ามา ตัวอย่างด้านล่างนี้เป็นการใช้งานฟังก์ชั่น `void Socket::onReceiver(UDPReceiverCallback callback);`

```c++
soc->onReceiver([](String ip, unsigned long port, byte* data, int len) {
  // พื้นที่วางโค้ดโปรแกรมที่จะให้ทำงานเมื่อมีข้อความเข้ามา
});
```

ข้อควรระวัง คือ ตัวแปร \*data จะไม่สามารถเพิ่มข้อมูลเข้าไปต่อท้ายได้ เพราะจองพื้นที่ในแรมไว้เท่าตัวแปร len พอดี หากต้องการรับข้อมูลในรูปแบบ string จำเป็นต้องสร้างตัวแปรใหม่ที่มีขนาด len + 1 ใช้ฟังก์ชั่น memcpy() คัดลอกข้อมูลใน \*data ไปลงตัวแปรใหม่ ที่ท้ายตัวแปรใหม่ หรือตำแหน่งที่ len ให้ใส่ข้อมูลเป็น 0 เพื่อให้ได้รูปแบบ string ในภาษา C ที่สมบูรณ์

สำคัญคือ หากใช้การรับข้อมูลเข้า จะต้องวาง `void IOXhop_BC95::loop();` ไว้ใน `void loop();` ด้วย

การส่งข้อมูลให้เรียกใช้ฟังก์ชั่น `int Socket::send(String ip, unsigned long port, byte *data, int len);` ได้เลย ค่าที่ตอบกลับมา คือจำนวนไบต์ของข้อมูลที่สามารถส่งไปได้

### ตัวอย่างโค้ดโปรแกรมแบบเต็ม (test.ino)

```c++
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
```

## อุปกรณ์ที่ผ่านการสอบแล้ว

 * Arduino Uno R3
 
 ## ลิขสิทธิ์การใช้งาน

ผู้จัดทำอนุญาตให้นำไปใช้งาน และแจกจ่ายได้โดยคงไว้ซึ่งที่มาของเนื้อหา ห้ามมีให้นำไปใช้งานในเชิงพาณีย์โดยตรง เช่น การนำไปจำหน่าย

 * http://www.ioxhop.com/
