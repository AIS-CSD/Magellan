# AIS Magellan Library
AIS Magellan Library is designed for connect to Magellan IoT Platform. 
The library is using Software Serial for communication with Arduino board.

### Call Library:
	include library
	#include "Magellan.h"
	Magellan magel;

### Initial Magellan Library:
	magel.begin(your-key);           //init Magellan LIB

### Payload Data: 
	Please use payload in JSON format 
	
	Example {"Temperature":25,"Humidity":90}

		payload="{\"Temperature\":"+Temperature+",\"Humidity\":"+Humidity+"}";

### Post Data:
	For post payload to Magellan

		magel.post(payload);

### Example Magellan payload format
	Mark location on MAP
	Example
		payload={"Location":[Latitude,Longitude]}

	Show battery on dashboard
	Battery is range in 0-100 %.
	Example
		payload={"Battery":100}   

	Show Lamp status on dashbord
	please use 0 or 1 to send status
	Example
		payload={"Lamp":0} 
		payload={"Lamp":1}

### Note
	The Magellan library is already include the Altsoftserial library. 

# Quick Started
  1. Connect `DEVIO NB-SHIELD I` on Arduino Board
  2. Open examples POST_MAGELLAN and Upload to your arduino board
