#ifndef IOXhop_BC95_CPP
#define IOXhop_BC95_CPP

#include "IOXhop_BC95.h"

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
		
	// Wait OK
	if (!findOK(10000)) {
		#ifdef DEBUG
			DEBUG("Reboot fail !, now module not rerun");
		#endif
		return false;
	}
	
	return true;
}

bool IOXhop_BC95::connect() {
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
	
	if (isConnected()) return true;
	
	for (int i=0;i<SetupCMDLen;i++) {
		if (!SendCMDfindOK(SetupCMD[i])) {
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
int IOXhop_BC95::CreateUDPSocket(int listen_port) {
	SendCMD("AT+NSOCR=DGRAM,17," + String(listen_port) + ",0");
	readString(0, 1000, "\r\n");
	String ros = readString(1, 100);
	if (!findOK()) return -1;
	return ros.toInt();
}

bool IOXhop_BC95::SendUDP(int socket_id, String ip, unsigned long port, byte *data, int len) {
	String cmd = "AT+NSOST=";
	cmd += String(socket_id) + ",";
	cmd += ip + ",";
	cmd += String(port) + ",";
	cmd += String(len) + ",";
	String hexStr;
	for (int i=0;i<len;i++) {
		hexStr = String((int)data[i], HEX);
		hexStr.toUpperCase();
		cmd += hexStr;
	}
	SendCMD(cmd);
	readString(0, 1000, "\r\n");
	String ros;
	ros = readString(0, 100, ",");
	int SocketId = ros.substring(0, ros.length() - 1).toInt();
	ros = readString(0, 100, "\r\n");
	int rosLength = ros.substring(0, ros.length() - 2).toInt();
	
	if (SocketId != socket_id || rosLength != len) {
		#ifdef DEBUG
			DEBUG("Send " + cmd + " fail")
			DEBUG(String(socket_id) + " -> " + String(SocketId));
			DEBUG(String(len) + " -> " + String(rosLength));
		#endif
		return false;
	}
	
	if (!findOK()) return false;
	return true;
}

void IOXhop_BC95::UDPReceiver(int socket_id, UDPReceiverCallback callback) {
	int SocketId = -1;
	String ip;
	unsigned long port = 0;
	byte buffer[255];
	int len = 0;
	
	SendCMD("AT+NSORF=" + String(socket_id) + ",255");
	readString(0, 1000, "\r\n");
	String ros = readString(0, 1000, "OK\r\n");
	ros = ros.substring(0, ros.indexOf("OK\r\n"));
	if (ros.length() > 0) {
		String tmp[7];
		int point = 0;
		for (int i=0;i<ros.length()-4;i++) {
			char c = ros.charAt(i);
			if (c != ',')	
				tmp[point] += (char)ros.charAt(i);
			else
				point++;
		}
		SocketId = tmp[0].toInt();
		ip = tmp[1];
		port = atol(tmp[2].c_str());
		len = tmp[3].toInt();
		int index = 0;
		for (int i=0;i<tmp[4].length();i+=2) {
			buffer[index++] = strtol(tmp[4].substring(i, i + 2).c_str(), 0, 16);
		}
		// memcpy(buffer, tmp[4].c_str(), tmp[4].length());
		callback(SocketId, ip, port, buffer, len);
	}
}

#endif