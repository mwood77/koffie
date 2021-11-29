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
* You can find an emaulated "koffie" running here --> https://wokwi.com/arduino/projects/316589477903467072
*
*/
#include <OneButton.h>
#include <Encoder.h>

// *************************************************************************************
// ********************************* CONFIGURABLES *************************************
// *************************************************************************************

/*
* `A0` = to the pin your switch is wired to, set it to whatever ANALOG pin you've wired your switch into
* 
* MILK_LED{4} = the positive side of your LED, set "4" to the pin your LED is wired to
*/
OneButton buttonMilk(A0, true);
int MILK_LED{4};

/*
* `A1` = to the pin your switch is wired to, set it to whatever ANALOG pin you've wired your switch into
* 
* ESPRESS_LED{5} = the positive side of your LED, set "5" to the pin your LED is wired to
*/
OneButton buttonEspresso(A1, true);
int ESPRESSO_LED{5};

/*
* `A1` = to the pin your switch is wired to, set it to whatever ANALOG pin you've wired your switch into
* 
* ESPRESS_LED{5} = the positive side of your LED, set "5" to the pin your LED is wired to
*/
Encoder encoder(2, 3);

// *************************************************************************************
// ******************************* END CONFIGURABLES ***********************************
// *************************************************************************************

int CURRENT_MODE;                 // USED TO INDICATE CURRENT MODE
bool PROGRAMMING_MODE = false;    // PROGRAMMING MODE FLAG
long oldEncoderPosition{-999};

void setup() {

  pinMode(MILK_LED, OUTPUT);
  pinMode(ESPRESSO_LED, OUTPUT);

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
  delay(100);

  if (PROGRAMMING_MODE == 1) {
    digitalWrite(CURRENT_MODE, LOW);
    delay(500);
    digitalWrite(CURRENT_MODE, HIGH);
  }

  long newEncoderPosition = encoder.read();

  if (newEncoderPosition != oldEncoderPosition) {
    oldEncoderPosition = newEncoderPosition;
    Serial.println(newEncoderPosition);
  }
}

static void initialize() {
  CURRENT_MODE = ESPRESSO_LED;
  toggleLED(ESPRESSO_LED);
  printMode(9999);
}

// RED BUTTON
static void handleClickMilk() {
  if (checkStateLED(ESPRESSO_LED) == HIGH) {
    toggleLED(ESPRESSO_LED);
  }

  CURRENT_MODE = MILK_LED;

  if (checkStateLED(MILK_LED) == LOW) {
    toggleLED(MILK_LED);
  }
  printMode(MILK_LED);
}

// BLUE BUTTON
static void handleClickEspresso() {
  if (checkStateLED(MILK_LED) == HIGH) {
    toggleLED(MILK_LED);
  }

  CURRENT_MODE = ESPRESSO_LED;

  if (checkStateLED(ESPRESSO_LED) == LOW) {
    toggleLED(ESPRESSO_LED);
  }
  
  printMode(ESPRESSO_LED);
}

static void enableProgrammingMode() {
  PROGRAMMING_MODE = !PROGRAMMING_MODE;
  printMode(PROGRAMMING_MODE);
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

static void printMode(int mode) {
  switch(mode) {
    case 0:
      Serial.println("Programming mode disabled");
      break;
    case 1:
      Serial.println("Programming mode enabled");
      break;
    case 4:
      Serial.println("Set High Pressure - Milk Mode");
      break;
    case 5:
      Serial.println("Set Low Pressure - Espresso Mode");
      break;
    case 9999:
      Serial.println("Koffie Initialized");    
      break;
  }
}