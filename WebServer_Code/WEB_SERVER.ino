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
#ifdef ESP32
  WiFi.setTxPower(WIFI_POWER_5dBm);
#endif
  WiFi.begin(STA_ssid.c_str(), STA_password.c_str());
  unsigned long previousMillis = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - previousMillis <= 3000) {
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    BlinkLed(100, 5);
  } else {
    // if (WiFi.softAPConfig(AP_ip, AP_gateway, AP_subnet));
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_ssid, AP_password);
    BlinkLed(300, 1);
  }
}

void initWebServer() {
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
  server.on("/freezeFrame.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/freezeFrame.html", "text/html");
    page = 3;
  });
  server.on("/speedTest.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/speedTest.html", "text/html");
    page = 4;
  });
  server.on("/vehicleInfo.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/vehicleInfo.html", "text/html");
    page = 5;
  });
  server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/settings.html", "text/html");
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
  server.on("/css/freezeFrame.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/freezeFrame.css", "text/css");
  });
  server.on("/css/speedTest.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/speedTest.css", "text/css");
  });
  server.on("/css/vehicleInfo.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/vehicleInfo.css", "text/css");
  });
  server.on("/css/settings.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/settings.css", "text/css");
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
  server.on("/js/freezeFrame.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/freezeFrame.js", "text/javascript");
  });
  server.on("/js/speedTest.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/speedTest.js", "text/javascript");
  });
  server.on("/js/vehicleInfo.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/vehicleInfo.js", "text/javascript");
  });
  server.on("/js/settings.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/settings.js", "text/javascript");
  });
  server.on("/js/webSocket.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/webSocket.js", "text/javascript");
  });
  server.on("/fonts/Montserrat-Bold.woff2", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/fonts/Montserrat-Bold.woff2", "application/font-woff2");
  });
  server.on("/api/getData", HTTP_GET, [](AsyncWebServerRequest *request) {
    page = -1;
    Melody2();
    getPID(VEHICLE_SPEED);
    getPID(ENGINE_RPM);
    getPID(ENGINE_COOLANT_TEMP);
    getPID(INTAKE_AIR_TEMP);
    getPID(THROTTLE_POSITION);
    getPID(TIMING_ADVANCE);
    getPID(ENGINE_LOAD);
    getPID(MAF_FLOW_RATE);
    get_DTCs();
    request->send(200, "application/json", JsonData());
  });
  server.on("/api/clearDTCs", HTTP_GET, [](AsyncWebServerRequest *request) {
    Melody2();
    clear_DTC();
    request->send(200, "text/plain", "Succesfully");
  });
  server.on("/wifiOptions", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Succesfully");
    String ssid = request->arg("SSID");
    String password = request->arg("WifiPassword");
    String ipAddress = request->arg("ipAddr");
    String subnetMask = request->arg("subnetMask");
    String gateway = request->arg("gateway");
    changeWifiSettings(ssid, password, ipAddress, subnetMask, gateway);
  });
  server.on("/protocolOptions", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Succesfully");
    String protocol = request->arg("protocol");
    changeCommunicationProtocol(protocol);
  });
  server.on(
    "/firmwareUpdate", HTTP_POST, [](AsyncWebServerRequest *request) {
      request->send(400, "text/plain", "No files uploaded.");
    },
    [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
      if (!index) {
        size_t freeSpace = ESP.getFlashChipSize() - ESP.getSketchSize();
        if (request->contentLength() > freeSpace) {
          request->send(413, "text/plain", "File too large for flash memory.");
          return;
        }

        if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
          request->send(500, "text/plain", "Firmware update failed to begin.");
          return;
        }
      }

      if (Update.write(data, len) != len) {
        request->send(500, "text/plain", "Firmware update failed during writing.");
        return;
      }

      if (final) {
        if (Update.end(true)) {
          request->send(200, "text/plain", "Firmware updated successfully. Restarting...");
          connectMelody();
          delay(1000);
          ESP.restart();
        } else {
          request->send(500, "text/plain", "Firmware update failed to end.");
        }
      }
    });

  server.on(
    "/fileSystemUpdate", HTTP_POST, [](AsyncWebServerRequest *request) {
      request->send(400, "text/plain", "No files uploaded.");
    },
    [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
      if (!index) {
        size_t freeSpace = ESP.getFlashChipSize() - ESP.getSketchSize() - SPIFFS.usedBytes();
        if (request->contentLength() > freeSpace) {
          request->send(413, "text/plain", "File too large for SPIFFS.");
          return;
        }

        if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS)) {
          request->send(500, "text/plain", "SPIFFS update failed to begin.");
          return;
        }
      }

      if (Update.write(data, len) != len) {
        request->send(500, "text/plain", "SPIFFS update failed during writing.");
        return;
      }

      if (final) {
        if (Update.end(true)) {
          request->send(200, "text/plain", "SPIFFS updated successfully. Restarting...");
          connectMelody();
        } else {
          request->send(500, "text/plain", "SPIFFS update failed to end.");
        }
      }
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

  if (page == -1) {
    JsonObject liveData = jsonDoc.createNestedObject("LiveData");
    for (const auto &mapping : liveDataMappings) {
      if (isInArray(desiredLiveData, sizeof(desiredLiveData), mapping.pid)) {
        JsonObject pidObject = liveData.createNestedObject(mapping.jsonKey);
        pidObject["value"] = mapping.value;
        pidObject["unit"] = mapping.unit;
      }
    }
    jsonDoc["DTCs"] = joinStringsWithComma(dtcBuffer, sizeof(dtcBuffer));
  } else if (page == 1) {
    JsonObject liveData = jsonDoc.createNestedObject("LiveData");
    for (const auto &mapping : liveDataMappings) {
      if (isInArray(desiredLiveData, sizeof(desiredLiveData), mapping.pid)) {
        JsonObject pidObject = liveData.createNestedObject(mapping.jsonKey);
        pidObject["value"] = mapping.value;
        pidObject["unit"] = mapping.unit;
      }
    }
  } else if (page == 2) {
    jsonDoc["DTCs"] = joinStringsWithComma(dtcBuffer, sizeof(dtcBuffer));
    jsonDoc[liveDataMappings[32].jsonKey] = liveDataMappings[32].value;  //Distance Traveled With MIL On
  } else if (page == 3) {
    jsonDoc["DTCs"] = joinStringsWithComma(dtcBuffer, sizeof(dtcBuffer));
    JsonObject freezeFrame = jsonDoc.createNestedObject("FreezeFrame");
    for (const auto &mapping : freezeFrameMappings) {
      if (isInArray(supportedFreezeFrame, sizeof(supportedFreezeFrame), mapping.pid)) {
        JsonObject pidObject = freezeFrame.createNestedObject(mapping.jsonKey);
        pidObject["value"] = mapping.value;
        pidObject["unit"] = mapping.unit;
      }
    }
  } else if (page == 4) {
    jsonDoc[liveDataMappings[11].jsonKey] = liveDataMappings[11].value;  //Speed
  } else if (page == 5) {
    jsonDoc["VIN"] = Vehicle_VIN;
    jsonDoc["ID"] = Vehicle_ID;
    jsonDoc["IDNum"] = Vehicle_ID_Num;
    jsonDoc["supportedLiveData"] = convertBytesToHexStringWithComma(supportedLiveData, sizeof(supportedLiveData));
    jsonDoc["supportedFreezeFrame"] = convertBytesToHexStringWithComma(supportedFreezeFrame, sizeof(supportedFreezeFrame));
    jsonDoc["supportedVehicleInfo"] = convertBytesToHexStringWithComma(supportedVehicleInfo, sizeof(supportedVehicleInfo));
  } else if (page == 6) {
    JsonObject liveData = jsonDoc.createNestedObject("SupportedLiveData");
    for (const auto &mapping : liveDataMappings) {
      if (isInArray(supportedLiveData, sizeof(supportedLiveData), mapping.pid)) {
        JsonObject pidObject = liveData.createNestedObject(mapping.jsonKey);
      }
    }
  }

  jsonDoc["selectedProtocol"] = selectedProtocol;
  jsonDoc["connectedProtocol"] = connectedProtocol;
  jsonDoc["Voltage"] = VOLTAGE;
  jsonDoc["KLineStatus"] = KLineStatus;
  serializeJson(jsonDoc, JSONtxt);
  return JSONtxt;
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0;
      const char *message = (const char *)data;

      if (strcmp(message, "clear_dtc") == 0) {
        Melody2();
        clear_DTC();
      }
      if (strcmp(message, "beep") == 0) {
        Melody2();
      }
      if (strcmp(message, "page0") == 0) {
        page = 0;
      }
      if (strcmp(message, "page1") == 0) {
        page = 1;
      }
      if (strcmp(message, "page2") == 0) {
        page = 2;
      }
      if (strcmp(message, "page3") == 0) {
        page = 3;
      }
      if (strcmp(message, "page4") == 0) {
        page = 4;
      }
      if (strcmp(message, "page5") == 0) {
        page = 5;
      }
      if (strcmp(message, "page6") == 0) {
        page = 6;
      }
    }
  }
}

void sendDataToServer() {
  ws.cleanupClients();
  String jsonDataString = JsonData();
  ws.textAll(jsonDataString);
}
