#include <Servo.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DHT_U.h>
#include <DHT.h>

#define SS_PIN 53
#define RST_PIN 5

#define FAN 8
#define PARLOUR_SENSOR 10
#define ROOM_SENSOR 11
#define PARLOUR_LED A0
#define ROOM_LED 13
#define FLAME_SENSOR 44
#define HOUSE_RELAY 43
#define BUZZER 45
#define ALARM_LED 22

MFRC522 rfid(SS_PIN, RST_PIN);

LiquidCrystal_I2C lcd(0x27, 20, 4);

byte alexUID[] = {0x73, 0x4A, 0x1B, 0xE3};
byte profUID[] = {0x23, 0x41, 0x62, 0x04};
byte macUID[] = {0x03, 0xF6, 0x40, 0x10};

Servo door;
int servoPin = 9;
int openPos = 90;
int closePos = 0;
int lastPosition = closePos;

String password = "0000";
String input = "";
unsigned long doorOpenTime = 5000; // Time in ms for door to stay open
unsigned long doorOpenedAt = 0;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {30, 32, 34, 36};
byte colPins[COLS] = {31, 33, 35, 37};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define greenLED 3
#define redLED 2

#define trig 40
#define echo 41
#define pump 42        
#define max_distance 30 // Distance when the tank is empty (in cm)
#define min_distance 11  // Distance when the tank is full (in cm)
#define threshold 30    // Threshold for low water level (in %)
//bool isPumping = false; 


int parlourValue;
int roomValue;
int flameValue;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  dht.begin();
  rfid.PCD_Init();
  door.attach(servoPin);
  door.write(lastPosition);

  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);

  pinMode(PARLOUR_SENSOR, INPUT);
  pinMode(ROOM_SENSOR, INPUT);
  pinMode(FLAME_SENSOR,INPUT);
  pinMode(HOUSE_RELAY,OUTPUT);
  pinMode(ALARM_LED,OUTPUT);
  pinMode(BUZZER,OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(PARLOUR_LED, OUTPUT);
  pinMode(ROOM_LED, OUTPUT);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(pump, OUTPUT); 
  digitalWrite(pump, LOW);

  lcd.init();
  lcd.backlight();
  frontPage();
  displayMenu();
}

void loop() {
  char key = keypad.getKey();
  if (key) handleKeyPress(key);
  autoCloseDoor();
  delay(200); // Reduced delay for better response

  automation();
  fire_alarm();
}
void frontPage(){
  lcd.setCursor(2,0);
  lcd.print("Computer vision");
  lcd.setCursor(2,1);
  lcd.print("Home Automation");
  lcd.setCursor(9,2);
  lcd.print("By");
  lcd.setCursor(3,3);
  lcd.print("Engr Macliszt ");
  delay(3000);
}
void displayMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Choose Method:");
  lcd.setCursor(0, 1);
  lcd.print("A: Face Recog.");
  lcd.setCursor(0, 2);
  lcd.print("B: Card Access");
  lcd.setCursor(0, 3);
  lcd.print("5: Password");
}

void handleKeyPress(char key) {
  switch (key) {
    case 'A':
      triggerFaceRecognition();
      break;
    case 'B':
      checkRFIDCard();
      break;
    case '5':
      promptPasswordEntry();
      break;
    case 'D':
      displayTemperature();
      break;
    case '#':
      controlTank();
      break;
    case '*':
      displayMenu(); // Back to main menu
      break;
    
    default:
      break;
  }
}

void triggerFaceRecognition() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Present face...");
  Serial.println("Face Recognition Requested");

  unsigned long startTime = millis();
  bool faceDetected = false;
  bool displayNotRecognized = false;

  // Wait for up to 10 seconds for a face recognition message
  while (millis() - startTime < 10000) {
    if (Serial.available() > 0) {
      String message = Serial.readStringUntil('\n');
      message.trim();

      if (message == "Face Recognized") {
        faceDetected = true;
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Face Recognized");
        digitalWrite(greenLED, HIGH);
        openDoor();  // Function to open the door
        delay(1000); // Keep the green LED on for 1 second
        digitalWrite(greenLED, LOW);
        break; // Exit the loop if face is recognized
      }
      // No immediate display of "Face Not Recognized"
      else if (message == "Face Not Recognized") {
        displayNotRecognized = true; // Set a flag to display the message later
      }
    }
  }

  // If no face was detected within 10 seconds
  if (!faceDetected) {
    // Wait additional 5 seconds before displaying "Face Not Recognized"
    delay(5000);

    // Display the "Face not recognized" message if it was flagged
    if (displayNotRecognized) {
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Face not recognized");
      lcd.setCursor(0, 2);
      lcd.print("Try another method");
      digitalWrite(redLED, HIGH); // Indicate with red LED
      delay(1000); // Keep the red LED on for 1 second
      digitalWrite(redLED, LOW);
    } else {
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("No face detected");
      lcd.setCursor(0, 2);
      lcd.print("Try another method");
    }
  }
}

void checkRFIDCard() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Present card...");
  delay(2000);

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Card read error");
    lcd.setCursor(0, 2);
    lcd.print("Try again or press *");
    return;
  }

  if (compareUID(rfid.uid.uidByte, rfid.uid.size, alexUID)) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Welcome Alex!");
    openDoor();
    delay(1000);
  } 
  else if (compareUID(rfid.uid.uidByte, rfid.uid.size, profUID)) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Welcome Prof!");
    openDoor();
    delay(1000);
  } 
  else if (compareUID(rfid.uid.uidByte, rfid.uid.size, macUID)) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Welcome Mac!");
    openDoor();
    delay(1000);
  }
  else {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("No Access");
    lcd.setCursor(0, 2);
    lcd.print("Try another method");
    digitalWrite(redLED, HIGH);
    delay(1000);
    digitalWrite(redLED, LOW);
  }
  rfid.PICC_HaltA();
}

void promptPasswordEntry() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Enter Password:");
  input = "";

  while (input.length() < password.length()) {
    char key = keypad.getKey();
    if (key) {
      if (key == '*') {
        displayMenu(); // Back to main menu
        return;
      } else if (key == '#') {
        input = "";
        lcd.setCursor(0, 2);
        lcd.print("Cleared");
        delay(500);
        lcd.setCursor(0, 2);
        lcd.print("Enter Password:");
      } else {
        input += key;
        lcd.setCursor(input.length() - 1, 2);
        lcd.print("*");
      }
    }
  }

  if (input == password) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Access Granted");
    digitalWrite(greenLED, HIGH);
    openDoor();
    //delay(1000);
    //digitalWrite(greenLED, LOW);
  } else {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Invalid Password");
    lcd.setCursor(0, 2);
    lcd.print("Access Denied");
    digitalWrite(redLED, HIGH);
//    delay(1000);
//    digitalWrite(redLED, LOW);
  }
}

void displayTemperature() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  if (isnan(humidity) || isnan(temperature)) {
    lcd.print("Sensor Error");
    return;
  }
  
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print(" %");

  if (temperature > 34) {
    digitalWrite(FAN, HIGH); // Turn the fan on
  } else {
    digitalWrite(FAN, LOW);  // Turn the fan off
  }
  delay(5000); // Display for 5 seconds
  displayMenu();
}

void controlTank() {
  // Trigger the ultrasonic sensor
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  // Measure the echo duration
  long duration = pulseIn(echo, HIGH);

  // Calculate the distance in cm
  int distance = (duration * 0.034) / 2;

  Serial.println(distance);
  // Ensure distance stays within expected bounds
  if (distance > max_distance) distance = max_distance;
  if (distance < min_distance) distance = min_distance;

  // Map distance to a water level percentage
  int level = map(distance, max_distance, min_distance, 0, 100);

  // Clear the LCD for new data
  lcd.clear();

  // Display water level percentage
  lcd.setCursor(0, 0); // First row, first column
  lcd.print("Water Level: ");
  lcd.print(level);
  lcd.print(" %");

  // Control the pump based on water level
  if (distance >= 12) {
    lcd.setCursor(0, 1); // Second row, first column
    lcd.print("Status: Too Low");
    lcd.setCursor(0, 2); // Third row, first column
    lcd.print("Pumping Water...");
    digitalWrite(pump, HIGH); // Turn the pump on
    //isPumping = true;
  } 
  else if (distance < 12 && distance >= 10) {
    lcd.setCursor(0, 1); // Second row, first column
    lcd.print("Status: Tank Full");
    lcd.setCursor(0, 2); // Third row, first column
    lcd.print("Stopping Pump...");
    digitalWrite(pump, LOW); // Turn the pump off
    //isPumping = false;
  } 
  else {
    lcd.setCursor(0, 1); // Second row, first column
    lcd.print("Status: Normal");
    lcd.setCursor(0, 2); // Third row, first column
    lcd.print("Water Level OK");
  }

//  delay(1000);
}


void automation() {
  parlourValue = digitalRead(PARLOUR_SENSOR);
  roomValue = digitalRead(ROOM_SENSOR);

  Serial.print(" Parlour: ");
  Serial.print(parlourValue);
  Serial.print("  Room:  ");
  Serial.println(roomValue);

  if (parlourValue == 1) {
    analogWrite(PARLOUR_LED, 255);
    //delay(5000);
  } 
  else {
    analogWrite(PARLOUR_LED, 10);
    //delay(2000);
  }

  if (roomValue == 1) {
    analogWrite(ROOM_LED, 255);
    //delay(5000);
  } 
  else {
    analogWrite(ROOM_LED, 10);
    //delay(2000);
  }
}


void openDoor() {
  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, LOW);
  door.write(openPos);
  doorOpenedAt = millis();
}

void autoCloseDoor() {
  if (doorOpenedAt != 0 && millis() - doorOpenedAt >= doorOpenTime) {
    closeDoor();
  }
}

void closeDoor() {
  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, LOW);
  door.write(closePos);
  doorOpenedAt = 0;
  displayMenu();
}

bool compareUID(byte *uid, byte uidSize, byte *knownUID) {
  for (byte i = 0; i < uidSize; i++) {
    if (uid[i] != knownUID[i]) return false;
  }
  return true;
}
void fire_alarm(){
  flameValue = digitalRead(FLAME_SENSOR);

  if (flameValue == 0){
    digitalWrite(HOUSE_RELAY,HIGH);
    digitalWrite(ALARM_LED,HIGH);
    tone(BUZZER,450);
    delay(500);
    digitalWrite(ALARM_LED,LOW);
    noTone(BUZZER);
    delay(500);
  }
  else{
    digitalWrite(HOUSE_RELAY,LOW);
    digitalWrite(ALARM_LED,LOW);
    noTone(BUZZER);
  }
}
