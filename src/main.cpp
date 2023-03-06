#include <Arduino.h>
#include <WiFiMulti.h>
#include <WebSocketsCLient.h>
#include <ArduinoJson.h>

#define WIFI_SSID "esptestssd"
#define WIFI_KEY "pass123"
#define WS_HOST ""
#define WS_PORT 443
#define WS_URL ""

#define JSON_DOC_SIZE 2048
#define MSG_SIZE 128


WiFiMulti wifiMulti;
WebSocketsClient wsClient;

void sendErrorMessage(const char *error) {
  char msg[MSG_SIZE];

  sprintf(msg, "{\"action\":\"msg\",\"msg\":{\"status\":\"error\",\"error\":\"%s\"}}", error);
  wsClient.sendTXT(msg);
}

unit8_t toMode(const char *val) {
   if (strcmp(value, "output") == 0) {
    return OUTPUT;
   }

   if (strcmp(value, "input_pullup") == 0) {
    return INPUT_PULLUP;
   }
    return INPUT;
   
}

void handleMessage(uint8_t * payload) {
  StaticJsonDocument<JSON_DOC_SIZE> doc;

  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    sendErrorMessage(error.c_str());
    return;
  }

  if (!doc["type"].is<const char *>( {
    sendErrorMessage("invalid message type");
    return;
  }

  if (strcmp(doc["type"], "cmd") == 0) {
    if (!doc["body"].is<JsonObject>()) {
      sendErrorMessage("Invalid command body");
      return;
    }
    if (strcmp(doc["body"]["type"], "pinMode") == 0) {
      pinMode(doc["body"]["pin"], toMode((doc["body"]["mode"]));
      return;
    }
    if (strcmp(doc["body"]["type"], "digitalWrite") == 0) {
      digitalWrite(doc["body"]["pin"], doc["body"]["value"]);
    }
    if (strcmp(doc["body"]["type"], "digitalRead") == 0) {
      auto value = digitalRead(doc["body"]["pin"]);
      char msg[MSG_SIZE];
      sprintf(msg, "{\"action\":\"msg\",\"msg\":{\"status\":\"error\",\"error\":\"%s\"}}", error);
      wsClient.sendTXT(msg);
      return;
    }
  }

}

void onWSEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.println("WS Connected");
      break;
    case WStype_DISCONNECTED:
      Serial.println("WS Disconnected");
      break;    
    case WStype_TEXT:
      Serial.printf("WS Message: %s\n, payload");
      handleMessage(payload);
  }
}

void setup() {
  Serial.begin(921600);
  pinMode(LED_BUILTIN, OUTPUT);
  wifiMulti.addAP(WIFI_SSID, WIFI_KEY);

  while (wifiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  Serial.println("starting process...");

  wsClient.beginSSL(WS_HOST, WS_PORT, WS_URL, "", "wss");
  wsClient.onEvent(onWSEvent);
}

void loop() {
    digitalWrite(LED_BUILTIN, WiFi.status() == WL_CONNECTED);
    wsClient.loop();
  }