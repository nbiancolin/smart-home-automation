// Load Wi-Fi library
#include <ESP8266WiFi.h>
// Load http library
#include <ESP8266HTTPClient.h>

//include properties
#include "properties.h"

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
enum State {OPEN, CLOSED};

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

  //register device IP with server
  registerDevice(ipAddress);
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
            }
            if(header.indexOf("POST /close") >= 0) {
              if(currState == CLOSED){
                Serial.println("Blinds are already closed, returning this message to the sender");
                client.println("{\"state:\": \"closed\"}");
              } else {
                closeBlinds();
              }
            }
            if(header.indexOf("POST /open") >=0) {
              if(currState == OPEN){
                Serial.println("Blinds are already open, returning this message to the sender");
                client.println("{\"state:\": \"open\"}");
              } else {
                openBlinds();
              }

            }
            

            /* DEPRECATED - Old code from tester
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop

            */
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
  ipAddress = WiFi.localIP().toString();
  properties[3] = "\"address\": \"" + ipAddress + "\"" ;

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

  String registerDeviceUrl = serverIp + "/device-auto-register";
  http.begin(client, registerDeviceUrl.c_str());

  //set appropriate web headers
  http.addHeader("Content-Type", "application/json"); //it is easy to work with json in Camel

  //build request bpdy
  String httpRequestData = "";
  for(int i = 0; i < 5; ++i){
    httpRequestData += properties[i] + "\n";
  }

  //send request
  int httpResponseCode = http.POST(httpRequestData);

  if(httpResponseCode == 200){
    Serial.println("Recieved code 200 OK. Proceeding with operation");
    return true; // request was successful
  } else {
    Serial.println("ERROR: Recived code:" + httpResponseCode);
    return false; //request failed
  }
}