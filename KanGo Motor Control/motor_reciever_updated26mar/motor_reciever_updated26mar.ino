#include <esp_now.h>
#include <WiFi.h>

#include <ESP32Servo.h>

Servo myservo1;  // Create a servo object
Servo myservo2;  // Create a servo object




// Task 3r.1 Modify pin definitions
#define LED_PIN 2  // Replace with the LED pin number

// Task 3r.2 Toggle LED
// Your code here for variables
// Declare a boolean variable to store the state of the LED
bool LEDstate;
int dataX;
int dataY;

// END YOUR CODE HERE


// Structure example to receive data
// Must match the sender structure
typedef struct message {
  bool joystickMode;
  bool previousSwitchState;

  bool directionPressed = false;
  bool buttLeft = false;
  bool buttRight = false;
  bool buttUp = false;
  bool buttDown = false;

  int mappedX;
  int mappedY;

} message;

// Create a struct_message called myData
message myMessage;



void printHex(const uint8_t *data, int len) {
  for (int i = 0; i < len; i++) {
    if (data[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  printHex(mac, 6);
  printHex(incomingData, len);

  struct message *received_message = (struct message *)incomingData;

  // Task 3r.3 Modify LED state upon receiving a message
  LEDstate = received_message->joystickMode;  //Replace ?? with your boolean declared in task 3r.2. For context, -> operator is a dereference from a pointer. You don't need to know how to use it, just use as is
  dataX = received_message->mappedX;
  dataY = received_message->mappedY;


  myservo1.write(dataX);
  myservo2.write(dataY);
  Serial.print("LEDstate");
  Serial.println(LEDstate);
  Serial.print("dataX");
  Serial.println(dataX);
  Serial.print("dataY");
  Serial.println(dataY);

  int wheelL = dataY;
  int wheelR = dataY;

  wheelL += (dataX - 90);
  wheelR -= (dataX - 90);

  wheelL = constrain(wheelL, 0, 180);
  wheelR = constrain(wheelR, 0, 180);

  Serial.print("wheelL");
  Serial.println(wheelL);
  Serial.print("wheelR");
  Serial.println(wheelR);

  myservo1.write(wheelL);
  myservo2.write(180-wheelR);
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(2000000);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  // Task 3r.4 initialise any 1 of the 2 LEDs on the board as an output
  // YOUR CODE BEGINS HERE
  pinMode(LED_PIN, OUTPUT);

  // END YOUR CODE HERE


  myservo1.attach(23, 1000, 2000);  // Attach the servo to Pin D2 (GPIO 2)
  myservo1.write(90);               // Set the servo position to 90 degrees;
  myservo2.attach(22, 1000, 2000);  // Attach the servo to Pin D2 (GPIO 2)
  myservo2.write(90);               // Set the servo position to 90 degrees;
  myservo1.setPeriodHertz(100);
  myservo2.setPeriodHertz(100);
}



void loop() {

  return;
  // Task 3r.5 turn on or off the LED depending on the state declared above
  // YOUR CODE BEGINS HERE
  if (LEDstate == true) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("HiHi");
    Serial.print("Data,");
    Serial.println(dataX);

  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("Bye");
    Serial.print("Data,");
    Serial.println(dataX);
  }

  delay(100);

  // END YOUR CODE HERE
}