#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
const int redLED = 12;    //D3
const int greenLED = 13;  //D2
const int blueLED = 14;   //D1e
DNSServer dns;
WiFiManager wifiManager;
String serverName = "http://diawan.io/api/get_url";
String formattedDate;
String linkdiawan = "http://asia-southeast2-diawanpremium-6d4a9.cloudfunctions.net/spechToTextDevice";
String name;
int lastOffState, lastOnState, onState, offState;
int red, green, blue;
int httpResponseCode;
bool portalRunning = false;
unsigned int startTime = millis();
bool startAP = true;
//Client ID
String clientId = "c8879e6e-db31-44e4-905e-ee87f238076a";
//User ID
String userId = "c8879e6e-db31-44e4-905e-ee87f238076a";
//ID Device
String idDevice = "bcab6c4a-dfce-4801-b591-57aeeb6fedc4";

char node_ID[] = "RGB Smart LED";
void setup() {
  // put your setup code here, to run once:
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);

  analogWrite(redLED, 0);
  analogWrite(greenLED, 0);
  analogWrite(blueLED, 0);
  Serial.begin(115200);
  wifiManager.setConfigPortalTimeout(300);
  if (wifiManager.autoConnect(node_ID)) {
    String wifi = WiFi.SSID();
    Serial.println(WiFi.localIP());
  }
  if (WiFi.status() != WL_CONNECTED) {
  } else {
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    //geturl();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  doWiFiManager();
  WiFiClient client;
  HTTPClient http;
  String load = "{}";
  http.begin(client, linkdiawan);
  Serial.println(linkdiawan);
  String jsonStr = "";
  http.addHeader("Content-Type", "application/json");
  String isi = "{\"clientId\":\"" + clientId + "\",\"userId\":\"" + userId + "\", \"idDevice\":\"" + idDevice + "\"}";
  Serial.println(isi);
  httpResponseCode = http.POST(isi);
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode != 200) {
    delay(500);
  } else if (httpResponseCode == 500) {
  }

  else if (httpResponseCode == -1 || httpResponseCode == -11) {
  }

  else {
    load = http.getString();
    Serial.print(load);
    DynamicJsonDocument doc(1024);
    String input = load;
    deserializeJson(doc, input);
    JsonObject obj = doc.as<JsonObject>();
    //onState = obj["result"]["on"].as<int>();
    offState = obj["result"]["off"].as<int>();
    red = obj["result"]["color"]["red"].as<int>();
    green = obj["result"]["color"]["green"].as<int>();
    blue = obj["result"]["color"]["blue"].as<int>();
    onState = obj["result"]["color"]["value"].as<int>();
    Serial.println(onState);
    Serial.println(offState);
    if (onState == 1 && lastOnState == 0 && offState == 0) {
      analogWrite(redLED, red);
      analogWrite(greenLED, green);
      analogWrite(blueLED, blue);
    }
    if (offState == 1 && lastOffState == 0 && onState == 0) {
      analogWrite(redLED, 0);
      analogWrite(greenLED, 0);
      analogWrite(blueLED, 0);
    }
    Serial.print("Red: ");
    Serial.println(red);
    Serial.print("Green: ");
    Serial.println(green);
    Serial.print("Blue: ");
    Serial.println(blue);
    Serial.println();

    lastOnState = onState;
    lastOffState = offState;
    http.end();
  }
  delay(200);
}

void geturl() {
  WiFiClient client;
  HTTPClient http;
  String serverPath = serverName + "/" + idDevice;
  http.begin(client, serverPath.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
    DynamicJsonDocument doc(1024);
    String input = payload;
    deserializeJson(doc, input);
    JsonObject obj = doc.as<JsonObject>();
    linkdiawan = obj["url"]["push"].as<String>();
    Serial.print("urlGet ");
    Serial.println(linkdiawan);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}
void doWiFiManager() {
  if (portalRunning) {
    wifiManager.process();  // do processing
  }
  if (!portalRunning) {
    if (startAP) {
      Serial.println("Button Pressed, Starting Config Portal");
      wifiManager.setConfigPortalBlocking(false);
      wifiManager.startConfigPortal(node_ID);
    } else {
      Serial.println("Button Pressed, Starting Web Portal");
      wifiManager.startWebPortal();
    }
    portalRunning = true;
    startTime = millis();
  }
}
void fade(int pin) {

  for (int u = 0; u < 1024; u++) {
    analogWrite(pin, u);
    delay(1);
  }
  for (int u = 0; u < 1024; u++) {
    analogWrite(pin, 1023 - u);
    delay(1);
  }
}