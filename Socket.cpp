/* File              : Socket.cpp
   Codeing By IOXhop : www.ioxhop.com
   Sonthaya Nongnuch : www.fb.me/maxthai */

#ifndef Socket_CPP
#define Socket_CPP

#include "Socket.h"

int Socket::send(String ip, unsigned long port, byte *data, int len) {
	String cmd = "AT+NSOST=";
	cmd += String(this->_socketId) + ",";
	cmd += ip + ",";
	cmd += String(port) + ",";
	cmd += String(len) + ",";
	String hexStr;
	for (int i=0;i<len;i++) {
		hexStr = (data[i] < 0x10 ? "0" : "") + String((int)data[i], HEX);
		hexStr.toUpperCase();
		cmd += hexStr;
	}
	this->_base->SendCMD(cmd);
	this->_base->readString(0, 1000, "\r\n");
	String ros;
	ros = this->_base->readString(0, 100, ",");
	int SocketId = ros.substring(0, ros.length() - 1).toInt();
	ros = this->_base->readString(0, 100, "\r\n");
	int rosLength = ros.substring(0, ros.length() - 2).toInt();
	
	if (SocketId != this->_socketId) {
		#ifdef DEBUG
			DEBUG("Send " + cmd + " fail")
			DEBUG(String(socket_id) + " -> " + String(SocketId));
			DEBUG(String(len) + " -> " + String(rosLength));
		#endif
		return 0;
	}
	
	if (!this->_base->findOK()) return 0;
	return rosLength;
}

void Socket::CheckReceiver(int maxLen = 255) {
	// int SocketId = -1;
	String ip;
	unsigned long port = 0;
	byte buffer[maxLen];
	int len = 0;
	
	this->_base->SendCMD("AT+NSORF=" + String(this->_socketId) + "," + String(maxLen));
	this->_base->readString(0, 1000, "\r\n");
	String ros = this->_base->readString(0, 1000, "OK\r\n");
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
		// SocketId = tmp[0].toInt();
		ip = tmp[1];
		port = atol(tmp[2].c_str());
		len = tmp[3].toInt();
		int index = 0;
		for (int i=0;i<tmp[4].length();i+=2) {
			buffer[index++] = strtol(tmp[4].substring(i, i + 2).c_str(), 0, 16);
		}
		// memcpy(buffer, tmp[4].c_str(), tmp[4].length());
		this->_callback(ip, port, buffer, len);
	}
}

bool Socket::close() {
	return this->_base->SendCMDfindOK("AT+NSOCL=" + String(this->_socketId));
}

#endif