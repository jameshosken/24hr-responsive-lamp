

void setupWiFi() {

  pixSetHSB(200, 100, 100);


  Serial.print("WiFi101 shield: ");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("NOT PRESENT");
    pixSetHue(0);  //Set Red
    pixDisplay(); //Display
    return; // don't continue
  }
  Serial.println("DETECTED");

  int attempts = 0;

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    attempts += 1;
    if (attempts == 3) {
      isConnected = false;
      break;
    }

    pixSetHue(35);
    pixDisplay();
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    isConnected = true;
    // wait 10 seconds for connection:
    delay(3000);
  }

  if (isConnected) {

    //setPhilipsHSB(0, 0, 0);

    setupRTC();
    pixSetHue(120);

    for (int i = 0; i < 5; i++) {

      pixSetBri(100);
      pixDisplay();
      delay(250);
      pixSetBri(0);
      pixDisplay();
      delay(250);
    }

    server.begin();                           // start the web server on port 80
    printWiFiStatus();                        // you're connected now, so print out the status
  } else {
    Serial.println("Problem connecting: Check WiFi credentials");
    isConnected = true;
    pixSetHue(0);
    pixDisplay();
  }
}

void setupRTC() {
  rtc.begin();
  unsigned long epoch;
  int numberOfTries = 0, maxTries = 8;

  pixSetHSB(330, 100, 100);
  pixDisplay();

  do {
    Serial.println("Trying to get epoch");
    epoch = WiFi.getTime();
    numberOfTries++;
    delay(250);
  }
  while ((epoch == 0) && (numberOfTries < maxTries));
  if (epoch == 0) {
    Serial.println("Failed. Trying again to get epoch");
    numberOfTries = 0;
    do {
      Serial.println("Trying to get epoch");
      epoch = WiFi.getTime();
      numberOfTries++;
      delay(250);
    }
    while ((epoch == 0) && (numberOfTries < maxTries));
  }

  if (numberOfTries >= maxTries) {
    Serial.print("NTP unreachable!!");
    while (epoch == 0) {
      epoch = WiFi.getTime();
      numberOfTries++;
      Serial.println("Trying to get epoch");
      delay(1000);

    }
  }


  Serial.print("Epoch received: ");
  Serial.println(epoch);

  rtc.setEpoch(epoch);

  Serial.println();

}



void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void handleServer() {
  // listen for incoming clients

  WiFiClient client = server.available();
  if (client) {
    
    long timeout = 5000;
    long timeoutCheckpoint = millis();
    Serial.println("new client");
    // an http request ends with a blank line

    bool storeData = false;
    char data = ' ';
    boolean currentLineIsBlank = true;
    while (client.connected()) {

      /////////////
      // TIMEOUT //
      /////////////
      
      if(millis() - timeoutCheckpoint > timeout){
        Serial.print("\n\n\nTIMEOUT\n\n\n");
        client.stop();
        return;
      }

      /////////////////
      // READ CLIENT //
      /////////////////
      
      if (client.available()) {
        char c = client.read();
        if (storeData) {
          data = c;
          storeData = false;
        }

        if (debug_verbose) {
          Serial.write(c);
        }
        
        /////////////
        // MESSAGE //
        /////////////
        
        //Since I have no idea what I'm doing, this is the hackey way I've figure to get info from route.
        if (c == '?') {
          storeData = true;   //Flag next char as read char
        }

        if (c == '\n' && currentLineIsBlank) {

          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 600");  // refresh the page automatically every 60 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");

          //Response HTML:
          client.println("Success!");
          client.println("</html>");
          break;
        }

        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }

        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }

    }
    // give the web browser time to receive the data
    delay(100);

    // close the connection:
    client.stop();
    Serial.println("client disconnected");

    if (data != userControlData) {
      prevUserControlData = userControlData;
      userControlData = data;
      Serial.println(">>> New Data! <<<");
      Serial.println(userControlData);
    }
  }
}




void sendRequest(int light, String cmd, String value) {

  // make a String for the HTTP request path:
  String request = "/api/" + hueUserName;
  request += "/lights/";
  request += light;
  request += "/state/";

  String contentType = "application/json";

  // make a string for the JSON command:
  String hueCmd = "{\"" + cmd;
  hueCmd += "\":";
  hueCmd += value;
  hueCmd += "}";
  // see what you assembled to send:
  Serial.print("PUT request to server: ");
  Serial.println(request);
  //Serial.print("JSON command to server: ");

  // make the PUT request to the hub:
  httpClient.put(request, contentType, hueCmd);

  // read the status code and body of the response
  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();

  //Serial.println(hueCmd);
  Serial.print("Status code from server: ");
  Serial.println(statusCode);
  Serial.print("Server response: ");
  Serial.println(response);
  //Serial.println();

}




