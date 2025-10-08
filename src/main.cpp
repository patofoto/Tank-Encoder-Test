#include <Arduino.h>

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

// Encoder objects
SimpleEncoder horizontalEncoder(1, 2);  // Pins 1 and 2 for horizontal encoder (A/D or Left/Right)
SimpleEncoder verticalEncoder(3, 4);    // Pins 3 and 4 for vertical encoder (W/S or Up/Down)

// Horizontal encoder (turret left/right movement) - A/D keys
int32_t lastHorizontalCount = 0;
const int32_t countsPerStep1 = 5; // dialed back: slightly less rotation per action

// Vertical encoder (turret up/down movement) - W/S keys
int32_t lastVerticalCount = 0;
const int32_t countsPerStep2 = 5; // dialed back: slightly less rotation per action

// Control mode: 0 = WASD keys, 1 = Arrow keys
bool useArrowKeys = false;

// Independent key tracking for each axis
int activeHorizontalKey = 0; // currently held horizontal key (0 = none)
int activeVerticalKey = 0;   // currently held vertical key (0 = none)
unsigned long lastHorizontalMoveMs = 0;
unsigned long lastVerticalMoveMs = 0;

// Independent velocity-smoothed tail hold for each axis
float horizontalFilteredSpeed = 0.0f; // low-pass of horizontal |delta|
float verticalFilteredSpeed = 0.0f;   // low-pass of vertical |delta|
const float speedAlpha = 0.20f; // smoother response to spikes
const unsigned long minTailHoldMs = 60;   // minimum hold after stop
const unsigned long maxTailHoldMs = 350;  // cap tail to avoid long sticks
const float tailHoldPerCountMs = 25.0f;   // less tail per speed unit
const unsigned long hardIdleReleaseMs = 500; // always release if idle this long

// Map char to Teensy keycode
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

// Function kept for possible tap actions (unused in hold logic)
void sendKeyPress(char key) {
    int keycode = mapCharToHid(key);
    if (keycode == 0) return;
    Keyboard.press(keycode);
    delay(30);
    Keyboard.release(keycode);
}

void setup(){
	// Initialize Serial
	Serial.begin(115200);
	delay(1000); // Give serial time to initialize
	
	// Initialize Teensy USB Keyboard
	Keyboard.begin();
	
	// Initialize encoders
	horizontalEncoder.begin();
	verticalEncoder.begin();
	
	// Initialize encoder counts
	lastHorizontalCount = horizontalEncoder.read();
	lastVerticalCount = verticalEncoder.read();
	
	Serial.println("Tank Encoder Controller Started (Teensy 4.1)");
	Serial.println("USB HID Keyboard Ready!");
	Serial.println("Horizontal Encoder: " + String(lastHorizontalCount));
	Serial.println("Vertical Encoder: " + String(lastVerticalCount));
    Serial.println("Control Mode: " + String(useArrowKeys ? "Arrow Keys" : "WASD"));
}

void loop(){
	// Read both encoders
	int32_t currentHorizontalCount = horizontalEncoder.read();
	int32_t deltaHorizontal = currentHorizontalCount - lastHorizontalCount;
	
	int32_t currentVerticalCount = verticalEncoder.read();
	int32_t deltaVertical = currentVerticalCount - lastVerticalCount;
	
	// ===== HORIZONTAL AXIS (Left/Right turret rotation) =====
	if (deltaHorizontal != 0) {
		// Horizontal encoder is moving
		char desiredChar;
		if (deltaHorizontal > 0) {
			desiredChar = useArrowKeys ? 'R' : 'd';
			Serial.println("Turning RIGHT");
		} else {
			desiredChar = useArrowKeys ? 'L' : 'a';
			Serial.println("Turning LEFT");
		}
		// Update horizontal velocity filter
		horizontalFilteredSpeed = (1.0f - speedAlpha) * horizontalFilteredSpeed + speedAlpha * abs(deltaHorizontal);
		int desiredKeycode = mapCharToHid(desiredChar);
		pressHorizontalKey(desiredKeycode);
		lastHorizontalMoveMs = millis();
		lastHorizontalCount = currentHorizontalCount;
	} else {
		// Horizontal encoder is idle - check if we should release the key
		horizontalFilteredSpeed *= 0.80f; // decay filter
		if (horizontalFilteredSpeed < 0.05f) horizontalFilteredSpeed = 0.0f;
		
		unsigned long horizontalDynamicReleaseMs = minTailHoldMs + (unsigned long)min(maxTailHoldMs - minTailHoldMs,
			(unsigned long)(horizontalFilteredSpeed * tailHoldPerCountMs));
		unsigned long horizontalIdleMs = millis() - lastHorizontalMoveMs;
		
		if (activeHorizontalKey != 0 && (horizontalIdleMs >= horizontalDynamicReleaseMs || horizontalIdleMs >= hardIdleReleaseMs)) {
			releaseHorizontalKey();
			Serial.println("Released HORIZONTAL");
		}
	}
	
	// ===== VERTICAL AXIS (Up/Down cannon elevation) =====
	if (deltaVertical != 0) {
		// Vertical encoder is moving
		char desiredChar;
		if (deltaVertical > 0) {
			desiredChar = useArrowKeys ? 'U' : 'w';
			Serial.println("Turning UP");
		} else {
			desiredChar = useArrowKeys ? 'D' : 's';
			Serial.println("Turning DOWN");
		}
		// Update vertical velocity filter
		verticalFilteredSpeed = (1.0f - speedAlpha) * verticalFilteredSpeed + speedAlpha * abs(deltaVertical);
		int desiredKeycode = mapCharToHid(desiredChar);
		pressVerticalKey(desiredKeycode);
		lastVerticalMoveMs = millis();
		lastVerticalCount = currentVerticalCount;
	} else {
		// Vertical encoder is idle - check if we should release the key
		verticalFilteredSpeed *= 0.80f; // decay filter
		if (verticalFilteredSpeed < 0.05f) verticalFilteredSpeed = 0.0f;
		
		unsigned long verticalDynamicReleaseMs = minTailHoldMs + (unsigned long)min(maxTailHoldMs - minTailHoldMs,
			(unsigned long)(verticalFilteredSpeed * tailHoldPerCountMs));
		unsigned long verticalIdleMs = millis() - lastVerticalMoveMs;
		
		if (activeVerticalKey != 0 && (verticalIdleMs >= verticalDynamicReleaseMs || verticalIdleMs >= hardIdleReleaseMs)) {
			releaseVerticalKey();
			Serial.println("Released VERTICAL");
		}
	}

	// Print encoder counts for debugging (less frequent)
	static unsigned long lastPrint = 0;
	if (millis() - lastPrint >= 2000) { // Print every 2 seconds
		Serial.print("H: " + String(currentHorizontalCount));
		Serial.print(" | V: " + String(currentVerticalCount));
		Serial.print(" | Active Keys: ");
		if (activeHorizontalKey) Serial.print((char)activeHorizontalKey);
		if (activeVerticalKey) Serial.print((char)activeVerticalKey);
		if (!activeHorizontalKey && !activeVerticalKey) Serial.print("none");
		Serial.println(" | Mode: " + String(useArrowKeys ? "Arrows" : "WASD"));
		lastPrint = millis();
	}

	delay(1); // faster loop to catch more encoder transitions
}