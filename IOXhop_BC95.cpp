/* File              : IOXhop_BC95.cpp
   Codeing By IOXhop : www.ioxhop.com
   Sonthaya Nongnuch : www.fb.me/maxthai */

#ifndef IOXhop_BC95_CPP
#define IOXhop_BC95_CPP

#include "IOXhop_BC95.h"

void IOXhop_BC95::easySetup(bool showProcess = true) {
	this->begin();
  
	if (showProcess) Serial.println("Reboot module");
	this->reboot();

	if (showProcess) Serial.print("Try to setup");
	while (!this->connect()) {
		if (showProcess) Serial.print(".");
	}
	if (showProcess) Serial.println(" OK !");
   
	if (showProcess) Serial.print("Connect to network");
	while (!this->isConnected()) {
		if (showProcess) Serial.print(".");
		delay(500);
	}
	if (showProcess) Serial.println(" Connected !");
}

void IOXhop_BC95::begin() {
	IOXhop_BC95_Base::begin(); 
}

bool IOXhop_BC95::reboot() {
	testOnline();
	
	// Send reboot command
	SendCMD("AT+NRB");
	if (!find("REBOOTING\r\n", 1000)) {
		#ifdef DEBUG
			DEBUG("Reboot fail !");
		#endif
		return false;
	}
	
	delay(500);
	/*
	// Wait OK
	if (!findOK(10000)) {
		#ifdef DEBUG
			DEBUG("Reboot fail !, now module not rerun");
		#endif
		return false;
	}
	*/
	
	return true;
}

bool IOXhop_BC95::connect() {
/*
	// wait module ready
	unsigned long start = millis();
	while(!testOnline() && (millis() - start) < 10000) {
		#ifdef DEBUG
			DEBUG("Wait ready");
		#endif
		delay(500);
	}
	
	if ((millis() - start) >= 10000) {
		#ifdef DEBUG
			DEBUG("Module turn on ?");
		#endif
		return false;
	}
*/

	if (isConnected()) return true;
	
	for (int i=0;i<SetupCMDLen;i++) {
		if (!SendCMDfindOK(SetupCMD[i], 500)) {
			#ifdef DEBUG
				DEBUG("Send " + SetupCMD[i] + " fail");
			#endif
			return false;
		}
	}
	
	return true;
}

bool IOXhop_BC95::testOnline() {
	return SendCMDfindOK("AT");
}

String IOXhop_BC95::getIMEI() {
	SendCMD("AT+CGSN=1");
	readString(0, 1000, "+CGSN:");
	String imei = readString(0, 100, "\r\n");
	imei = imei.substring(0, imei.length() - 2);
	if (!findOK()) return "";
	return imei;
}

bool IOXhop_BC95::isConnected() {
	SendCMD("AT+CGATT?");
	readString(0, 1000, "+CGATT:");
	String ros = readString(1, 100);
	if (!findOK()) return false;
	return ros == "1";
}

// UDP
Socket* IOXhop_BC95::CreateUDPSocket(int listen_port) {
	if (_SocketPointersIndex >= MAX_SOCKET) return NULL;
	
	SendCMD("AT+NSOCR=DGRAM,17," + String(listen_port) + ",1");
	readString(0, 1000, "\r\n");
	String ros = readString(1, 100);
	if (!findOK()) return NULL;
	_SocketPointers[_SocketPointersIndex++] = new Socket(this, ros.toInt());
	return _SocketPointers[_SocketPointersIndex - 1];
}

void IOXhop_BC95::loop() {
	while (IOXhop_BC95_Base::_ser->available() > 0) {
		if (IOXhop_BC95_Base::_ser->read() == '+') {
			if (find("NSONMI:", 100)) {
				String ros;
				ros = readString(0, 50, ",");
				int incomeSocketId = ros.substring(0, ros.length() - 1).toInt();
				ros = readString(0, 50, "\r\n");
				int incomeLen = ros.substring(0, ros.length() - 2).toInt();
				#ifdef DEBUG
					DEBUG("incom Socket id: " + String(incomeSocketId) + ", len: " + String(incomeLen));
				#endif
				for (int i=0;i<_SocketPointersIndex;i++) {
					Socket* soc = _SocketPointers[i];
					if (incomeSocketId == soc->getID()) {
						soc->CheckReceiver(incomeLen);
						break;
					}
				}
				// clsBuffer();
				break;
			}
		}
		// delay(10);
	}
}

#endif