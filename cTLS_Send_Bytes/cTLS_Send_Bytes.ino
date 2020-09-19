/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "192.168.179.36";
const uint16_t port = 1234;

void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  static bool wait = false;
  unsigned long StartTime, EndTime, Runtime;
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  // This will send a string to the server
  Serial.println("sending data to server");
  if (client.connected()) {
    String test;
    for(int i=0;i<50;i++){
      test+="x";
    }
    StartTime = micros();
    client.write(test.c_str(), 50);
    client.flush();
  }
  // wait for data to be available
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(60000);
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  Serial.println("receiving from remote server");
  // not testing 'client.connected()' since we do not need to send data here
  String response="";
  uint8_t receivearray[1024]={0};
    client.read(receivearray, 152);

    Serial.printf("Bytes received: %d\n", String((const char*)receivearray).length());
  if (client.connected()) {
    String finished;
    for(int i=0;i<100;i++){
      finished+="x";
    }
  client.write(finished.c_str(), 100);
  client.flush();
  EndTime = micros();
  Serial.println("Client Finished");
  }
  // Close the connection
  Serial.println();
  Serial.println("closing connection");
  client.stop();
  Runtime = EndTime - StartTime;
  Serial.println(Runtime);
  if (wait) {
    delay(10000); // execute once every 5 minutes, don't flood remote service
  }
  wait = true;
}
