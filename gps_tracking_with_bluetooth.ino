
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// GPS connection
#define GPS_RX 4
#define GPS_TX 5   // Not physically connected

// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
TinyGPSPlus gps;

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

unsigned long lastDisplayUpdate = 0;

void setup() {
  // Hardware TX (D1) sends NMEA data to HC-05
  // Keep debugging text off Serial so NMEA stays clean
  Serial.begin(9600);

  // GPS module baud rate
  gpsSerial.begin(9600);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    while (true) {
      // Stop if OLED initialization fails
    }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("GPS Bluetooth"));
  display.println(F("Initializing..."));
  display.display();
}

void loop() {
  // Read GPS data and forward every character to HC-05
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();

    gps.encode(c);

    // Forward original NMEA sentences to phone
    Serial.write(c);
  }

  // Refresh OLED every 1 second
  if (millis() - lastDisplayUpdate >= 1000) {
    lastDisplayUpdate = millis();
    updateOLED();
  }
}

void updateOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  display.println(F("GPS LIVE TRACKER"));
  display.println(F("----------------"));

  if (gps.location.isValid()) {
    display.print(F("LAT: "));
    display.println(gps.location.lat(), 5);

    display.print(F("LON: "));
    display.println(gps.location.lng(), 5);
  } else {
    display.println(F("Waiting for GPS"));
    display.println(F("location fix..."));
  }

  display.print(F("Satellites: "));
  if (gps.satellites.isValid()) {
    display.println(gps.satellites.value());
  } else {
    display.println(F("--"));
  }

  display.print(F("GPS age: "));
  if (gps.location.isValid()) {
    display.print(gps.location.age() / 1000);
    display.println(F(" sec"));
  } else {
    display.println(F("No fix"));
  }

  display.display();
}
