#include <TinyGPS++.h>
#include <TinyGPSPlus.h>

#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial gpsSerial(4, 3);
TinyGPSPlus gps;

// =========================
// ULTRASONIC SENSOR 1
// =========================
#define trigPin1 9
#define echoPin1 10

// =========================
// ULTRASONIC SENSOR 2
// =========================
#define trigPin2 6
#define echoPin2 7

// =========================
// OUTPUT DEVICES
// =========================
#define motor 5
#define buzzer 8

// =========================
// SOS BUTTON
// =========================
#define helpButton 2

long duration1, duration2;
int distance1, distance2;

const int obstacleRange = 30;

bool alertSent = false;

void setup() {

  Serial.begin(9600);

  // GPS Baud Rate
  gpsSerial.begin(9600);

  // Sensor 1
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);

  // Sensor 2
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  // Output Devices
  pinMode(motor, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // SOS Button
  pinMode(helpButton, INPUT_PULLUP);

  Serial.println("SMART BLIND STICK SYSTEM STARTED");
}

void loop() {

  // =========================
  // READ GPS DATA CONTINUOUSLY
  // =========================
  while (gpsSerial.available() > 0) {

    char c = gpsSerial.read();

    gps.encode(c);
  }

  // =========================
  // SENSOR 1 READING
  // =========================
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin1, LOW);

  duration1 = pulseIn(echoPin1, HIGH);

  distance1 = duration1 * 0.034 / 2;

  // =========================
  // SENSOR 2 READING
  // =========================
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin2, LOW);

  duration2 = pulseIn(echoPin2, HIGH);

  distance2 = duration2 * 0.034 / 2;

  // =========================
  // DISPLAY DISTANCES
  // =========================
  Serial.print("Sensor 1 Distance: ");
  Serial.print(distance1);
  Serial.println(" cm");

  Serial.print("Sensor 2 Distance: ");
  Serial.print(distance2);
  Serial.println(" cm");

  // =========================
// OBSTACLE DETECTION
// DIFFERENT PATTERNS
// =========================

// HEAD LEVEL OBSTACLE (Sensor 1)
if (distance1 > 2 && distance1 <= obstacleRange) {

  Serial.println("Head Level Obstacle!");

  // Fast continuous vibration
  digitalWrite(motor, HIGH);
  delay(500);
  digitalWrite(motor, LOW);
  delay(100);
}

// GROUND LEVEL OBSTACLE (Sensor 2)
else if (distance2 > 2 && distance2 <= obstacleRange) {

  Serial.println("Ground Level Obstacle!");

  // Vibrate -> Pause -> Vibrate
  digitalWrite(motor, HIGH);
  delay(200);

  digitalWrite(motor, LOW);
  delay(300);

  digitalWrite(motor, HIGH);
  delay(200);

  digitalWrite(motor, LOW);
  delay(300);
}

// NO OBSTACLE
else {

  digitalWrite(motor, LOW);
}
  // =========================
  // SOS BUTTON SYSTEM
  // =========================
  int buttonState = digitalRead(helpButton);

  if (buttonState == LOW && !alertSent) {

    Serial.println("\n========================");
    Serial.println("SOS ALERT BUTTON PRESSED!");
    Serial.println("========================");

    // Emergency Buzzer
    tone(buzzer, 2000);
    delay(2000);
    noTone(buzzer);

    // =========================
    // GPS LOCATION
    // =========================
    if (gps.location.isValid()) {

      float latitude = gps.location.lat();
      float longitude = gps.location.lng();

      Serial.println("\nGPS LOCATION FOUND!");

      Serial.print("Latitude: ");
      Serial.println(latitude, 6);

      Serial.print("Longitude: ");
      Serial.println(longitude, 6);

      Serial.println("\nGoogle Maps Link:");

      Serial.print("https://maps.google.com/?q=");
      Serial.print(latitude, 6);
      Serial.print(",");
      Serial.println(longitude, 6);

      Serial.println("---------------------------");
    }
    
      else {
        Serial.println("GPS location not available.");
        Serial.println("Move near window or outside.");
    }

    alertSent = true;
  }

  // Reset alert
  if (buttonState == HIGH) {
    alertSent = false;
  }

  delay(100);
}
