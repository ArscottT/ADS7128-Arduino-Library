# ADS7128 Arduino Library

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino Library](https://img.shields.io/badge/Arduino-Library-blue.svg)](https://www.arduino.cc/reference/en/libraries/)
[<img src="https://cdn.registry.platformio.org/apple-touch-icon.png" height="36" alt="PlatformIO Registry">](https://registry.platformio.org/libraries/ghostcrab/ADS7128/)


Arduino library for the Texas Instruments ADS7128 8-channel, 12-bit analog-to-digital converter with integrated GPIO and programmable window comparator.

## Features

- **8 Analog Input Channels**: 12-bit resolution (0-4095) ADC
- **Dual Operating Modes**: Manual or autonomous conversion
- **Configurable GPIO**: Each pin can be analog input, digital input, or digital output
- **Window Comparator**: Programmable high/low thresholds with hysteresis
- **Alert System**: Trigger GPIO pins based on analog threshold events
- **I2C Interface**: Flexible addressing (0x10-0x17)x

## Hardware Information

- **Chip**: Texas Instruments ADS7128
- **Resolution**: 12-bit (4096 levels)
- **Channels**: 8 analog inputs (can be configured as GPIO)
- **Interface**: I2C (up to 400 kHz)
- **Supply Voltage**: 2.7V to 5.5V
- **Reference Voltage**: Internal or external (up to AVDD)

## Quick Start

### Digital read

```cpp
#include <Wire.h>
#include "ADS7128.h"

// Enable debug output (Must Serial.begin() before adc.begin())
#define ADS7128_DEBUG 1

ADS7128 adc(0x10);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  adc.begin();

  // Configure pin 7 as a digital input
  adc.setPinMode(7, PIN_MODE_DIGITAL_INPUT);
}

void loop() {
  bool state = adc.digitalRead(7);

  Serial.print("Pin 7: ");
  Serial.println(state ? "HIGH" : "LOW");

  delay(500);
}
```

### Analog read

```cpp
#include <Wire.h>
#include "ADS7128.h"

// Enable debug output (Must Serial.begin() before adc.begin())
#define ADS7128_DEBUG 1

// Create ADS7128 instance (default I2C address: 0x10)
ADS7128 adc();

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  // Initialize the ADC
  if (!adc.begin()) {
    Serial.println("ADS7128 not found!");
    while(1);
  }
  
  // Configure pin 0 as analog input
  adc.setPinMode(0, PIN_MODE_ANALOG_INPUT);
  adc.adc.enableManualMode();
}

void loop() {
  // Read analog value on pin 0(0-4095)
  uint16_t value = adc.analogRead(0);
  
  // Read voltage on pin 0 (assuming 3.3V reference)
  float voltage = adc.analogReadVoltage(0, 3.3);
  
  Serial.print("Value: ");
  Serial.print(value);
  Serial.print(" | Voltage: ");
  Serial.print(voltage);
  Serial.println(" V");
  
  delay(1000);
}
```

## TODO:
- 1:
This library currently uses a less efficient register update method.
The ADS7128 includes the ability to set or clear individual bits of a register through specific register commands. 
This allows for less I2C commands to be issued, as the typical read-set-write register operations do not need to be performed. 

- 2:
The continuous register block read and write are also not implemented, meaning each result must be read individually.

- 3: 
Additional features of the ADS7128 are missing: RMS, CRC, sample rate selection, I2C mode, OSR and some aspects of the zero crossing module.

- 4:
Scale analogRead delay with the configured OSR ratio, or poll conversion done register.

## Known Issues
- 1:
The analogRead function while in Manual mode uses a fixed delay (delayMicroseconds(10)), this means if conversion is not complete before reading, the data will be stale.  

## API Reference

### Initialization

#### `ADS7128(uint8_t i2cAddr = 0x10)`
Constructor. Creates an ADS7128 instance.
- **i2cAddr**: I2C address (default: 0x10, range: 0x10-0x17)

#### `bool begin(TwoWire &wirePort = Wire)`
Initialize the device. Returns `true` if successful.
- **wirePort**: I2C bus to use (default: Wire)

### Pin Configuration

#### `bool setPinMode(uint8_t pin, PinMode mode)`
Configure a pin's operating mode.
- **pin**: Pin number (0-7)
- **mode**: `PIN_MODE_ANALOG_INPUT`, `PIN_MODE_DIGITAL_INPUT`, or `PIN_MODE_DIGITAL_OUTPUT`

#### `PinMode getPinMode(uint8_t pin)`
Get the current pin mode.

#### `bool setDriveMode(uint8_t pin, DriveMode mode)`
Set output drive mode (for output pins only).
- **mode**: `DRIVE_MODE_OPEN_DRAIN` or `DRIVE_MODE_PUSH_PULL`

### Analog Operations

#### `uint16_t analogRead(uint8_t channel)`
Read analog value from a channel (0-4095).
- **channel**: Channel number (0-7)

#### `float analogReadVoltage(uint8_t channel, float vref = 3.3)`
Read analog value as voltage.
- **channel**: Channel number (0-7)
- **vref**: Reference voltage in volts

#### `bool enableManualMode(uint8_t channels)`
Enable manual conversion mode. Conversions triggered on-demand.
- **channels**: Bit mask of channels to enable (e.g., `0x03` for channels 0 and 1)

#### `bool enableAutonomousMode(uint8_t channels)`
Enable autonomous conversion mode. Continuous background conversions.
- **channels**: Bit mask of channels to enable

### Digital I/O

#### `bool digitalWrite(uint8_t pin, bool value)`
Write a digital value to an output pin.
- **pin**: Pin number (0-7)
- **value**: `true` (HIGH) or `false` (LOW)

#### `bool digitalRead(uint8_t pin)`
Read a digital value from an input pin. Returns `true` (HIGH) or `false` (LOW).

### Window Comparator

#### `bool enableWindowMode(bool state)`
Enable or disable window comparator functionality.

#### `bool setChannelWindow(uint8_t channel, float windowMax, float windowMin, uint8_t eventCount, float Vref = 3.3)`
Configure window comparator thresholds for a channel.
- **channel**: Channel number (0-7)
- **windowMax**: Upper threshold voltage
- **windowMin**: Lower threshold voltage
- **eventCount**: Number of consecutive events before triggering (0-15)
- **Vref**: Reference voltage

#### `bool setChannelHysteresis(uint8_t channel, float hysteresis_v, float Vref = 3.3)`
Set hysteresis voltage to prevent threshold oscillation.

#### `bool setWindowRegion(uint8_t regions)`
Define alert region (inside or outside window).
- **regions**: 0x00 for outside window, 0xFF for inside window

### Alert Configuration

#### `bool setAlertLogic(bool drive, uint8_t logic)`
Configure alert pin behavior.
- **drive**: Drive mode (0 = open-drain, 1 = push-pull)
- **logic**: Logic configuration (0-3)

#### `bool setAlertChannels(uint8_t channels)`
Select which channels can trigger alerts.
- **channels**: Bit mask of channels

#### `bool setTriggerOn(uint8_t pins)`
Enable GPIO pins to be triggered by alerts.
- **pins**: Bit mask of pins

#### `bool setTriggerPins(uint8_t pin, uint8_t alerts)`
Map alert sources to a specific GPIO pin.
- **pin**: GPIO pin number (0-7)
- **alerts**: Bit mask of alert sources

#### `bool setValueOnTrigger(uint8_t pin, bool state)`
Set the output value when a pin is triggered.
- **pin**: Pin number (0-7)
- **state**: Value to set on trigger

### System Status

#### `SystemStatus getSystemStatus()`
Get device status. Returns a `SystemStatus` struct with:
- `bool sequenceActive`: Conversion sequence active
- `bool highSpeedI2C`: High-speed I2C mode
- `bool osrDone`: Oversampling complete
- `bool crcError`: CRC error detected
- `bool powerOnReset`: Power-on reset occurred
- `uint8_t rawValue`: Raw status register value

#### `bool isDeviceReady()`
Check if device is ready for operation.

#### `bool clearPowerOnResetFlag()`
Clear the power-on reset flag.

#### `bool softwareReset()`
Perform a software reset.

### Low-Level Register Access

#### `bool writeRegister(uint8_t reg, uint8_t value)`
Write to a register directly.

#### `uint8_t readRegister(uint8_t reg)`
Read from a register directly.

## Examples

### Example: Manual Analog Reading

See `examples/ManualReadings/ManualReadings.ino`

### Example: Autonomous Analog Reading

See `examples/ManualReadings/ManualReadings.ino`

### Example: Autonomous window comparator

See `examples/ManualReadings/ManualReadings.ino`

## I2C Addressing

The ADS7128 I2C address is determined by the ADDR pin:

| ADDR Pin | I2C Address |
|----------|-------------|
| Floating | 0x10        |
| AVDD/8   | 0x11        |
| 2×AVDD/8 | 0x12        |
| 3×AVDD/8 | 0x13        |
| 4×AVDD/8 | 0x14        |
| 5×AVDD/8 | 0x15        |
| 6×AVDD/8 | 0x16        |
| 7×AVDD/8 | 0x17        |


## Dependencies

- **Wire**: Arduino I2C library (included with Arduino core)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Resources

- [ADS7128 Datasheet](https://www.ti.com/lit/ds/symlink/ads7128.pdf?ts=1771665530977)
- [TI ADS7128 Product Page](https://www.ti.com/product/ADS7128)
- [GitHub Repository](https://github.com/ArscottT/ADS7128-Arduino-Library)
