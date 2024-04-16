#include <LiquidCrystal_I2C.h>  // change this
#include <WiFi.h>
#include <esp_now.h>
#define LCD_COLUMNS 20

// LCD setup
LiquidCrystal_I2C lcd(0x27, 20, 4);  // change this

// New added
uint8_t broadcastAddress[] = { 0xC8, 0xF0, 0x9E, 0xEB, 0xF9, 0xD4 };  //is equivalent to AB:CD:EF:01:23:45 #Address C8:F0:9E:EB:F9:D4

// SPEED LIMIT
int speed_limited = 75;

// Joystick pins
const int joystickPinX = 35;  // change this // Analog input pin for X-axis
const int joystickPinY = 34;  // change this // Analog input pin for Y-axis

// Joystick threshold
const int joystickThreshold = 250;  // Adjust this value to set the joystick dead zone

const int joystickThreshold2 = 600;  // Adjust this value to set the joystick dead zone

// Buttons threshold
const int upperThreshold = 4096 - 200;  // Adjust this value to set the joystick dead zone

// Buttons threshold
const int lowerThreshold = 200;  // Adjust this value to set the joystick dead zone

int joystickCenterX = 1875;  // joystick center point changes
int joystickCenterY = 1875;  // joystick center point changes

// Toggle switch pin
const int toggleSwitchPin = 32;  // change this

// Toggle mode variables
////bool joystickMode = true;  // true: joystick mode, false: button mode
//bool previousSwitchState = HIGH;

// Direction variable
//bool directionPressed = false;

// Structure example to send data
// Must match the receiver structure
typedef struct message {
  bool joystickMode = true;  // true: joystick mode, false: button mode
  bool previousSwitchState = HIGH;

  bool directionPressed = false;  //button mode state
  bool buttLeft = false;
  bool buttRight = false;
  bool buttUp = false;
  bool buttDown = false;

  int mappedX;  //joystick mode x
  int mappedY;  //joystick mode y
} message;

// Create a struct_message called myMessage
message myMessage;

esp_now_peer_info_t peerInfo;

void printHex(const uint8_t *data, int len) { // get the info bits
  for (int i = 0; i < len; i++) {
    if (data[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}


// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void clearline(int line) {
  lcd.setCursor(0, line);
  lcd.print("                    ");
}

void printPercent(int percent) {
  // Ensure that percent is within the range of -100 to 100
  percent = constrain(percent, -100, 100);

  // Determine the number of characters required to display the value of percent
  int numDigits = (percent >= 0) ? 1 : 2;
  int temp = abs(percent);
  while (temp /= 10) {
    numDigits++;
  }

  // Calculate the number of leading spaces required to align the printed value
  int leadingSpaces = 4 - numDigits;

  // Print the leading spaces
  for (int i = 0; i < leadingSpaces; i++) {
    lcd.print(" ");
  }

  // Print the value of percent
  lcd.print(percent);
}

void setup() {
  // Initialize serial communication
  Serial.begin(2000000);

  // Initialize LCD
  lcd.init();
  lcd.clear();
  lcd.backlight();  // Make sure backlight is on
  lcd.print("===Press: ModeChg===");

  // Set toggle switch pin as input
  pinMode(toggleSwitchPin, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Read toggle switch state
  bool currentSwitchState = digitalRead(toggleSwitchPin);

  // Check if toggle switch is pressed
  if (currentSwitchState == LOW && myMessage.previousSwitchState == HIGH) {
    myMessage.joystickMode = !myMessage.joystickMode;  // Toggle joystick mode

    lcd.setCursor(0, 0);
    if (myMessage.joystickMode) {
      lcd.clear();
      lcd.print("===Mode: Joystick===");
    } else {
      lcd.clear();
      lcd.print("===Mode: 4Buttons===");
    }
    // Reset direction variable
    myMessage.directionPressed = false;
  }

  printHex((uint8_t *)&myMessage, sizeof(myMessage));
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myMessage, sizeof(myMessage));

  // Send message to the receivers
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
  //delay(300);

  // Update previous switch state
  myMessage.previousSwitchState = currentSwitchState;

  if (myMessage.joystickMode) {
    // Joystick mode
    readJoystick();
  } else {
    // Button mode
    readButtons();
  }

  delay(50);  // Delay for stability
}

void readJoystick() {
  // Read joystick values
  int joystickX = analogRead(joystickPinX);
  int joystickY = analogRead(joystickPinY);

  // Apply dead zone
  if (abs(joystickX - joystickCenterX) < joystickThreshold) {
    joystickX = joystickCenterX;
    myMessage.mappedX = 90;
  } else {
  myMessage.mappedX = (joystickX > joystickCenterX) ? map(joystickX, joystickCenterX+joystickThreshold, 4096, 90, 180-speed_limited) : map(joystickX, 0, joystickCenterX-joystickThreshold, 0+speed_limited, 90);  // change this
  }
  if (abs(joystickY - joystickCenterY) < joystickThreshold) {
    joystickY = joystickCenterY;
    myMessage.mappedY = 90;
  } else {
  myMessage.mappedY = (joystickY > joystickCenterY) ? map(joystickY, joystickCenterY+joystickThreshold, 4096, 90, 180-speed_limited) : map(joystickY, 0, joystickCenterY-joystickThreshold, 0+speed_limited, 90);  // change this
  }
  
  // Map joystick values to a range of -100 to 100

  // Print the joystick values to LCD
  //clearline(0);
  lcd.setCursor(0, 1);
  lcd.print("X-axis:");
  printPercent(myMessage.mappedX-90);
  lcd.print("%=d");

  //clearline(1);
  lcd.setCursor(0, 2);
  lcd.print("Y-axis:");
  printPercent(myMessage.mappedY-90);
  lcd.print("d");

  // Print the joystick values to serial (optional)
  //Serial.print("X-axis: ");
  Serial.print(myMessage.mappedX);
  Serial.print(",");
  //Serial.print("%   Y-axis: ");
  Serial.print(myMessage.mappedY);
  Serial.println();
  //Serial.println("%");
}

void readButtons() {
  // Read joystick values
  int joystickX = analogRead(joystickPinX);
  int joystickY = analogRead(joystickPinY);

  // Check if joystick is in the up position
  if (joystickY < lowerThreshold && !myMessage.directionPressed) {
    // Call function for down button
    buttonDown();
    myMessage.directionPressed = true;
    myMessage.buttDown = true;
  }

  // Check if joystick is in the down position
  if (joystickY > upperThreshold && !myMessage.directionPressed) {
    // Call function for up button
    buttonUp();
    myMessage.directionPressed = true;
    myMessage.buttUp = true;
  }

  // Check if joystick is in the left position
  if (joystickX < lowerThreshold && !myMessage.directionPressed) {
    // Call function for left button
    buttonLeft();
    myMessage.directionPressed = true;
    myMessage.buttLeft = true;
  }

  // Check if joystick is in the right position
  if (joystickX > upperThreshold && !myMessage.directionPressed) {
    // Call function for right button
    buttonRight();
    myMessage.directionPressed = true;
    myMessage.buttRight = true;
  }

  // Check if joystick returned to neutral position
  if ((abs(joystickX - joystickCenterX) < joystickThreshold2) && (abs(joystickY - joystickCenterY) < joystickThreshold2) && myMessage.directionPressed) {
    myMessage.directionPressed = false;
    myMessage.buttLeft = false;
    myMessage.buttRight = false;
    myMessage.buttUp = false;
    myMessage.buttDown = false;
    clearline(1);
  }
}

void buttonUp() {
  // Placeholder - Add your code here
  lcd.setCursor(0, 1);
  lcd.print("Up button pressed");
  Serial.println("Up");
}

void buttonDown() {
  //Placeholder - Add your code here
  lcd.setCursor(0, 1);
  lcd.print("Down button pressed");
  Serial.println("Down");
}

void buttonLeft() {
  // Placeholder - Add your code here
  lcd.setCursor(0, 1);
  speed_limited -= 5;
  speed_limited = constrain(speed_limited, 0, 85);
  lcd.print(speed_limited);
  Serial.println("Left");
}

void buttonRight() {
  // Placeholder - Add your code here
  lcd.setCursor(0, 1);
  speed_limited += 5;
  speed_limited = constrain(speed_limited, 0, 85);
  lcd.print(speed_limited);
  Serial.println("Right");
}
