#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
SoftwareSerial mySerial(9, 10);

int sensorPin = A5;
int speakerPin = 8;
int gasValue, gasAlertValue, gasShutdownValue;
int gasLeakStatus;
int smsCount = 0;

void setup() {
  pinMode(sensorPin, INPUT);
  pinMode(speakerPin, OUTPUT);
  mySerial.begin(9600);
  Serial.begin(9600);
  lcd.begin(16, 2);
  delay(500);
}

void loop() {
  checkGas();
  checkShutdown();
  gasValue = analogRead(sensorPin);
  Serial.println("Flammable Gas in ppm = ");
  Serial.println(gasValue, DEC);
  delay(3000);
}

void checkGas() {
  lcd.setCursor(0, 0);
  lcd.print("Gas Scan - ON");
  lcd.setCursor(0, 1);
  lcd.print(".........................");
  gasAlertValue = scanGasLevel();

  if (gasAlertValue > 150) {
    Serial.println("Flammable Gas in ppm = ");
    Serial.println(gasValue, DEC);
    lcd.setCursor(0, 1);
    lcd.print("Leakage Detected..!!");
    setAlert();
  }
}

int scanGasLevel() {
  gasValue = analogRead(sensorPin);
  return gasValue;
}

void setAlert() {
  digitalWrite(speakerPin, HIGH);
  
  while (smsCount < 3) {
    sendTextMessage();
  }

  gasLeakStatus = 1;
  lcd.setCursor(0, 1);
  lcd.print("Gas Alert - SMS Sent!");
}

void checkShutdown() {
  if (gasLeakStatus == 1) {
    gasShutdownValue = scanGasLevel();
    if (gasShutdownValue < 150) {
      lcd.setCursor(0, 1);
      lcd.print("Gas Leak Resolved");
      digitalWrite(speakerPin, LOW);
      smsCount = 0;
      gasLeakStatus = 0;
    }
  }
}

void sendTextMessage() {
  mySerial.println("AT+CMGF=1");
  delay(1000);
  mySerial.println("AT+CMGS=\"+91XXXXXXXXXX\"\r");
  delay(1000);
  mySerial.println("Gas Leak Detected!");
  delay(200);
  mySerial.println((char)26);
  delay(1000);

  mySerial.println("AT+CMGS=\"+91XXXXXXXXXX\"\r");
  delay(1000);
  mySerial.println("Gas Leak Detected!");
  delay(200);
  mySerial.println((char)26);
  delay(1000);
  smsCount++;
}

