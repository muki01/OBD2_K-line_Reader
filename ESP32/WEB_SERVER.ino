const char *AP_ssid = "OBD2 Master";
const char *AP_password = "12345678";

// IPAddress AP_ip(192, 168, 0, 1);
// IPAddress AP_subnet(255, 255, 255, 0);
// IPAddress AP_gateway(192, 168, 0, 1);

IPAddress STA_ip;
IPAddress STA_gateway;
IPAddress STA_subnet;

void initWiFi() {
  if (IP_address != "") {
    STA_ip.fromString(IP_address);
    STA_gateway.fromString(Gateway);
    STA_subnet.fromString(SubnetMask);
    if (WiFi.config(STA_ip, STA_gateway, STA_subnet))
      ;
  }
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_5dBm);
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
  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/");
  });
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
    page = 0;
  });
  server.on("/liveData.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/liveData.html", "text/html");
    page = 1;
  });
  server.on("/errorCodes.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/errorCodes.html", "text/html");
    page = 2;
  });
  server.on("/frozenData.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/frozenData.html", "text/html");
    page = 3;
  });
  server.on("/speedTest.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/speedTest.html", "text/html");
    page = 4;
  });
  server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/settings.html", "text/html");
    page = 5;
  });
  server.on("/about.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/about.html", "text/html");
    page = 6;
  });
  server.on("/css/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/style.css", "text/css");
  });
  server.on("/css/liveData.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/liveData.css", "text/css");
  });
  server.on("/css/errorCodes.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/errorCodes.css", "text/css");
  });
  server.on("/css/frozenData.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/frozenData.css", "text/css");
  });
  server.on("/css/speedTest.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/speedTest.css", "text/css");
  });
  server.on("/css/settings.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/settings.css", "text/css");
  });
  server.on("/css/about.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/about.css", "text/css");
  });
  server.on("/js/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/script.js", "text/javascript");
  });
  server.on("/js/liveData.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/liveData.js", "text/javascript");
  });
  server.on("/js/errorCodes.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/errorCodes.js", "text/javascript");
  });
  server.on("/js/frozenData.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/frozenData.js", "text/javascript");
  });
  server.on("/js/speedTest.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/speedTest.js", "text/javascript");
  });
  server.on("/js/settings.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/settings.js", "text/javascript");
  });
  server.on("/js/about.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/about.js", "text/javascript");
  });
  server.on("/js/webSocket.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/webSocket.js", "text/javascript");
  });
  server.on("/fonts/Montserrat-Bold.woff2", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/fonts/Montserrat-Bold.woff2", "application/font-woff2");
  });
  server.on("/api", HTTP_GET, [](AsyncWebServerRequest *request) {
    page = -1;
    request->send(200, "application/json", JsonData());
  });
  server.on("/api/clearDTCs", HTTP_GET, [](AsyncWebServerRequest *request) {
    clear_DTC();
    request->send(200, "text/plain", "Succesfully");
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
    String protocol = request->arg("protocol");
    changeCommunicationProtocol(protocol);
  });
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send_P(404, "text/plain", "404 Not found");
  });
  server.begin();
}

void initWebSocket() {
  server.addHandler(&ws);
  ws.onEvent(onEvent);
}

String JsonData() {
  String JSONtxt;
  jsonDoc.clear();
  if (page == -1) {  //Add All Data
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
  } else if (page == 0) {
    jsonDoc["KLineStatus"] = KLineStatus;
  } else if (page == 1) {
    jsonDoc["Speed"] = SPEED;
    jsonDoc["RPM"] = RPM;
    jsonDoc["CoolantTemp"] = COOLANT_TEMP;
    jsonDoc["IntakeTemp"] = INTAKE_TEMP;
    jsonDoc["Throttle"] = THROTTLE;
    jsonDoc["TimingAdvance"] = TIMINGADVANCE;
    jsonDoc["EngineLoad"] = ENGINELOAD;
    jsonDoc["MAF"] = MAF;
    jsonDoc["KLineStatus"] = KLineStatus;
  } else if (page == 2) {
    jsonDoc["KLineStatus"] = KLineStatus;
    JsonArray dtcArray = jsonDoc.createNestedArray("DTCs");
    for (int i = 0; i < 20; i++) {
      if (!dtcBuffer[i].isEmpty()) {
        dtcArray.add(dtcBuffer[i]);
      }
    }
  } else if (page == 3) {
    jsonDoc["KLineStatus"] = KLineStatus;
    JsonArray dtcArray = jsonDoc.createNestedArray("DTCs");
    for (int i = 0; i < 20; i++) {
      if (!dtcBuffer[i].isEmpty()) {
        dtcArray.add(dtcBuffer[i]);
      }
    }
  } else if (page == 4) {
    jsonDoc["KLineStatus"] = KLineStatus;
    jsonDoc["Speed"] = SPEED;
  } else if (page == 5) {

  } else if (page == 6) {
  }

  serializeJson(jsonDoc, JSONtxt);
  return JSONtxt;
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0;
      const char *message = (const char *)data;
      
      if (strcmp(message, "get_dtc") == 0) {
        mode1();
        get_DTCs();
      }
      if (strcmp(message, "clear_dtc") == 0) {
        mode2();
        clear_DTC();
      }
      if (strcmp(message, "beep") == 0) {
        mode2();
      }
    }
  }
}

void wsSend() {
  ws.cleanupClients();
  String jsonDataString = JsonData();
  ws.textAll(jsonDataString);
}
