#include "HX711.h"
#include <Servo.h>
#include <Wire.h> // Include the Wire library for I2C communication
#include <LiquidCrystal_I2C.h> // Include the LiquidCrystal_I2C library

LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address (0x27 for common I2C address)

// Define other pin assignments
#define calibration_factor 104909.090909 
#define ESC_PIN 9
#define POT_PIN A0
#define BUZZER 2
#define CURRENT_SENSOR A2
#define VOLTAGE_PIN A1

// Specify the data and clock pins for the HX711 module
HX711 scale;
const int DATA_PIN = 6;  // Replace with the actual data pin
const int CLOCK_PIN = 7; // Replace with the actual clock pin

Servo esc;

void setup() {
  pinMode(BUZZER, OUTPUT);

  // Initialize the I2C LCD screen
  lcd.init();
  lcd.backlight(); // Turn on the backlight

  lcd.setCursor(0, 0);
  lcd.print("Thrust Stand 1");
  delay(1000);

  // Initialize the HX711 object with data and clock pins
  scale.begin(DATA_PIN, CLOCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();
  esc.attach(ESC_PIN);


  while (analogRead(POT_PIN) > 20) {
    lcd.setCursor(0, 0);
    lcd.print("TO START SET");
    lcd.setCursor(0, 1);
    lcd.print("THROTTLE TO 0");
    delay(200);
    lcd.clear();
  }
}

void loop() {
  float current = 0.0;
  long int sum_of_reads = 0;

  for (int x = 0; x < 100; x++) {
    sum_of_reads += analogRead(CURRENT_SENSOR);
    delay(2);
  }
  delay(50);
  lcd.clear();

  current = sum_of_reads / 100;
  current = (2.5 - (current * (5.0 / 1024.0))) / 0.066;
  current = abs(current);
  current -= 0.07;
  current = abs(current);

  int escValue = map(analogRead(POT_PIN), 0, 1023, 900, 2000);
  esc.write(escValue);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(abs(scale.get_units()));
  lcd.print("kg ");
  lcd.print("C:");
  lcd.print(current);
  lcd.print("A");
  lcd.setCursor(0, 1);
  lcd.print("E:");
  lcd.print(escValue);
  lcd.print(" P:");
  lcd.print((((analogRead(VOLTAGE_PIN) * 5.0) / 1024.0) * 3.5) * current);
  lcd.print("W");


  
  if (escValue > 1500) {
    if (digitalRead(BUZZER) == HIGH) {
      digitalWrite(BUZZER, LOW);
    } else {
      digitalWrite(BUZZER, HIGH);
    }
  } else {
    digitalWrite(BUZZER, LOW);
  }
}
