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
* You can find an emaulated "koffie" running here --> https://wokwi.com/arduino/projects/316601625012601409
*
*/
#include <SPI.h>
#include <Wire.h>
#include <Encoder.h>
#include <OneButton.h>
#include <Adafruit_SSD1306.h>

#include "bitmaps.h"
#include "Thread.h"

int CURRENT_MODE;                 // USED TO INDICATE CURRENT MODE
bool PROGRAMMING_MODE = false;    // PROGRAMMING MODE FLAG
long oldEncoderPosition{0};
long const threadInterval{700};

Thread timerThread = Thread();

// *************************************************************************************
// ********************************* CONFIGURABLES *************************************
// *************************************************************************************

/*
* Set which analog pins your temperature probe's signal wire are connected to
*/
int const groupTempPin{A2};
int const boilerTempPin{A3};
const float BETA = 3950;        // This must be set to the beta Coefficient of you thermistor(s), usually 3000-4000

/*
* Set which unit of measure you want to display.
* Options are:
*     - 'C'   (temperature in C and pressure in BAR)
*     - 'F'   (temperature in F and pressure in PSI)
*     - 'K'   (temperature in K and pressure in BAR)
*/
char const MEASUREMENT_UNIT{'C'};

/*
* A0 = to the pin your switch is wired to, set it to whatever ANALOG pin you've wired your switch into
* MILK_LED{4} = the positive side of your LED, set "4" to the pin your LED is wired to
*/
OneButton buttonMilk(A0, true);
int const MILK_LED{6};

/*
* A1 = to the pin your switch is wired to, set it to whatever ANALOG pin you've wired your switch into
* ESPRESSO_LED{5} = the positive side of your LED, set "5" to the pin your LED is wired to
*/
OneButton buttonEspresso(A1, true);
int const ESPRESSO_LED{7};

/*
* 2 = the DT pin your encoder is wired to
* 3 = the CLK pin your encoder is wired to
*/
Encoder encoder(2, 3);

/*
* Set the screen width and heigh here. If you bought the OLED listed in the documentation, there's no need
* to change anything. 
*/
int const SCREEN_WIDTH{128};        // OLED display width, in pixels
int const SCREEN_HEIGHT{64};        // OLED display height, in pixels
int const OLED_RESET{4};            // Reset pin 
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

  timerThread.onRun(updateTemps);
  timerThread.setInterval(threadInterval);
  
  initialize();                     // initializes the state as "espresso"

}

void loop() {
  buttonMilk.tick();
  buttonEspresso.tick();
  handleEncoder();
  delay(100);

  if (timerThread.shouldRun()) {
    timerThread.run();
  }
  
}

/**
* Handles encoder functionality if movement detected
*/
static void handleEncoder() {
  long newEncoderPosition = encoder.read();

  if (newEncoderPosition != oldEncoderPosition) {
    CURRENT_MODE = 25;
    oldEncoderPosition = newEncoderPosition;
    Serial.println(newEncoderPosition);
    drawActiveMode(CURRENT_MODE);
  }
}

/**
* RED BUTTON IN EMULATOR
*
* Handles buttons functionality for Milk
*/
static void handleClickMilk() {
  if (checkStateLED(ESPRESSO_LED) == HIGH) {
    toggleLED(ESPRESSO_LED);
  }

  CURRENT_MODE = MILK_LED;

  if (checkStateLED(MILK_LED) == LOW) {
    toggleLED(MILK_LED);
  }

  printMode(MILK_LED);
  drawActiveMode(CURRENT_MODE);
}

/**
* BLUE BUTTON IN EMULATOR
*
* Handles buttons functionality for Espresso
*/
static void handleClickEspresso() {
  if (checkStateLED(MILK_LED) == HIGH) {
    toggleLED(MILK_LED);
  }

  CURRENT_MODE = ESPRESSO_LED;

  if (checkStateLED(ESPRESSO_LED) == LOW) {
    toggleLED(ESPRESSO_LED);
  }
  
  printMode(ESPRESSO_LED);
  drawActiveMode(CURRENT_MODE);
}

/**
* Toggles programming mode modifier
*/
static void enableProgrammingMode() {
  PROGRAMMING_MODE = !PROGRAMMING_MODE;
  printMode(PROGRAMMING_MODE);
}

/**
* Configures operation mode and static gui elements
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
  drawGuiWireframe();
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
* Blinks the current mode's LED once
*
* @param pin the LED's pin number to toggle ON or OFF.
*/
static void blinkLED() {
    digitalWrite(CURRENT_MODE, LOW);
    delay(500);
    digitalWrite(CURRENT_MODE, HIGH);
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
    case 6:
      Serial.println("Set High Pressure - Milk Mode");
      break;
    case 7:
      Serial.println("Set Low Pressure - Espresso Mode");
      break;
    case 25:
      Serial.println("Manual Mode Enabled");
      break;
    case 9999:
      Serial.println("Koffie Initialized");    
      break;
  }
}

/**
* Measures the temperature of attached temperature probes.
* Also blinks current mode's LED if PROGRAMMING_MODE = TRUE
*/
static void updateTemps() {
  int const group = analogRead(groupTempPin);
  int const boiler = analogRead(boilerTempPin);

  int const groupTemperature = convertTemperatureUnits(group);
  int const boilerTemperature = convertTemperatureUnits(boiler);

  Serial.print("Group Temp:");
  Serial.print(groupTemperature);
  Serial.println(MEASUREMENT_UNIT);
  Serial.print("Boiler Temp:");
  Serial.print(boilerTemperature);
  Serial.println(MEASUREMENT_UNIT);

  if (PROGRAMMING_MODE == 1) {
    digitalWrite(CURRENT_MODE, LOW);
    delay(250);
    digitalWrite(CURRENT_MODE, HIGH);
    drawActiveMode(CURRENT_MODE);
  }

  drawTemperatures(groupTemperature, boilerTemperature);
}

/**
* Converts milivolts to celcius, fahrenheit, or kelvin
*
* @param mv integer, milivolts measured from the temperature sensor
* @return int the number converted to the desired units
*/
static int convertTemperatureUnits(int mV) {
  float celsius = 1 / (log(1 / (1023. / mV - 1)) / BETA + 1.0 / 298.15) - 273.15;
  switch(MEASUREMENT_UNIT) {
    case 'C':
      return celsius;
      break;
    case 'F':
      return celsius * (9 / 5) + 32;
      break;
    case 'K':
      return celsius + 273.15;
      break;
  }
}

/**
* Draws all static elements of the GUI
*/
static void drawGuiWireframe() {
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.drawRoundRect(2, 2, 40, 14, 4, WHITE);
  display.drawRoundRect(44, 2, 40, 14, 4, WHITE);
  display.drawRoundRect(86, 2, 40, 14, 4, WHITE);
  
  display.drawLine(0, 19, display.width(), 19, WHITE);
  display.drawLine(64, 19, 64, display.height(), WHITE);

  display.setCursor(10, 6);
  display.println("ESPR");
  display.setCursor(52, 6);
  display.println("MILK");
  display.setCursor(94, 6);
  display.println("MANU");

  display.setCursor(10, 28);
  display.println("GRP");
  display.setCursor(10, 48);
  display.println("BLR");
  
  display.setCursor(108, 54);
  display.println("BAR");

  display.setTextColor(WHITE, BLACK);     // Prepare for overwriting data

  display.display();

  drawPressure();
  drawActiveMode(CURRENT_MODE);
}

/**
* Draws temperature values on screen - called in protothread
*/
static void drawTemperatures(int groupTemp, int boilerTemp) {
  display.setCursor(40, 28);
  display.print(groupTemp);
  display.print(MEASUREMENT_UNIT);
  
  display.setCursor(40, 48);
  display.print(boilerTemp);
  display.print(MEASUREMENT_UNIT);
  
  display.display();
}

/**
* Draws temperature values on screen - called in protothread
*/
static void drawPressure() {
  display.setTextSize(2);
  display.setCursor(74, 35);
  display.println("0.88");
  
  display.display();
  display.setTextSize(1);
}

/**
* Draws corrent mode as a filled in rectangle
*/
static void drawModes() {

  display.fillRect(0, 0, 128, 14, BLACK);         // Clear modes area

  display.drawRoundRect(2, 2, 40, 14, 4, WHITE);
  display.drawRoundRect(44, 2, 40, 14, 4, WHITE);
  display.drawRoundRect(86, 2, 40, 14, 4, WHITE);

  display.setTextColor(WHITE);

  display.setCursor(10, 6);
  display.println("ESPR");
  display.setCursor(52, 6);
  display.println("MILK");
  display.setCursor(94, 6);
  display.println("MANU");
  
  display.display();

}

/**
* Draws current mode as a filled rectangle
*/
static void drawActiveMode(int activeMode) {

  drawModes();
  display.setTextColor(BLACK);

  switch(activeMode) {
    case 6:
      display.fillRoundRect(44, 2, 40, 14, 4, WHITE);
      display.setCursor(52, 6);
      display.println("MILK");
      break;
    case 7:
      display.fillRoundRect(2, 2, 40, 14, 4, WHITE);
      display.setCursor(10, 6);
      display.println("ESPR");
      break;
    case 25:
      display.fillRoundRect(86, 2, 40, 14, 4, WHITE);
      display.setCursor(94, 6);
      display.println("MANU");
      break;
  }
  
  display.display();
  display.setTextColor(WHITE);
}
