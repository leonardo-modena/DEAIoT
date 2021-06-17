#include <Arduino.h>
#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>
#include <PMserial.h>
#include <ArduinoJson.h>

const char *ssid = "VodafoneFossHouseWifi";
const char *password = "aARHxJFNy7kK4Tce";
const String authToken = "318e4f91b76cdcd9e64dc647cea6e322ee675207581d927f71b8b7b496884170d8b3e964614e09b266cb8737605d389ec4d5037f4384791a06e5d1eed2585db1";


String serverAddress = "64.227.118.102"; 
int port = 3200;


const char *secretKey = "The%20Game";
int intervalTime = 10;
int myId = 1;


SerialPM pms(PMS5003, Serial1); 


WiFiClient client;
HttpClient http = HttpClient(client, serverAddress, port);

DynamicJsonDocument httpGETRequest(String url)
{
  http.beginRequest();


  http.get(url);

  http.sendHeader("authorization", "318e4f91b76cdcd9e64dc647cea6e322ee675207581d927f71b8b7b496884170d8b3e964614e09b266cb8737605d389ec4d5037f4384791a06e5d1eed2585db1");
  http.endRequest();
  
  int httpResponseCode = http.responseStatusCode();

  String payload = "";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.responseBody();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    payload = http.responseBody();
  }

  payload = payload.substring(1, (payload.length() - 1));

  DynamicJsonDocument requestObj(1024);
  deserializeJson(requestObj, payload);

  return requestObj;
}

String httpPOSTRequest(String url, String bodyString)
{
  String body = bodyString;

  http.beginRequest();

  http.post(url);
  
  http.sendHeader("authorization", "318e4f91b76cdcd9e64dc647cea6e322ee675207581d927f71b8b7b496884170d8b3e964614e09b266cb8737605d389ec4d5037f4384791a06e5d1eed2585db1");
  http.sendHeader("Content-Type", "application/x-www-form-urlencoded");
  http.sendHeader("Content-Length", body.length());

  http.beginBody();
  http.print(body);

  http.endRequest();

  int httpResponseCode = http.responseStatusCode();

  String payload = "";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.responseBody();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    payload = http.responseBody();
  }

  return payload;
}

void setup()
{

  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    WiFi.begin(ssid, password);
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  DynamicJsonDocument mySecretKeyObj = httpGETRequest("/device/by_id/" + String(myId));
  secretKey = mySecretKeyObj["secretKey"];

  DynamicJsonDocument intervalTimeObj = httpGETRequest(String("/records_interval/") + String(secretKey));
  intervalTime = intervalTimeObj["intervalTime"];

  Serial.begin(9600);
  Serial1.begin(9600);
  pms.init();
}

void loop()
{

  pms.read();
  Serial.print(F("PM1.0 "));
  Serial.print(pms.pm01);
  Serial.print(F(", "));
  Serial.print(F("PM2.5 "));
  Serial.print(pms.pm25);
  Serial.print(F(", "));
  Serial.print(F("PM10 "));
  Serial.print(pms.pm10);
  Serial.println(F(" [ug/m3]"));

  String bodyString = String("pm25=") + pms.pm25 + String("&pm10=") + pms.pm10 + String("&fkDevice=") + myId;

  Serial.print(bodyString);

  httpPOSTRequest("/records/new_record", bodyString);

  delay(intervalTime * 60000);
}
