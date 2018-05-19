/* File              : IOXhop_BC95.h
   Codeing By IOXhop : www.ioxhop.com
   Sonthaya Nongnuch : www.fb.me/maxthai */

#ifndef IOXhop_BC95_H
#define IOXhop_BC95_H

#include <Arduino.h>
#include "IOXhop_BC95_Base.h"
#include "Socket.h"

// #define DEBUG(...) { Serial.print("[debug] "); Serial.println(__VA_ARGS__); }

static int SetupCMDLen = 3;
static String SetupCMD[3] = {
	"AT+CFUN=1", // Set Phone Functionality
	"AT+NCONFIG=AUTOCONNECT,TRUE",
	"AT+CGATT=1"
};

#define MAX_SOCKET 10
static Socket* _SocketPointers[MAX_SOCKET];
static int _SocketPointersIndex = 0;

class IOXhop_BC95 : public IOXhop_BC95_Base {
	public:
		IOXhop_BC95() { }
		
		void easySetup(bool showProcess = true) ;
		
		// Network
		void begin() ;
		bool reboot() ;
		bool connect() ;
		bool testOnline() ;
		String getIMEI() ;
		bool isConnected() ;
		
		// UDP
		Socket* CreateUDPSocket(int listen_port) ;
		void loop() ;
		void closeAllSocket() ;
		
		
}
;

#endif