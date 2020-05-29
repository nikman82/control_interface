#include <Wire.h>
#include <ESPAsyncWebServer.h>
#include <ETH.h>
#include <SPIFFS.h>
#include <WebSocketsServer.h>
#include "Interface_Library.h" 

// Callback: send homepage
void onIndexRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/Main_Site.html", "text/html");
}

void onMotorRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/Motor_Control.html", "text/html");
}

void onPulsformRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/Pulsform.html", "text/html");
}

void onStapRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/Stap_Converter.html", "text/html");
}

void onMotorStapRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(SPIFFS,"/Motor_Control_and_Stap.html", "text/html");
}

void onStopRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
      data.State_Start = STATE_STOP;
      data.Instruction=18;
      send_Value(data.State_Start, buffer_c,data.Instruction);//send stop instruction
      data.State_Measure=MEASURE_STOP;//stop state in loop()-function 
      //set all data to null
      data.Instruction=0;
      data.State_Mode=0;
      data.State_Start=0;
      data_P.f1=0.0;
      data_P.D=0.0;
      data_P.q=0.0;
      data_P.Um_S=0.0;
      data_P.fp_S = 0.0;
      data_P.D_S = 0.0;
      data_P.Im_S = 0.0;
  request->send(SPIFFS, "/Main_Site.html", "text/html");
}

 
// Callback: send style sheet
void onCSSRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/style.css", "text/css");
}

 
// Callback: send 404 if requested file does not exist
void onPageNotFound(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(404, "text/plain", "Not found");
}

// Callback: receiving any WebSocket message
void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {
 
  // Figure out the type of WebSocket event
  switch(type) {
 
    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", client_num);
      break;
 
    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(client_num);
        Serial.printf("[%u] Connection from ", client_num);
        Serial.println(ip.toString());
      }
      break;
 
    // Handle text messages from client
    case WStype_TEXT:

    if ( strcmp((char *)payload,"Pulsform_State" ) == 0 ) { 
      data.State_Site = STATE_PULSFORM;   
      }else if ( strcmp((char *)payload,"Motor_State" ) == 0 ) { 
         data.State_Site = STATE_MOTOR;
         }else if ( strcmp((char *)payload,"Stap_State" ) == 0 ) {
           data.State_Site = STATE_STAP;
            }else if ( strcmp((char *)payload,"Motor_Stap_State" ) == 0 ) {
              data.State_Site = STATE_MOTOR_STAP; 
          }else  if ( strcmp((char *)payload,"U" ) == 0 ) { 
             webSocket.sendTXT(client_num, readVoltage().c_str());
          }else if( strcmp((char *)payload,"I" ) == 0 ){
             webSocket.sendTXT(client_num,readCurrent().c_str() );
          } else  if ( strcmp((char *)payload,"U_Stap" ) == 0 ) { 
             webSocket.sendTXT(client_num, readStap_U().c_str());
          }else if( strcmp((char *)payload,"I_Stap" ) == 0 ){
             webSocket.sendTXT(client_num,readStap_I().c_str() );
          }
 
      switch(data.State_Site)
      {
        case STATE_PULSFORM:
        {
        Serial.println("Pulsform");
         if ( strcmp((char *)payload,"Deadtime_ja" ) == 0 ) {     //
          data.State_Option=OPTION_WITH_DEAD_TIME;
          data.Instruction=8;  
          send_Value(data.State_Option,buffer_c,data.Instruction);
      } else if ( strcmp((char *)payload, "Deadtime_nein") == 0 ) {
          data.State_Option=OPTION_WITHOUT_DEAD_TIME;
          data.Instruction=8;  
          send_Value(data.State_Option,buffer_c,data.Instruction);
      } else if  ( strcmp((char *)payload, "PWM") == 0 ) {
          Serial.println("getPWM");
          data.State_Mode=STATE_PWM;  
          send_Value(data.State_Mode,buffer_c,data.Instruction=9);
          Serial.println(buffer_c);
      } else if  ( strcmp((char *)payload, "Block") == 0 ) {
          data.State_Mode=STATE_BLOCKTACKTUNG;  
          send_Value(data.State_Mode,buffer_c,data.Instruction=9);
      } else if  ( strcmp((char *)payload, "Schwenk") == 0 ) {
          data.State_Mode=STATE_SCHWENKSTEUERUNG;
          send_Value(data.State_Mode,buffer_c,data.Instruction=9);  
      } else if  ( strcmp((char *)payload, "START_P") == 0 ) {
          data.State_Start=STATE_START;    
          webSocket.sendTXT(client_num, "START_M");//Start Measuring Pulsform
          if(data.State_Mode==STATE_PWM){
          send_Value(data.State_Start, buffer_c,data.Instruction=23);
          }
          else{
          send_Value(data.State_Start, buffer_c,data.Instruction=13);  
          }
          data.State_Measure=MEASURE_PULSFORM;    
          //State Input Pulsform      
       }else if( strcmp((char *)payload,"Fest_q")==0){
          data.State_Param=1; 
       }else if( strcmp((char *)payload,"Fest_f1")==0){
          data.State_Param=2; 
       }else if( strcmp((char *)payload,"Fest_m")==0){
          data.State_Param=3;  
       } else {
           Serial.println("[%u] Message not recognized");
      }
      
       switch(data.State_Param)
            {
             //Parameter Input Pulsform
             case 1:
            {
             q_Message=(char *)payload; 
               //Serial.println(q_Message); 
               data_P.q =q_Message.toDouble();
               if(data_P.q>0.00){
               data.Instruction=1;
               send_Value(data_P.q, buffer_c,data.Instruction);
               }
            }
              break;
             case 2:
            {
              f1_Message=(char *)payload;
              data_P.f1 = f1_Message.toDouble();
              if(data_P.f1>0.00){
              data.Instruction=11;
              send_Value(data_P.f1, buffer_c,data.Instruction);
              }
            }
              break;

            case 3:
            {
              D_Message=(char *)payload;
              data_P.D = D_Message.toDouble();
              if(data_P.D>0.00){
              data.Instruction = 3;
              send_Value(data_P.D, buffer_c,data.Instruction);
            }
            }
            break;
        default:
        break;
            }
        }
        break;
        case STATE_MOTOR:
        {
        if ( strcmp((char *)payload,"Deadtime_ja" ) == 0 ) { 
          data.State_Option=OPTION_WITH_DEAD_TIME;
          data.Instruction=8;  
          send_Value(data.State_Option,buffer_c,data.Instruction);
          } else if  ( strcmp((char *)payload, "Deadtime_nein") == 0 ) {
          data.State_Option=OPTION_WITHOUT_DEAD_TIME;
          data.Instruction=8;  
          send_Value(data.State_Option,buffer_c,data.Instruction); 
          } else if  ( strcmp((char *)payload, "Direction_L") == 0 ) {
          data.State_Direction=DIRECTION_LEFT;
          data.Instruction=17;
          send_Value(data.State_Direction,buffer_c,data.Instruction); 
          } else if  ( strcmp((char *)payload, "Direction_R") == 0 ) {
          data.State_Direction=DIRECTION_RIGHT;
          data.Instruction=17;
          send_Value(data.State_Direction,buffer_c,data.Instruction); 
          } else if  ( strcmp((char *)payload, "Direction_S") == 0 ) {
          data.State_Direction=DIRECTION_STOP;
          data.Instruction=17;
          send_Value(data.State_Direction,buffer_c,data.Instruction); 
          }else if(strcmp((char *)payload, "Motor_q")==0){
          data.State_Param=4;
          }else if(strcmp((char *)payload, "Motor_f1")==0){
          data.State_Param=5;
          } else if  ( strcmp((char *)payload, "START_S") == 0 ) {
          data.State_Start=STATE_START;
           webSocket.sendTXT(client_num, "START_M");//Start Measuring Motor Control 
          send_Value(data.State_Start, buffer_c,data.Instruction=15);
          } else {
          Serial.println("[%u] Message not recognized");
          }
        
           //Parameter Input Motor Control
           switch(data.State_Param){
            case 4:
            {
            q_Message=(char *)payload;
            data_P.q =q_Message.toDouble();
            if(data_P.q>0.00){
            data.Instruction=1;
            send_Value(data_P.q, buffer_c,data.Instruction);
            }
            }
            break;
            case 5:
            {
            f1_Message=(char *)payload;
            data_P.f1 = f1_Message.toDouble();
            if(data_P.f1>0.00){
            data.Instruction=11;
            send_Value(data_P.f1, buffer_c,data.Instruction);
            }
            }
            break;
            deffault:
            break;
          }
        }
        break;     
        case STATE_STAP:
        {
            if ( strcmp((char *)payload, "Stap_UP") == 0 ) {
          data.State_Mode=STATE_STAP_UP;
          data.Instruction=10;
          send_Value(data.State_Mode, buffer_c,data.Instruction);   
           } else if ( strcmp((char *)payload, "Stap_DOWN") == 0 ) {
          data.State_Mode=STATE_STAP_DOWN;
          data.Instruction=10;
          send_Value(data.State_Mode, buffer_c,data.Instruction);
             //State Input Stap Converter       
          }else if(strcmp((char *)payload, "Stap_fp")==0){
           data.State_Param=6;
           }else if(strcmp((char *)payload, "Stap_D")==0){
           data.State_Param=7;  
           }else if(strcmp((char *)payload, "Stap_Im")==0){
           data.State_Param=8; 
           }else if(strcmp((char *)payload, "Stap_Um")==0){
           data.State_Param=9; 
          } else if ( strcmp((char *)payload, "START_S") == 0 ) {
          data.State_Measure=MEASURE_STAP;
          data.State_Start=STATE_START;
           webSocket.sendTXT(client_num, "START_M");//Start Measuring  Stap Converter
          send_Value(data.State_Start, buffer_c,data.Instruction=14);
          } else {
          Serial.println("[%u] Message not recognized");
          }
        


         //Parameter Input Stap Converter
            switch(data.State_Param){
            case 6:
            {
            fp_Message = (char *)payload;  
            data_P.fp_S = fp_Message.toDouble();
            if(data_P.fp_S>0.00){
            data.Instruction=5;
            send_Value(data_P.fp_S, buffer_c,data.Instruction);
            }
            }
            break;
            case 7:
            {
            D_S_Message = (char *)payload;  
            data_P.D_S = D_S_Message.toDouble();
            if(data_P.D_S>0.00){
            data.Instruction=6;
            send_Value(data_P.D_S, buffer_c,data.Instruction);
            }
            }
            break;
            case 8:
            {
            Im_S_Message = (char *)payload;
            data_P.Im_S = Im_S_Message.toDouble();
            if(data_P.Im_S>0.00){
            data.Instruction=7;
            send_Value(data_P.Im_S, buffer_c,data.Instruction);
            }
            }
            break;
            case 9:
            {
            Um_S_Message = (char *)payload;
            data_P.Um_S = Um_S_Message.toDouble();
            if(data_P.Um_S>0.00){
            data.Instruction=4;
            send_Value(data_P.Um_S, buffer_c,data.Instruction); 
            }
            }
            break;
            default:
            break;
            }
              } 
        break;
        case STATE_MOTOR_STAP:
        {
          
        if ( strcmp((char *)payload,"MS_Deadtime_ja" ) == 0 ) {     //
          data.State_Option=OPTION_WITH_DEAD_TIME;
          data.Instruction=8;  
          send_Value(data.State_Option,buffer_c,data.Instruction);
          } else if ( strcmp((char *)payload, "MS_Deadtime_nein") == 0 ) {
          data.State_Option=OPTION_WITHOUT_DEAD_TIME;
          data.Instruction=8;  
          send_Value(data.State_Option,buffer_c,data.Instruction);
          }else if ( strcmp((char *)payload, "MS_Up") == 0 ) {
          data.State_Mode=STATE_STAP_UP;
          data.Instruction=12;
          send_Value(data.State_Mode, buffer_c,data.Instruction);   
           } else if ( strcmp((char *)payload, "MS_Down") == 0 ) {
          data.State_Mode=STATE_STAP_DOWN;
          data.Instruction=12;
          send_Value(data.State_Mode, buffer_c,data.Instruction);
          } else if ( strcmp((char *)payload, "MS_Direction_L") == 0 ) {
          data.State_Direction=DIRECTION_LEFT;
          data.Instruction=17;
          send_Value(data.State_Direction,buffer_c,data.Instruction); 
          } else if ( strcmp((char *)payload, "MS_Sirection_R") == 0 ) {
          data.State_Direction=DIRECTION_RIGHT;
          data.Instruction=17;
          send_Value(data.State_Direction,buffer_c,data.Instruction); 
          } else if ( strcmp((char *)payload, "MS_Direction_S") == 0 ) {
          data.State_Direction=DIRECTION_STOP;
          data.Instruction=17;
          send_Value(data.State_Direction,buffer_c,data.Instruction); 
           //State Input Motor Control and Stap Converter                   
          }else if(strcmp((char *)payload,"MS_q")==0){
          data.State_Param=10;
          }else if(strcmp((char *)payload,"MS_f1")==0){
          data.State_Param=11;
          }else if(strcmp((char *)payload,"MS_Um")==0){
          data.State_Param=12;
          }else if(strcmp((char *)payload,"MS_fp")==0){
          data.State_Param=13;
          }else if(strcmp((char *)payload,"MS_D")==0){
          data.State_Param=14;
          }else if(strcmp((char *)payload,"MS_Im")==0){
          data.State_Param=15;      
          } else if ( strcmp((char *)payload, "MS_START") == 0 ) {
          data.State_Measure=MEASURE_UF_STAP;
          data.State_Start=STATE_START;
           webSocket.sendTXT(client_num, "START_M"); //Start Measuring Motor Control and Stap Converter
          send_Value(data.State_Start, buffer_c,data.Instruction=16);
           } else {
          Serial.println("[%u] Message not recognized");
           }
            switch(data.State_Param)
            {
            //Parameter Input Motor Control and Stap Converter
            case 10: 
            {
            q_Message = (char *)payload;
            data_P.q = q_Message.toDouble();
            if(data_P.q>0.00){
            data.Instruction=1;
            send_Value(data_P.q, buffer_c,data.Instruction);
            }
            }
            break;
            case 11:
            {
            f1_Message = (char *)payload;
            data_P.f1 = f1_Message.toDouble();
            if(data_P.f1>0.00){
            data.Instruction=11;
            send_Value(data_P.f1, buffer_c,data.Instruction);
            }
            }
            break;
            case 12:
            {
            Um_S_Message = (char *)payload;
            data_P.Um_S = Um_S_Message.toDouble();
            if(data_P.Um_S>0.00){
            data.Instruction=4;
            send_Value(data_P.Um_S, buffer_c,data.Instruction);
            }
            }
            break;
            case 13:
            {
            fp_Message = (char *)payload;  
            data_P.fp_S = fp_Message.toDouble();
            if(data_P.fp_S>0.00){
            data.Instruction=5;
            send_Value(data_P.fp_S, buffer_c,data.Instruction);
            }
            }
            break;
            case 14:
            {
            D_S_Message = (char *)payload;  
            data_P.D_S = D_S_Message.toDouble();
            if(data_P.D_S>0.00){
            data.Instruction=6;
            send_Value(data_P.D_S, buffer_c,data.Instruction);
            }
            }
            break;
            case 15:
            {
            Im_S_Message = (char *)payload;  
            data_P.Im_S = Im_S_Message.toDouble();
            if(data_P.Im_S>0.00){
            data.Instruction=7;
            send_Value(data_P.Im_S, buffer_c,data.Instruction);
            }
            }
            break;
             default:
             break;
            } 
        }
        break;
        default:
        break;
      }    
      break;
        
    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}
