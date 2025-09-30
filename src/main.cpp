#include <Arduino.h>
#include <ESP32Encoder.h>
#include <USB.h>
#include <USBHIDKeyboard.h>

ESP32Encoder encoder;
ESP32Encoder encoder2;

USBHIDKeyboard keyboard;
// Encoder 1 (horizontal turret movement) - A/D keys
int32_t lastEncoder1Count = 0;
const int32_t countsPerStep1 = 2; // sensitivity for horizontal movement

// Encoder 2 (vertical cannon movement) - W/S keys  
int32_t lastEncoder2Count = 0;
const int32_t countsPerStep2 = 2; // sensitivity for vertical movement

// Control mode: 0 = WASD keys, 1 = Arrow keys
bool useArrowKeys = false;

void setup(){
	
	Serial.begin(115200);
	// Enable the weak pull down resistors

	//ESP32Encoder::useInternalWeakPullResistors = puType::down;
	// Enable the weak pull up resistors
	ESP32Encoder::useInternalWeakPullResistors = puType::up;

	// use pin 12 and 13 for the first encoder
	encoder.attachHalfQuad(12, 13);
	// use pin 17 and 16 for the second encoder
	encoder2.attachHalfQuad(17, 16);
		
	// clear both encoders and set the tracked count to zero
	encoder.clearCount();
	encoder2.clearCount();
	Serial.println("Tank Encoder Controller Started");
	Serial.println("Encoder1 (Turret): " + String((int32_t)encoder.getCount()));
	Serial.println("Encoder2 (Cannon): " + String((int32_t)encoder2.getCount()));

	// Start USB (CDC + HID)
	USB.begin();
	keyboard.begin();
	lastEncoder1Count = (int32_t)encoder.getCount();
	lastEncoder2Count = (int32_t)encoder2.getCount();
}

void loop(){
	// Handle Encoder 1 - Horizontal turret movement (A/D or Left/Right)
	int32_t currentEncoder1Count = (int32_t)encoder.getCount();
	int32_t delta1 = currentEncoder1Count - lastEncoder1Count;
	
	if (delta1 != 0) {
		if (delta1 > 0) {
			// Turn turret right
			int32_t steps = delta1 / countsPerStep1;
			for (int32_t i = 0; i < steps; i++) {
				if (useArrowKeys) {
					keyboard.press(KEY_RIGHT_ARROW);
					delay(50);
					keyboard.release(KEY_RIGHT_ARROW);
				} else {
					keyboard.press('d');
					delay(50);
					keyboard.release('d');
				}
			}
		} else {
			// Turn turret left
			int32_t steps = (-delta1) / countsPerStep1;
			for (int32_t i = 0; i < steps; i++) {
				if (useArrowKeys) {
					keyboard.press(KEY_LEFT_ARROW);
					delay(50);
					keyboard.release(KEY_LEFT_ARROW);
				} else {
					keyboard.press('a');
					delay(50);
					keyboard.release('a');
				}
			}
		}
		// Update last count by consumed steps
		int32_t consumed = (delta1 > 0 ? (delta1 / countsPerStep1) * countsPerStep1
									  : -(((-delta1) / countsPerStep1) * countsPerStep1));
		lastEncoder1Count += consumed;
	}

	// Handle Encoder 2 - Vertical cannon movement (W/S or Up/Down)
	int32_t currentEncoder2Count = (int32_t)encoder2.getCount();
	int32_t delta2 = currentEncoder2Count - lastEncoder2Count;
	
	if (delta2 != 0) {
		if (delta2 > 0) {
			// Move cannon up
			int32_t steps = delta2 / countsPerStep2;
			for (int32_t i = 0; i < steps; i++) {
				if (useArrowKeys) {
					keyboard.press(KEY_UP_ARROW);
					delay(50);
					keyboard.release(KEY_UP_ARROW);
				} else {
					keyboard.press('w');
					delay(50);
					keyboard.release('w');
				}
			}
		} else {
			// Move cannon down
			int32_t steps = (-delta2) / countsPerStep2;
			for (int32_t i = 0; i < steps; i++) {
				if (useArrowKeys) {
					keyboard.press(KEY_DOWN_ARROW);
					delay(50);
					keyboard.release(KEY_DOWN_ARROW);
				} else {
					keyboard.press('s');
					delay(50);
					keyboard.release('s');
				}
			}
		}
		// Update last count by consumed steps
		int32_t consumed = (delta2 > 0 ? (delta2 / countsPerStep2) * countsPerStep2
									  : -(((-delta2) / countsPerStep2) * countsPerStep2));
		lastEncoder2Count += consumed;
	}

	// Print encoder counts for debugging
	Serial.println("Encoder1 (Turret): " + String(currentEncoder1Count) + 
				   " | Encoder2 (Cannon): " + String(currentEncoder2Count) + 
				   " | Mode: " + (useArrowKeys ? "Arrow Keys" : "WASD"));

	// Toggle control mode every 10 seconds (for testing)
	static unsigned long lastModeToggle = 0;
	if (millis() - lastModeToggle >= 10000) {
		useArrowKeys = !useArrowKeys;
		Serial.println("Switched to " + String(useArrowKeys ? "Arrow Keys" : "WASD") + " mode");
		lastModeToggle = millis();
	}

	delay(100);
}