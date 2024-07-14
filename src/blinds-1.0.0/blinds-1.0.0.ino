// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Load http library
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

//include properties
#include "properties.h"
#include "constants.h"

// ** FUNCTION DECLARATIONS **

//connects device to network - returns IP Address of device

/**
 * connects device to network - returns IP Address of device
 *
 * @returns Ip Address of device
 * 
 * @param {{ssid}} Network Name
 * @param {{password}} Network Password
 */
void connectToWifi(String ssid, String password);

// Send HTTP request to server to register devcie
bool registerDevice();

//open and close blinds
void openBlinds();
void closeBlinds();

// ** GLOBALS *

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

//enum for state of blinds
enum State {B_OPEN, B_CLOSED};

State currState;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);

  Serial.println("Nick's Smart Home Automation\n");

  //connect to network 
  connectToWifi(SSID, PASSWORD);

  Serial.println("Connected to wifi...");
  //register device IP with server
  if(!registerDevice()) {
    Serial.println("ERROR: Device was unable to register itself, check if the web server is operational or configured properly");
  }
  Serial.print("Device registered");
}

void loop(){

  //initialize server - device will run as a server for remainder of operation
  WiFiClient client = server.available();   // Listen for incoming clients

  currState = B_CLOSED;

  Serial.println("Starting WiFi Server");
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:application/json");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /request-device") >= 0){
              Serial.println("Device Information requested, returning value to sender");

              String msg = "{\"name\": \"" + DEVICE_NAME + "\", \"type\": \"" + DEVICE_TYPE + "\", \"address\": \"" + ADDRESS + "\"}";
              client.println(msg);

            } else
            if(header.indexOf("POST /close") >= 0) {
              if(currState == B_CLOSED){
                Serial.println("Blinds are already closed, returning this message to the sender");
                client.println("{\"state:\": \"closed\"}");
              } else {
                closeBlinds();
              }
            } else 
            if(header.indexOf("POST /open") >=0) {
              if(currState == B_OPEN){
                Serial.println("Blinds are already open, returning this message to the sender");
                client.println("{\"state:\": \"open\"}");
              } else {
                openBlinds();
              }

            }
            else {
              Serial.println("Someone connected with a browser");
              client.println("{\"oniline\": \"true\"}");
            }
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}


void connectToWifi(String ssid, String password){
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); //TODO: add some LED stuff here
    Serial.print(".");
  }

  //add IP to properties string
  ADDRESS = WiFi.localIP().toString();

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(ADDRESS);
  server.begin();
}

//On startup, send device info to sderver to be stored in database
bool registerDevice(){
  WiFiClient client;
  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
  // configure traged server and url
  http.begin(client, REGISTER_DEVICE_URL);  // HTTP - only works if you put the whole string in like this ...
  http.addHeader("Content-Type", "application/json");

  Serial.print("[HTTP] POST...\n");
  // start connection and send HTTP header and body
  //int httpCode = http.POST("{\"hello\":\"world\"}");
  String msg = "{\"name\": \"" + DEVICE_NAME + "\", \"type\": \"" + DEVICE_TYPE + "\", \"address\": \"" + ADDRESS + "\"}";
  int httpCode = http.POST(msg);

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      const String& payload = http.getString();
      Serial.println("received payload:\n<<");
      Serial.println(payload);
      Serial.println(">>");
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  return true;
}

void openBlinds(){
  Serial.println("Blinds Opening");
}
void closeBlinds(){
  Serial.println("Blinds Closing");
}