#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
// PIN CONFIGURATION
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
// CONTROL MODE
// =============================================================================
// false = WASD keys, true = Arrow keys
const bool USE_ARROW_KEYS = false;

// =============================================================================
// REALISTIC TANK GEARING SYSTEM
// =============================================================================
// Gear reduction ratios (higher = more encoder turns needed, more realistic)
// Real tanks: turret might be 20-30:1, cannon 15-20:1
const float HORIZONTAL_GEAR_RATIO = 15.0f;  // Turret rotation: ~15 encoder clicks per key action
const float VERTICAL_GEAR_RATIO = 12.0f;    // Cannon elevation: ~12 encoder clicks per key action

// =============================================================================
// SPEED SENSITIVITY
// =============================================================================
// Key hold duration based on turning speed
const unsigned long MIN_KEY_HOLD_MS = 30;   // Minimum tap for slow, precise movements
const unsigned long MAX_KEY_HOLD_MS = 200;  // Maximum hold for fast cranking
const float SPEED_SENSITIVITY = 3.0f;       // Multiplier: how much speed affects hold time (higher = more sensitive)

// =============================================================================
// MOMENTUM & INERTIA
// =============================================================================
// Speed decay when encoder stops (0.0-1.0, lower = faster decay)
const float SPEED_DECAY = 0.85f;  // 0.85 = realistic inertia, 0.5 = quick stop, 0.95 = lots of momentum

// =============================================================================
// RELEASE TIMING
// =============================================================================
// How long to wait before releasing keys when idle
const unsigned long MIN_TAIL_HOLD_MS = 50;   // Minimum hold after stopping
const unsigned long MAX_TAIL_HOLD_MS = 250;  // Maximum tail hold
const unsigned long HARD_IDLE_RELEASE_MS = 400; // Always release if idle this long (safety)

// =============================================================================
// FIRE BUTTONS
// =============================================================================
// Debounce time to prevent false triggers from mechanical bounce
const unsigned long FIRE_DEBOUNCE_MS = 50;  // 50ms is good for most buttons

// =============================================================================
// SERIAL DEBUG
// =============================================================================
// Baud rate for serial communication
const unsigned long SERIAL_BAUD_RATE = 115200;

// Status print interval (milliseconds)
const unsigned long STATUS_PRINT_INTERVAL_MS = 3000;  // Print status every 3 seconds

// =============================================================================
// TUNING PRESETS
// =============================================================================
// Uncomment ONE of these to quickly switch between profiles

// --- ARCADE MODE (fast, responsive) ---
// #define PRESET_ARCADE
#ifdef PRESET_ARCADE
    #undef HORIZONTAL_GEAR_RATIO
    #undef VERTICAL_GEAR_RATIO
    #define HORIZONTAL_GEAR_RATIO 5.0f
    #define VERTICAL_GEAR_RATIO 5.0f
#endif

// --- SIMULATION MODE (realistic, slow) ---
// #define PRESET_SIMULATION
#ifdef PRESET_SIMULATION
    #undef HORIZONTAL_GEAR_RATIO
    #undef VERTICAL_GEAR_RATIO
    #define HORIZONTAL_GEAR_RATIO 25.0f
    #define VERTICAL_GEAR_RATIO 20.0f
#endif

// --- SNIPER MODE (ultra-precise) ---
// #define PRESET_SNIPER
#ifdef PRESET_SNIPER
    #undef HORIZONTAL_GEAR_RATIO
    #undef VERTICAL_GEAR_RATIO
    #undef MIN_KEY_HOLD_MS
    #define HORIZONTAL_GEAR_RATIO 30.0f
    #define VERTICAL_GEAR_RATIO 25.0f
    #define MIN_KEY_HOLD_MS 20
#endif

#endif // CONFIG_H

