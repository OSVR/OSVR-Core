/// Nearly-trivial Arduino sketch to use with uvbi-measure-camera-latency

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// "Standard" Arduino-type boards have an onboard visible-LED on a pin,
/// we'll use this for a visible indicator to confirm that the serial commands
/// are making it through.
///
/// Comment out this define if you're using some other board that uses the
/// Arduino IDE but doesn't have such an LED.
#define BUILTIN_LED_PIN 13

/// Chosen arbitrarily, anything pin that's not already used for something
/// (like serial rx/tx) should work fine.
#define IR_LED_PIN 8

/// Chosen for reliable performance
#define PULSE_DURATION_MS 10

void setup() {
  Serial.begin(115200);
  pinMode(IR_LED_PIN, OUTPUT);
#ifdef BUILTIN_LED_PIN
  pinMode(BUILTIN_LED_PIN, OUTPUT);
#endif
}

/// Turn on IR LED, as well as built-in Arduino Pin 13 LED (as visible indicator)
inline void turnOn() {
  digitalWrite(IR_LED_PIN, HIGH);
#ifdef BUILTIN_LED_PIN
  digitalWrite(BUILTIN_LED_PIN, HIGH);
#endif
}

/// Turn off IR LED, as well as built-in Arduino Pin 13 LED (as visible indicator)
inline void turnOff() {
  digitalWrite(IR_LED_PIN, LOW);
#ifdef BUILTIN_LED_PIN
  digitalWrite(BUILTIN_LED_PIN, LOW);
#endif
}

/// Flag set by the serial handler, cleared by the main loop.
volatile bool triggered = false;

void loop() {
  /// Check the flag to see if we got anything over serial that
  /// should cause us to pulse the LED
  if (triggered) {
    turnOn();
    delay(PULSE_DURATION_MS);
    turnOff();
    /// Only clear the flag once the cycle is finished.
    triggered = false;
    Serial.println("got it");
  } else {
    delayMicroseconds(20);
  }
}

/// Handler for serial events: consume all available bytes and set the flag.
void serialEvent() {
  while (Serial.available()) {
    Serial.read();
  }
  triggered = true;
}

