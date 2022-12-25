#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include<SoftwareSerial.h>
SoftwareSerial s(3,1);
const char* ssid = "Virus";
const char* password = "mbega123455";
//String serverName = "http://137.184.232.255/smart_public_tape/data.php";
//String serverName = "https://didier.requestcatcher.com/";
String serverName = "http://192.168.43.17/smart_public_tape/data.php";
void setup() {
  s.begin(9600);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
  delay(500);
  }
}

void loop() {
    if(s.available( ) > 0){
      String httpRequestData = s.readStringUntil('\n');
      String reqUrl = serverName+httpRequestData;
      s.println(reqUrl);
      WiFiClient client;
      HTTPClient http;
      http.begin(client, reqUrl);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      int      httpResponseCode = http.GET();
  //    if (httpResponseCode>0) {
        s.println(httpResponseCode);
        String payload = http.getString();
        s.println(payload);
//      }
      http.end();
    }
  }
