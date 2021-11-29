/*
*
* Welcome!
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the MIT Public License attached to this repository
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* You can find an emaulated "koffie" running here --> https://wokwi.com/arduino/projects/316591046196724289
*
*/
#include <OneButton.h>

OneButton buttonMilk(A0, true);
OneButton buttonEspresso(A1, true);

int CURRENT_MODE = 0;                 // USE LED PIN TO INDICATE MODE
bool PROGRAMMING_MODE = false;

void setup() {

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  Serial.begin(9600);

  buttonMilk.attachClick(handleClickMilk);
  buttonMilk.attachLongPressStart(enableProgrammingMode);
  
  buttonEspresso.attachClick(handleClickEspresso);
  buttonEspresso.attachLongPressStart(enableProgrammingMode);
  
  initialize();                     // Initialize state as "espresso" on boot

}

void loop() {
  buttonMilk.tick();
  buttonEspresso.tick();
  delay(10);

  if (PROGRAMMING_MODE == 1) {
    digitalWrite(CURRENT_MODE, LOW);
    delay(500);
    digitalWrite(CURRENT_MODE, HIGH);
  }

}

static void initialize() {
  CURRENT_MODE = 3;
  toggleLED(3);
  Serial.println("Initialized");
}

// RED BUTTON
static void handleClickMilk() {
  Serial.println("Milk Mode");
  if (checkStateLED(3) == HIGH) {
    toggleLED(3);
  }

  CURRENT_MODE = 2;

  if (checkStateLED(2) == LOW) {
    toggleLED(2);
  }
}

// BLUE BUTTON
static void handleClickEspresso() {
  Serial.println("Espresso Mode");
  if (checkStateLED(2) == HIGH) {
    toggleLED(2);
  }

  CURRENT_MODE = 3;

  if (checkStateLED(3) == LOW) {
    toggleLED(3);
  }
}

static void enableProgrammingMode() {
  PROGRAMMING_MODE = !PROGRAMMING_MODE;

  if (PROGRAMMING_MODE == 1) {
	  Serial.println("Programming mode enabled");
  } else {
	  Serial.println("Programming mode disabled");
  }
  
}

/**
* Changes the continuous ON of OFF state of an LED
*
* @param pin  the LED's pin number to toggle ON or OFF.
*/
static void toggleLED(int pin) {
  const boolean state = digitalRead(pin);
  digitalWrite(pin, !state);
}

/**
* Returns the current byte state (HIGH or LOW) of an LED's pin.
*
* @param pin  the LED's pin number to check.
* @return the byte code of the given LED's state (HIGH or LOW)
*/
static byte checkStateLED(int pin) {
  return digitalRead(pin);
}
