/*
Magellan v1.57 NB-IoT Playgroud Platform .
support Quectel BC95 
NB-IoT with AT command

supported board and hardware competible
  Arduino UNO please use software serial on pin8=RX pin9=TX and pin4=RST
  Arduino MEGA please use software serial on pin48=RX pin46=TX and pin4=RST 

Develop with coap protocol 
reference with 
https://tools.ietf.org/html/rfc7252

support post and get method
only Magellan IoT Platform 
https://www.aismagellan.io

*** the payload limit up on dynamic memory of your board   
*** if you use arduino uno it limit 100
    character for payload string 
 
    example
      payload = {"temperature":25.5}
      the payload is contain 20 character

Author:   Phisanupong Meepragob
Create Date:     1 May 2017. 
Modified: 14 May 2018.

(*bug fix can not get response form buffer but can send data to the server)
(*bug fix get method retransmit message)
(*bug fix multiple definition variable with AIS_NB_BC95)
Released for private use.
*/

#include "Magellan.h"

AltSoftSerial moduleserial;
const char serverIP[] = "103.20.205.85";
char *pathauth;

unsigned int Msg_ID=0;

//################### Buffer #######################
String data_input;
String data_buffer;
String send_buffer;
String rcvdata="";
//################### counter value ################
byte k=0;
//################## flag ##########################
bool end=false;
bool send_NSOMI=false;
bool flag_rcv=true;
bool en_get=true;
bool en_post=true;
bool en_send=true;
bool en_chk_buff=false;
bool getpayload=false;
bool Created=false;
bool sendget=false;
bool NOTFOUND=false;
bool GETCONTENT=false;
bool RCVRSP=false;
bool success=false;
bool connected=false;
bool get_process=false;
bool post_process=false;
bool ACK=false;
bool send_ACK=false;
//################### Parameter ####################
//unsigned int send_cnt=0;
unsigned int resp_cnt=0;
unsigned int resp_msgID=0;
unsigned int get_ID=0;
unsigned int auth_len=0;
unsigned int path_hex_len=0;
//-------------------- timer ----------------------
//long data_interval=10000;
//unsigned int Latency=2000;
unsigned int previous_send=0;
//unsigned int previous_get=0;
unsigned long previous=0;
unsigned int wait_time=5;
//################### Diagnostic & Report ###########
byte maxtry=0;
byte cnt_retrans=0;
byte cnt_retrans_get=0;
byte cnt_error=0;
byte cnt_cmdgetrsp=0;
String LastError="";


Magellan::Magellan()
{
}
/*-------------------------------
    Check NB Network connection
    Author: Device innovation Team
  -------------------------------
*/
/*
bool Magellan:: getNBConnect()
{
  _Serial->println(F("AT+CGATT?"));
  AIS_BC95_RES res = wait_rx_bc(500,F("+CGATT"));
  bool ret;
  if(res.status)
  {
    if(res.data.indexOf(F("+CGATT:0"))!=-1)
      ret = false;
    if(res.data.indexOf(F("+CGATT:1"))!=-1)
      ret = true;
  }
  res = wait_rx_bc(500,F("OK"));
  return(ret);
}
*/

/*-------------------------------
  Check response from BC95
  Author: Device innovation Team
  -------------------------------
*/

/*
AIS_BC95_RES Magellan:: wait_rx_bc(long tout,String str_wait)
{
  unsigned long pv_ok = millis();
  unsigned long current_ok = millis();
  String data_input;
  unsigned char flag_out=1;
  unsigned char res=-1;
  AIS_BC95_RES res_;
  res_.temp="";
  res_.data = "";

  while(flag_out)
  {
    if(_Serial->available())
    {
      data_input = _Serial->readStringUntil('\n');
      
      res_.temp+=data_input;
      if(data_input.indexOf(str_wait)!=-1)
      {
        res=1;
        flag_out=0;
      } 
        else if(data_input.indexOf(F("ERROR"))!=-1)
      {
        res=0;
        flag_out=0;
      }
    }
    current_ok = millis();
    if (current_ok - pv_ok>= tout) 
    {
      flag_out=0;
      res=0;
      pv_ok = current_ok;
    }
  }
  
  res_.status = res;
  res_.data = data_input;
  data_input="";
  return(res_);
}
*/

/*------------------------------
  Connect to NB-IoT Network
              &
  Initial Magellan
  ------------------------------
*/
bool Magellan::begin(char *auth)
{
  bool created=false;
  moduleserial.begin(9600);
  _Serial = &moduleserial;
  
  Serial.println(F("               AIS BC95 NB-IoT Magellan V1.57"));
  //Serial.println(F("                  Magellan IoT Platform"));
  //Serial.println(F("                 Welcome to NB-IoT network"));
  
  /*---------------------------------------
      Initial BC95 Module 
    ---------------------------------------
  */
  if(LastError!=""){
    Serial.print(F("LastError"));
    Serial.println(LastError);    
  }


  Serial.print(F(">>Rebooting "));
  _Serial->println(F("AT+NRB"));
  delay(100);
  /*
  AIS_BC95_RES res = wait_rx_bc(6000,F("OK"));
  if(res.status)
  {
    Serial.println(res.data);
  }
  */
  while(1){
    if(_Serial->available()){
      data_input = _Serial->readStringUntil('\n');
      Serial.println(data_input);
      if(data_input.indexOf(F("OK"))!=-1){
        Serial.println(F("OK"));
        break;
      }
      //else{
      //  Serial.print(F("."));
        //delay(2000);
      //}
    }
  }
  data_input="";
  //delay(5000);
  Serial.print(F(">>Setting"));
  _Serial->println(F("AT+CMEE=1"));
  delay(500);
  Serial.print(F("."));
  /*--------------------------------------
      Pre Test AT command
    --------------------------------------
  */

  /*
  _Serial->println(F("AT"));
  res = wait_rx_bc(1000,F("OK"));
  _Serial->println(F("AT"));
  res = wait_rx_bc(1000,F("OK"));
  */

  //_Serial->println(F("AT"));
  //res = wait_rx_bc(1000,F("OK"));

  /*--------------------------------------
      Config module parameter
    --------------------------------------
  */
  //if(debug) Serial.println(F(">>Setup CFUN"));
  _Serial->println(F("AT+CFUN=1"));
  delay(6000);
  Serial.print(F("."));
  _Serial->println(F("AT+NCONFIG=AUTOCONNECT,true"));
  delay(1000);
  Serial.println(F("OK"));
  //Serial.println(F(">>Connecting to network"));
  _Serial->println(F("AT+CGATT=1"));
  delay(1000);
  Serial.print(F(">>Connecting"));
  /*--------------------------------------
      Check network connection
    --------------------------------------
  */
  /*
  while(!connected){
    connected=getNBConnect();
    Serial.print(F("."));
    delay(2000);
  }
  */
  while(1){
    if(_Serial->available()){
      data_input = _Serial->readStringUntil('\n');
      //Serial.println(data_input);
      if(data_input.indexOf(F("+CGATT:1"))!=-1){
        break;
      }
    }
    _Serial->println(F("AT+CGATT?"));
    Serial.print(F("."));
    delay(2000);
  }
  data_input="";
  /*-------------------------------------
      Create network socket
    -------------------------------------
  */
  Serial.println(F("OK"));
  _Serial->println(F("AT+NSOCR=DGRAM,17,5684,1"));
  delay(6000);
 

  auth_len=0;
  pathauth=auth;
  cnt_auth_len(pathauth);

  if(auth_len>13){
    path_hex_len=2;
  }
  else{
    path_hex_len=1;
  }
  
  previous=millis();
  created=true;

  return created;
}

/*------------------------------
    CoAP Message menagement 
  ------------------------------
*/
void Magellan::printHEX(char *str)
{
  char *hstr;
  hstr=str;
  char out[3]="";
  int i=0;
  bool flag=false;
  while(*hstr)
  {
    flag=itoa((int)*hstr,out,16);
    
    if(flag)
    {
      _Serial->print(out);

      if(debug)
      {
        Serial.print(out);
      }
      
    }
    hstr++;
  }
}
void Magellan::printmsgID(unsigned int messageID)
{
  char Msg_ID[3];
  
  utoa(highByte(messageID),Msg_ID,16);
  if(highByte(messageID)<16)
  {
    if(debug) Serial.print(F("0"));
    _Serial->print(F("0"));
    if(debug) Serial.print(Msg_ID);
    _Serial->print(Msg_ID);
  }
  else
  {
    _Serial->print(Msg_ID);
    if(debug)  Serial.print(Msg_ID);
  }

  utoa(lowByte(messageID),Msg_ID,16);
  if(lowByte(messageID)<16)
  {
    if(debug)  Serial.print(F("0"));
    _Serial->print(F("0"));
    if(debug)  Serial.print(Msg_ID);
    _Serial->print(Msg_ID);
  }
  else
  {
    _Serial->print(Msg_ID);
    if(debug)  Serial.print(Msg_ID);
  }
} 
void Magellan::print_pathlen(unsigned int path_len,char *init_str)
{   
    unsigned int extend_len=0;

    if(path_len>13){
      extend_len=path_len-13;

      char extend_L[3];
      itoa(lowByte(extend_len),extend_L,16);
      _Serial->print(init_str);
      _Serial->print(F("d"));

      if(debug) Serial.print(init_str);
      if(debug) Serial.print(F("d"));

      if(extend_len<=15){
        _Serial->print(F("0"));
        _Serial->print(extend_L);

        if(debug) Serial.print(F("0"));
        if(debug) Serial.print(extend_L);
      }
      else{
        _Serial->print(extend_L);
        if(debug) Serial.print(extend_L);
      }
      

    }
    else{
      if(path_len<=9)
      {
        char hexpath_len[2]="";   
        sprintf(hexpath_len,"%i",path_len);
        _Serial->print(init_str);
        _Serial->print(hexpath_len);
        if(debug) Serial.print(init_str);
        if(debug) Serial.print(hexpath_len);

      }
      else
      {
        if(path_len==10) 
        {
          _Serial->print(init_str);
          _Serial->print(F("a"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("a"));
        }
        if(path_len==11)
        {
          _Serial->print(init_str);
          _Serial->print(F("b"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("b"));
        } 
        if(path_len==12)
        {
          _Serial->print(init_str);
          _Serial->print(F("c"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("c"));
        } 
        if(path_len==13)
        {
          _Serial->print(init_str);
          _Serial->print(F("d"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("d"));
        } 
        
      }
   }  
}
void Magellan::cnt_auth_len(char *auth)
{
  char *hstr;
  hstr=auth;
  char out[3]="";
  int i=0;
  bool flag=false;
  while(*hstr)
  {
    auth_len++;
    
    
    hstr++;
  }
}

/*----------------------------- 
    CoAP Method POST
  -----------------------------
*/
String Magellan::post(String payload)
{
  success=false;
  unsigned int prev_send=millis();
  
  while(!success && !get_process){
    send(payload);
    waitResponse();

    unsigned int cur_send=millis();
    if ((cur_send-prev_send>10000) && send_ACK)
    {
      if(printstate) Serial.println(F("Rsp Timeout"));
      
      en_post=true;
      en_get=true;

      flag_rcv=true;
      post_process=false;
      prev_send=cur_send;
      send_ACK=false;
      break;

    }
  }
  post_process=false;
  return data_buffer;
}

/*----------------------------- 
    CoAP Method GET
  -----------------------------
*/
String Magellan::get(String Resource,String Report="")
{

  int timeout[5]={2000,4000,8000,16000,32000};
  unsigned int previous_time=millis();
  if(!post_process && en_get){
    for (int i = 0; i < 4; ++i)
      {
        get_process=true;
        get_data(Resource);
        while(true)
        {
          unsigned int current_time=millis();

          if(current_time-previous_time>timeout[i]){
            previous_time=current_time;
            if(i==4){
              if(printstate) Serial.println(F("Get timeout"));
              get_process=false;
            }
            break;
          }
          waitResponse();
        }


        if(rcvdata.length()>0 && GETCONTENT){
          get_process=false;
          break;
        }
        else{
          if(printstate) Serial.print(F("Retransmit"));
          if(printstate) Serial.println(i+1);

          //if(printstate) Serial.print(F("wait_time :"));
          if(printstate) Serial.println(timeout[i]);
        }
      }
  }
  return rcvdata;
}

/*-----------------------------
    Massage Package
  -----------------------------
*/
void Magellan::send(String payload,int style=1,int interval=20)
{
  AIS_BC95_RES res;
  _Serial->flush();

  unsigned int currenttime=millis();

  //retransmit data
  if(!flag_rcv && cnt_retrans>=4){
    if(printstate) Serial.println(F("Timeout"));
    _Serial->println(F("AT"));
    
    /*
    res = wait_rx_bc(100,F("OK"));
    if(res.status)
    {
      if(printstate) Serial.println(res.data);
    }
    */
  }

  //Load new payload for send
  if(flag_rcv || cnt_retrans>=4)
  {
    //if(printstate) Serial.println(F("Load new payload"));
    send_buffer=payload;
    Msg_ID++;

    cnt_retrans=0;
    en_chk_buff=false;
    en_send=true;
    wait_time=2;
    data_buffer="";				//Ver 1.54
  }
  else{

    if (en_post)
    {
      en_chk_buff=true;
      unsigned int Difftime=currenttime-previous_send;
      if(Difftime>=wait_time*1000){

        //if(debug) Serial.print(F("Difftime :"));
        //if(debug) Serial.println(Difftime);
        if(printstate) Serial.println();
        if(printstate) Serial.print(F("wait_time :"));
        if(printstate) Serial.println(wait_time*1000);

        if(printstate) Serial.print(F("Retransmit :"));
        if(printstate) Serial.println(cnt_retrans+1);

        cnt_retrans++;
        previous_send=currenttime;
        en_send=true;
      }
      else{
        en_send=false;
      }


      if(cnt_retrans>=4){
        post_process=false;
        success=true;
      } 
    }
  }

  char data[send_buffer.length()+1]="";
  send_buffer.toCharArray(data,send_buffer.length()+1);
  
  if(debug) Serial.println(resp_msgID);

  if(en_send && en_post){
      post_process=true;
      ACK=false;

      //send_cnt++;
      //if(printstate) Serial.print(F("post cnt :"));
      //if(printstate) Serial.println(send_cnt);
      if(printstate) Serial.print(F(">> post: Msg_ID "));
      if(printstate) Serial.print(Msg_ID);
      if(printstate) Serial.print(F(" "));
      if(printstate) Serial.print(send_buffer);

      _Serial->print(F("AT+NSOST=0,"));
      _Serial->print(serverIP);
      _Serial->print(F(",5683,"));

      //if(debug) Serial.print(F("AT+NSOST=0,")); 
      //if(debug) Serial.print(serverIP);
      //if(debug) Serial.print(F(",5683,")); 


      //if(debug) Serial.print(auth_len+11+path_hex_len+send_buffer.length());
      _Serial->print(auth_len+11+path_hex_len+send_buffer.length());

      //if(debug) Serial.print(F(",4002"));
      _Serial->print(F(",4002"));
      printmsgID(Msg_ID);
      _Serial->print(F("b54e42496f54"));
      //if(debug) Serial.print(F("b54e42496f54"));
      print_pathlen(auth_len,"0");

      printHEX(pathauth);
      _Serial->print(F("ff")); 
      //if(debug) Serial.print(F("ff"));
      printHEX(data);  
      _Serial->println();

      if(printstate) Serial.println();
      
      switch (cnt_retrans) {
        case 1:
          // statements
          wait_time=4;
          break;
        case 2:
          // statements
          wait_time=8;
          break;
        case 3:
          // statements
          wait_time=16;
          break;
        case 4:
          // statements
          wait_time=32;
          break;
        //default:
          // statements
      }

      //if(printstate) Serial.println(wait_time*1000);
      flag_rcv=false;
      previous_send=millis();
  }   
}

String Magellan::get_data(String Resource)
{
  
  char data[Resource.length()+1]="";
  Resource.toCharArray(data,Resource.length()+1); 
  GETCONTENT=false;
  ACK=false;
  Msg_ID++;
  get_ID=Msg_ID;
  if(printstate) Serial.print(F(">> GET data : Msg_ID "));
  if(printstate) Serial.print(Msg_ID);
  if(printstate) Serial.print(" ");
  if(printstate) Serial.print(Resource);
  
  _Serial->print(F("AT+NSOST=0,"));
  _Serial->print(serverIP);
  _Serial->print(F(",5683,"));

  //if(debug) Serial.print(F("AT+NSOST=0,")); 
  //if(debug) Serial.print(serverIP);
  //if(debug) Serial.print(F(",5683,")); 

  unsigned int path_len=Resource.length();
  if(path_len>13){
    if(debug) Serial.print(52+path_len);
    _Serial->print(52+path_len);
  }
  else{
    if(debug) Serial.print(51+path_len);
    _Serial->print(51+path_len);
  }

  //if(debug) Serial.print(F(",4001"));
  _Serial->print(F(",4001"));
  printmsgID(Msg_ID);
  _Serial->print(F("b54e42496f54"));
  //if(debug) Serial.print(F("b54e42496f54"));
  _Serial->print(F("0d17"));
  //if(debug) Serial.print(F("0d17"));
  printHEX(pathauth);

  print_pathlen(path_len,"4");

  printHEX(data);

  _Serial->print(F("8106"));
  //if(debug) Serial.print(F("8106"));

  _Serial->println();

  if(printstate) Serial.println();
  sendget=true;
  return rcvdata;
}

/*-------------------------------------
    Response Message management

    Example Message
    Type: ACK
    MID: 000001
    Code: Created
    Payload:200
  -------------------------------------
*/
void Magellan::print_rsp_header(const String Msgstr)
{
  
  resp_msgID = (unsigned int) strtol( &Msgstr.substring(4,8)[0], NULL, 16);
  print_rsp_Type(Msgstr.substring(0,2),resp_msgID);

   switch((int) strtol(&Msgstr.substring(2,4)[0], NULL, 16))
   {
      case CREATED: 
                    NOTFOUND=false;
                    GETCONTENT=false;
                    RCVRSP=true;
                    //if(printstate) Serial.println(F("2.01 CREATED")); 
                    break;
      case DELETED: //if(printstate) Serial.println(F("2.02 DELETED")); 
                    break;
      case VALID: //if(printstate) Serial.println(F("2.03 VALID")); 
                  break;
      case CHANGED: //if(printstate) Serial.println(F("2.04 CHANGED")); 
                  break;
      case CONTENT: 
                    NOTFOUND=false;
                    GETCONTENT=true;
                    RCVRSP=false;
                    //if(printstate) Serial.println(F("2.05 CONTENT")); 
                    break;
      case CONTINUE: //if(printstate) Serial.println(F("2.31 CONTINUE")); 
                    break;
      case BAD_REQUEST: //if(printstate) Serial.println(F("4.00 BAD_REQUEST")); 
                    break;
      case FORBIDDEN: //if(printstate) Serial.println(F("4.03 FORBIDDEN")); 
                    break;
      case NOT_FOUND: 
                    //if(printstate) Serial.println(F("4.04 NOT_FOUND"));
                    GETCONTENT=false; 
                    NOTFOUND=true;
                    RCVRSP=false;
                    break;
      case METHOD_NOT_ALLOWED: 
                    RCVRSP=false;
                    //if(printstate) Serial.println(F("4.05 METHOD_NOT_ALLOWED")); 
                    break;
      case NOT_ACCEPTABLE: //if(printstate) Serial.println(F("4.06 NOT_ACCEPTABLE")); 
                    break;
      case REQUEST_ENTITY_INCOMPLETE: //if(printstate) Serial.println(F("4.08 REQUEST_ENTITY_INCOMPLETE")); 
                    break;
      case PRECONDITION_FAILED: //if(printstate) Serial.println(F("4.12 PRECONDITION_FAILED")); 
                    break;
      case REQUEST_ENTITY_TOO_LARGE: //if(printstate) Serial.println(F("4.13 REQUEST_ENTITY_TOO_LARGE")); 
                    break;
      case UNSUPPORTED_CONTENT_FORMAT: //if(printstate) Serial.println(F("4.15 UNSUPPORTED_CONTENT_FORMAT")); 
                    break;
      case INTERNAL_SERVER_ERROR: //if(printstate) Serial.println(F("5.00 INTERNAL_SERVER_ERROR")); 
                    break;
      case NOT_IMPLEMENTED: //if(printstate) Serial.println(F("5.01 NOT_IMPLEMENTED")); 
                    break;
      case BAD_GATEWAY: //if(printstate) Serial.println(F("5.02 BAD_GATEWAY")); 
                    break;
      case SERVICE_UNAVAILABLE: //if(printstate) Serial.println(F("5.03 SERVICE_UNAVAILABLE")); 
                    break;
      case GATEWAY_TIMEOUT: //if(printstate) Serial.println(F("5.04 GATEWAY_TIMEOUT")); 
                    break;
      case PROXY_NOT_SUPPORTED: //if(printstate) Serial.println(F("5.05 PROXY_NOT_SUPPORTED")); 
                    break;

      default : //Optional
                GETCONTENT=false;
   }

   if(printstate) Serial.print(F("Msg_ID "));

   
   //Serial.println(number);
   if(printstate) Serial.print(resp_msgID);
}

void Magellan::print_rsp_Type(const String Msgstr,unsigned int msgID)
{
  if(Msgstr.indexOf(ack)!=-1)
  {
    //if(printstate) Serial.println(F("Acknowledge"));
    if(printstate) Serial.print(F("<< ACK: "));
    flag_rcv=true;
    post_process=false;
    success=true;
    en_post=true;
    en_get=true;
    ACK=true;
    send_ACK=false;
    cnt_cmdgetrsp=0;
  }
  if(Msgstr.indexOf(con)!=-1)
  {
    //if(printstate) Serial.println(F("Confirmation"));
    if(printstate) Serial.print(F("<< CON: "));
    //if(printstate) Serial.println();
    resp_cnt++;

    en_post=false;
    en_get=false;

    if (resp_cnt>5)
    {
      _Serial->print(F("AT+NSOST=0,"));
      _Serial->print(serverIP);
      _Serial->print(F(",5683,"));
      _Serial->print(F("4"));
      _Serial->print(F(",7000"));
      printmsgID(msgID);
      _Serial->println();      
      resp_cnt=0;
    }
    else{
      _Serial->print(F("AT+NSOST=0,"));
      _Serial->print(serverIP);
      _Serial->print(F(",5683,"));
      _Serial->print(F("4"));
      _Serial->print(F(",6000"));
      printmsgID(msgID);
      _Serial->println();
      send_ACK=true;      
    }

    success=true;
    ACK=false;
    cnt_cmdgetrsp=0;
  }
  if(Msgstr.indexOf(rst)!=-1)
  {
    //if(printstate) Serial.println(F("Reset"));
    if(printstate) Serial.print(F("<< RST: "));
    flag_rcv=true;
    post_process=false;
    success=true;
    ACK=false;
    cnt_cmdgetrsp=0;
  }
  if(Msgstr.indexOf(non_con)!=-1)
  {
    //if(printstate) Serial.println(F("Non-Confirmation"));
    if(printstate) Serial.print(F("<< Non-Con: "));
    flag_rcv=true;
    post_process=false;
    success=true;
    ACK=false;
    cnt_cmdgetrsp=0;
  }
}

/*-----------------------------------
  Get response data from BC95 Buffer 
  -----------------------------------  
*/
void Magellan::miniresponse(String rx)
{
  print_rsp_header(rx);


  String data_payload="";

  unsigned int indexff=rx.indexOf(F("FF"));

  if(rx.indexOf(F("FFF"))!=-1)
  {
      data_payload=rx.substring(indexff+3,rx.length());

      if(printstate) Serial.print(F(" RSP:"));
      data_buffer="";                                          //clr buffer
      for(byte k=2;k<data_payload.length()+1;k+=2)
      {
        char str=(char) strtol(&data_payload.substring(k-2,k)[0], NULL, 16);
        if(printstate) Serial.print(str);

        if(GETCONTENT or RCVRSP){
          data_buffer+=str;
        }
      }
      if(GETCONTENT)
      {
        rcvdata=data_buffer;
        data_buffer="";
        getpayload=true;
      } 
      if(printstate) Serial.println(F(""));
  }
  else
  {
      data_payload=rx.substring(indexff+2,rx.length());
      if(printstate) Serial.print(F(" RSP:"));
      data_buffer="";                                        //clr buffer
      for(byte k=2;k<data_payload.length()+1;k+=2)
      {
        char str=(char) strtol(&data_payload.substring(k-2,k)[0], NULL, 16);
        if(printstate) Serial.print(str);

        if(GETCONTENT or RCVRSP){
          data_buffer+=str;
        }
      }
      if(GETCONTENT) {
        rcvdata=data_buffer;
        data_buffer="";
        getpayload=true;
      } 
      if(printstate) Serial.println(F(""));     
  }

  if(ACK)
  { 
    if(printstate) Serial.println(F("------------ End ------------"));
    if(printstate) Serial.println();
    //if(printstate) Serial.println();
  }
}

void Magellan:: waitResponse()
{ 
  unsigned long current=millis();
  if(en_chk_buff && (current-previous>=500) && !(_Serial->available()))
  {
      _Serial->println(F("AT+NSORF=0,100"));
      cnt_cmdgetrsp++;
      previous=current;
  }

  if(_Serial->available())
  {
    char data=(char)_Serial->read();
    if(data=='\n' || data=='\r')
    {
      if(k>2)
      {
        end=true;
        k=0;
      }
      k++;
    }
    else
    {
      data_input+=data;
    }
    if(debug) Serial.println(data_input); 
  }
  if(end){

      if(data_input.indexOf(F("+NSONMI:"))!=-1)
      {
          //if(debug) Serial.print(F("send_NSOMI "));
          //if(debug) Serial.println(data_input);
          if(data_input.indexOf(F("+NSONMI:"))!=-1)
          {
            //if(debug) Serial.print(F("found NSONMI "));
            _Serial->println(F("AT+NSORF=0,100"));
            data_input=F("");
            send_NSOMI=true;
            if(printstate) Serial.println();
          }
          end=false;
      }
      else
        {
          //if(debug) Serial.print(F("get buffer "));
          //if(debug) Serial.println(data_input);

          end=false;
          if(data_input.indexOf(F("0,103.20.205.85"))!=-1)
          {
            int index1 = data_input.indexOf(F(","));
            if(index1!=-1) 
            {
              int index2 = data_input.indexOf(F(","),index1+1);
              index1 = data_input.indexOf(F(","),index2+1);
              index2 = data_input.indexOf(F(","),index1+1);
              index1 = data_input.indexOf(F(","),index2+1);
              //if(debug) Serial.println(data_input.substring(index2+1,index1));
              //if(debug) Serial.println(F("--->>>  Response "));
              miniresponse(data_input.substring(index2+1,index1));
            }
          }
          else if((data_input.indexOf(F("ERROR"))!=-1))
          {
            if((data_input.indexOf(F("+CME ERROR")))!=-1)
            {
              int index1 = data_input.indexOf(F(":"));
              if(index1!=-1) {
                LastError=data_input.substring(index1+1,data_input.length());

                if((LastError.indexOf(F("159")))!=-1)
                {
                   Serial.print(F("Uplink busy"));
                   data_buffer="";				//Ver 1.54  clr buffer   
                }
              }
            }
          }

          send_NSOMI=false;
          data_input=F("");
          }       
        }
}



