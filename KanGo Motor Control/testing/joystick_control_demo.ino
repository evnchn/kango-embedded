#include <LiquidCrystal_I2C.h> // change this
#define LCD_COLUMNS 20

// LCD setup
LiquidCrystal_I2C lcd(0x27,20,4); // change this

// Joystick pins
const int joystickPinX = 35; // change this // Analog input pin for X-axis
const int joystickPinY = 34; // change this // Analog input pin for Y-axis

// Joystick threshold
const int joystickThreshold = 250;  // Adjust this value to set the joystick dead zone

const int joystickThreshold2 = 600;  // Adjust this value to set the joystick dead zone

// Buttons threshold
const int upperThreshold = 4096-200;  // Adjust this value to set the joystick dead zone

// Buttons threshold
const int lowerThreshold = 200;  // Adjust this value to set the joystick dead zone

int joystickCenterX = 1875; // joystick center point changes
int joystickCenterY = 1875; // joystick center point changes

// Toggle switch pin
const int toggleSwitchPin = 32; // change this

// Toggle mode variables
bool joystickMode = true;  // true: joystick mode, false: button mode
bool previousSwitchState = HIGH;

// Direction variable
bool directionPressed = false;

void clearline(int line){
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
  Serial.begin(9600);

  // Initialize LCD
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  lcd.print("===Press: ModeChg===");

  // Set toggle switch pin as input
  pinMode(toggleSwitchPin, INPUT_PULLUP);
}

void loop() {
  // Read toggle switch state
  bool currentSwitchState = digitalRead(toggleSwitchPin);

  // Check if toggle switch is pressed
  if (currentSwitchState == LOW && previousSwitchState == HIGH) {
    joystickMode = !joystickMode;  // Toggle joystick mode

    lcd.setCursor(0,0);
    if (joystickMode) {
      lcd.clear();
      lcd.print("===Mode: Joystick===");
    } else {
      lcd.clear();
      lcd.print("===Mode: 4Buttons===");
    }
    // Reset direction variable
    directionPressed = false;
  }

  // Update previous switch state
  previousSwitchState = currentSwitchState;

  if (joystickMode) {
    // Joystick mode
    readJoystick();
  } else {
    // Button mode
    readButtons();
  }

  delay(100);  // Delay for stability
}

void readJoystick() {
  // Read joystick values
  int joystickX = analogRead(joystickPinX);
  int joystickY = analogRead(joystickPinY);

  // Apply dead zone
  if (abs(joystickX - joystickCenterX) < joystickThreshold) {
    joystickX = joystickCenterX;
  }
  if (abs(joystickY - joystickCenterY) < joystickThreshold) {
    joystickY = joystickCenterY;
  }

  // Map joystick values to a range of -100 to 100
  int mappedX = (joystickX > joystickCenterX) ? map(joystickX, joystickCenterX, 4096, 0, 100): map(joystickX, 0, joystickCenterX, -100, 0); // change this
  int mappedY = (joystickY > joystickCenterY) ? map(joystickY, joystickCenterY, 4096, 0, 100): map(joystickY, 0, joystickCenterY, -100, 0); // change this

  // Print the joystick values to LCD
  //clearline(0);
  lcd.setCursor(0, 1);
  lcd.print("X-axis:");
  printPercent(mappedX);
  lcd.print("%   ");

  //clearline(1);
  lcd.setCursor(0, 2);
  lcd.print("Y-axis:");
  printPercent(mappedY);
  lcd.print("%");

  // Print the joystick values to serial (optional)
  //Serial.print("X-axis: ");
  Serial.print(mappedX);
  Serial.print(",");
  //Serial.print("%   Y-axis: ");
  Serial.print(mappedY);
  Serial.println();
  //Serial.println("%");
}

void readButtons() {
  // Read joystick values
  int joystickX = analogRead(joystickPinX);
  int joystickY = analogRead(joystickPinY);

  // Check if joystick is in the up position
  if (joystickY < lowerThreshold && !directionPressed) {
    // Call function for down button
    buttonDown();
    directionPressed = true;
  }

  // Check if joystick is in the down position
  if (joystickY > upperThreshold && !directionPressed) {
    // Call function for up button
    buttonUp();
    directionPressed = true;
  }

  // Check if joystick is in the left position
  if (joystickX < lowerThreshold && !directionPressed) {
    // Call function for left button
    buttonLeft();
    directionPressed = true;
  }

  // Check if joystick is in the right position
  if (joystickX > upperThreshold && !directionPressed) {
    // Call function for right button
    buttonRight();
    directionPressed = true;
  }

  // Check if joystick returned to neutral position
  if ((abs(joystickX - joystickCenterX) < joystickThreshold2) && (abs(joystickY - joystickCenterY) < joystickThreshold2) && directionPressed) {
    directionPressed = false;
    clearline(1);
  }
}

void buttonUp() {
  // Placeholder - Add your code here
  lcd.setCursor(0, 1);
  lcd.print("Up button pressed");
}

void buttonDown() {
  //Placeholder - Add your code here
  lcd.setCursor(0, 1);
  lcd.print("Down button pressed");
}

void buttonLeft() {
  // Placeholder - Add your code here
  lcd.setCursor(0, 1);
  lcd.print("Left button pressed");
}

void buttonRight() {
  // Placeholder - Add your code here
  lcd.setCursor(0, 1);
  lcd.print("Right button pressed");
}