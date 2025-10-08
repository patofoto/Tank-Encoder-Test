#include <Arduino.h>
#include "config.h"

// Teensy 4.1 has native USB Keyboard support - no additional library needed

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

// Map char to Teensy keycode (uses USE_ARROW_KEYS from config.h)
int mapCharToHid(char key) {
    if (key == 'a' || key == 'A') return 'a';
    if (key == 'd' || key == 'D') return 'd';
    if (key == 'w' || key == 'W') return 'w';
    if (key == 's' || key == 'S') return 's';
    if (key == 'L') return KEY_LEFT;
    if (key == 'R') return KEY_RIGHT;
    if (key == 'U') return KEY_UP;
    if (key == 'D') return KEY_DOWN;
    return 0;
}

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
void handleFireButton(int buttonPin, bool &lastState, unsigned long &lastChangeTime, bool &isPressed, const char* buttonName) {
    bool currentReading = digitalRead(buttonPin);
    unsigned long currentTime = millis();
    
    // Check if button state changed and debounce time has passed
    if (currentReading != lastState && (currentTime - lastChangeTime) >= FIRE_DEBOUNCE_MS) {
        lastState = currentReading;
        lastChangeTime = currentTime;
        
        // Button is pressed (LOW because of pull-up)
        if (currentReading == LOW && !isPressed) {
            isPressed = true;
            Keyboard.press(KEY_SPACE); // Fire button mapped to SPACE key
            Serial.print(buttonName);
            Serial.println(" FIRED!");
        }
        // Button is released (HIGH)
        else if (currentReading == HIGH && isPressed) {
            isPressed = false;
            Keyboard.release(KEY_SPACE);
            Serial.print(buttonName);
            Serial.println(" released");
        }
    }
}

// Function kept for possible tap actions (unused in hold logic)
void sendKeyPress(char key) {
    int keycode = mapCharToHid(key);
    if (keycode == 0) return;
    Keyboard.press(keycode);
    delay(30);
    Keyboard.release(keycode);
}

void setup(){
	// Initialize built-in LED to indicate power
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, HIGH); // Turn on LED to show device is powered and running
	
	// Initialize Serial (baud rate from config.h)
	Serial.begin(SERIAL_BAUD_RATE);
	delay(1000); // Give serial time to initialize
	
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
	
	Serial.println("Tank Encoder Controller Started (Teensy 4.1)");
	Serial.println("USB HID Keyboard Ready!");
	Serial.println("LED Power Indicator: ON");
	Serial.println("Horizontal Encoder: " + String(lastHorizontalCount));
	Serial.println("Vertical Encoder: " + String(lastVerticalCount));
	Serial.println("Fire Button 1: Pin " + String(FIRE_BUTTON_1_PIN));
	Serial.println("Fire Button 2: Pin " + String(FIRE_BUTTON_2_PIN));
    Serial.println("Control Mode: " + String(USE_ARROW_KEYS ? "Arrow Keys" : "WASD"));
	Serial.println("Gear Ratios - H: " + String(HORIZONTAL_GEAR_RATIO) + " | V: " + String(VERTICAL_GEAR_RATIO));
}

void loop(){
	unsigned long currentTime = millis();
	
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
		if (horizontalAccumulator >= HORIZONTAL_GEAR_RATIO) {
			// Determine direction
			char desiredChar = (deltaHorizontal > 0) ? (USE_ARROW_KEYS ? 'R' : 'd') : (USE_ARROW_KEYS ? 'L' : 'a');
			int desiredKeycode = mapCharToHid(desiredChar);
			
			// Calculate hold duration based on speed (faster = longer hold)
			horizontalHoldDuration = MIN_KEY_HOLD_MS + (unsigned long)(min(horizontalSpeed * SPEED_SENSITIVITY, 
				(float)(MAX_KEY_HOLD_MS - MIN_KEY_HOLD_MS)));
			
			// Press the key
			pressHorizontalKey(desiredKeycode);
			horizontalKeyPressTime = currentTime;
			
			// Reset accumulator (keep remainder for smooth feel)
			horizontalAccumulator -= HORIZONTAL_GEAR_RATIO;
			
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
		horizontalSpeed *= SPEED_DECAY;
		if (horizontalSpeed < 0.1f) horizontalSpeed = 0.0f;
	}
	
	// Check if horizontal key should be released (after hold duration)
	if (activeHorizontalKey != 0) {
		unsigned long keyHeldTime = currentTime - horizontalKeyPressTime;
		unsigned long idleTime = currentTime - lastHorizontalMoveMs;
		
		// Release if: 1) hold duration expired, OR 2) been idle too long
		if (keyHeldTime >= horizontalHoldDuration || idleTime >= HARD_IDLE_RELEASE_MS) {
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
		if (verticalAccumulator >= VERTICAL_GEAR_RATIO) {
			// Determine direction
			char desiredChar = (deltaVertical > 0) ? (USE_ARROW_KEYS ? 'U' : 'w') : (USE_ARROW_KEYS ? 'D' : 's');
			int desiredKeycode = mapCharToHid(desiredChar);
			
			// Calculate hold duration based on speed (faster = longer hold)
			verticalHoldDuration = MIN_KEY_HOLD_MS + (unsigned long)(min(verticalSpeed * SPEED_SENSITIVITY, 
				(float)(MAX_KEY_HOLD_MS - MIN_KEY_HOLD_MS)));
			
			// Press the key
			pressVerticalKey(desiredKeycode);
			verticalKeyPressTime = currentTime;
			
			// Reset accumulator (keep remainder for smooth feel)
			verticalAccumulator -= VERTICAL_GEAR_RATIO;
			
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
		verticalSpeed *= SPEED_DECAY;
		if (verticalSpeed < 0.1f) verticalSpeed = 0.0f;
	}
	
	// Check if vertical key should be released (after hold duration)
	if (activeVerticalKey != 0) {
		unsigned long keyHeldTime = currentTime - verticalKeyPressTime;
		unsigned long idleTime = currentTime - lastVerticalMoveMs;
		
		// Release if: 1) hold duration expired, OR 2) been idle too long
		if (keyHeldTime >= verticalHoldDuration || idleTime >= HARD_IDLE_RELEASE_MS) {
			releaseVerticalKey();
			verticalAccumulator = 0; // Reset accumulator on full stop
		}
	}

	// Handle fire buttons with debouncing
	handleFireButton(FIRE_BUTTON_1_PIN, fireButton1LastState, fireButton1LastChangeTime, fireButton1Pressed, "Fire Button 1");
	handleFireButton(FIRE_BUTTON_2_PIN, fireButton2LastState, fireButton2LastChangeTime, fireButton2Pressed, "Fire Button 2");

	// Print status for debugging (interval from config.h)
	static unsigned long lastPrint = 0;
	if (currentTime - lastPrint >= STATUS_PRINT_INTERVAL_MS) {
		Serial.println("===== STATUS =====");
		Serial.print("H Accum: ");
		Serial.print(horizontalAccumulator);
		Serial.print("/");
		Serial.print(HORIZONTAL_GEAR_RATIO);
		Serial.print(" | V Accum: ");
		Serial.print(verticalAccumulator);
		Serial.print("/");
		Serial.println(VERTICAL_GEAR_RATIO);
		
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