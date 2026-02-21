/*
 * ADS7128 Window Comparator Example
 * 
 * This example demonstrates advanced features:
 * - Window comparator with programmable thresholds
 * - GPIO outputs triggered by analog events
 * - Hysteresis to prevent oscillation
 * - Autonomous mode with continuous monitoring
 * 
 * Functionality:
 * - Monitors analog inputs on channels 3 and 4
 * - When voltage goes outside the defined window (0.4V - 1.0V):
 *   - GPIO pins 5 and 6 are automatically pulled LOW
 * - When voltage returns inside the window:
 *   - GPIO pins return to HIGH state
 * 
 * Hardware Connections:
 * - SDA to microcontroller SDA pin
 * - SCL to microcontroller SCL pin
 * - VDD to 3.3V
 * - GND to ground
 * - ADDR pin to GND (I2C address: 0x10)
 * - Connect analog signals to AIN0-AIN4 (inputs)
 * - Connect LEDs or other loads to GPIO5 and GPIO6 (outputs)
 * 
 * Use Case:
 * This is useful for battery monitoring, overvoltage/undervoltage protection,
 * temperature monitoring, or any application where you need automatic alerts
 * when analog values go outside acceptable ranges.
 * 
 * Created by TA
 * MIT License
 */

#include <Wire.h>
#include "ADS7128.h"

// I2C pins for ESP32
#define I2C_SDA 0
#define I2C_SCL 5

// ADS7128 I2C Address (default: 0x10)
#define ADS7128_I2C_ADDR 0x10

// Pin definitions
#define INPUT0 0
#define INPUT1 1
#define INPUT2 2
#define INPUT3 3  // Channel with window comparator
#define INPUT4 4  // Channel with window comparator
#define OUTPUT0 5 // GPIO output triggered by INPUT3
#define OUTPUT1 6 // GPIO output triggered by INPUT4

// Window comparator settings
#define WINDOW_MAX 1.0    // Upper threshold (volts)
#define WINDOW_MIN 0.4    // Lower threshold (volts)
#define HYSTERESIS 0.1    // Hysteresis voltage (volts)
#define EVENT_COUNT 4     // Number of events before trigger
#define VREF 3.3          // Reference voltage

// Create ADS7128 instance
ADS7128 adc(ADS7128_I2C_ADDR);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port
  }
  
  Serial.println("ADS7128 Window Comparator Example");
  Serial.println("==================================");
  
  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  
  Serial.println("\nStep 1: Initializing ADS7128...");
  if (!adc.begin()) {
    Serial.println("ERROR: ADS7128 not found on I2C bus!");
    while (1) delay(1000);
  }
  Serial.println("Device initialized successfully!");
  
  // Read and display initial status
  Serial.println("\nStep 2: Reading initial status...");
  SystemStatus status = adc.getSystemStatus();
  Serial.print("Status register: 0x");
  Serial.println(status.rawValue, HEX);
  
  // Configure input pins (analog inputs)
  Serial.println("\nStep 3: Configuring input pins...");
  adc.setPinMode(INPUT0, PIN_MODE_ANALOG_INPUT);
  adc.setPinMode(INPUT1, PIN_MODE_ANALOG_INPUT);
  adc.setPinMode(INPUT2, PIN_MODE_ANALOG_INPUT);
  adc.setPinMode(INPUT3, PIN_MODE_ANALOG_INPUT);
  adc.setPinMode(INPUT4, PIN_MODE_ANALOG_INPUT);
  Serial.println("Input pins configured as analog inputs");
  
  // Configure output pins (digital outputs)
  Serial.println("\nStep 4: Configuring output pins...");
  adc.setPinMode(OUTPUT0, PIN_MODE_DIGITAL_OUTPUT);
  adc.setPinMode(OUTPUT1, PIN_MODE_DIGITAL_OUTPUT);
  adc.setDriveMode(OUTPUT0, DRIVE_MODE_PUSH_PULL);
  adc.setDriveMode(OUTPUT1, DRIVE_MODE_PUSH_PULL);
  Serial.println("Output pins configured as push-pull outputs");
  
  // Set initial output state to HIGH
  adc.digitalWrite(OUTPUT0, true);
  adc.digitalWrite(OUTPUT1, true);
  Serial.println("Output pins set to HIGH (inactive state)");
  
  // Enable autonomous mode for continuous conversions
  Serial.println("\nStep 5: Enabling autonomous mode...");
  uint8_t channelMask = (1 << INPUT0) | (1 << INPUT1) | (1 << INPUT2) | 
                        (1 << INPUT3) | (1 << INPUT4);
  adc.enableAutonomousMode(channelMask);
  Serial.println("Autonomous mode enabled on inputs 0-4");
  
  // Configure window comparator
  Serial.println("\nStep 6: Configuring window comparator...");
  adc.enableWindowMode(true);
  Serial.print("  Window thresholds: ");
  Serial.print(WINDOW_MIN, 1);
  Serial.print("V - ");
  Serial.print(WINDOW_MAX, 1);
  Serial.println("V");
  
  // Set window for INPUT3
  adc.setChannelWindow(INPUT3, WINDOW_MAX, WINDOW_MIN, EVENT_COUNT, VREF);
  Serial.print("  Channel ");
  Serial.print(INPUT3);
  Serial.print(": Window configured (");
  Serial.print(EVENT_COUNT);
  Serial.println(" events required)");
  
  // Set window for INPUT4
  adc.setChannelWindow(INPUT4, WINDOW_MAX, WINDOW_MIN, EVENT_COUNT, VREF);
  Serial.print("  Channel ");
  Serial.print(INPUT4);
  Serial.print(": Window configured (");
  Serial.print(EVENT_COUNT);
  Serial.println(" events required)");
  
  // Set hysteresis
  adc.setChannelHysteresis(INPUT3, HYSTERESIS, VREF);
  adc.setChannelHysteresis(INPUT4, HYSTERESIS, VREF);
  Serial.print("  Hysteresis set to ");
  Serial.print(HYSTERESIS, 1);
  Serial.println("V");
  
  // Configure alert system
  Serial.println("\nStep 7: Configuring alert system...");
  adc.setAlertLogic(1, 0x00);  // Push-pull drive, active low
  adc.setWindowRegion(0x00);   // Alert when OUTSIDE window
  Serial.println("  Alert triggers when voltage goes outside window");
  
  // Configure GPIO trigger pins
  Serial.println("\nStep 8: Configuring GPIO triggers...");
  adc.setTriggerOn((1 << OUTPUT0) | (1 << OUTPUT1));
  Serial.println("  GPIO triggers enabled on OUTPUT0 and OUTPUT1");
  
  // Map alert sources to GPIO pins
  adc.setTriggerPins(OUTPUT0, (1 << INPUT3));
  adc.setTriggerPins(OUTPUT1, (1 << INPUT4));
  Serial.println("  OUTPUT0 triggered by INPUT3");
  Serial.println("  OUTPUT1 triggered by INPUT4");
  
  // Set pin values on trigger (0 = LOW when alert fires)
  adc.setValueOnTrigger(OUTPUT0, 0);
  adc.setValueOnTrigger(OUTPUT1, 0);
  Serial.println("  Outputs go LOW when alert fires");
  
  Serial.println("\n========================================");
  Serial.println("Window comparator configured!");
  Serial.println("Monitoring analog inputs...");
  Serial.println("========================================\n");
  
  delay(100);
}

void loop() {
  static unsigned long lastUpdate = 0;
  
  // Print status every 500ms
  if (millis() - lastUpdate >= 500) {
    lastUpdate = millis();
    
    // Read all analog inputs
    uint16_t analog0 = adc.analogRead(INPUT0);
    uint16_t analog1 = adc.analogRead(INPUT1);
    uint16_t analog2 = adc.analogRead(INPUT2);
    uint16_t analog3 = adc.analogRead(INPUT3);
    uint16_t analog4 = adc.analogRead(INPUT4);
    
    // Convert to voltages
    float voltage3 = adc.analogReadVoltage(INPUT3, VREF);
    float voltage4 = adc.analogReadVoltage(INPUT4, VREF);
    
    // Print readings
    Serial.println("=== Analog Readings ===");
    Serial.print("CH0: "); Serial.print(analog0);
    Serial.print(" | CH1: "); Serial.print(analog1);
    Serial.print(" | CH2: "); Serial.println(analog2);
    
    // Highlight monitored channels
    Serial.print("CH3 (monitored): "); 
    Serial.print(analog3);
    Serial.print(" (");
    Serial.print(voltage3, 3);
    Serial.print("V)");
    if (voltage3 < WINDOW_MIN || voltage3 > WINDOW_MAX) {
      Serial.print(" *** OUT OF RANGE ***");
    } else {
      Serial.print(" OK");
    }
    Serial.println();
    
    Serial.print("CH4 (monitored): ");
    Serial.print(analog4);
    Serial.print(" (");
    Serial.print(voltage4, 3);
    Serial.print("V)");
    if (voltage4 < WINDOW_MIN || voltage4 > WINDOW_MAX) {
      Serial.print(" *** OUT OF RANGE ***");
    } else {
      Serial.print(" OK");
    }
    Serial.println();
    
    // Print window limits for reference
    Serial.print("Window: ");
    Serial.print(WINDOW_MIN, 1);
    Serial.print("V - ");
    Serial.print(WINDOW_MAX, 1);
    Serial.println("V");
    Serial.println();
  }
}
