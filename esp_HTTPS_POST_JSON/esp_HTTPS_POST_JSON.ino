//----------------------------------------------------------------------------------------------
// Project: ESP8266 send data to Google Spreadsheet
// Author: Aadrit
// Public date: 19 Dec 2022
// Reversion: 2.0
// Description: This project receives an array of 4800 words from UART and sends it to Google Spreadsheet using HTTP POST
//              
//----------------------------------------------------------------------------------------------
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define DATA_SIZE 4800
#define ROWS  80
//----------------------------------------------------------------------------------------------
#define WIFI_SSID   "SpectrumSetup-2D"
#define WIFI_PASS   "purpledrama524"
String GAS_ID_URL = "AKfycbzXsseZGoZYe4ZOqdl5_dE_QKF72vPewV0llsI_nRO2DDQnOsXtEV_b6LghdyFph09yXA";   // Replace by your GAS Service ID 
//----------------------------------------------------------------------------------------------
const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASS;
const char *host = "script.google.com";
const int   httpsPort = 443;  //HTTPS= 443 and HTTP = 80
// SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "CC 92 D5 FD 41 33 CC C4 9F 7D A1 5A 04 F1 11 20 04 3D FB 48";

uint8_t cp;

const char cert_DigiCert_Global_Root_CA [] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
MIIFVzCCAz+gAwIBAgINAgPlk28xsBNJiGuiFzANBgkqhkiG9w0BAQwFADBHMQsw
CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU
MBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw
MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp
Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjEwggIiMA0GCSqGSIb3DQEBAQUA
A4ICDwAwggIKAoICAQC2EQKLHuOhd5s73L+UPreVp0A8of2C+X0yBoJx9vaMf/vo
27xqLpeXo4xL+Sv2sfnOhB2x+cWX3u+58qPpvBKJXqeqUqv4IyfLpLGcY9vXmX7w
Cl7raKb0xlpHDU0QM+NOsROjyBhsS+z8CZDfnWQpJSMHobTSPS5g4M/SCYe7zUjw
TcLCeoiKu7rPWRnWr4+wB7CeMfGCwcDfLqZtbBkOtdh+JhpFAz2weaSUKK0Pfybl
qAj+lug8aJRT7oM6iCsVlgmy4HqMLnXWnOunVmSPlk9orj2XwoSPwLxAwAtcvfaH
szVsrBhQf4TgTM2S0yDpM7xSma8ytSmzJSq0SPly4cpk9+aCEI3oncKKiPo4Zor8
Y/kB+Xj9e1x3+naH+uzfsQ55lVe0vSbv1gHR6xYKu44LtcXFilWr06zqkUspzBmk
MiVOKvFlRNACzqrOSbTqn3yDsEB750Orp2yjj32JgfpMpf/VjsPOS+C12LOORc92
wO1AK/1TD7Cn1TsNsYqiA94xrcx36m97PtbfkSIS5r762DL8EGMUUXLeXdYWk70p
aDPvOmbsB4om3xPXV2V4J95eSRQAogB/mqghtqmxlbCluQ0WEdrHbEg8QOB+DVrN
VjzRlwW5y0vtOUucxD/SVRNuJLDWcfr0wbrM7Rv1/oFB2ACYPTrIrnqYNxgFlQID
AQABo0IwQDAOBgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4E
FgQU5K8rJnEaK0gnhS9SZizv8IkTcT4wDQYJKoZIhvcNAQEMBQADggIBAJ+qQibb
C5u+/x6Wki4+omVKapi6Ist9wTrYggoGxval3sBOh2Z5ofmmWJyq+bXmYOfg6LEe
QkEzCzc9zolwFcq1JKjPa7XSQCGYzyI0zzvFIoTgxQ6KfF2I5DUkzps+GlQebtuy
h6f88/qBVRRiClmpIgUxPoLW7ttXNLwzldMXG+gnoot7TiYaelpkttGsN/H9oPM4
7HLwEXWdyzRSjeZ2axfG34arJ45JK3VmgRAhpuo+9K4l/3wV3s6MJT/KYnAK9y8J
ZgfIPxz88NtFMN9iiMG1D53Dn0reWVlHxYciNuaCp+0KueIHoI17eko8cdLiA6Ef
MgfdG+RCzgwARWGAtQsgWSl4vflVy2PFPEz0tv/bal8xa5meLMFrUKTX5hgUvYU/
Z6tGn6D/Qqc6f1zLXbBwHSs09dR2CQzreExZBfMzQsNhFRAbd03OIozUhfJFfbdT
6u9AWpQKXCBfTkBdYiJ23//OYb2MI3jSNwLgjt7RETeJ9r/tSQdirpLsQBqvFAnZ
0E6yove+7u7Y/9waLd64NnHi/Hm3lCXRSHNboTXns5lndcEZOitHTtNCjv0xyBZm
2tIMPNuzjsmhDYAPexZ3FL//2wmUspO8IFgV6dtxQ/PeEMMA3KgqlbbC1j+Qa3bb
bP6MvPJwNQzcmRk13NfIRmPVNnGuV/u3gm3c
-----END CERTIFICATE-----
)CERT";

X509List cert(cert_DigiCert_Global_Root_CA);

// Json object to store data
StaticJsonDocument<2000> json;
uint16_t store_array[DATA_SIZE];
// This string use to store JSON data
String publish_data;
// Declare object of class WiFiClient
WiFiClientSecure httpsClient;    
String getData;
// GAS ID base URL
String base_url = "/macros/s/" + GAS_ID_URL + "/exec?data=";
String base_url_post = "/macros/s/" + GAS_ID_URL + "/exec";
//----------------------------------------------------------------------------------------------
void setup() 
{
  // Init Serial port for debug
  Serial.begin(115200);
  Serial. flush();
  // Prevents reconnection issue (taking too long to connect)
  WiFi.mode(WIFI_OFF);        
  delay(100);
  // Only Station No AP, This line hides the viewing of ESP as wifi hotspot
  WiFi.mode(WIFI_STA);        
  // Connect to WIFI AP
  WiFi.begin(ssid, password);     
  Serial.println("Connecting to WIFI AP");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}
//----------------------------------------------------------------------------------------------
void loop() 
{

  String readstr = "";
  int byte1,byte2;
  uint16_t data;
  String lineindex;  

  bool go = 1;
  while(!Serial.available()) {Serial.println("wait for serial data ");};
  delay(10);
  if  (Serial.available()) 
  {
//    Serial.print("data array is = ");
    for (uint16_t i = 0; i < DATA_SIZE; i++)
    {
      while(!Serial.available());
      
      byte1 = Serial.read();

      //clear any trailing buffer data
      while(Serial.available())
      {
        Serial.read();
      }

      //send ACK
      Serial.write(35);

      //wait for next data
      while(!Serial.available());
      
      byte2 = Serial.read();
      
      Serial.print("byte 1 = ");
      Serial.println(byte1, HEX);
      Serial.print("byte 2 = ");
      Serial.println(byte2, HEX);
      
      data = ((uint16_t)byte2 << 8) | ((uint16_t)byte1);
      store_array[i] = data;

      //clear any trailing buffer data
      while(Serial.available())
      {
        Serial.read();
      }

      //send ACK
      Serial.write(64);
      Serial.write(64);
      Serial.write(64);
      //wait for next data
      while(!Serial.available() && i!=(DATA_SIZE-1));
      
      Serial.println(data);
      Serial.print("element ");
      Serial.println(i);

    }
    
    Serial.write(64);
    Serial.write(64);
    Serial.write(64);
  }
  go = 0;


  Serial.print("Connect to: ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(httpsPort);
  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  httpsClient.setFingerprint(fingerprint);


//  httpsClient.setTrustAnchors(&cert);
  httpsClient.setTimeout(15000); // 15 Seconds
  int retry = 0; //retry counter
  while((!httpsClient.connect(host, httpsPort)) && (retry < 30))
  {
      delay(100);
      Serial.print(".");
      retry++;
  } 
  // Timeout after 30s
  if (retry >= 30) 
  {
    Serial.println("\r\nConnection failed");
  }
  else 
  {
    Serial.println("\r\nConnected to host");
  }
  

  for (int i = 0; i < ROWS ; i++)
  {
    publish_data = "";
    for (int j = 0; j < 60; j++)
    {
      lineindex = "data" + String(j);
      json[lineindex] = store_array[60*i + j];
    }

    Serial.print("Checkpoint ROW ");
    Serial.println(i);
    
    serializeJson(json, publish_data);
    httpsClient.print(String("POST ") + base_url_post + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Content-Type: application/json\r\n" +
         "Content-Length: " + publish_data.length() + "\r\n" +
         "Connection: Keep-Alive\r\n\r\n" +
         publish_data + "\r\n");  

    Serial.print("Checkpoint HTTP ROW ");
    Serial.println(i);
    
    while (httpsClient.connected()) 
    {  
      String line = httpsClient.readStringUntil('\n');
      if (line == "\r")
      {
        Serial.println("Host was received data");
        break;
      }
      // Serial.println(line);
    }          
    delay(100);  
  }


  Serial.println("Send HTTPS POST request...");
         

   httpsClient.print(String("POST ") + base_url_post + " HTTP/1.1\r\n" +
       "Host: " + host + "\r\n" +
       "User-Agent: BuildFailureDetectorESP8266\r\n" +
       "Content-Type: application/json\r\n" +
       "Content-Length: " + 13 + "\r\n" +
       "Connection: close\r\n\r\n" +
       "{\"data0\":0}" + "\r\n"); 
       
  Serial.println("Request was sent successfully");

  // Waiting for host receiving data             
  while (httpsClient.connected()) 
  {  
    String line = httpsClient.readStringUntil('\n');
    if (line == "\r")
    {
      Serial.println("Host was received data");
      break;
    }
    // Serial.println(line);
  }
  
  Serial.println("Close connection");
    
  delay(5000);  // Send data at every 5 seconds
}
//----------------------------------------------------------------------------------------------
// End of file
//----------------------------------------------------------------------------------------------
