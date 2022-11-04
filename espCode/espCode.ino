#include <Arduino.h>
#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClientSecureBearSSL.h>
// Fingerprint for demo URL, expires on June 2, 2021, needs to be updated well before this date
//const uint8_t fingerprint[20] = { 0x40, 0xaf, 0x00, 0x6b, 0xec, 0x90, 0x22, 0x41, 0x8e, 0xa3, 0xad, 0xfa, 0x1a, 0xe8, 0x25, 0x41, 0x1d, 0x1a, 0x54, 0xb3 };

ESP8266WiFiMulti WiFiMulti;

// GLOBAL VAR
String plant_id = "0";
int temp_value = 0;
int moisture_value = 0;
String plant_type = "none";
String user_id = "auth0|630523e8ea2e70160cd8bd3b";

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
//    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  // WiFiMulti.addAP("Verizon-SM-G970U-909c", "frankieo");
  WiFiMulti.addAP("Verizon-SM-G970U-909c", "frankieo");
}

void getPlantInfo() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

//    client->setFingerprint(fingerprint);
    // Or, if you happy to ignore the SSL certificate, then use the following line instead:
     client->setInsecure();

    HTTPClient https;

//    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://leafit.vercel.app/api/plants")) {  // HTTPS
          https.addHeader("Content-Type", "application/json");

//      Serial.print("[HTTPS] GET...\n");
      String reqData = "{\"user_id\":\"" + user_id +"\"}";           
      // start connection and send HTTP header
      int httpCode = https.POST(reqData);
//      Serial.println(reqData);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
//        Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
         Serial.println(payload);
         // String input;
        DynamicJsonDocument doc(6144);
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }

        for (JsonObject item : doc.as<JsonArray>()) {

          const char* id = item["_id"]; // "63646160c210ad0008060d4a", "6364616dc210ad0008060d4b", ...
          const char* user_id = item["user_id"]; // "auth0|630523e8ea2e70160cd8bd3b", ...
          const char* plantNickname = item["plantNickname"]; // "Test", "Hello", "Test", "Test", "Test", "Test", ...
          const char* plantType = item["plantType"]; // "Temperate", "Arid", "Temperate", "Temperate", ...
          bool activated = item["activated"]; // true, false, true, true, true, true, true, true, true, true, ...

          if(activated){
            plant_type = plantType;
            plant_id = id;
            Serial.println(plant_type);
            Serial.println(plant_id);
          }
        }

        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

//  Serial.println("Wait 3s before next round...");
  delay(3000);
}


void postRequest() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

//    client->setFingerprint(fingerprint);
    // Or, if you happy to ignore the SSL certificate, then use the following line instead:
     client->setInsecure();

    HTTPClient https;

//    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://leafit.vercel.app/api/sendSensorData")) {  // HTTPS
          https.addHeader("Content-Type", "application/json");

//      Serial.print("[HTTPS] GET...\n");
      String reqData = "{\"plant_id\":\"" + plant_id + "\",\"temp_value\":\"" + temp_value + "\",\"moisture_value\":\"" + moisture_value + "\"}";           
      // start connection and send HTTP header
      int httpCode = https.POST(reqData);
//      Serial.println(reqData);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
//        Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
//          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

//  Serial.println("Wait 3s before next round...");
  delay(3000);
}

void loop(){
  getPlantInfo();
  sendPlantTypeToArduino(plant_type);
  Serial.flush();
  // lets get the plant id (Hard coded rn but we need to fetch that also)
  // plant_id = "63115046a16d5cd929a233cc";
  // lets set sensor data.
  // this is setting moisture and temp values.
  setSensorData();
  // send the post request.
  postRequest();
  Serial.flush();
  
}

void sendPlantTypeToArduino(String plant_type){
    Serial.flush();
    Serial.println(plant_type);
    delay(1000);
    Serial.flush();
}

void setSensorData() {
  if (Serial.available() > 0) {
    String combinedString = Serial.readString();
//    Serial.println(combinedString);
    // lets seperate the string
    int indexOfSlash = combinedString.indexOf('/');
    moisture_value = combinedString.substring(0, indexOfSlash).toInt();
    temp_value = combinedString.substring(indexOfSlash + 1).toDouble();
  }
}
