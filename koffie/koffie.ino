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
#include <EEPROM.h>
#include <Encoder.h>
#include <OneButton.h>
#include <Adafruit_SSD1306.h>

#include "PID_v1.h"
#include "Thread.h"

unsigned int EEPROM_PRESSURE_ADDRESS = 0;

int CURRENT_MODE;                 // USED TO INDICATE CURRENT MODE
bool PROGRAMMING_MODE = false;    // PROGRAMMING MODE FLAG
long OLD_ENCODER_POSITION{0};
long const THREAD_INTERVAL{1000};
double const UPPER_LIMIT{1.0};      // declare and initialize PID variables
double const LOWER_LIMIT{0.0};      // declare and initialize PID variables
double SETPOINT;                   // declare and initialize PID variables
double KP{2};                      // declare and initialize PID variables
double KI{5};                      // declare and initialize PID variables
double KD{1};                      // declare and initialize PID variables


/*
* Data Model
*/
struct PRESSURE_SETTINGS {
  float espresso;
  float milk;
};

/*
* Default pressure values in BAR
*/
static PRESSURE_SETTINGS defaultPressures = {
  0.75f, 
  0.9f
};

/*
* Instantiate object for user's programmable temperature values
*/
PRESSURE_SETTINGS usersDesiredTemperatures;


// *************************************************************************************
// ****************************** BEGIN CONFIGURABLES **********************************
// *************************************************************************************

/*
* Set which digital pin your PRESSURE probe's signal wire is connected to.
* Set which digital pin your RELAY's signal wire is connected to.
*
* This is critical for the relay control
* 
* Some good background reading regarding the Arrduino's PWM: https://www.arduino.cc/en/Tutorial/SecretsOfArduinoPWM
*/
double const PID__PRESSURE_SENSOR_INPUT{12};
double const PID__RELAY_CONTROL_OUTPUT{11};       // PWM compatible output @ 498 Hz (CHECK BEFORE CHANGING)

/*
* Set which analog pins your TEMPERATURE probe's signal wire(s) are connected to
*/
int const GROUP_TEMP_PIN{A2};
int const BOILER_TEMP_PIN{A3};
const float BETA = 3950;        // This must be set to the beta Coefficient of you thermistor(temperature probe), usually 3000-4000

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
* MILK_LED{6} = the positive side of your LED, set "4" to the pin your LED is wired to
*/
OneButton buttonMilk(A0, true);
int const MILK_LED{6};

/*
* A1 = to the pin your switch is wired to, set it to whatever ANALOG pin you've wired your switch into
* ESPRESSO_LED{7} = the positive side of your LED, set "5" to the pin your LED is wired to
*/
OneButton buttonEspresso(A1, true);
int const ESPRESSO_LED{7};

/*
* 2 = the DT pin your encoder is wired to
* 3 = the CLK pin your encoder is wired to
*/
Encoder encoder(3, 2);

/*
* Set the screen width and heigh here. If you bought the OLED listed in the documentation, there's no need
* to change anything. 
*/
int const SCREEN_WIDTH{128};        // OLED display width, in pixels
int const SCREEN_HEIGHT{64};        // OLED display height, in pixels
int const OLED_RESET{-1};            // Reset pin 

// *************************************************************************************
// ******************************* END CONFIGURABLES ***********************************
// *************************************************************************************

Thread timerThread = Thread();
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
PID pidControl(&PID__PRESSURE_SENSOR_INPUT, &PID__RELAY_CONTROL_OUTPUT, &SETPOINT, KP, KI, KD, DIRECT);

void setup() {

  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(MILK_LED, OUTPUT);
  pinMode(ESPRESSO_LED, OUTPUT);
  pinMode(PID__RELAY_CONTROL_OUTPUT, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  buttonMilk.attachClick(handleClickMilk);
  buttonMilk.attachLongPressStart(enableProgrammingMode);
  
  buttonEspresso.attachClick(handleClickEspresso);
  buttonEspresso.attachLongPressStart(enableProgrammingMode);

  timerThread.onRun(updateTemps);
  timerThread.setInterval(THREAD_INTERVAL);

  checkEepromState();
  
  initialize();                     // initializes state as "espresso" & does user feedback setup

  pidControl.SetOutputLimits(LOWER_LIMIT, UPPER_LIMIT);
  pidControl.SetMode(AUTOMATIC);

}

void loop() {
  buttonMilk.tick();
  buttonEspresso.tick();
  handleEncoder();
  delay(100);

  if (timerThread.shouldRun()) {
    timerThread.run();
  }

  pidControl.Compute();
  
}

static void checkEepromState() {
  PRESSURE_SETTINGS result;

  if (EEPROM.read(EEPROM_PRESSURE_ADDRESS) == 255) {
    Serial.println(F("First run - saving defaults"));
    EEPROM.put(EEPROM_PRESSURE_ADDRESS, defaultPressures);
  } 

  result = EEPROM.get(EEPROM_PRESSURE_ADDRESS, result);
  Serial.println(result.espresso);
  Serial.println(result.milk);

  usersDesiredTemperatures = result;
 
}

/**
* Handles encoder functionality if movement detected
*/
static void handleEncoder() {
  int const manualMode{25};
  long newEncoderPosition = encoder.read();

  if (newEncoderPosition != OLD_ENCODER_POSITION) {
    if ( CURRENT_MODE != manualMode ) {
      CURRENT_MODE = manualMode;
      drawActiveMode(CURRENT_MODE);

      if (checkStateLED(ESPRESSO_LED) == HIGH) {
        toggleLED(ESPRESSO_LED);
      }

      if (checkStateLED(MILK_LED) == HIGH) {
        toggleLED(MILK_LED);
      }
    }
    OLD_ENCODER_POSITION = newEncoderPosition;
    Serial.println(newEncoderPosition);
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
 
  drawActiveMode(CURRENT_MODE);
}

/**
* Toggles programming mode modifier
*/
static void enableProgrammingMode() {
  if (CURRENT_MODE != 25 ) {
    PROGRAMMING_MODE = !PROGRAMMING_MODE;
    
    if (checkStateLED(CURRENT_MODE) == LOW) {
      toggleLED(CURRENT_MODE);
    }

  }
}

/**
* Configures operation mode and static gui elements
*/
static void initialize() {
  CURRENT_MODE = ESPRESSO_LED;
  toggleLED(ESPRESSO_LED);
  initializeDisplay();
}

/**
* Clears the display then begins the screen initilization
*/
static void initializeDisplay() {
  display.clearDisplay();
  drawStaticGUI();
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
* Measures the temperature of attached temperature probes.
* Also blinks current mode's LED if PROGRAMMING_MODE = TRUE
*/
static void updateTemps() {
  
	digitalWrite(LED_BUILTIN, HIGH); 

  int const group = analogRead(GROUP_TEMP_PIN);
  int const boiler = analogRead(BOILER_TEMP_PIN);

  int const groupTemperature = convertTemperatureUnits(group);
  int const boilerTemperature = convertTemperatureUnits(boiler);

  if (PROGRAMMING_MODE == 1) {
    toggleLED(CURRENT_MODE);
    drawActiveMode(CURRENT_MODE);
  }

  drawTemperatures(groupTemperature, boilerTemperature);
  drawPressure(0.88, PROGRAMMING_MODE);     //@todo - figure out wiring/code for pressure sensor

	digitalWrite(LED_BUILTIN, LOW); 
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
static void drawStaticGUI() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  display.drawLine(0, 19, display.width(), 19, WHITE);
  display.drawLine(64, 19, 64, display.height(), WHITE);

  display.setCursor(10, 28);
  display.println(F("GRP"));
  display.setCursor(10, 48);
  display.println(F("BLR"));
  
  display.setCursor(108, 54);
  display.println(F("BAR"));

  drawActiveMode(CURRENT_MODE);
}

/**
* Draws temperature values on screen - called in protothread
*/
static void drawTemperatures(int groupTemp, int boilerTemp) {
    
  display.fillRect(40, 24, 24, 64, BLACK);         // Clear modes area
  display.setTextColor(WHITE, BLACK);              // Prepare for overwriting data

  display.setCursor(40, 28);
  display.print(groupTemp);
  display.print(MEASUREMENT_UNIT);
  
  display.setCursor(40, 48);
  display.print(boilerTemp);
  display.print(MEASUREMENT_UNIT);
  
}

/**
* Draws pressure values on screen - called in protothread
*
* @param pressure float, pressure in units (not mV)
* @param programmingMode int, takes global variable PROGRAMMING_MODE
*/
static void drawPressure(double pressure, int programmingMode) {

  display.fillRect(66, 20, 62, 64, BLACK);

  if (CURRENT_MODE == 25) {
    display.setCursor(70, 28);
    display.print(F("TARG "));
    display.print(pressure);
    
    display.setCursor(70, 48);
    display.print(F("ACTL "));
    display.print(pressure);

  } else {
    display.setTextSize(2);
    display.setCursor(74, 35);
    display.print(pressure);

    display.setTextSize(1);
    display.setCursor(108, 54);
    display.print(F("BAR"));
  }
  
  display.display();

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
  display.println(F("ESPR"));
  display.setCursor(52, 6);
  display.println(F("MILK"));
  display.setCursor(94, 6);
  display.println(F("MANU"));

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
      display.println(F("MILK"));
      break;
    case 7:
      display.fillRoundRect(2, 2, 40, 14, 4, WHITE);
      display.setCursor(10, 6);
      display.println(F("ESPR"));
      break;
    case 25:
      display.fillRoundRect(86, 2, 40, 14, 4, WHITE);
      display.setCursor(94, 6);
      display.println(F("MANU"));
      break;
  }
  
  display.setTextColor(WHITE);
}
