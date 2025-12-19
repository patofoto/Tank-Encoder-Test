#include <Arduino.h>
#include "config.h"

// Teensy 4.1 has native USB Keyboard support - no additional library needed

// =============================================================================
// RUNTIME SETTINGS (Global Instance)
// =============================================================================
Settings currentSettings;

// =============================================================================
// EEPROM MANAGEMENT FUNCTIONS
// =============================================================================

// Load settings from EEPROM
void loadSettingsFromEEPROM() {
    Settings loadedSettings;
    EEPROM.get(EEPROM_START_ADDRESS, loadedSettings);
    
    // Verify magic number and version
    if (loadedSettings.magicNumber == EEPROM_MAGIC_NUMBER && 
        loadedSettings.version == CONFIG_VERSION) {
        currentSettings = loadedSettings;
        Serial.println("✓ Settings loaded from EEPROM");
    } else {
        // Invalid or no data - use factory defaults
        resetToFactoryDefaults();
        Serial.println("⚠ No valid EEPROM data - using factory defaults");
    }
}

// Save current settings to EEPROM
void saveSettingsToEEPROM() {
    currentSettings.magicNumber = EEPROM_MAGIC_NUMBER;
    currentSettings.version = CONFIG_VERSION;
    EEPROM.put(EEPROM_START_ADDRESS, currentSettings);
    Serial.println("✓ Settings saved to EEPROM");
}

// Reset to factory defaults
void resetToFactoryDefaults() {
    currentSettings = FACTORY_DEFAULTS;
    Serial.println("✓ Reset to factory defaults");
}

// Load a preset configuration
void loadPreset(const Settings& preset) {
    // Copy preset but preserve debug settings
    unsigned long savedPrintInterval = currentSettings.statusPrintIntervalMs;
    currentSettings = preset;
    currentSettings.statusPrintIntervalMs = savedPrintInterval;
}

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

// Helper function to get key name for display
String getKeyName(uint16_t keyCode) {
    if (keyCode >= 'a' && keyCode <= 'z') return String((char)keyCode).toUpperCase();
    if (keyCode >= 'A' && keyCode <= 'Z') return String((char)keyCode);
    if (keyCode >= '0' && keyCode <= '9') return String((char)keyCode);
    if (keyCode == ' ') return "SPACE";
    if (keyCode == KEY_LEFT) return "LEFT";
    if (keyCode == KEY_RIGHT) return "RIGHT";
    if (keyCode == KEY_UP) return "UP";
    if (keyCode == KEY_DOWN) return "DOWN";
    if (keyCode == KEY_RETURN) return "ENTER";
    if (keyCode == KEY_TAB) return "TAB";
    if (keyCode == KEY_ESC) return "ESC";
    if (keyCode == KEY_LEFT_CTRL) return "LCTRL";
    if (keyCode == KEY_LEFT_SHIFT) return "LSHIFT";
    if (keyCode == KEY_LEFT_ALT) return "LALT";
    if (keyCode == KEY_RIGHT_CTRL) return "RCTRL";
    if (keyCode == KEY_RIGHT_SHIFT) return "RSHIFT";
    if (keyCode == KEY_RIGHT_ALT) return "RALT";
    return "KEY_" + String(keyCode);
}

// =============================================================================
// SERIAL COMMAND PARSER
// =============================================================================

String serialBuffer = "";

// Parse key name to key code
uint16_t parseKeyName(String keyName) {
    keyName.toUpperCase();
    keyName.trim();
    
    // Single character keys
    if (keyName.length() == 1) {
        char c = keyName.charAt(0);
        if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
            return (uint16_t)c;
        }
    }
    
    // Special keys
    if (keyName == "SPACE" || keyName == "SPC") return ' ';
    if (keyName == "LEFT" || keyName == "LEFTARROW") return KEY_LEFT;
    if (keyName == "RIGHT" || keyName == "RIGHTARROW") return KEY_RIGHT;
    if (keyName == "UP" || keyName == "UPARROW") return KEY_UP;
    if (keyName == "DOWN" || keyName == "DOWNARROW") return KEY_DOWN;
    if (keyName == "ENTER" || keyName == "RETURN") return KEY_RETURN;
    if (keyName == "TAB") return KEY_TAB;
    if (keyName == "ESC" || keyName == "ESCAPE") return KEY_ESC;
    if (keyName == "LCTRL") return KEY_LEFT_CTRL;
    if (keyName == "LSHIFT") return KEY_LEFT_SHIFT;
    if (keyName == "LALT") return KEY_LEFT_ALT;
    if (keyName == "RCTRL") return KEY_RIGHT_CTRL;
    if (keyName == "RSHIFT") return KEY_RIGHT_SHIFT;
    if (keyName == "RALT") return KEY_RIGHT_ALT;
    
    // Try to parse as number (key code)
    if (keyName.startsWith("KEY_")) {
        return (uint16_t)keyName.substring(4).toInt();
    }
    
    return 0; // Invalid
}

// Process serial commands
void processSerialCommand(String cmd) {
    cmd.trim();
    cmd.toLowerCase();
    
    if (cmd == "help") {
        Serial.println("\n=== Tank Controller Configuration ===");
        Serial.println("Commands:");
        Serial.println("  list                - Show all settings");
        Serial.println("  get <setting>       - Get specific setting");
        Serial.println("  set <setting> <val> - Set specific setting");
        Serial.println("  save                - Save to EEPROM");
        Serial.println("  load                - Reload from EEPROM");
        Serial.println("  reset               - Reset to factory defaults");
        Serial.println("  preset <name>       - Load preset (arcade/simulation/sniper)");
        Serial.println("\nKey Bindings:");
        Serial.println("  key_left, key_right, key_up, key_down, key_fire1, key_fire2");
        Serial.println("  Example: set key_left A");
        Serial.println("  Supported: A-Z, 0-9, SPACE, LEFT, RIGHT, UP, DOWN, etc.");
        Serial.println("\nOther Settings:");
        Serial.println("  gear_h, gear_v, speed_sense,");
        Serial.println("  min_hold, max_hold, decay, fire_debounce");
        Serial.println("\nJSON Format: {\"cmd\":\"get\",\"setting\":\"gear_h\"}\n");
    }
    else if (cmd == "list") {
        Serial.println("\n=== Current Settings ===");
        Serial.println("--- Key Bindings ---");
        Serial.print("key_left: "); Serial.println(getKeyName(currentSettings.keyHorizontalLeft));
        Serial.print("key_right: "); Serial.println(getKeyName(currentSettings.keyHorizontalRight));
        Serial.print("key_up: "); Serial.println(getKeyName(currentSettings.keyVerticalUp));
        Serial.print("key_down: "); Serial.println(getKeyName(currentSettings.keyVerticalDown));
        Serial.print("key_fire1: "); Serial.println(getKeyName(currentSettings.keyFire1));
        Serial.print("key_fire2: "); Serial.println(getKeyName(currentSettings.keyFire2));
        Serial.println("--- Parameters ---");
        Serial.print("gear_h: "); Serial.println(currentSettings.horizontalGearRatio, 2);
        Serial.print("gear_v: "); Serial.println(currentSettings.verticalGearRatio, 2);
        Serial.print("speed_sense: "); Serial.println(currentSettings.speedSensitivity, 2);
        Serial.print("min_hold: "); Serial.println(currentSettings.minKeyHoldMs);
        Serial.print("max_hold: "); Serial.println(currentSettings.maxKeyHoldMs);
        Serial.print("decay: "); Serial.println(currentSettings.speedDecay, 3);
        Serial.print("min_tail: "); Serial.println(currentSettings.minTailHoldMs);
        Serial.print("max_tail: "); Serial.println(currentSettings.maxTailHoldMs);
        Serial.print("idle_release: "); Serial.println(currentSettings.hardIdleReleaseMs);
        Serial.print("fire_debounce: "); Serial.println(currentSettings.fireDebounceMs);
        Serial.println("=======================\n");
    }
    else if (cmd.startsWith("get ")) {
        String setting = cmd.substring(4);
        setting.trim();
        
        // Output in JSON format for web UI
        Serial.print("{\"setting\":\""); Serial.print(setting); Serial.print("\",\"value\":");
        
        if (setting == "key_left") { Serial.print(currentSettings.keyHorizontalLeft); Serial.print(",\"name\":\""); Serial.print(getKeyName(currentSettings.keyHorizontalLeft)); Serial.println("\"}"); }
        else if (setting == "key_right") { Serial.print(currentSettings.keyHorizontalRight); Serial.print(",\"name\":\""); Serial.print(getKeyName(currentSettings.keyHorizontalRight)); Serial.println("\"}"); }
        else if (setting == "key_up") { Serial.print(currentSettings.keyVerticalUp); Serial.print(",\"name\":\""); Serial.print(getKeyName(currentSettings.keyVerticalUp)); Serial.println("\"}"); }
        else if (setting == "key_down") { Serial.print(currentSettings.keyVerticalDown); Serial.print(",\"name\":\""); Serial.print(getKeyName(currentSettings.keyVerticalDown)); Serial.println("\"}"); }
        else if (setting == "key_fire1") { Serial.print(currentSettings.keyFire1); Serial.print(",\"name\":\""); Serial.print(getKeyName(currentSettings.keyFire1)); Serial.println("\"}"); }
        else if (setting == "key_fire2") { Serial.print(currentSettings.keyFire2); Serial.print(",\"name\":\""); Serial.print(getKeyName(currentSettings.keyFire2)); Serial.println("\"}"); }
        else if (setting == "gear_h") { Serial.print(currentSettings.horizontalGearRatio, 2); Serial.println("}"); }
        else if (setting == "gear_v") { Serial.print(currentSettings.verticalGearRatio, 2); Serial.println("}"); }
        else if (setting == "speed_sense") { Serial.print(currentSettings.speedSensitivity, 2); Serial.println("}"); }
        else if (setting == "min_hold") { Serial.print(currentSettings.minKeyHoldMs); Serial.println("}"); }
        else if (setting == "max_hold") { Serial.print(currentSettings.maxKeyHoldMs); Serial.println("}"); }
        else if (setting == "decay") { Serial.print(currentSettings.speedDecay, 3); Serial.println("}"); }
        else if (setting == "min_tail") { Serial.print(currentSettings.minTailHoldMs); Serial.println("}"); }
        else if (setting == "max_tail") { Serial.print(currentSettings.maxTailHoldMs); Serial.println("}"); }
        else if (setting == "idle_release") { Serial.print(currentSettings.hardIdleReleaseMs); Serial.println("}"); }
        else if (setting == "fire_debounce") { Serial.print(currentSettings.fireDebounceMs); Serial.println("}"); }
        else Serial.println("null}");
    }
    else if (cmd.startsWith("set ")) {
        int firstSpace = cmd.indexOf(' ');
        int secondSpace = cmd.indexOf(' ', firstSpace + 1);
        
        if (secondSpace > 0) {
            String setting = cmd.substring(firstSpace + 1, secondSpace);
            String value = cmd.substring(secondSpace + 1);
            setting.trim();
            value.trim();
            
            bool updated = false;
            
            // Key binding settings
            if (setting == "key_left") {
                uint16_t keyCode = parseKeyName(value);
                if (keyCode != 0) {
                    currentSettings.keyHorizontalLeft = keyCode;
                    Serial.print("✓ key_left = "); Serial.print(getKeyName(keyCode)); Serial.print(" ("); Serial.print(keyCode); Serial.println(")");
                    updated = true;
                } else {
                    Serial.println("✗ Invalid key name");
                }
            }
            else if (setting == "key_right") {
                uint16_t keyCode = parseKeyName(value);
                if (keyCode != 0) {
                    currentSettings.keyHorizontalRight = keyCode;
                    Serial.print("✓ key_right = "); Serial.print(getKeyName(keyCode)); Serial.print(" ("); Serial.print(keyCode); Serial.println(")");
                    updated = true;
                } else {
                    Serial.println("✗ Invalid key name");
                }
            }
            else if (setting == "key_up") {
                uint16_t keyCode = parseKeyName(value);
                if (keyCode != 0) {
                    currentSettings.keyVerticalUp = keyCode;
                    Serial.print("✓ key_up = "); Serial.print(getKeyName(keyCode)); Serial.print(" ("); Serial.print(keyCode); Serial.println(")");
                    updated = true;
                } else {
                    Serial.println("✗ Invalid key name");
                }
            }
            else if (setting == "key_down") {
                uint16_t keyCode = parseKeyName(value);
                if (keyCode != 0) {
                    currentSettings.keyVerticalDown = keyCode;
                    Serial.print("✓ key_down = "); Serial.print(getKeyName(keyCode)); Serial.print(" ("); Serial.print(keyCode); Serial.println(")");
                    updated = true;
                } else {
                    Serial.println("✗ Invalid key name");
                }
            }
            else if (setting == "key_fire1") {
                uint16_t keyCode = parseKeyName(value);
                if (keyCode != 0) {
                    currentSettings.keyFire1 = keyCode;
                    Serial.print("✓ key_fire1 = "); Serial.print(getKeyName(keyCode)); Serial.print(" ("); Serial.print(keyCode); Serial.println(")");
                    updated = true;
                } else {
                    Serial.println("✗ Invalid key name");
                }
            }
            else if (setting == "key_fire2") {
                uint16_t keyCode = parseKeyName(value);
                if (keyCode != 0) {
                    currentSettings.keyFire2 = keyCode;
                    Serial.print("✓ key_fire2 = "); Serial.print(getKeyName(keyCode)); Serial.print(" ("); Serial.print(keyCode); Serial.println(")");
                    updated = true;
                } else {
                    Serial.println("✗ Invalid key name");
                }
            }
            // Numeric settings
            else if (setting == "gear_h") {
                currentSettings.horizontalGearRatio = constrain(value.toFloat(), 5.0f, 30.0f);
                Serial.print("✓ "); Serial.print(setting); Serial.print(" = "); Serial.println(value);
                updated = true;
            }
            else if (setting == "gear_v") {
                currentSettings.verticalGearRatio = constrain(value.toFloat(), 5.0f, 30.0f);
                Serial.print("✓ "); Serial.print(setting); Serial.print(" = "); Serial.println(value);
                updated = true;
            }
            else if (setting == "speed_sense") {
                currentSettings.speedSensitivity = constrain(value.toFloat(), 1.0f, 10.0f);
                Serial.print("✓ "); Serial.print(setting); Serial.print(" = "); Serial.println(value);
                updated = true;
            }
            else if (setting == "min_hold") {
                currentSettings.minKeyHoldMs = constrain(value.toInt(), 10, 500);
                Serial.print("✓ "); Serial.print(setting); Serial.print(" = "); Serial.println(value);
                updated = true;
            }
            else if (setting == "max_hold") {
                currentSettings.maxKeyHoldMs = constrain(value.toInt(), 10, 500);
                Serial.print("✓ "); Serial.print(setting); Serial.print(" = "); Serial.println(value);
                updated = true;
            }
            else if (setting == "decay") {
                currentSettings.speedDecay = constrain(value.toFloat(), 0.0f, 1.0f);
                Serial.print("✓ "); Serial.print(setting); Serial.print(" = "); Serial.println(value);
                updated = true;
            }
            else if (setting == "min_tail") {
                currentSettings.minTailHoldMs = constrain(value.toInt(), 10, 1000);
                Serial.print("✓ "); Serial.print(setting); Serial.print(" = "); Serial.println(value);
                updated = true;
            }
            else if (setting == "max_tail") {
                currentSettings.maxTailHoldMs = constrain(value.toInt(), 10, 1000);
                Serial.print("✓ "); Serial.print(setting); Serial.print(" = "); Serial.println(value);
                updated = true;
            }
            else if (setting == "idle_release") {
                currentSettings.hardIdleReleaseMs = constrain(value.toInt(), 100, 2000);
                Serial.print("✓ "); Serial.print(setting); Serial.print(" = "); Serial.println(value);
                updated = true;
            }
            else if (setting == "fire_debounce") {
                currentSettings.fireDebounceMs = constrain(value.toInt(), 10, 200);
                Serial.print("✓ "); Serial.print(setting); Serial.print(" = "); Serial.println(value);
                updated = true;
            }
            
            if (!updated) {
                Serial.println("✗ Unknown setting");
            }
        }
    }
    else if (cmd == "save") {
        saveSettingsToEEPROM();
    }
    else if (cmd == "load") {
        loadSettingsFromEEPROM();
    }
    else if (cmd == "reset") {
        resetToFactoryDefaults();
    }
    else if (cmd.startsWith("preset ")) {
        String presetName = cmd.substring(7);
        presetName.trim();
        
        if (presetName == "arcade") {
            loadPreset(PRESET_ARCADE);
            Serial.println("✓ Loaded ARCADE preset");
        } else if (presetName == "simulation") {
            loadPreset(PRESET_SIMULATION);
            Serial.println("✓ Loaded SIMULATION preset");
        } else if (presetName == "sniper") {
            loadPreset(PRESET_SNIPER);
            Serial.println("✓ Loaded SNIPER preset");
        } else if (presetName == "default") {
            resetToFactoryDefaults();
            Serial.println("✓ Loaded DEFAULT preset");
        } else {
            Serial.println("✗ Unknown preset (arcade/simulation/sniper/default)");
        }
    }
    else if (cmd.length() > 0) {
        Serial.println("✗ Unknown command (type 'help')");
    }
}

// Check for serial input
void handleSerialCommands() {
    while (Serial.available()) {
        char c = Serial.read();
        
        if (c == '\n' || c == '\r') {
            if (serialBuffer.length() > 0) {
                processSerialCommand(serialBuffer);
                serialBuffer = "";
            }
        } else {
            serialBuffer += c;
        }
    }
}

// Improved encoder class with better debouncing
class SimpleEncoder {
private:
    int pinA, pinB;
    volatile long position;
    volatile int lastEncoded;
    volatile long lastMillis;
    volatile long lastChangeTime;
    static const long DEBOUNCE_TIME = 1; // faster response for quick spins
    
public:
    SimpleEncoder(int pinA, int pinB) : pinA(pinA), pinB(pinB), position(0), lastEncoded(0), lastMillis(0), lastChangeTime(0) {}
    
    void begin() {
        pinMode(pinA, INPUT_PULLUP);
        pinMode(pinB, INPUT_PULLUP);
        lastEncoded = (digitalRead(pinB) << 1) | digitalRead(pinA);
        lastChangeTime = millis();
    }
    
    long read() {
        long currentTime = millis();
        
        // Debounce check
        if (currentTime - lastChangeTime < DEBOUNCE_TIME) {
            return position;
        }
        
        int MSB = digitalRead(pinA);
        int LSB = digitalRead(pinB);
        int encoded = (MSB << 1) | LSB;
        int sum = (lastEncoded << 2) | encoded;
        
        // Only process if we have a valid state change
        if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
            position++;
            lastChangeTime = currentTime;
        }
        if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
            position--;
            lastChangeTime = currentTime;
        }
        
        lastEncoded = encoded;
        return position;
    }
    
    void write(long p) {
        position = p;
    }
};

// Encoder objects (using pin configuration from config.h)
SimpleEncoder horizontalEncoder(HORIZONTAL_ENCODER_PIN_A, HORIZONTAL_ENCODER_PIN_B);
SimpleEncoder verticalEncoder(VERTICAL_ENCODER_PIN_A, VERTICAL_ENCODER_PIN_B);

// Encoder position tracking
int32_t lastHorizontalCount = 0;
int32_t lastVerticalCount = 0;

// Accumulators for gear reduction
float horizontalAccumulator = 0.0f;
float verticalAccumulator = 0.0f;

// Independent key tracking for each axis
int activeHorizontalKey = 0; // currently held horizontal key (0 = none)
int activeVerticalKey = 0;   // currently held vertical key (0 = none)
unsigned long lastHorizontalMoveMs = 0;
unsigned long lastVerticalMoveMs = 0;
unsigned long horizontalKeyPressTime = 0;   // When key was pressed
unsigned long verticalKeyPressTime = 0;     // When key was pressed
unsigned long horizontalHoldDuration = 0;   // How long to hold key
unsigned long verticalHoldDuration = 0;     // How long to hold key

// Velocity tracking for realistic feel
float horizontalSpeed = 0.0f;  // Current turning speed
float verticalSpeed = 0.0f;    // Current elevation speed

// Fire button state tracking
bool fireButton1LastState = HIGH;  // Buttons are active LOW (pulled up)
bool fireButton2LastState = HIGH;
unsigned long fireButton1LastChangeTime = 0;
unsigned long fireButton2LastChangeTime = 0;
bool fireButton1Pressed = false;  // Track if button is currently pressed
bool fireButton2Pressed = false;

// Press and hold a horizontal key if not already active
void pressHorizontalKey(int keycode) {
    if (keycode == 0) return;
    if (activeHorizontalKey == keycode) return; // already holding
    // release previous horizontal key if any
    if (activeHorizontalKey != 0) {
        Keyboard.release(activeHorizontalKey);
    }
    Keyboard.press(keycode);
    activeHorizontalKey = keycode;
}

// Press and hold a vertical key if not already active
void pressVerticalKey(int keycode) {
    if (keycode == 0) return;
    if (activeVerticalKey == keycode) return; // already holding
    // release previous vertical key if any
    if (activeVerticalKey != 0) {
        Keyboard.release(activeVerticalKey);
    }
    Keyboard.press(keycode);
    activeVerticalKey = keycode;
}

// Release horizontal key
void releaseHorizontalKey() {
    if (activeHorizontalKey != 0) {
        Keyboard.release(activeHorizontalKey);
        activeHorizontalKey = 0;
    }
}

// Release vertical key
void releaseVerticalKey() {
    if (activeVerticalKey != 0) {
        Keyboard.release(activeVerticalKey);
        activeVerticalKey = 0;
    }
}

// Handle fire button with debouncing
void handleFireButton(int buttonPin, bool &lastState, unsigned long &lastChangeTime, bool &isPressed, uint16_t keyCode, const char* buttonName) {
    bool currentReading = digitalRead(buttonPin);
    unsigned long currentTime = millis();
    
    // Check if button state changed and debounce time has passed
    if (currentReading != lastState && (currentTime - lastChangeTime) >= currentSettings.fireDebounceMs) {
        lastState = currentReading;
        lastChangeTime = currentTime;
        
        // Button is pressed (LOW because of pull-up)
        if (currentReading == LOW && !isPressed) {
            isPressed = true;
            Keyboard.press(keyCode); // Fire button mapped to configured key
            Serial.print(buttonName);
            Serial.print(" FIRED (");
            Serial.print(getKeyName(keyCode));
            Serial.println(")!");
        }
        // Button is released (HIGH)
        else if (currentReading == HIGH && isPressed) {
            isPressed = false;
            Keyboard.release(keyCode);
            Serial.print(buttonName);
            Serial.println(" released");
        }
    }
}


void setup(){
	// Initialize built-in LED to indicate power
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, HIGH); // Turn on LED to show device is powered and running
	
	// Initialize Serial (baud rate from config.h)
	Serial.begin(SERIAL_BAUD_RATE);
	delay(1000); // Give serial time to initialize
	
	// Load settings from EEPROM or use defaults
	loadSettingsFromEEPROM();
	
	// Initialize Teensy USB Keyboard
	Keyboard.begin();
	
	// Initialize encoders
	horizontalEncoder.begin();
	verticalEncoder.begin();
	
	// Initialize fire buttons with internal pull-up resistors
	pinMode(FIRE_BUTTON_1_PIN, INPUT_PULLUP);
	pinMode(FIRE_BUTTON_2_PIN, INPUT_PULLUP);
	
	// Initialize encoder counts
	lastHorizontalCount = horizontalEncoder.read();
	lastVerticalCount = verticalEncoder.read();
	
	Serial.println("\n╔════════════════════════════════════════╗");
	Serial.println("║  Tank Encoder Controller (Teensy 4.1) ║");
	Serial.println("╚════════════════════════════════════════╝");
	Serial.print("Firmware Version: ");
	Serial.println(FIRMWARE_VERSION);
	Serial.println("✓ USB HID Keyboard Ready");
	Serial.println("✓ LED Power Indicator: ON");
	Serial.println("✓ Encoders Initialized");
	Serial.println("✓ Fire Buttons Ready");
	Serial.println("\n--- Current Configuration ---");
	Serial.print("Key Bindings - Left: "); Serial.print(getKeyName(currentSettings.keyHorizontalLeft));
	Serial.print(" | Right: "); Serial.print(getKeyName(currentSettings.keyHorizontalRight));
	Serial.print(" | Up: "); Serial.print(getKeyName(currentSettings.keyVerticalUp));
	Serial.print(" | Down: "); Serial.print(getKeyName(currentSettings.keyVerticalDown));
	Serial.print(" | Fire: "); Serial.println(getKeyName(currentSettings.keyFire1));
	Serial.print("Gear Ratios - H: "); Serial.print(currentSettings.horizontalGearRatio);
	Serial.print(" | V: "); Serial.println(currentSettings.verticalGearRatio);
	Serial.print("Speed Sensitivity: "); Serial.println(currentSettings.speedSensitivity);
	Serial.print("Speed Decay: "); Serial.println(currentSettings.speedDecay);
	Serial.println("\nType 'help' for configuration commands\n");
}

void loop(){
	unsigned long currentTime = millis();
	
	// Handle serial configuration commands
	handleSerialCommands();
	
	// Read both encoders
	int32_t currentHorizontalCount = horizontalEncoder.read();
	int32_t deltaHorizontal = currentHorizontalCount - lastHorizontalCount;
	
	int32_t currentVerticalCount = verticalEncoder.read();
	int32_t deltaVertical = currentVerticalCount - lastVerticalCount;
	
	// ===== HORIZONTAL AXIS (Left/Right turret rotation) =====
	if (deltaHorizontal != 0) {
		// Add to accumulator with gear reduction
		horizontalAccumulator += abs(deltaHorizontal);
		
		// Update speed (for momentum feel)
		horizontalSpeed = abs(deltaHorizontal);
		
		// Check if we've accumulated enough movement to trigger an action
		if (horizontalAccumulator >= currentSettings.horizontalGearRatio) {
			// Determine direction using configured key bindings
			int desiredKeycode = (deltaHorizontal > 0) ? currentSettings.keyHorizontalRight : currentSettings.keyHorizontalLeft;
			
			// Calculate hold duration based on speed (faster = longer hold)
			horizontalHoldDuration = currentSettings.minKeyHoldMs + (unsigned long)(min(horizontalSpeed * currentSettings.speedSensitivity, 
				(float)(currentSettings.maxKeyHoldMs - currentSettings.minKeyHoldMs)));
			
			// Press the key
			pressHorizontalKey(desiredKeycode);
			horizontalKeyPressTime = currentTime;
			
			// Reset accumulator (keep remainder for smooth feel)
			horizontalAccumulator -= currentSettings.horizontalGearRatio;
			
			Serial.print("HORIZONTAL: ");
			Serial.print(deltaHorizontal > 0 ? "RIGHT" : "LEFT");
			Serial.print(" | Speed: ");
			Serial.print(horizontalSpeed);
			Serial.print(" | Hold: ");
			Serial.println(horizontalHoldDuration);
		}
		
		lastHorizontalMoveMs = currentTime;
		lastHorizontalCount = currentHorizontalCount;
	} else {
		// Decay speed when idle
		horizontalSpeed *= currentSettings.speedDecay;
		if (horizontalSpeed < 0.1f) horizontalSpeed = 0.0f;
	}
	
	// Check if horizontal key should be released (after hold duration)
	if (activeHorizontalKey != 0) {
		unsigned long keyHeldTime = currentTime - horizontalKeyPressTime;
		unsigned long idleTime = currentTime - lastHorizontalMoveMs;
		
		// Release if: 1) hold duration expired, OR 2) been idle too long
		if (keyHeldTime >= horizontalHoldDuration || idleTime >= currentSettings.hardIdleReleaseMs) {
			releaseHorizontalKey();
			horizontalAccumulator = 0; // Reset accumulator on full stop
		}
	}
	
	// ===== VERTICAL AXIS (Up/Down cannon elevation) =====
	if (deltaVertical != 0) {
		// Add to accumulator with gear reduction
		verticalAccumulator += abs(deltaVertical);
		
		// Update speed (for momentum feel)
		verticalSpeed = abs(deltaVertical);
		
		// Check if we've accumulated enough movement to trigger an action
		if (verticalAccumulator >= currentSettings.verticalGearRatio) {
			// Determine direction using configured key bindings
			int desiredKeycode = (deltaVertical > 0) ? currentSettings.keyVerticalUp : currentSettings.keyVerticalDown;
			
			// Calculate hold duration based on speed (faster = longer hold)
			verticalHoldDuration = currentSettings.minKeyHoldMs + (unsigned long)(min(verticalSpeed * currentSettings.speedSensitivity, 
				(float)(currentSettings.maxKeyHoldMs - currentSettings.minKeyHoldMs)));
			
			// Press the key
			pressVerticalKey(desiredKeycode);
			verticalKeyPressTime = currentTime;
			
			// Reset accumulator (keep remainder for smooth feel)
			verticalAccumulator -= currentSettings.verticalGearRatio;
			
			Serial.print("VERTICAL: ");
			Serial.print(deltaVertical > 0 ? "UP" : "DOWN");
			Serial.print(" | Speed: ");
			Serial.print(verticalSpeed);
			Serial.print(" | Hold: ");
			Serial.println(verticalHoldDuration);
		}
		
		lastVerticalMoveMs = currentTime;
		lastVerticalCount = currentVerticalCount;
	} else {
		// Decay speed when idle
		verticalSpeed *= currentSettings.speedDecay;
		if (verticalSpeed < 0.1f) verticalSpeed = 0.0f;
	}
	
	// Check if vertical key should be released (after hold duration)
	if (activeVerticalKey != 0) {
		unsigned long keyHeldTime = currentTime - verticalKeyPressTime;
		unsigned long idleTime = currentTime - lastVerticalMoveMs;
		
		// Release if: 1) hold duration expired, OR 2) been idle too long
		if (keyHeldTime >= verticalHoldDuration || idleTime >= currentSettings.hardIdleReleaseMs) {
			releaseVerticalKey();
			verticalAccumulator = 0; // Reset accumulator on full stop
		}
	}

	// Handle fire buttons with debouncing (pass configured key codes)
	handleFireButton(FIRE_BUTTON_1_PIN, fireButton1LastState, fireButton1LastChangeTime, fireButton1Pressed, currentSettings.keyFire1, "Fire Button 1");
	handleFireButton(FIRE_BUTTON_2_PIN, fireButton2LastState, fireButton2LastChangeTime, fireButton2Pressed, currentSettings.keyFire2, "Fire Button 2");

	// Print status for debugging (interval from currentSettings)
	static unsigned long lastPrint = 0;
	if (currentTime - lastPrint >= currentSettings.statusPrintIntervalMs) {
		Serial.println("===== STATUS =====");
		Serial.print("H Accum: ");
		Serial.print(horizontalAccumulator);
		Serial.print("/");
		Serial.print(currentSettings.horizontalGearRatio);
		Serial.print(" | V Accum: ");
		Serial.print(verticalAccumulator);
		Serial.print("/");
		Serial.println(currentSettings.verticalGearRatio);
		
		Serial.print("Active Keys: ");
		if (activeHorizontalKey) Serial.print((char)activeHorizontalKey);
		if (activeVerticalKey) Serial.print((char)activeVerticalKey);
		if (fireButton1Pressed || fireButton2Pressed) Serial.print(" [FIRE]");
		if (!activeHorizontalKey && !activeVerticalKey && !fireButton1Pressed && !fireButton2Pressed) Serial.print("none");
		Serial.println();
		Serial.println("==================");
		lastPrint = currentTime;
	}

	delay(1); // Fast loop to catch encoder transitions
}