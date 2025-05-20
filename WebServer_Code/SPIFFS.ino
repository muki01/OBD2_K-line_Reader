void initSpiffs() {
  if (!SPIFFS.begin()) {
    while (1) {
      BlinkLed(1000, 100);
    }
  }
  debugPrintln("SPIFFS Init Successfully");
}

void readSettings() {
  if (SPIFFS.exists("/settings.conf")) {
    File configFile = SPIFFS.open("/settings.conf", "r");
    while (configFile.available()) {
      String line = configFile.readStringUntil('\n');
      if (line.startsWith("ssid=")) STA_ssid = line.substring(5);
      else if (line.startsWith("password=")) STA_password = line.substring(9);
      else if (line.startsWith("ipAddress=")) IP_address = line.substring(10);
      else if (line.startsWith("subnetMask=")) SubnetMask = line.substring(11);
      else if (line.startsWith("gateway=")) Gateway = line.substring(8);
      else if (line.startsWith("protocol=")) protocol = line.substring(9);
    }
    configFile.close();
  }
}

void updateSetting(const String& key, const String& value) {
  if (SPIFFS.exists("/settings.conf")) {
    String updatedSettings = "";
    File configFile = SPIFFS.open("/settings.conf", "r");
    while (configFile.available()) {
      String line = configFile.readStringUntil('\n');
      if (line.startsWith(key)) {
        updatedSettings += key + "=" + value + "\n";
      } else {
        updatedSettings += line + "\n";
      }
    }
    configFile.close();
    configFile = SPIFFS.open("/settings.conf", "w");
    configFile.print(updatedSettings);
    configFile.close();
  }
}

void changeWifiSettings(String ssid, String password, String ipAddress, String subnetMask, String gateway) {
  updateSetting("ssid", ssid);
  updateSetting("password", password);
  updateSetting("ipAddress", ipAddress);
  updateSetting("subnetMask", subnetMask);
  updateSetting("gateway", gateway);
  ESP.restart();
}

void changeCommunicationProtocol(String communicationProtocol) {
  updateSetting("protocol", communicationProtocol);
  protocol = communicationProtocol;
  if (conectionStatus) {
    conectionStatus = false;
  }
}