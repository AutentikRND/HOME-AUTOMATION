//library..................................................................................
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Samsung.h>
String serverName = "http://diawan.io/api/get_url";
String formattedDate;
String linkdiawan = "http://asia-southeast2-diawanpremium-6d4a9.cloudfunctions.net/spechToTextDevice";
String name;
int httpResponseCode;
//Client ID
String clientId = "c8879e6e-db31-44e4-905e-ee87f238076a";
//User ID
String userId = "c8879e6e-db31-44e4-905e-ee87f238076a";
//ID Device
String idDevice = "1bd196fb-963c-4b12-9299-f049369ae2c3";
String humidifier = "4b76b78d-30d9-4a46-8e63-1a8341b2348d";
String ac = "93cd88bc-4a07-11ed-b878-0242ac120002";
String listDevice = "[\"" + ac + "\",\"" + humidifier + "\"]";
WiFiManager wifiManager;
char node_ID[] = "IR BLAST";
float datsuhu, datrh, maksimalsuhu, minimalsuhu, maksimalrh, minimalrh;
int a = 1;
int b = 1;
bool portalRunning = false;
unsigned int startTime = millis();
bool startAP = true;
const int ledin = 2;
const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRSamsungAc ac(kIrLed);  // Set the GPIO to be used to sending the message.

// Example of data captured by IRrecvDumpV2.ino
uint16_t OFFac[349] = { 706, 17746, 3206, 8770, 596, 398, 620, 1370, 506, 490, 680, 314, 506, 490, 648, 348, 506, 490, 504, 492, 620, 400, 650, 1320, 556, 460, 478, 518, 598, 1394, 556, 1434, 502, 492, 562, 1430, 506, 1486, 506, 1486, 504, 1486, 590, 1402, 592, 406, 586, 410, 648, 346, 500, 498, 626, 386, 594, 402, 676, 318, 500, 496, 558, 438, 670, 326, 592, 402, 504, 490, 560, 436, 592, 402, 506, 490, 506, 490, 598, 398, 560, 436, 674, 322, 614, 380, 500, 496, 556, 440, 584, 428, 480, 516, 620, 376, 586, 408, 618, 378, 504, 492, 680, 314, 560, 434, 504, 490, 506, 490, 562, 434, 652, 342, 506, 1486, 560, 1434, 616, 2878, 3126, 8830, 556, 1456, 598, 396, 620, 376, 586, 408, 650, 346, 588, 406, 650, 346, 616, 378, 506, 490, 562, 1428, 622, 374, 506, 490, 506, 1486, 504, 494, 558, 1434, 646, 1368, 622, 1368, 622, 1370, 502, 1488, 648, 1344, 592, 402, 560, 436, 506, 490, 622, 374, 618, 378, 504, 490, 646, 350, 502, 494, 556, 464, 476, 496, 498, 516, 478, 518, 622, 372, 650, 346, 556, 440, 558, 438, 620, 376, 620, 374, 592, 404, 506, 490, 506, 490, 504, 490, 506, 488, 506, 492, 636, 358, 504, 496, 500, 494, 500, 496, 500, 516, 478, 516, 568, 426, 644, 350, 556, 440, 704, 290, 502, 492, 560, 436, 620, 2876, 3038, 8912, 506, 1486, 506, 490, 560, 434, 504, 492, 588, 406, 502, 494, 652, 344, 636, 360, 616, 400, 566, 1424, 644, 352, 586, 408, 502, 1490, 562, 1428, 506, 1486, 562, 1428, 506, 490, 624, 1368, 504, 1490, 500, 1492, 584, 1428, 536, 1456, 472, 1518, 500, 1490, 620, 1372, 594, 400, 596, 400, 562, 434, 650, 1340, 564, 1428, 504, 1490, 638, 382, 534, 440, 556, 460, 534, 460, 622, 374, 556, 438, 620, 376, 588, 408, 648, 1342, 614, 1376, 562, 434, 504, 1486, 650, 1342, 504, 492, 562, 436, 502, 518, 566, 406, 614, 402, 624, 370, 624, 372, 646, 348, 584, 412, 616, 378, 648, 1342, 506, 1486, 682 };
uint16_t ONac[349] = { 592, 17850, 3152, 8822, 564, 432, 712, 1278, 620, 374, 596, 398, 684, 312, 594, 400, 506, 490, 506, 490, 504, 490, 504, 1490, 502, 496, 498, 516, 624, 1366, 622, 372, 560, 434, 560, 1432, 506, 1484, 506, 1484, 506, 1484, 506, 1484, 748, 250, 620, 374, 502, 496, 500, 496, 588, 426, 478, 516, 480, 516, 652, 342, 590, 406, 502, 492, 620, 376, 706, 286, 508, 488, 652, 344, 506, 490, 506, 488, 506, 488, 564, 432, 650, 346, 624, 374, 678, 318, 504, 494, 500, 496, 498, 516, 566, 428, 718, 278, 650, 346, 622, 372, 624, 372, 506, 486, 596, 400, 594, 402, 592, 1398, 506, 1484, 594, 1398, 562, 1430, 584, 2912, 3092, 8864, 500, 1510, 592, 402, 648, 348, 558, 436, 590, 406, 646, 350, 594, 400, 624, 370, 594, 400, 656, 1334, 562, 434, 562, 432, 622, 1370, 622, 374, 560, 1434, 646, 1366, 680, 1310, 618, 1372, 504, 1488, 650, 1340, 564, 432, 564, 430, 594, 402, 562, 432, 680, 314, 508, 490, 564, 432, 504, 490, 562, 436, 590, 406, 618, 398, 600, 396, 644, 350, 644, 350, 560, 436, 650, 344, 594, 402, 594, 400, 508, 488, 564, 430, 680, 314, 596, 400, 508, 488, 652, 344, 506, 490, 562, 436, 730, 264, 622, 374, 558, 460, 654, 340, 560, 436, 620, 376, 558, 436, 590, 404, 652, 344, 590, 404, 564, 2934, 3150, 8798, 564, 1428, 562, 432, 564, 432, 712, 284, 642, 354, 618, 378, 504, 492, 618, 398, 576, 420, 506, 1484, 648, 348, 560, 436, 592, 1398, 564, 432, 564, 1426, 622, 1368, 594, 400, 592, 1400, 678, 1314, 562, 1432, 676, 1336, 480, 1510, 558, 1430, 640, 1352, 678, 1312, 682, 314, 704, 290, 508, 488, 654, 1338, 618, 1372, 506, 1486, 652, 344, 558, 438, 502, 514, 536, 458, 512, 484, 618, 376, 646, 348, 648, 346, 650, 1340, 506, 1484, 710, 284, 596, 1396, 626, 1364, 506, 490, 506, 490, 592, 404, 560, 436, 502, 498, 554, 460, 622, 374, 590, 406, 558, 1430, 594, 1398, 684, 1306, 706, 1284, 622 };
uint16_t OFFhum[71] = { 9080, 4528, 580, 586, 580, 584, 724, 440, 578, 586, 578, 586, 666, 498, 666, 498, 578, 1672, 578, 1670, 578, 1672, 578, 1672, 578, 1670, 666, 1584, 578, 1670, 578, 1672, 578, 586, 692, 472, 578, 1672, 578, 586, 730, 434, 578, 1670, 578, 586, 578, 586, 578, 586, 582, 1666, 754, 410, 578, 1672, 666, 1582, 578, 586, 578, 1670, 578, 1672, 578, 1670, 578, 39694, 9082, 2254, 578 };
uint16_t ONhum[71] = { 9086, 4524, 636, 528, 636, 528, 638, 526, 636, 528, 580, 584, 580, 582, 666, 498, 636, 1612, 580, 1668, 638, 1610, 692, 1556, 580, 1668, 580, 1668, 608, 1640, 580, 1668, 606, 558, 664, 502, 580, 1666, 580, 584, 580, 584, 634, 1614, 588, 574, 666, 498, 580, 584, 662, 1584, 608, 554, 666, 1582, 582, 1668, 634, 528, 580, 1668, 608, 1640, 580, 1668, 580, 39670, 9140, 2194, 662 };
// Example Samsung A/C state captured from IRrecvDumpV2.ino
uint8_t samsungState[kSamsungAcStateLength] = {
  0x02, 0x92, 0x0F, 0x00, 0x00, 0x00, 0xF0,
  0x01, 0xE2, 0xFE, 0x71, 0x40, 0x11, 0xF0
};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  wifiManager.setConfigPortalTimeout(300);
  //wifiManager.setSTAStaticIPConfig(IPAddress(192, 168, 1, 200), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0), IPAddress(8, 8, 8, 8));
  wifiManager.autoConnect(node_ID);
  ac.begin();
  pinMode(ledin, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
#if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
#else   // ESP8266
  Serial.begin(115200, SERIAL_8N1);
#endif  // ESP8266
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  // irsend.sendRaw(ONhum, 71, 38);  // Send a raw data capture at 38kHz.
  // delay(500);
  // irsend.sendRaw(OFFhum, 71, 38);  // Send a raw data capture at 38kHz.
  // delay(500);
  // irsend.sendRaw(ONac, 349, 38);  // Send a raw data capture at 38kHz.
  // delay(500);
  // irsend.sendRaw(OFFac, 349, 38);  // Send a raw data capture at 38kHz.
  // delay(500);
  ac.off();
  ac.setFan(kSamsungAcFanLow);
  ac.setMode(kSamsungAcCool);
  ac.setTemp(25);
  ac.setSwing(false);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledin, HIGH);
    delay(200);
  } else {
    stream();
    digitalWrite(ledin, LOW);
    delay(200);
    digitalWrite(ledin, HIGH);
    doWiFiManager();
  }
  delay(200);
}

void stream() {
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
    if (obj["result"]["on"].as<int>() == 1) {
      Serial.println("AC nyala");
      
      //irsend.sendRaw(ONac, 349, 38);  // Send a raw data capture at 38kHz.
    } else if (obj["result"]["off"].as<int>() == 1) {
      Serial.println("AC mati");
      i//rsend.sendRaw(OFFac, 349, 38);  // Send a raw data capture at 38kHz.
    }
    if (obj["result"][humidifier]["on"].as<int>() == 1) {
      Serial.println("HUM nyala");
      irsend.sendRaw(ONhum, 71, 38);  // Send a raw data capture at 38kHz.
    } else if (obj["result"][humidifier]["off"].as<int>() == 1) {
      Serial.println("HUM mati");
      irsend.sendRaw(OFFhum, 71, 38);  // Send a raw data capture at 38kHz.
    }
    http.end();
  }
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