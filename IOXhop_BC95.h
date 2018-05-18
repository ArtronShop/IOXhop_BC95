#ifndef IOXhop_BC95_H
#define IOXhop_BC95_H

#include <Arduino.h>
#include "IOXhop_BC95_Base.h"

#define DEBUG(...) { Serial.print("[debug] "); Serial.println(__VA_ARGS__); }

static int SetupCMDLen = 3;
static String SetupCMD[3] = {
	"AT+CFUN=1", // Set Phone Functionality
	"AT+NCONFIG=AUTOCONNECT,TRUE",
	"AT+CGATT=1"
};

typedef void(*UDPReceiverCallback)(int, String, unsigned long, byte*, int);

class IOXhop_BC95 : public IOXhop_BC95_Base {
	public:
		IOXhop_BC95() { }
	
		// Network
		void begin() ;
		bool reboot() ;
		bool connect() ;
		bool testOnline() ;
		String getIMEI() ;
		bool isConnected() ;
		
		// UDP
		int CreateUDPSocket(int listen_port) ;
		bool SendUDP(int socket_id, String ip, unsigned long port, byte *data, int len) ;
		void UDPReceiver(int socket_id, UDPReceiverCallback callback) ;
		
		
}
;

#endif