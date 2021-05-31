#include <Arduino.h>
#include <HttpClient.h>
#include <WiFiNINA.h>
#include <PMserial.h>
#include <ArduinoJson.hs>

const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

String serverName = "http://localhost:3200";

int intervalRecord = 0;

SerialPM pms(PMS5003, Serial1);  // PM Serial, RX, TX

void setup() {

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");

  Serial.begin(9600);
  Serial1.begin(9600);
  pms.init();    

  intervalRecord = std::stoi(getInterval("" + serverName + "/interval-for-minute"));               
}

void loop() {
  pms.read();                   // read
  Serial.print(F("PM1.0 "));Serial.print(pms.pm01);Serial.print(F(", "));
  Serial.print(F("PM2.5 "));Serial.print(pms.pm25);Serial.print(F(", "));
  Serial.print(F("PM10 ")) ;Serial.print(pms.pm10);Serial.println(F(" [ug/m3]"));
  
  
  
  delay(5000);   


}

String httpGETRequest(const char* url) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(url);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}