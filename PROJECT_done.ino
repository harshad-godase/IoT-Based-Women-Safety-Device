#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define BUTTON_PIN 2
#define GSM_RX_PIN 8
#define GSM_TX_PIN 7
#define GPS_RX_PIN 10
#define GPS_TX_PIN 9

SoftwareSerial gsmSerial(GSM_RX_PIN, GSM_TX_PIN); // RX, TX
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN); // RX, TX

TinyGPSPlus gps;

// Array to store phone numbers
String phoneNumbers[] = {"+91******", "+91xxxxxxx","+91xxxxxxxxx"}; // Add more phone numbers as needed

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600); // For debugging
  gsmSerial.begin(9600); // Initialize GSM serial communication
  gpsSerial.begin(9600); // Initialize GPS serial communication
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    // Button is pressed, send distress message with location
    sendDistressMessage();
    delay(1000); // Debouncing delay
  }

  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        Serial.print("Latitude: ");
        Serial.print(gps.location.lat(), 6);
        Serial.print(" Longitude: ");
        Serial.println(gps.location.lng(), 6);
      }
    }
  }
}

void sendDistressMessage() {
  digitalWrite(LED_BUILTIN, HIGH); // Turn on onboard LED to indicate message sending

  // Message to be sent
  String message = "Help! I am in danger.\nLocation:\nLatitude: ";
  message += String(gps.location.lat(), 6);
  message += "\nLongitude: ";
  message += String(gps.location.lng(), 6);
  
  // Generate Google Maps link
  message += "\nGoogle Maps Link: https://www.google.com/maps?q=";
  message += String(gps.location.lat(), 6);
  message += ",";
  message += String(gps.location.lng(), 6);

  // Send SMS to each phone number in the array
  for (int i = 0; i < sizeof(phoneNumbers) / sizeof(phoneNumbers[0]); i++) {
    sendSMS(phoneNumbers[i], message);
    delay(5000); // Delay between sending SMS to each number to avoid SMS rate limit
  }

  digitalWrite(LED_BUILTIN, LOW); // Turn off onboard LED after message sent
}

void sendSMS(String phoneNumber, String message) {
  // Send SMS
  gsmSerial.println("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);
  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(phoneNumber);
  gsmSerial.println("\"");
  delay(1000);
  gsmSerial.print(message);
  delay(100);
  gsmSerial.println((char)26); // End AT command with CTRL+Z
  delay(1000);
}
