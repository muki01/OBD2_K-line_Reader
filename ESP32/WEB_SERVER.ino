const char* AP_ssid = "OBD2 Master";
const char* AP_password = "12345678";

IPAddress AP_ip(192, 168, 0, 1);
IPAddress AP_subnet(255, 255, 255, 0);
IPAddress AP_gateway(192, 168, 0, 1);

IPAddress STA_ip;
IPAddress STA_gateway;
IPAddress STA_subnet;

// const char* STA_ssid = "AnOnYmOuS";
// const char* STA_password = "muki2004";

void initWiFi() {
  if (IP_address != "") {
    STA_ip.fromString(IP_address);
    STA_gateway.fromString(Gateway);
    STA_subnet.fromString(SubnetMask);
    if (WiFi.config(STA_ip, STA_gateway, STA_subnet));
  }
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_2dBm);
  WiFi.begin(STA_ssid.c_str(), STA_password.c_str());
  unsigned long previousMillis = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - previousMillis <= 3000) {
  }

  if (WiFi.status() == WL_CONNECTED) {
    BlinkLed(100, 5);
  } else {
    // if (WiFi.softAPConfig(AP_ip, AP_gateway, AP_subnet));
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_ssid, AP_password);
    dnsServer.start(53, "*", WiFi.softAPIP());
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
  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request){
    request->redirect("/");
  });
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/liveData.html", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/liveData.html", "text/html");
  });
  server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/settings.html", "text/html");
  });
  server.on("/errorCodes.html", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/errorCodes.html", "text/html");
  });
  server.on("/css/style.css", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/css/style.css", "text/css");
  });
  server.on("/css/liveData.css", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/css/liveData.css", "text/css");
  });
  server.on("/css/settings.css", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/css/settings.css", "text/css");
  });
  server.on("/css/errorCodes.css", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/css/errorCodes.css", "text/css");
  });
  server.on("/js/script.js", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/js/script.js", "text/javascript");
  });
  server.on("/js/liveData.js", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/js/liveData.js", "text/javascript");
  });
  server.on("/js/settings.js", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/js/settings.js", "text/javascript");
  });
  server.on("/js/errorCodes.js", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/js/errorCodes.js", "text/javascript");
  });
  server.on("/js/webSocket.js", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/js/webSocket.js", "text/javascript");
  });
  server.on("/fonts/Montserrat-Bold.woff2", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/fonts/Montserrat-Bold.woff2", "application/font-woff2");
  });
  server.on("/api", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "application/json", JsonData());
  });
  server.on("/wifiOptions", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Options succesfully saved. Restarting ESP!");
    String ssid = request->arg("SSID");
    String password = request->arg("WifiPassword");
    String ipAddress = request->arg("ipAddr");
    String subnetMask = request->arg("subnetMask");
    String gateway = request->arg("gateway");
    changeWifiSettings(ssid, password, ipAddress, subnetMask, gateway);
  });
  server.on("/protocolOptions", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Options succesfully saved. Restarting ESP!");
    String protocol = request->arg("cars");
    changeCommunicationProtocol(protocol);
  });
  server.onNotFound([](AsyncWebServerRequest* request) {
    request->send_P(404, "text/plain", "404 Not found");
    request->redirect("/");
  });
  server.begin();
}

String JsonData() {
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

  JsonArray dtcArray = jsonDoc.createNestedArray("DTCs");
  for (int i = 0; i < 20; i++) {
    if (!dtcBuffer[i].isEmpty()) {
      dtcArray.add(dtcBuffer[i]);
    }
  }

  serializeJson(jsonDoc, JSONtxt);
  return JSONtxt;
}

void ws() {
  webSocket.loop();
  String jsonDataString = JsonData();
  webSocket.broadcastTXT(jsonDataString);
}
