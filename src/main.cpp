#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

// USB HID Keyboard
Adafruit_USBD_HID usb_hid;
uint8_t const desc_hid_report[] = { TUD_HID_REPORT_DESC_KEYBOARD() };

// Keyboard object
Adafruit_USBD_HID* keyboard_hid;

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
SimpleEncoder encoder1(2, 3);  // Pins 2 and 3 for first encoder

// Encoder 1 (horizontal turret movement) - A/D keys
int32_t lastEncoder1Count = 0;
const int32_t countsPerStep1 = 5; // dialed back: slightly less rotation per action

// Control mode: 0 = WASD keys, 1 = Arrow keys
bool useArrowKeys = false;

// Held-key control state
uint8_t activeKeycode = 0; // currently held HID keycode (0 = none)
unsigned long lastMovementTimeMs = 0;

// Velocity-smoothed tail hold for smoother stop
float filteredSpeed = 0.0f; // low-pass of |delta|
const float speedAlpha = 0.20f; // smoother response to spikes
const unsigned long minTailHoldMs = 60;   // minimum hold after stop
const unsigned long maxTailHoldMs = 350;  // cap tail to avoid long sticks
const float tailHoldPerCountMs = 25.0f;   // less tail per speed unit
const unsigned long hardIdleReleaseMs = 500; // always release if idle this long

// Map char to HID keycode
uint8_t mapCharToHid(char key) {
    if (key == 'a' || key == 'A') return HID_KEY_A;
    if (key == 'd' || key == 'D') return HID_KEY_D;
    if (key == 'w' || key == 'W') return HID_KEY_W;
    if (key == 's' || key == 'S') return HID_KEY_S;
    if (key == 'L') return HID_KEY_ARROW_LEFT;
    if (key == 'R') return HID_KEY_ARROW_RIGHT;
    if (key == 'U') return HID_KEY_ARROW_UP;
    if (key == 'D') return HID_KEY_ARROW_DOWN;
    return 0;
}

// Press and hold a key if not already active
void pressKeyIfNeeded(uint8_t keycode) {
    if (keycode == 0) return;
    if (activeKeycode == keycode) return; // already holding
    // release previous if any
    if (activeKeycode != 0) {
        usb_hid.keyboardRelease(0);
    }
    uint8_t keycodes[6] = { keycode, 0, 0, 0, 0, 0 };
    usb_hid.keyboardReport(0, 0, keycodes);
    activeKeycode = keycode;
}

// Release currently held key
void releaseHeldKey() {
    if (activeKeycode != 0) {
        usb_hid.keyboardRelease(0);
        activeKeycode = 0;
    }
}

// Function kept for possible tap actions (unused in hold logic)
void sendKeyPress(char key) {
    uint8_t keycode = mapCharToHid(key);
    if (keycode == 0) return;
    uint8_t keycodes[6] = { keycode, 0, 0, 0, 0, 0 };
    usb_hid.keyboardReport(0, 0, keycodes);
    delay(30);
    usb_hid.keyboardRelease(0);
}

void setup(){
	// Initialize USB HID
	usb_hid.setPollInterval(2);
	usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
	usb_hid.setStringDescriptor("Tank Encoder Keyboard");
	usb_hid.begin();
	
	// Wait for USB to be ready
	while (!TinyUSBDevice.mounted()) {
		delay(1);
	}
	
	Serial.begin(115200);
	delay(1000); // Give serial time to initialize
	
	// Initialize encoder
	encoder1.begin();
	
	// Initialize encoder count
	lastEncoder1Count = encoder1.read();
	
	Serial.println("Tank Encoder Controller Started (Raspberry Pi Pico)");
	Serial.println("USB HID Keyboard Ready!");
	Serial.println("Encoder1 (Turret): " + String(lastEncoder1Count));
    Serial.println("Control Mode: " + String(useArrowKeys ? "Arrow Keys" : "WASD"));
}

void loop(){
	// Handle Encoder 1 - Horizontal turret movement (A/D or Left/Right)
	int32_t currentEncoder1Count = encoder1.read();
	int32_t delta1 = currentEncoder1Count - lastEncoder1Count;
	
	// If moving, press and hold the corresponding key; if idle, release after timeout
	if (delta1 != 0) {
		char desiredChar;
		if (delta1 > 0) {
			desiredChar = useArrowKeys ? 'R' : 'd';
			Serial.println("Turning RIGHT (hold)");
		} else {
			desiredChar = useArrowKeys ? 'L' : 'a';
			Serial.println("Turning LEFT (hold)");
		}
		// Update velocity filter
		filteredSpeed = (1.0f - speedAlpha) * filteredSpeed + speedAlpha * abs(delta1);
		uint8_t desiredKeycode = mapCharToHid(desiredChar);
		pressKeyIfNeeded(desiredKeycode);
		lastMovementTimeMs = millis();
		lastEncoder1Count = currentEncoder1Count; // consume immediately for responsiveness
	} else {
		// Decay velocity filter when idle to shorten tail over time (faster decay)
		filteredSpeed *= 0.80f;
		if (filteredSpeed < 0.05f) filteredSpeed = 0.0f;
		// Compute dynamic tail hold from filtered speed
		unsigned long dynamicReleaseMs = minTailHoldMs + (unsigned long)min(maxTailHoldMs - minTailHoldMs,
			(unsigned long)(filteredSpeed * tailHoldPerCountMs));
		// No movement this loop; release if we've been idle long enough
		unsigned long idleMs = millis() - lastMovementTimeMs;
		if (activeKeycode != 0 && (idleMs >= dynamicReleaseMs || idleMs >= hardIdleReleaseMs)) {
			releaseHeldKey();
		}
	}

	// Print encoder counts for debugging (less frequent)
	static unsigned long lastPrint = 0;
	if (millis() - lastPrint >= 2000) { // Print every 2 seconds
		Serial.println("Encoder1 (Turret): " + String(currentEncoder1Count) + 
					   " | Mode: " + (useArrowKeys ? "Arrow Keys" : "WASD"));
		lastPrint = millis();
	}


	delay(1); // faster loop to catch more encoder transitions
}