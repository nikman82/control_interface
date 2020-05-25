#include <Wire.h>
#include <ESPAsyncWebServer.h>
#include <ETH.h>
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
/*
void onMeasurePulsform(AsyncWebServerRequest *request) {
   request->send(SPIFFS, "text/html","/Pulsform.html", processor);
}
void onMeasureMotor(AsyncWebServerRequest *request) {
   request->send(SPIFFS, "text/html","/Motor_Control.html",processor_m );
}
void onMeasureStap(AsyncWebServerRequest *request) {
   request->send(SPIFFS, "text/html","/Stap_Converter.html",processor_s);
}
void onMeasureMotorStap(AsyncWebServerRequest *request) {
   request->send(SPIFFS, "text/html","/Motor_Control_and_Stap.html",processor_ms );
}

void readVoltage(AsyncWebServerRequest *request) {
    request->send(SPIFFS, "text/plain", readVoltage().c_str());
}
void readCurrent(AsyncWebServerRequest *request) {
    request->send(SPIFFS, "text/plain", readCurrent().c_str());
}

void readVoltage_S(AsyncWebServerRequest *request) {
    request->send(SPIFFS, "text/plain", readStap_U().c_str());
}

void readCurrent_S(AsyncWebServerRequest *request) {
    request->send(SPIFFS, "text/plain", readStap_I().c_str());
}
*/
