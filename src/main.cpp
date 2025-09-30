#include <Arduino.h>
#include <ESP32Encoder.h>
#include <USB.h>
#include <USBHIDConsumerControl.h>

ESP32Encoder encoder;
ESP32Encoder encoder2;

USBHIDConsumerControl consumer;
int32_t lastEncoderCount = 0;
const int32_t countsPerStep = 4; // adjust sensitivity: higher = fewer volume steps

// timer and flag for example, not needed for encoders
unsigned long encoder2lastToggled;
bool encoder2Paused = false;

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
		
	// set starting count value after attaching
	encoder.setCount(37);

	// clear the encoder's raw count and set the tracked count to zero
	encoder2.clearCount();
	Serial.println("Encoder Start = " + String((int32_t)encoder.getCount()));
	// set the lastToggle
	encoder2lastToggled = millis();

	// Start USB (CDC + HID)
	USB.begin();
	consumer.begin();
	lastEncoderCount = (int32_t)encoder.getCount();
}

void loop(){
	// Loop and read the count
	int32_t currentCount = (int32_t)encoder.getCount();
	int32_t delta = currentCount - lastEncoderCount;
    if (delta != 0) {
		// Map encoder movement to volume up/down
        if (delta > 0) {
            int32_t steps = delta / countsPerStep;
            for (int32_t i = 0; i < steps; i++) {
				consumer.press(CONSUMER_CONTROL_VOLUME_INCREMENT);
				consumer.release();
			}
        } else {
            int32_t steps = (-delta) / countsPerStep;
            for (int32_t i = 0; i < steps; i++) {
				consumer.press(CONSUMER_CONTROL_VOLUME_DECREMENT);
				consumer.release();
			}
		}
        // Only advance lastEncoderCount by the counts actually consumed
        int32_t consumed = (delta > 0 ? (delta / countsPerStep) * countsPerStep
                                      : -(((-delta) / countsPerStep) * countsPerStep));
        lastEncoderCount += consumed;
	}
	Serial.println("Encoder count = " + String(currentCount) + " " + String((int32_t)encoder2.getCount()));
	delay(100);

	// every 5 seconds toggle encoder 2
	if (millis() - encoder2lastToggled >= 5000) {
		if(encoder2Paused) {
			Serial.println("Resuming Encoder 2");
			encoder2.resumeCount();
		} else {
			Serial.println("Paused Encoder 2");
			encoder2.pauseCount();
		}

		encoder2Paused = !encoder2Paused;
		encoder2lastToggled = millis();
	}
}