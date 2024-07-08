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
String connectToWifi(String ssid, String password);

// Send HTTP request to server to register devcie
bool registerDevice(String ipAddress);

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
  String ipAddress = connectToWifi(SSID, PASSWORD);

  Serial.print("Connected to wifi...");
  //register device IP with server
  registerDevice(ipAddress);
  Serial.print("Device registered");
}

void loop(){

  //initialize server
  WiFiClient client = server.available();   // Listen for incoming clients

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
              for(int i = 0; i < 5; ++i){ //return properties sent
                client.println(properties[i]);
              }
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


String connectToWifi(String ssid, String password){
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); //TODO: add some LED stuff here
    Serial.print(".");
  }

  //add IP to properties string
  String ipAddress = WiFi.localIP().toString();
  properties[2] = "    \"address\": \"" + ipAddress + "\"" ;
  //a bug appears when modifying an array of strings, in that thjis value gets overwritten. Adding this to fix it
  properties[3] = "} ";

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(ipAddress);
  server.begin();
  return ipAddress;
}

bool registerDevice(String ipAddress){
  //on setup, send a post request to the server to register device
  HTTPClient http;
  WiFiClient client;

  String registerDeviceUrl = "http://192.168.1.158:8080/device-auto-register";
  http.begin(client, registerDeviceUrl.c_str());
  Serial.println("MADE IT IN FN");
  //set appropriate web headers
  http.addHeader("Content-Type", "application/json"); //it is easy to work with json in Camel

  //build request bpdy
  String httpRequestData = "";
  for(int i = 0; i < 5; ++i){
    httpRequestData += properties[i] + "\n";
  }
  Serial.println("BEFORE POST");

  //send request
  int httpResponseCode = http.POST(httpRequestData);
  Serial.println("After post");
  Serial.println(httpResponseCode);
  if(httpResponseCode == 200){
    Serial.println("Recieved code 200 OK. Proceeding with operation");
    return true; // request was successful
  } else {
    Serial.println("ERROR: Recived code:" + httpResponseCode);
    return false; //request failed
  }
}

void openBlinds(){
  Serial.println("Blinds Opening");
}
void closeBlinds(){
  Serial.println("Blinds Closing");
}