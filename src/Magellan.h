#ifndef Magellan_h
#define Magellan_h

#define ATLSOFTSERIAL 1
#include <Arduino.h>
#include <Stream.h>

#define MODE_STRING 0
#define MODE_STRING_HEX 1

/*
#if ATLSOFTSERIAL 
	#include "AltSoftSerial.h"
#endif
	#include <SoftwareSerial.h>
*/
#if ATLSOFTSERIAL 
	#include "AltSoftSerial.h"
#endif

  const byte maxretrans=4;	

  const char con[]="40";
  const char non_con[]="50";
  const char ack[]="60";
  const char rst[]="70";

  const char EMPTY[]="00";
  const char GET[]="01";
  const char POST[]="02";
  const char PUT[]="03";
  const char DELETE[]="04";

struct AIS_BC95_RES
{
	char status;
	String data;
	String temp;
};

/*
struct signal
{
	String csq;
	String rssi;
	String ber;
};
*/

/*
struct pingRESP
{
	bool status;
	String addr;
	String ttl;
	String rtt;
};
*/

enum rspcode {
	CREATED=65,
	DELETED=66,
	VALID=67,
	CHANGED=68,
	CONTENT=69,
	CONTINUE=95,
	BAD_REQUEST=128,
	FORBIDDEN=131,
	NOT_FOUND=132,
	METHOD_NOT_ALLOWED=133,
	NOT_ACCEPTABLE=134,
	REQUEST_ENTITY_INCOMPLETE=136,
	PRECONDITION_FAILED=140,
	REQUEST_ENTITY_TOO_LARGE=141,
	UNSUPPORTED_CONTENT_FORMAT=143,
	INTERNAL_SERVER_ERROR=160,
	NOT_IMPLEMENTED=161,
	BAD_GATEWAY=162,
	SERVICE_UNAVAILABLE=163,
	GATEWAY_TIMEOUT=164,
	PROXY_NOT_SUPPORTED=165
};


class Magellan
{


public:
	Magellan();

	bool debug;
	bool default_server;
	bool printstate=true;

	//void reset();
	//bool getNBConnect();
	//signal getSignal();
    //pingRESP pingIP(char *IP);

    //################# Magellan Platform ########################
    bool begin(char *auth);
    void printHEX(char *str);
    void printmsgID(unsigned int messageID);
    void cnt_auth_len(char *auth);
    String post(String payload);
    void send(String payload,int style=1,int interval=20);
    String get(String Resource,String Report="");
    String get_data(String Resource);
    void  waitResponse();
private:
	
	AIS_BC95_RES  wait_rx_bc(long tout,String str_wait);
	void print(char *str);
	void print_rsp_header(const String Msgstr);
	void print_rsp_Type(const String Msgstr,unsigned int msgID);
	void miniresponse(String rx);

	void print_pathlen(unsigned int path_len,char *init_str);

protected:
	 Stream *_Serial;
};


#endif