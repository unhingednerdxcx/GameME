/* WHAT TO DO:
  IF YOU DON'T FOLLOW CHANGE LINE: 31-39 AKA {
    int led_pin = 13;        // Connection status LED
    int button_a = 12;       // Button A
    int button_b = 14;       // Button B
    int button_c = 15;       // Button C
    int button_d = 18;       // Button D
    int button_e = 19;       // Button E (optional)
    int button_f = 21;       // Button F (optional)
    int xpin = 34;           // Joystick X axis
    int ypin = 35;           // Joystick Y axis
  }
  CHANGE LINE: 42-44 AKA {
    const char* ssid = "SSID";         // put your WiFi SSID
    const char* pswd = "PSWD";            // put your WiFi password
    const char* host = "IPv4"; // put your computers's IP(v4) address
  }
  
    
  
*/

// === INCLUDES: ===//

#include <WiFi.h>
#include <Arduino.h>

//=== VARIABLES: ===//

//=== PINS: ===//
int led_pin = 13;        // Connection status LED
int button_a = 12;       // Button A
int button_b = 14;       // Button B
int button_c = 15;       // Button C
int button_d = 18;       // Button D
int button_e = 19;       // Button E (optional)
int button_f = 21;       // Button F (optional)
int xpin = 34;           // Joystick X axis
int ypin = 35;           // Joystick Y axis

//=== WIFI: ===//
const char* ssid = "SSID";         // Your WiFi SSID
const char* pswd = "PSWD";            // Your WiFi password
const char* host = "IPv4"; // Your computers's IP(v4) address
const uint16_t port = 3658;       // Server port

WiFiClient client; // create client for connecting to the server

//=== MESSAGES/STATE: ===//
String msg;                       // Message to send
int a, b, c, d, e, f;            // Button states
int x, y;                        // Joystick positions
const String endMarker = "/END/"; // Message end marker

// Connection management
unsigned long lastConnectionAttempt = 0; // Last connection attempt time
const unsigned long connectionInterval = 5000; // 5 seconds between attempts
unsigned long lastHeartbeat = 0; // Last heartbeat time
const unsigned long heartbeatInterval = 3000; // 3 seconds between heartbeats


// === FUNCTIONS: ===//

void connectToWiFi() { // function to connect to WiFi
  if (WiFi.status() == WL_CONNECTED) return; // If already connected, exit
  
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, pswd); // Connect to WiFi
  
  unsigned long startTime = millis(); // Start time for connection attempt
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 20000) { // 20 seconds timeout
    delay(500);
    Serial.print(".");
    digitalWrite(led_pin, !digitalRead(led_pin)); // Blink LED while connecting {okay but was this line smart?}
  }
  
  if (WiFi.status() == WL_CONNECTED) { // If connected successfully
    Serial.println("\nWiFi connected!");
    digitalWrite(led_pin, HIGH); // Turn on LED to indicate connection
    printNetworkInfo(); // Print network info
  } else { // If connection failed
    Serial.println("\nWiFi connection failed");
    digitalWrite(led_pin, LOW); // Turn off LED to indicate failure
  }
}

void maintainConnection() { // Function to maintain connection to the server
  // First ensure WiFi is connected
  if (WiFi.status() != WL_CONNECTED) { // If WiFi is not connected
    digitalWrite(led_pin, LOW); // Turn off LED
    connectToWiFi(); // Attempt to reconnect to WiFi
    return;
  }
  
  // Then handle server connection
  if (!client.connected()) { // If not connected to server
    digitalWrite(led_pin, LOW); // Turn off LED
    attemptServerConnection(); // Attempt to connect to the server
  }
}

void attemptServerConnection() {
  if (millis() - lastConnectionAttempt < connectionInterval) return; // Avoid too frequent connection attempts
  lastConnectionAttempt = millis(); // Update last connection attempt time
  
  Serial.print("Connecting to server...");
  if (client.connect(host, port)) { // Attempt to connect to the server
    Serial.println("Connected!"); 
    digitalWrite(led_pin, HIGH); // Turn on LED to indicate connection
    client.setNoDelay(true); // Disable Nagle's algorithm for faster response
    lastHeartbeat = millis(); // Reset heartbeat timer
  } else { // If connection failed
    Serial.println("Failed");
    if (millis() - lastConnectionAttempt > 30000) { // If connection attempt exceeds 30 seconds
      Serial.println("Resetting ESP due to connection timeout");
      ESP.restart(); // Restart ESP32 if connection fails for too long
    }
  }
}

void checkForHeartbeat() { // Function to check for heartbeat messages from the server
  if (millis() - lastHeartbeat >= heartbeatInterval) { // If no heartbeat received for a while
    if (client.available()) { // If client is available, read heartbeat
      String response = client.readStringUntil('\n');
      if (response == "HB") {
        client.println("OK");
        lastHeartbeat = millis();
      }
    } else {
      // No heartbeat received, assume connection is dead
      client.stop();
    }
  }
}

void collectInputs() { // Function to collect inputs from buttons and joystick
  // Read button states and joystick positions
  a = digitalRead(button_a);
  b = digitalRead(button_b);
  c = digitalRead(button_c);
  d = digitalRead(button_d);
  e = digitalRead(button_e);
  f = digitalRead(button_f);
  x = analogRead(xpin);
  y = analogRead(ypin);

  // Prepare the message to send
  // Format: a : b : c : d : e : f : x : y : endMarker
  // Example: "1 : 0 : 1 : 0 : 0 : 345 : 678 : /END/"

  msg = String(a) + " : " + String(b) + " : " + String(c) + " : " + 
        String(d) + " : " + String(e) + " : " + String(f) + " : " + 
        String(x) + " : " + String(y) + " : " + endMarker;
}

void sendControllerData() { // Function to send controller data to the server
  collectInputs(); // Collect inputs from buttons and joystick
  
  if (!client.connected()) return; // If not connected, exit

  client.println(msg); // Send the message to the server
  Serial.println("Sent: " + msg); 
}

void receiveServerAck() { // Function to receive acknowledgment from the server
  if (!client.connected()) return; // If not connected, exit
  
  unsigned long start = millis();  // Start time for waiting for ACK
  while (client.connected() && millis() - start < 200) { // 200ms timeout
    if (client.available()) { // If data is available from the server
      String response = client.readStringUntil('\n'); // Read the response
      response.trim(); // Remove any trailing whitespace
      if (response == "M") { // If the response is "M", it means the server acknowledged the message
        Serial.println("Received ACK");
        return; // Exit the function after receiving ACK 
      }
    }
    delay(5); // Small delay to avoid busy-waiting
  }
  Serial.println("No ACK received");
}

void printNetworkInfo() { // Function to print network information
  Serial.println("Network Information:");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}

// == ON STARTUP: ===//
void setup() {
  Serial.begin(115200); // Start serial communication
  while(!Serial); // Wait for serial to be ready SOME BOARDS MIGHT WAIT FOREVER | BUT SOME BOARDS MIGHT NEED THIS LINE TO WORK PROPERLY, ESP32 needs this line to work properly
  // Initialize pins
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  pinMode(button_a, INPUT);
  pinMode(button_b, INPUT);
  pinMode(button_c, INPUT);
  pinMode(button_d, INPUT);
  pinMode(button_e, INPUT);
  pinMode(button_f, INPUT);
  
  // Connect to WiFi
  connectToWiFi();
}

// === MAIN LOOP: ===//

void loop() { // Main loop
  maintainConnection(); // Ensure WiFi and server connection are maintained
  
  if (client.connected()) { // If connected to the server
    checkForHeartbeat(); // Check for heartbeat messages from the server
    sendControllerData(); // Send controller data to the server
    receiveServerAck(); // Receive acknowledgment from the server
  }
  
  delay(20); // ~50Hz loop rate
}


/*
TIPS for Arduino IDE
  CTRL + A to select all
  CTRL + X to cut
  CTRL + C to copy
  CTRL + V to paste
  CTRL + F for find press the sidewise V that looks like '>' to replace
  CTRL + R to compile
  CTRL + Z to undo
  CTRL + U to verify
  CTRL + S to save 
  CTRL + I for suggestion

DETAILS
  Sketch uses 926559 bytes (70%) of program storage space. Maximum is 1310720 bytes.
  Global variables use 44804 bytes (13%) of dynamic memory, leaving 282876 bytes for local variables. Maximum is 327680 bytes.
  Tested using FreeRTOS
  My OS is Manjaro Arch linux
  Using version 2.3.4
  Using baud 115200
  Made for ESP32 dev kit or ESP32-WROOM or similar boards
  Run the .sh file to start the python server
  RECOMENDED TO run the python server before you start the ESP32 code
  The MainGame folder has the game whilst the files outside the folder are the controller code
*/
