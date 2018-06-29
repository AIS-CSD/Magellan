![alt text](https://www.aismagellan.io/modules/core/client/img/logo-logo-84-px@3x.png "Magellan")
# AIS Magellan Library

   AIS Magellan Library is designed for connect to Magellan IoT Platform.
Please register with [aismagellan](https://www.aismagellan.io)  for see more detail. 

Note The library is using Software Serial for communication with NB-IoT BC95 Shield board.

### supported board and hardware competible
  
  	Arduino UNO please use software serial on pin8=RX pin9=TX and pin4=RST
  	Arduino MEGA please use software serial on pin48=RX pin46=TX and pin4=RST 
  	NUCLEO_L476RG please use Hardware serial on pin2=RX pin8=TX and pin4=RST
  
| Boards        | Pin RX | Pin TX  | Pin RESET |
| ------------- |:------:| -------:| ---------:|
| Arduino UNO   |    8   |    9    |	  4    |
| Arduino MEGA  |   48   |   46    | 	  4    |
| NUCLEO_L476RG |    2   |    8    |      4    |

### Installing Prerequisite Library
	- Altsoftserial

### Call Library:
```C++
	#include "Magellan.h"
	Magellan magel;
```
### Initial Magellan Library:
```C++
	magel.begin(your-key);           //init Magellan LIB
```
### Payload Data: 
	Please use payload in JSON format 
	
	Example {"Temperature":25,"Humidity":90}
```C++
		payload="{\"Temperature\":"+Temperature+",\"Humidity\":"+Humidity+"}";
```
### Post Data:
	For post payload to Magellan
```C++
		magel.post(payload);
```
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
	The Magellan library is not include the Altsoftserial library.please install it before use the magellan Lib 

# Quick Started
  1. Connect `DEVIO NB-SHIELD I` to your Arduino Board
  2. Open examples POST_MAGELLAN and replace token key in code.
  3. Upload to your arduino board and Enjoy with [aismagellan](https://www.aismagellan.io)
