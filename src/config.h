#ifndef CONFIG_H
#define CONFIG_H

#include <EEPROM.h>

// =============================================================================
// PIN CONFIGURATION (Hardware - Not Adjustable)
// =============================================================================
// Encoder pins
const int HORIZONTAL_ENCODER_PIN_A = 1;
const int HORIZONTAL_ENCODER_PIN_B = 2;
const int VERTICAL_ENCODER_PIN_A = 3;
const int VERTICAL_ENCODER_PIN_B = 4;

// Fire button pins
const int FIRE_BUTTON_1_PIN = 5;
const int FIRE_BUTTON_2_PIN = 6;

// LED pin
const int LED_PIN = 13;

// =============================================================================
// EEPROM CONFIGURATION
// =============================================================================
#define EEPROM_MAGIC_NUMBER 0xABCD1234  // Used to verify valid EEPROM data
#define EEPROM_START_ADDRESS 0          // Start address in EEPROM
#define CONFIG_VERSION 1                // Increment when struct changes

// =============================================================================
// RUNTIME ADJUSTABLE SETTINGS
// =============================================================================
// These values can be adjusted via Web UI or serial commands
struct Settings {
    uint32_t magicNumber;              // Verify valid EEPROM data
    uint8_t version;                   // Config version
    
    // Key Bindings (USB HID Key Codes)
    uint16_t keyHorizontalLeft;        // Left/CCW turret rotation key
    uint16_t keyHorizontalRight;       // Right/CW turret rotation key
    uint16_t keyVerticalUp;            // Up/elevation key
    uint16_t keyVerticalDown;          // Down/depression key
    uint16_t keyFire1;                 // Fire button 1 key
    uint16_t keyFire2;                 // Fire button 2 key
    
    // Gear Ratios (5.0 to 30.0)
    float horizontalGearRatio;         // Turret rotation gear reduction
    float verticalGearRatio;           // Cannon elevation gear reduction
    
    // Speed Sensitivity (1.0 to 10.0)
    float speedSensitivity;            // How much speed affects hold time
    
    // Key Hold Timing (10 to 500 ms)
    unsigned long minKeyHoldMs;        // Minimum tap for slow movements
    unsigned long maxKeyHoldMs;        // Maximum hold for fast cranking
    
    // Momentum & Inertia (0.0 to 1.0)
    float speedDecay;                  // Speed decay when encoder stops
    
    // Release Timing (10 to 1000 ms)
    unsigned long minTailHoldMs;       // Minimum hold after stopping
    unsigned long maxTailHoldMs;       // Maximum tail hold
    unsigned long hardIdleReleaseMs;   // Always release if idle this long
    
    // Fire Button Debounce (10 to 200 ms)
    unsigned long fireDebounceMs;      // Debounce time for fire buttons
    
    // Debug Settings
    unsigned long statusPrintIntervalMs; // Status print interval
};

// Factory default settings (WASD + Space)
const Settings FACTORY_DEFAULTS = {
    .magicNumber = EEPROM_MAGIC_NUMBER,
    .version = CONFIG_VERSION,
    .keyHorizontalLeft = 'a',           // A key
    .keyHorizontalRight = 'd',          // D key
    .keyVerticalUp = 'w',               // W key
    .keyVerticalDown = 's',             // S key
    .keyFire1 = ' ',                    // Space key
    .keyFire2 = ' ',                    // Space key (both buttons = same key)
    .horizontalGearRatio = 15.0f,       // Balanced
    .verticalGearRatio = 12.0f,         // Balanced
    .speedSensitivity = 3.0f,           // Moderate sensitivity
    .minKeyHoldMs = 30,                 // Quick taps
    .maxKeyHoldMs = 200,                // Reasonable max
    .speedDecay = 0.85f,                // Realistic inertia
    .minTailHoldMs = 50,                // Smooth releases
    .maxTailHoldMs = 250,               // Natural feel
    .hardIdleReleaseMs = 400,           // Safety timeout
    .fireDebounceMs = 50,               // Standard debounce
    .statusPrintIntervalMs = 3000       // 3 second intervals
};

// Preset configurations
const Settings PRESET_ARCADE = {
    .magicNumber = EEPROM_MAGIC_NUMBER,
    .version = CONFIG_VERSION,
    .keyHorizontalLeft = 'a',
    .keyHorizontalRight = 'd',
    .keyVerticalUp = 'w',
    .keyVerticalDown = 's',
    .keyFire1 = ' ',
    .keyFire2 = ' ',
    .horizontalGearRatio = 5.0f,        // Fast, responsive
    .verticalGearRatio = 5.0f,          // Fast, responsive
    .speedSensitivity = 2.0f,           // Less speed influence
    .minKeyHoldMs = 20,
    .maxKeyHoldMs = 150,
    .speedDecay = 0.70f,                // Quick stop
    .minTailHoldMs = 30,
    .maxTailHoldMs = 150,
    .hardIdleReleaseMs = 300,
    .fireDebounceMs = 50,
    .statusPrintIntervalMs = 3000
};

const Settings PRESET_SIMULATION = {
    .magicNumber = EEPROM_MAGIC_NUMBER,
    .version = CONFIG_VERSION,
    .keyHorizontalLeft = 'a',
    .keyHorizontalRight = 'd',
    .keyVerticalUp = 'w',
    .keyVerticalDown = 's',
    .keyFire1 = ' ',
    .keyFire2 = ' ',
    .horizontalGearRatio = 25.0f,       // Slow, realistic
    .verticalGearRatio = 20.0f,         // Slow, realistic
    .speedSensitivity = 4.0f,           // More speed influence
    .minKeyHoldMs = 40,
    .maxKeyHoldMs = 250,
    .speedDecay = 0.92f,                // High inertia
    .minTailHoldMs = 80,
    .maxTailHoldMs = 350,
    .hardIdleReleaseMs = 500,
    .fireDebounceMs = 50,
    .statusPrintIntervalMs = 3000
};

const Settings PRESET_SNIPER = {
    .magicNumber = EEPROM_MAGIC_NUMBER,
    .version = CONFIG_VERSION,
    .keyHorizontalLeft = 'a',
    .keyHorizontalRight = 'd',
    .keyVerticalUp = 'w',
    .keyVerticalDown = 's',
    .keyFire1 = ' ',
    .keyFire2 = ' ',
    .horizontalGearRatio = 30.0f,       // Ultra precise
    .verticalGearRatio = 25.0f,         // Ultra precise
    .speedSensitivity = 5.0f,           // High sensitivity
    .minKeyHoldMs = 20,                 // Very short taps
    .maxKeyHoldMs = 180,
    .speedDecay = 0.80f,                // Quick damping
    .minTailHoldMs = 40,
    .maxTailHoldMs = 200,
    .hardIdleReleaseMs = 350,
    .fireDebounceMs = 50,
    .statusPrintIntervalMs = 3000
};

// Global runtime settings (loaded from EEPROM or defaults)
extern Settings currentSettings;

// =============================================================================
// SERIAL CONFIGURATION
// =============================================================================
const unsigned long SERIAL_BAUD_RATE = 115200;

// =============================================================================
// EEPROM FUNCTIONS
// =============================================================================
void loadSettingsFromEEPROM();
void saveSettingsToEEPROM();
void resetToFactoryDefaults();
void loadPreset(const Settings& preset);

#endif // CONFIG_H

