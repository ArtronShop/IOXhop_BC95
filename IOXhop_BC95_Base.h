/* File              : IOXhop_BC95_Base.h
   Codeing By IOXhop : www.ioxhop.com
   Sonthaya Nongnuch : www.fb.me/maxthai */

#ifndef IOXhop_BC95_Base_H
#define IOXhop_BC95_Base_H

#include <Arduino.h>
#include <Stream.h>
#include <AltSoftSerial.h>

class IOXhop_BC95_Base {
	public:
		AltSoftSerial *_ser = new AltSoftSerial;
		
		IOXhop_BC95_Base() { }
		
		void begin() { 
			this->_ser->begin(9600); 
		}
		bool SendCMD(String cmd, bool newLine = true, int timeout = 0) ;
		bool SendCMDfindOK(String cmd, int timeout = 1000) ;
		String readString(int len = 0, int timeout = 0, String find = "") ;
		bool find(String str, int timeout = 1000) ;
		bool findOK(int timeout = 1000) ;
		void clsBuffer() ;
		
		
	private:
		int _timeout = 500;

}
;

#endif