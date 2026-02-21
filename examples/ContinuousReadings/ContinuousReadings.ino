/*
 * ADS7128 Basic Analog Reading Example
 * 
 * This example demonstrates how to:
 * - Initialize the ADS7128
 * - Configure pins as analog inputs
 * - Enable autonomous mode for continuous conversions
 * - Read analog values and convert to voltage
 * 
 * Hardware Connections:
 * - SDA to microcontroller SDA pin
 * - SCL to microcontroller SCL pin
 * - VDD to 3.3V or 5V
 * - GND to ground
 * - ADDR pin to GND (I2C address: 0x10)
 * - Connect analog signals to AIN0-AIN4
 * 
 * Circuit:
 * - ADS7128 connected via I2C
 * - Analog signals on channels 0-4 (voltage range: 0V to AVDD)
 * 
 * Created by TA
 * MIT License
 */

#include <Wire.h>
#include "ADS7128.h"

// I2C pins - adjust for your board
// ESP32 example:
#define I2C_SDA 21
#define I2C_SCL 22

// Arduino Uno/Mega: Use default SDA/SCL pins
// #define I2C_SDA SDA
// #define I2C_SCL SCL

// ADS7128 I2C Address (default: 0x10)
#define ADS7128_I2C_ADDR 0x10

// Reference voltage (adjust to match your circuit)
#define VREF 3.3

// Number of channels to read
#define NUM_CHANNELS 5

// Create ADS7128 instance
ADS7128 adc(ADS7128_I2C_ADDR);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect (needed for some boards)
  }
  
  Serial.println("ADS7128 Basic Analog Reading Example");
  Serial.println("====================================");
  
  // Initialize I2C
  #ifdef ESP32
    Wire.begin(I2C_SDA, I2C_SCL);
  #else
    Wire.begin();
  #endif
  
  // Initialize ADS7128
  Serial.println("Initializing ADS7128...");
  if (!adc.begin()) {
    Serial.println("ERROR: ADS7128 not found on I2C bus!");
    Serial.println("Check wiring and I2C address.");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("ADS7128 initialized successfully!");
  
  // Configure channels 0-4 as analog inputs
  Serial.println("\nConfiguring analog input channels...");
  for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
    if (adc.setPinMode(i, PIN_MODE_ANALOG_INPUT)) {
      Serial.print("  Channel ");
      Serial.print(i);
      Serial.println(": Configured as analog input");
    } else {
      Serial.print("  Channel ");
      Serial.print(i);
      Serial.println(": Configuration FAILED!");
    }
  }
  
  // Enable autonomous mode for continuous background conversions
  // This is more efficient than manual mode for continuous monitoring
  Serial.println("\nEnabling autonomous mode...");
  uint8_t channelMask = (1 << NUM_CHANNELS) - 1; // Create bit mask for channels 0-4
  if (adc.enableAutonomousMode(channelMask)) {
    Serial.println("Autonomous mode enabled");
  } else {
    Serial.println("WARNING: Could not enable autonomous mode");
  }
  
  Serial.println("\nStarting measurements...");
  Serial.println("=========================================");
  delay(100);
}

void loop() {
  static unsigned long lastPrint = 0;
  
  // Print readings every 500ms
  if (millis() - lastPrint >= 500) {
    lastPrint = millis();
    
    Serial.println();
    Serial.println("Analog Readings:");
    Serial.println("----------------");
    
    // Read all configured channels
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
      // Read raw ADC value (0-4095 for 12-bit)
      uint16_t rawValue = adc.analogRead(i);
      
      // Convert to voltage
      float voltage = adc.analogReadVoltage(i, VREF);
      
      // Print channel number, raw value, and voltage
      Serial.print("CH");
      Serial.print(i);
      Serial.print(": ");
      
      // Print raw value
      Serial.print(rawValue);
      Serial.print(" (");
      
      // Print voltage with 3 decimal places
      Serial.print(voltage, 3);
      Serial.print("V)");
      
      // Add spacing for alignment
      if (i < NUM_CHANNELS - 1) {
        Serial.print("  |  ");
      }
    }
    Serial.println();
  }
}
