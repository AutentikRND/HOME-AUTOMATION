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
#define led 2
#define r1 4
DNSServer dns;
WiFiManager wifiManager;
String serverName = "http://diawan.io/api/get_url";
String formattedDate;
String linkdiawan = "http://asia-southeast2-diawanpremium-6d4a9.cloudfunctions.net/spechToTextDevice";
String name;
int lastOffState, lastOnState, onState, offState;
int httpResponseCode;
bool portalRunning = false;
unsigned int startTime = millis();
bool startAP = true;
//Client ID
String clientId = "c8879e6e-db31-44e4-905e-ee87f238076a";
//User ID
String userId = "c8879e6e-db31-44e4-905e-ee87f238076a";
//ID Device
String idDevice = "e46d2806-4c07-4542-9cbd-25e232f23e00";
char node_ID[] = "Talpha-Smart Plug";
void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
  pinMode(r1, OUTPUT);
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
    onState = obj["result"]["on"].as<int>();
    offState = obj["result"]["off"].as<int>();
    Serial.println(onState);
    Serial.println(offState);
    if (onState == 1 && lastOnState == 0 && offState == 0) {
      digitalWrite(led, LOW);
      digitalWrite(r1, HIGH);
    }
    if (offState == 1 && lastOffState == 0 && onState == 0) {
      digitalWrite(led, HIGH);
      digitalWrite(r1, LOW);
    }
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