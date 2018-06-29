/*
  supported board and hardware competible
  
  Arduino UNO please use software serial on pin8=RX pin9=TX and pin4=RST
  Arduino MEGA please use software serial on pin48=RX pin46=TX and pin4=RST 
  NUCLEO_L476RG please use Hardware serial on pin2=RX pin8=TX and pin4=RST
  
  Pre required install library
         - Altsoftserial 

	Example for post random data to Magellan IoT Platform
	please use payload with json format e.g. {"Temperature":25.5,"Humidity":90.4}
	and {"Location":["13.001","100.44"]} for plot the location on MAP
			     |        |
		        latitude  longitude 

	and {"Lamp":0} or {"Lamp":1"} for show the Lamp status on dashboard
*/
#include "Magellan.h"
Magellan magel;					 
char auth[]="your-key"; 		//Token Key you can get from magellan platform

String payload;

void setup() {
  Serial.begin(9600);
  magel.begin(auth);           //init Magellan LIB
}

void loop() {

  /*
  	Example send random temperature and humidity to Magellan IoT platform
  */
  String Temperature=String(random(0,100));
  String Humidity=String(random(0,100));

  payload="{\"Temperature\":"+Temperature+",\"Humidity\":"+Humidity+"}";       //please provide payload with json format

  magel.post(payload);                            							   //post payload data to Magellan IoT platform
  
}
