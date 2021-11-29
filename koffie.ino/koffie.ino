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
// #include <SPI.h>
// #include <Wire.h>
#include <Encoder.h>
#include <OneButton.h>
#include <Adafruit_SSD1306.h>
#include "bitmaps.h"

int CURRENT_MODE;                 // USED TO INDICATE CURRENT MODE
bool PROGRAMMING_MODE = false;    // PROGRAMMING MODE FLAG
long oldEncoderPosition{-999};

// *************************************************************************************
// ********************************* CONFIGURABLES *************************************
// *************************************************************************************

/*
* A0 = to the pin your switch is wired to, set it to whatever ANALOG pin you've wired your switch into
* MILK_LED{4} = the positive side of your LED, set "4" to the pin your LED is wired to
*/
OneButton buttonMilk(A0, true);
int MILK_LED{6};

/*
* A1 = to the pin your switch is wired to, set it to whatever ANALOG pin you've wired your switch into
* ESPRESSO_LED{5} = the positive side of your LED, set "5" to the pin your LED is wired to
*/
OneButton buttonEspresso(A1, true);
int ESPRESSO_LED{7};

/*
* 2 = the DT pin your encoder is wired to
* 3 = the CLK pin your encoder is wired to
*/
Encoder encoder(2, 3);

/*
* Set the screen width and heigh here. If you bought the OLED listed in the documentation, there's no need
* to change anything. 
*/
int SCREEN_WIDTH{128};         // OLED display width, in pixels
int SCREEN_HEIGHT{64};         // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
int OLED_RESET{4};            // Reset pin 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// *************************************************************************************
// ******************************* END CONFIGURABLES ***********************************
// *************************************************************************************


void setup() {

  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(MILK_LED, OUTPUT);
  pinMode(ESPRESSO_LED, OUTPUT);

  buttonMilk.attachClick(handleClickMilk);
  buttonMilk.attachLongPressStart(enableProgrammingMode);
  
  buttonEspresso.attachClick(handleClickEspresso);
  buttonEspresso.attachLongPressStart(enableProgrammingMode);
  
  initialize();                    // initializes the state as "espresso" on boot

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

/**
* Configures operation mode
*/
static void initialize() {
  CURRENT_MODE = ESPRESSO_LED;
  toggleLED(ESPRESSO_LED);
  printMode(9999);
  initializeDisplay();
}

/**
* Displays a boot image then clears buffer & screen
*/
static void initializeDisplay() {
  display.clearDisplay();
  display.drawBitmap(0, 0, bootLogo, 128, 64, WHITE);
  display.display();
  display.clearDisplay();
  delay(2000);
  display.display();
}

static void enableProgrammingMode() {
  PROGRAMMING_MODE = !PROGRAMMING_MODE;
  printMode(PROGRAMMING_MODE);
}

/**
* Changes the continuous ON of OFF state of an LED
*
* @param pin the LED's pin number to toggle ON or OFF.
*/
static void toggleLED(int pin) {
  const boolean state = digitalRead(pin);
  digitalWrite(pin, !state);
}

/**
* Returns the current byte state (HIGH or LOW) of an LED's pin.
*
* @param pin the LED's pin number to check.
* @return the byte code of the given LED's state (HIGH or LOW)
*/
static byte checkStateLED(int pin) {
  return digitalRead(pin);
}

/**
* Prints a pre-defined status message to the serial monitor
*
* @param mode integer, the current mode (typically the LED pin)
*/
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