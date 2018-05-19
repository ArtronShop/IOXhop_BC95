/* File              : Socket.h
   Codeing By IOXhop : www.ioxhop.com
   Sonthaya Nongnuch : www.fb.me/maxthai */

#ifndef Socket_H
#define Socket_H

#include <Arduino.h>
#include "IOXhop_BC95_Base.h"

typedef void(*UDPReceiverCallback)(String, unsigned long, byte*, int);

class Socket {
	public:
		Socket(IOXhop_BC95_Base *base, int socketId) : _base(base), _socketId(socketId) {  }
		~Socket() {
			this->close();
		}
		
		// 
		int getID() { return _socketId; }
		
		int send(String ip, unsigned long port, byte *data, int len) ;
		void onReceiver(UDPReceiverCallback callback) {
			this->_callback = callback;
		};
		void CheckReceiver(int maxLen = 255) ;
		bool close() ;
		
	private:
		IOXhop_BC95_Base *_base = NULL;
		int _socketId = -1;
		UDPReceiverCallback _callback;
		
}
;

#endif