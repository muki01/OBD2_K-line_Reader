const char* AP_ssid = "OBD2 Master";
const char* AP_password = "12345678";

const char* STA_ssid = "AnOnYmOuS";
const char* STA_password = "muki2004";

void initWiFi() {
  // WiFi.mode(WIFI_STA);
  // WiFi.setTxPower(WIFI_POWER_8_5dBm);
  // WiFi.begin(STA_ssid, STA_password);
  // while (WiFi.status() != WL_CONNECTED) {
  // }
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_ssid, AP_password);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
}

void initWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", webpageCont);
  });
  server.onNotFound([](AsyncWebServerRequest* request) {
    request->send(404, "text/plain", "404 Not found");
  });
  server.begin();
}

void ws() {
  webSocket.loop();
  jsonDoc["Speed"] = SPEED;
  jsonDoc["RPM"] = RPM;
  jsonDoc["CoolantTemp"] = COOLANT_TEMP;
  jsonDoc["IntakeTemp"] = INTAKE_TEMP;
  jsonDoc["Throttle"] = THROTTLE;
  jsonDoc["KLineStatus"] = KLineStatus;
  serializeJson(jsonDoc, JSONtxt);
  webSocket.broadcastTXT(JSONtxt);
}
