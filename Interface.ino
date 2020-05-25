#include <Wire.h>
#include <ESPAsyncWebServer.h>
#include <ETH.h>
#include <SPIFFS.h>
#include <WebSocketsServer.h>
#include "Interface_Library.h" 

ComData data(0,0,0,0,0,0,0,0,0,0,0);
ParamData data_P(0,0,0,0,0,0,0,0);
MeasureData data_M(0,0,0,0);

//------------------------------------------------------------------------------------------
// Ethernet configuration
//------------------------------------------------------------------------------------------

const char* ssid     = "ESP32-AP";         //Name WLAN Access Point 
const char* password = "12345";  //Access Password
const int ds_port = 53;
const int http_port = 80;
const int ws_port = 1337;

WebSocketsServer webSocket = WebSocketsServer(1337);

uint8_t mac_addr[]  = {0x24, 0x0A, 0xC4, 0x26, 0xCA, 0x4B};//mac-adresse for Ethernet konfiguration
byte my_network[] = {192, 168, 0 }; //IP-Adresse for Webclient by Ethernet kommunication    
const byte my_ip     = 166; //IP-Adresse  
IPAddress eth_ip( my_network[0], my_network[1], my_network[2], my_ip );   //ethernet network setting parameter
//IPAddress my_gateway(   0,   0,   0,   0 );     //
//IPAddress my_dns(       0,   0,   0,   0 );
//IPAddress my_subnet(  255, 255, 255,   255 );

bool setup_Wifi_Done = false;
bool setup_Eth_Done  = false;

//EthernetClient client;
unsigned int  my_port  = 80;   // local port to listen on
ETHClass      my_Ethernet;

AsyncWebServer server(80); //port 80 standard TCP Hypertext Transfer Protocol (HTTP)


//Zuhause 
//const char* ssid     = "KabelBox-9048";         //Name WLAN Access Point 
//const char* password = "99126976548776216854";  //Access Password

//E-Labor Z. 126
//const char* ssid     = "E-Labor2";
//const char* password = "adfecd0864";

byte buffer[MAX_BUFFER];      //Buffer Array for I2C(Wire) Parameter transfer 
char buffer_c[MAX_BUFFER];    //Buffer Array for I2C(Wire) Statment transfer 
/*
const char* INPUT_1 = "q";          //listen for input q parameter from Webclient
const char* INPUT_2 = "f1";         //listen for input f1 parameter from Webclient
const char* INPUT_3 = "D";          //listen for input D parameter from Webclient
const char* S_INPUT_1 = "Um_S";     //listen for input Stap Converter threshold voltage from Webclient
const char* S_INPUT_2 = "fp_S";     //listen for input Stap Converter pulsfrequenzy from Webclient
const char* S_INPUT_3 = "D_S";      //listen for input Stap Converter  Duty Circle from Webclient
const char* S_INPUT_4 = "Im_S";     //listen for input Stap Converter threshold current parameter from Webclient
*/
//String MENU_Message;

String q_Message;   //get Taktzahl q from Webclient 
String f1_Message;  //get fundamental frequenzy f1 from Webclient    
String D_Message;   //get Duty Circle D from Webclient
String Up_Message;  //get threshold voltage from Webclient
String fp_Message;  //get puls frequenzy fp from Webclient
String D_S_Message; //get Duty Circle D for Stap Converter  from Webclient
String Im_S_Message;//get threshold current for Stap Converter from Webclient
String Um_S_Message;//get threshold current for Stap Converter from Webclient
String var;         //measure parameter set to Webserver      
uint16_t sys_ADC = 1023;		//AD-Condverter resolution number 10 Bit resolution
float sens_U[] = {3.3,5};		//AD-Converter max.Voltage [V] 

float v = 0.00;      //voltage measure variable for three phasen Converter    
float c = 0.00;      //current measure variable for three phasen Converter  
float f = 0.00;      //frequenzy measure variable for three phasen Converter 
float v_s = 0.00;    //voltage measure variable for stap converter mode
float i_s = 0.00;    //current measure variable for stap converter mode

static bool eth_connected = false;

char msg_buf[10];
//String param = "";
/***********************************************************
 * Functions
 */
 
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
       }else if  ( strcmp((char *)payload, "STOP") == 0 ){
       }else if  ( strcmp((char *)payload, "U") == 0 ){
       webSocket.sendTXT(client_num, readVoltage().c_str());
       } else {
           Serial.println("[%u] Message not recognized");
      }
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
        break;
        case STATE_MOTOR:
        {
        Serial.println("Motor");
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
        Serial.println("Invalid Input");
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



void setup()
{
Serial.begin(115200);   //UART Boud rate 
Wire.begin(SDA_PIN, SCL_PIN,100000); //I2C setting: SDA pin nummer, SCL pin nummer und transfer frequenzy

  // Make sure we can read the file system
  if( !SPIFFS.begin()){
    Serial.println("Error mounting SPIFFS");
    while(1);
  }

  // Start access point
  WiFi.softAP(ssid, password);
 
 // Print our IP address
  Serial.println();
  Serial.println("AP running");
  Serial.print("My IP address: ");
  Serial.println(WiFi.softAPIP());

  

/*
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);  //Station Mode
  WiFi.begin(ssid, password);//begin Wifi setting
  while (WiFi.status() != WL_CONNECTED) { //check wifi Connecting status
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
*/

 //WiFi.onEvent(WiFiEvent);
  ETH.begin();
/*
// Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){    //main web page
    IPAddress remote_ip = request->client()->remoteIP();
     Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
    request->send(SPIFFS, "Main_Site.html", "text/html");    
  });

  */

  // On HTTP request for root, provide index.html file
  server.on("/", HTTP_GET, onIndexRequest);
  
  server.on("/Pulsform", HTTP_GET, onPulsformRequest);
  server.on("/Motor_Controll", HTTP_GET, onMotorRequest);
  server.on("/Stap_Converter", HTTP_GET, onStapRequest);
  server.on("/Motor_Controll_and_Stap", HTTP_GET, onMotorStapRequest);
  server.on("/STOP", HTTP_GET, onStopRequest);
  /*
  server.on("/", HTTP_GET,onMeasurePulsform);
  server.on("/", HTTP_GET,onMeasureMotor);
  server.on("/", HTTP_GET,onMeasureStap);
  server.on("/", HTTP_GET,onMeasureMotorStap);
  
  server.on("/Voltage", HTTP_GET, readVoltage); 
  server.on("/Current", HTTP_GET, readCurrent);
  server.on("/Voltage_Stap", HTTP_GET, readVoltage_S); 
  server.on("/Current_Stap", HTTP_GET, readCurrent_S);
  */
  

  //Pulsform_();    //three Puls Mode:PWM,Blocktaktung and Schwenksteuerung
/*
  Stap_Converter_();//Stap Converter:Hochsetzsteller and Tiefsetzsteller
  Motor_Controll_();//Motor Controll whit U/f-Kennlinie
  Motor_Controll_and_Stap_();//Motor Controll and Stap Converter 
  Measure_();    //measure data to Web client
 */
/*STOP, SET all DATA TO NULL AND BACK TO START*/


  
  server.onNotFound(notFound); //send notFound Web Page, if no connection wjit server 
  server.begin(); // start server
       
  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}


void loop()
{
// Look for and handle WebSocket data
  webSocket.loop();
  
switch (data.State_Measure) 
{
     
case MEASURE_PULSFORM:  //measure data for mode pulsform 
//measure Data: voltage 
send_Command(data.Instruction=19,2);      

      data_M.U = Wire.read();
      data_M.U <<= 8;
      data_M.U |= Wire.read(); 
    
//measure Data: current  
send_Command(data.Instruction=20,2);

      data_M.I = Wire.read();
      data_M.I <<= 8;
      data_M.I |= Wire.read();
break;
case MEASURE_STAP:      //measure data for mode stap converter 
//measure data: stap converter voltage
send_Command(data.Instruction=21,2);

      data_M.U_S = Wire.read();
      data_M.U_S <<= 8;
      data_M.U_S |= Wire.read(); 
//measure data: stap converter current
send_Command(data.Instruction=22,2);

      data_M.I_S = Wire.read();
      data_M.I_S <<= 8;
      data_M.I_S |= Wire.read();
break;
case MEASURE_UF:        //measure data for mode motor controll whit U/f-Kennlinie
//measure Data: voltage 
send_Command(data.Instruction=19,2);

      data_M.U = Wire.read();
      data_M.U <<= 8;
      data_M.U |= Wire.read(); 
//measure Data: current 
send_Command(data.Instruction=20,2);

      data_M.I = Wire.read();
      data_M.I <<= 8;
      data_M.I |= Wire.read();

break;
case MEASURE_UF_STAP://measure data for mode motor controll whit U/f-Kennlinie
                     //and Stap Converter
//measure Data: voltage 
send_Command(data.Instruction=19,2);

      data_M.U = Wire.read();
      data_M.U <<= 8;
      data_M.U |= Wire.read(); 
//measure Data: current 
send_Command(data.Instruction=20,2);

      data_M.I = Wire.read();
      data_M.I <<= 8;
      data_M.I |= Wire.read();
//measure Data: stap voltage  
send_Command(data.Instruction=21,2);

      data_M.U_S = Wire.read();
      data_M.U_S <<= 8;
      data_M.U_S |= Wire.read();

break;
//measure Data: stap current  
send_Command(data.Instruction=22,2);

      data_M.I_S = Wire.read();
      data_M.I_S <<= 8;
      data_M.I_S |= Wire.read();

break;

case MEASURE_STOP:

break;
  default:
  break;
}
delay(500);//waits 0.5 miliseconds

}
