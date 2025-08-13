/* LINES YOU NEED TO EDIT {
  L:33 TO L:42 aka {
    int led_pin = x_val; // this led pin is used to tell you if your device is connected to server or not
    int button_a = x_val; // this var is for pin to  button a
    int button_b = x_val; // this var is for pin to button b
    int button_c = x_val; // this var is for pin to button c
    int button_d = x_val; // this var is for pin to button d
    int button_e = x_val; // this var is for pin to button e YOU CAN IGNORE THIS IF YOU DONT HAVE A BUTTON E
    int button_f = x_val; // this var is for pin to button f YOU CAN IGNORE THIS IF YOU DONT HAVE A BUTTON F
    int xpin = x_val; // this var is for pin to joysticks x axis
    int ypin = x_val; // this var is for pin to joysticks y axis
  }
  L:46 TO L:48 aka {
    const char* ssid = "SSID"; // CHANGE THIS VAR TO YOUR WIFI SSID
    const char* pswd = "PSWD"; // CHANGE THIS TO YOUR WIFI PASSWORD
    const char* host = "XXX.XXX.XX.XX"; // CHANGE THIS TO YOUR LAPTOPS IP
  }
  L:138 aka {while(!Serial); // ON SOME BOARDS YOU NEED TO ADD THIS ON OTHER, THIS BLOCK FOREVER, BUT, this is ok on ESP32 if you use USB serial.}
}

comments are included to guide you on those and every other line! and check Lines L:173 to L:178 */

//=== LIBRARIES: ===//

#include <WiFi.h>
#include <Arduino.h>
#include <stdio.h>

//=== VARIABLES: ===//

//=== PINS: ===//

int x_val = 0; // THIS IS JUST A PLACE HOLDER DELETE THIS VARIABLE
int led_pin = x_val; // this led pin is used to tell you if your device is connected to server or not
int button_a = x_val; // this var is for pin to  button a
int button_b = x_val; // this var is for pin to button b
int button_c = x_val; // this var is for pin to button c
int button_d = x_val; // this var is for pin to button d
int button_e = x_val; // this var is for pin to button e YOU CAN IGNORE THIS IF YOU DONT HAVE A BUTTON E
int button_f = x_val; // this var is for pin to button f YOU CAN IGNORE THIS IF YOU DONT HAVE A BUTTON F
int xpin = x_val; // this var is for pin to joysticks x axis
int ypin = x_val; // this var is for pin to joysticks y axis

//=== WIFI: ===//

const char* ssid = "SSID"; // CHANGE THIS VAR TO YOUR WIFI SSID
const char* pswd = "PSWD"; // CHANGE THIS TO YOUR WIFI PASSWORD
const char* host = "XXX.XXX.XX.XX"; // CHANGE THIS TO YOUR LAPTOPS IP
const uint16_t port = 3658; // the port that python looks for you

WiFiClient client; // this object is for TCP control

//=== MESSAGES/STATE: ===//

String msg; // this var is to store the message we will send to the server
int a; // this var is for storing the state of button a (HIGH or LOW/ 1 or 0)
int b; // this var is for storing the state of button b (HIGH or LOW/ 1 or 0)
int c; // this var is for storing the state of button c (HIGH or LOW/ 1 or 0)
int d; // this var is for storing the state of button d (HIGH or LOW/ 1 or 0)
int e; // this var is for storing the state of button e (HIGH or LOW/ 1 or 0)
int f; // this var is for storing the state of button f (HIGH or LOW/ 1 or 0)
int x; // this var is for storing the state of button x (0 to 4095)
int y; // this var is for storing the state of button y (0 to 4095)
String end = "/END/"; // this var is to recognize the end of the message we will send to server

//=== FREERTOS/SYNCRONIZATION: ===//

bool core0 = false; // this var is to tell if core 0 has done its task or not
bool core1 = false; // this var is to tell if core 1 has done its task ot not


//=== FUNCTION FOR CONNECTING TO HOST: ===//

void connectToServer() { // to make sure we are always connected to the server, cause this is a controller 
  Serial.print("\nConnecting to server..."); // \n means next line
  // this part runs UNTIL host is conneced
  while (!client.connected()) { // if it is not connected
    if (client.connect(host, port)) { // conncting to the server was succesfull
      Serial.println("\nConnected!");
      digitalWrite(led_pin, HIGH); // turns on light
    } else { // conncting to the server was unsuccesfull
      Serial.print(".");
      delay(500);
    }
  }
}

//=== FUNCTION FOR COLLECTING INPUTS FROM CONTROLLER: ===//

void cont_inp(void *pvParameters){
  while(1) {
    //=== Collecting data: ===//
    core0 = false; // core 0 now has not finished its task
    a = digitalRead(button_a); // collects state of button a
    b = digitalRead(button_b); // collects state of button b
    c = digitalRead(button_c); // collects state of button c
    d = digitalRead(button_d); // collects state of button d
    e = digitalRead(button_e); // collects state of button e
    f = digitalRead(button_f); // collects state of button f
    x = analogRead(xpin); // collects value of the joysticks x axis
    y = analogRead(ypin); // collects value of the joysticks y axis

    msg = String(a) + " : " + String(b) + " : " + String(c) + " : " + String(d) + " : " + String(e) + " : " + String(f) + " : " + String(x) + " : " + String(y) + " : " + end; // forms the message we will send to the server in a variable
    core0 = true; // core 0 has done has task
    while(core1 == false) { // wait until core1 finishes its task
      vTaskDelay(1); // tell the core pinned to task to wait for 1 ticks (~1 ms at default config for ESP32)
    } 
  }
}

//=== FUNCTION FOR SENDING THE INPUTS TO SERVER: ===//

void transfer(void *pvParameters) {

  while (1) {
    core1 = false; // core 1 has not finished its task
    while (core0 == false) {// wait until core 0 finished its task
      vTaskDelay(1); // tell the core pinned to task to wait for 1 ticks (~1 ms at default config for ESP32)
    } 
    if (!client.connected()) { // if connection to server had been inturupted
      digitalWrite(led_pin, LOW); // turn the led off
      Serial.println("\nDisconnected, reconnecting...");
      client.stop(); // stop the connection
      connectToServer(); // reconnect
    }
    client.println(msg); // send the message
    core1 = true; // core finished its task
    vTaskDelay(5); // tell the core pinned to task to wait for 5 ticks (~5 ms at default config for ESP32)
  }
}

//=== FUNCTION WHEN THE ESP FIRST LOADS: ===//

void setup() {

  //=== Connecting to stuff: ===//
  Serial.begin(115200); // starts serial
  while(!Serial); // ON SOME BOARDS YOU NEED TO ADD THIS ON OTHER, THIS BLOCK FOREVER, BUT, this is ok on ESP32 if you use USB serial.

  WiFi.begin(ssid, pswd); // connect to the wifi
  while (!(WiFi.status() == WL_CONNECTED)) { // if not connected to wifi wait until connected
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nConnected!"); 

  //=== setting pin modes: ===//

  pinMode(button_a, INPUT); // set button a to input
  pinMode(button_b, INPUT); // set button b to input
  pinMode(button_c, INPUT); // set button c to input
  pinMode(button_d, INPUT); // set button d to input
  pinMode(button_e, INPUT); // set button e to input
  pinMode(button_f, INPUT); // set button f to input
  pinMode(xpin, INPUT); // set x axis to input
  pinMode(ypin, INPUT); // set y axis to input
  pinMode(led_pin, OUTPUT); // set led light to output

 
  //=== free rtos task pinning: ===//
  
  connectToServer();
  xTaskCreatePinnedToCore(cont_inp, "InputTask", 4096, NULL, 1, NULL, 0); // tell core 0 to do cont_inp task
  xTaskCreatePinnedToCore(transfer, "TransferTask", 4096, NULL, 1, NULL, 1); // tell core 1 to do transfer task

}


void loop() {
  // nothing needs to be looped since we are using FreeRTOS
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
  Sketch uses 923731 bytes (70%) of program storage space. Maximum is 1310720 bytes.
  Global variables use 44788 bytes (13%) of dynamic memory, leaving 282892 bytes for local variables. Maximum is 327680 bytes.
  Tested using FreeRTOS
  My OS is Manjaro Arch linux
  Using version 2.3.4
  Using baud 115200
  Made for ESP32 dev kit or ESP32-WROOM or similar boards
  Run the .sh file to start the python server
  RECOMENDED TO run the python server before you start the ESP32 code
  The MainGame folder has the game whilst the files outside the folder are the controller code
*/
