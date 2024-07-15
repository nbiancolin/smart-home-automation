#ifndef CONSTANTS_H
#define CONSTANTS_H

#define REGISTER_DEVICE_URL "http://192.168.1.120:8080/device-autoregister" 

// CHIP-SPECIFIC GPIO Pins
// NOTE: Don't use D0 or D3 if it is avoidable
#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15


const String SSID = "JUPITER";
const String PASSWORD = "Potatoes";

const String SERVER_IP = "http://192.128.1.120:8080";
//const String SERVER_IP = "http://192.168.1.158";

//enum for state of blinds
enum State {B_OPEN, B_CLOSED};

//enum for different errors
enum Error {E_WIFI_CONNECT, 
            E_SERVER_CONNECT};

#endif // CONSTANTS_H