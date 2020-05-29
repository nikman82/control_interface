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

byte buffer[MAX_BUFFER];      //Buffer Array for I2C(Wire) Parameter transfer 
char buffer_c[MAX_BUFFER];    //Buffer Array for I2C(Wire) Statment transfer 

//String MENU_Message;

String q_Message;   //get Taktzahl q from Webclient 
String f1_Message;  //get fundamental frequenzy f1 from Webclient    
String D_Message;   //get Duty Circle D from Webclient
String Up_Message;  //get threshold voltage from Webclient
String fp_Message;  //get puls frequenzy fp from Webclient
String D_S_Message; //get Duty Circle D for Stap Converter  from Webclient
String Im_S_Message;//get threshold current for Stap Converter from Webclient
String Um_S_Message;//get threshold current for Stap Converter from Webclien


uint16_t sys_ADC = 1023;		//AD-Condverter resolution number 10 Bit resolution
float sens_U[] = {3.3,5};		//AD-Converter max.Voltage [V] 

float v = 0.00;      //voltage measure variable for three phasen Converter    
float c = 0.00;      //current measure variable for three phasen Converter  
float f = 0.00;      //frequenzy measure variable for three phasen Converter 
float v_s = 0.00;    //voltage measure variable for stap converter mode
float i_s = 0.00;    //current measure variable for stap converter mode

static bool eth_connected = false;

char msg_buf[10];


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

 //WiFi.onEvent(WiFiEvent);
  ETH.begin();

  // On HTTP request for root, provide index.html file
  server.on("/", HTTP_GET, onIndexRequest);
  
  server.on("/Pulsform", HTTP_GET, onPulsformRequest);
  server.on("/Motor_Controll", HTTP_GET, onMotorRequest);
  server.on("/Stap_Converter", HTTP_GET, onStapRequest);
  server.on("/Motor_Controll_and_Stap", HTTP_GET, onMotorStapRequest);
  server.on("/STOP", HTTP_GET, onStopRequest);

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
