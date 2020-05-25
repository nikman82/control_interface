#include <Wire.h>
#include <ESPAsyncWebServer.h>
#include <ETH.h>
#include <WebSocketsServer.h>
#include "Interface_Library.h"

void send_Value(double data_, char* buffer_,uint8_t buff_){
  dtostrf(data_,7,2, buffer_);
  Wire.beginTransmission(slaveWriter);
  Wire.write(buffer_);
  Wire.write(buff_);
  Wire.endTransmission();

}

void send_Command(const byte buffer_, int instr_)
 { 
  Wire.beginTransmission(slaveWriter);
  Wire.write(buffer_);
  Wire.endTransmission();
  Wire.requestFrom(slaveWriter,instr_);
 }


String readVoltage()
{
 v = (float)data_M.U*sens_U[1]/sys_ADC;
  return String(v);
}

String readCurrent()
{
 c = (float)data_M.I*sens_U[1]/sys_ADC;
  return String(c);
}

String readStap_U()
{
v_s = (float)data_M.U_S*sens_U[1]/sys_ADC;
  return String(v_s);
}

String readStap_I()
{
i_s = (float)data_M.I_S*sens_U[1]/sys_ADC;
  return String(v_s);
}
String processor(const String& var){
  Serial.println(var);
  if(var == "SPANNUNG"){
    return readVoltage();
  }
  else if(var == "STROM"){
    return readCurrent();
  }
}
String processor_s(const String& var){
  Serial.println(var);
  if(var == "STAP_U"){
  return readStap_U();
  }
  else if(var == "STAP_STROM"){
  return readStap_I();
  }
}

String processor_m(const String& var){
  Serial.println(var);
  if(var == "SPANNUNG"){
    return readVoltage();
  }
  else if(var == "STROM"){
    return readCurrent();
  }
  else if(var == "STAP_U"){
  return readStap_U();
  }
  else if(var == "STAP_STROM"){
  return readStap_I();
  }
}

String processor_ms(const String& var){
  Serial.println(var);
  if(var == "SPANNUNG"){
    return readVoltage();
  }
  else if(var == "STROM"){
    return readCurrent();
  }
  else if(var == "STAP_U"){
  return readStap_U();
  }
  else if(var == "STAP_STROM"){
  return readStap_I();
  }
}

void WiFiEvent(WiFiEvent_t event)
{
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = true;
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
}

void testClient(const char * host, uint16_t port)
{
  Serial.print("\nconnecting to ");
  Serial.println(host);

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available());
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("closing connection\n");
  client.stop();
}

 void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}
