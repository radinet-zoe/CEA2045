/*
 * cea2045lib.h
 *
 *  Created on: 29-Sep-2014
 *      Author: shyama
 */

#ifndef CEA2045LIB_H_
#define CEA2045LIB_H_
#define CEA2045Port "/dev/ttyUSB0"
#define CEA2045logFile "/opt/venmanager/log/CEA2045.log"
#define dploggingfile "/opt/venmanager/data/dplogging.csv"
#define SHED 0x01
#define END_SHED 0x02
#define APP_ACK 0x03
#define APP_NAK 0x04
#define POWER_LEVEL 0x06
#define PRESENT_RELATIVE_PRICE 0x07
#define NEXT_RELATIVE_PRICE 0x08
#define TIME_REMAINING_PRICE 0x09
#define CPP 0x0a
#define GRID_EMERGENCY 0x0b
#define GRID_GUIDANCE 0x0c
#define COMM_STATUS 0x0e
#define CUST_OVERRIDE 0x11
#define OPER_STATE_REQ 0x12
#define OPER_STATE_RESP 0x13
#define SLEEP_STATE 0x14
#define WAKE_STATE 0x15
#define SIMPLE_TIME_SYNC 0x16
#define LOADUP 0x17
#define INFO_REQ 0x01

#define ENERGY_PRICE 0x03
#define SETPOINT 0x03
#define TEMP_OFFSET 0x03
#define COMMODITY 0x06

#define TIMEOUT 500				// 500 millisecond from first to last byte of message
#define NB_ENABLE 0
#define NB_DISABLE 1

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/select.h>

/* Structure definitions */
struct cea2045BasicStruct {
	float	priceRatio;			// Present Relative Price ratio to send to SGD
	time_t	endTime;			// Date/Time in OADR format converted to UTC
	bool	overRide;			// Override status
	bool	online;				// Outside Communication status
	bool	sleepState;			// Sleep status
	bool	pendingOpState;		// Set to true if a OpState response is pending
	bool 	pendingAppAck;		// Flag to show a Application Ack is waiting to be sent to SGD
	bool 	pendingAppNak;		// Flag to show a Application Nak is waiting to be sent to SGD
	unsigned char opState;		// Latest operationa state
	unsigned char appAckSend;	// Application Ack/Nak Opcode 2 value to be sent to SGD in App Ack/Nak message
	unsigned char appAckRtn;	// Return opcode expected on SGD application ack response
	unsigned char opCode1;		// Last received OpCode1 from SGD
	unsigned char opCode2;		// Last received OpCode2 from SGD
	time_t timeoutTime;			// Time to wait for reply, 0 = not waiting for reply
};

struct cea2045InterStruct {
	unsigned char respOpCode1;	// Expected response opcode 1, 0 if none pending
	unsigned char respOpCode2;	// Expected response opcode 2, 0 if none pending
	bool respPending;			// A reaponse is expected
	time_t timeoutTime;			// Time to wait for reply, 0 = not waiting for reply
};

struct linkStruct {
	int		txCount;			// App layer writes a value to this after filling the txBuffer
								// Link layer clears this value after sending characters
	int		rxCount;			// Link layer writes a value to this after all characters are received
								// App layer clears this value after processing characters
	int		nakVal;				// Value returned in link nak from SGD or -1 if link ack returned
	bool	rxOverflow;			// Set by Link layer if rxCount exceeds rxBuffer size. Cleared on rxFlush
	bool	rxFlush;			// App layer sets to true to flush rxBuffer.
								// Link layer sets to false after setting rxCount to 0
	bool	txFlush;			// App layer sets to true to flush txBuffer.
								// Link layer sets to false after setting txCount to 0
	bool	txBlock;			// Transmit must wait 100ms before sending message after link ack/nak.
	bool	sunSpec;			// App layer sets to true to this is a SunSpec SGD
};

struct devInfoStruct {			// Device Information structure
	unsigned char respCode;		// Response code
	unsigned char ceaVer[2];	// ASCII CEA-2045 version
	uint16_t vendorID;			// 16 bit vendor ID
	uint16_t devType;			// 16 bit device type
	uint16_t devRev;			// 16 bit device revision
	uint32_t capBitmap;			// 32 bit capability bitmap
	unsigned char reserve;		// Reserved (not currently used)
	unsigned char modelNum[16];	// ASCII model number
	unsigned char serialNum[16];	// ASCII serial number
	unsigned char firmYear;		//Firmware year
	unsigned char firmMonth;	//Firmware month
	unsigned char firmDay;		//Firmware day
	unsigned char firmMajor;	//Firmware major
	unsigned char firmMinor;	//Firmware minor
};

struct commodityreadStruct{		//Commodity Read Structure
	unsigned char respCode;		//Response code
	uint commoditycode[3];
	uint64_t instantaneousrate[3];
	uint64_t cumulativeamount[3];
	float payload;
	float capmax;
	float capcurrent;
};



// Function declorations
int CEA2045basic(unsigned char cmd,long cmdparam, float cmdparamf);
int CEA2045basicRx();
int CEA2045inter(unsigned char opcode1,unsigned char opcode2);
int CEA2045interRx();
int linkLayer();
long int calcTimeDiff(struct timeval *compareTime);
void addTimeOffset(struct timeval *timestruct, long int adder);
unsigned char calcRelativePriceByte(float relativePrice);
int calcChecksum(unsigned char* buffData, int msgSize, bool validate);
int initSerialPort(char commport[]);
int CEA2045inter_SetEnergyPrice(unsigned char opcode1,unsigned char opcode2, uint32_t currentPrice, uint32_t currencyCode, uint digitsafterDecimal);
int CEA2045inter_SetTempOffset(unsigned char opcode1,unsigned char opcode2,uint currentOffset, uint unit);
int CEA2045inter_SetSetPoint(unsigned char opcode1, unsigned char opcode2, uint16_t devType, int units, int16_t setpoint1, int16_t setpoint2);
int open_serialport(const char *commport);
int close_serialport();
void logger(const char *message);
bool getDeviceInfo();
bool sendCommstatus(int status);
int getOperationalState();
bool setCommoditySubscription();
bool oadr_cea_logReportInfo(int status);
void checkCustOverride();
bool oadr_cea_EventMap(const char *signalname, const char *signaltype, const char *signalval, int param);
int CEA2045inter_Setcommodity(unsigned char opcode1,unsigned char opcode2, int commtype1, int ufreq1, int commtype2, int ufreq2, int commtype3, int ufreq3);
int check_ucmtype();




#endif /* CEA2045LIB_H_ */
