/* File              : IOXhop_BC95_Base.cpp
   Codeing By IOXhop : www.ioxhop.com
   Sonthaya Nongnuch : www.fb.me/maxthai */

#ifndef IOXhop_BC95_Base_CPP
#define IOXhop_BC95_Base_CPP

#include "IOXhop_BC95_Base.h"

bool IOXhop_BC95_Base::SendCMD(String cmd, bool newLine = true, int timeout = 0) {
	clsBuffer();
	
	this->_ser->print(cmd);
	if (newLine) this->_ser->println();
	// String ros = readString(0, timeout, cmd);
	// return ros.indexOf(cmd) >= 0;
	return true;
}

String IOXhop_BC95_Base::readString(int len = 0, int timeout = 0, String find = "") {
	int timeout_local = (timeout == 0 ? this->_timeout : timeout);
	long lastPut = millis();
	String bufString = "";
	while ((millis() - lastPut) <= timeout_local) {
		if (this->_ser->available() > 0) {
			bufString += (char)_ser->read();
			// Serial.println(bufString);
			if ((len != 0 && len == bufString.length()) || (find.length() > 0 && bufString.indexOf(find) >= 0)) break;
			lastPut = millis();
		} else {
			// Serial.println(millis() - lastPut);
			delay(10);
		}
	}

	// Serial.println("[SIM] " + bufString);
	return bufString;
}

bool IOXhop_BC95_Base::SendCMDfindOK(String cmd, int timeout = 1000) {
	SendCMD(cmd);
	return findOK(timeout);
}

bool IOXhop_BC95_Base::find(String str, int timeout = 1000) {
	String ros = readString(0, timeout, str);
	return ros.indexOf(str) >= 0;
}

bool IOXhop_BC95_Base::findOK(int timeout = 1000) {
	return find("\r\nOK\r\n", timeout);
}

void IOXhop_BC95_Base::clsBuffer() {
	while (this->_ser->available() > 0) this->_ser->read();
}

#endif