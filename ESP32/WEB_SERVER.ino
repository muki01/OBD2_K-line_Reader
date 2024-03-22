const char* AP_ssid = "OBD2 Master";
const char* AP_password = "12345678";

const char* STA_ssid = "AnOnYmOuS";
const char* STA_password = "muki2004";

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(STA_ssid, STA_password);
  unsigned long previousMillis = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - previousMillis <= 3000) {
  }

  if (WiFi.status() == WL_CONNECTED) {
    BlinkLed(100, 5);
  } else {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_ssid, AP_password);
    WiFi.setTxPower(WIFI_POWER_5dBm);
    BlinkLed(400, 1);
  }
}

void BlinkLed(int time, int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(Led, LOW);
    delay(time);
    digitalWrite(Led, HIGH);
    delay(time);
  }
}

void initWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/liveData.html", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/liveData.html", "text/html");
  });
  server.on("/css/style.css", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/css/style.css", "text/css");
  });
  server.on("/css/liveData.css", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/css/liveData.css", "text/css");
  });
  server.on("/js/script.js", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/js/script.js", "text/javascript");
  });
  server.on("/js/liveData.js", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/js/liveData.js", "text/javascript");
  });
  server.on("/js/webSocket.js", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/js/webSocket.js", "text/javascript");
  });
  server.on("/img/1.png", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/img/1.png", "image/png");
  });
  server.on("/img/2.png", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/img/2.png", "image/png");
  });
  server.on("/img/3.png", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/img/3.png", "image/png");
  });
  server.on("/img/4.png", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/img/4.png", "image/png");
  });
  server.on("/img/5.png", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/img/5.png", "image/png");
  });
  server.on("/img/car.png", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/img/car.png", "image/png");
  });
  server.on("/fonts/Montserrat-Bold.woff2", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/fonts/Montserrat-Bold.woff2", "application/font-woff2");
  });
  server.on("/api", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "application/json", JsonData());
  });
  server.onNotFound([](AsyncWebServerRequest* request) {
    request->send_P(404, "text/plain", "404 Not found");
  });
  server.begin();
}

String JsonData(){
  String JSONtxt;
  jsonDoc["Speed"] = SPEED;
  jsonDoc["RPM"] = RPM;
  jsonDoc["CoolantTemp"] = COOLANT_TEMP;
  jsonDoc["IntakeTemp"] = INTAKE_TEMP;
  jsonDoc["Throttle"] = THROTTLE;
  jsonDoc["TimingAdvance"] = TIMINGADVANCE;
  jsonDoc["EngineLoad"] = ENGINELOAD;
  jsonDoc["MAF"] = MAF;
  jsonDoc["KLineStatus"] = KLineStatus;
  serializeJson(jsonDoc, JSONtxt);
  return JSONtxt;
}

void ws() {
  webSocket.loop();
  String jsonDataString = JsonData();
  webSocket.broadcastTXT(jsonDataString);
}
