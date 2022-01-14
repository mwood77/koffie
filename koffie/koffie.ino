  /*
  *
  *  
  * Welcome!
  * Author: Michael Wood
  * Date: 10 January, 2022
  *
  * You can find an emaulated "koffie" running here --> https://wokwi.com/arduino/projects/316601625012601409
  * There are some caveats for the emulator:
  *     - the temperature doesn't display right (tmp36 is not available)
  *     - there's no pressure sensor connected
  *     - there's no relay connected
  *
  *
  * LICENSE
  * 
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GPL 3.0 Public License attached to this repository
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  * SOFTWARE.
  */
  #include <SPI.h>
  #include <Wire.h>
  #include <EEPROM.h>
  #include <Encoder.h>
  #include <OneButton.h>
  #include <Adafruit_SSD1306.h>

  #include "Relay.h"
  #include "PID_v1.h"
  #include "Thread.h"

  /*
  * DO NOT CHANGE THESE VARIABLES!
  */
  unsigned int EEPROM_PRESSURE_ADDRESS{0};
  int TEMPERATURE_SMOOTHING_HOLDER_POSITION{0};
  int TEMPERATURE_SMOOTHING_HOLDER[5]{0, 0, 0, 0, 0};
  int CURRENT_MODE;
  bool PROGRAMMING_MODE = false;
  long OLD_ENCODER_POSITION{0};
  double const STEP_SIZE{0.02};
  double const UPPER_LIMIT{1.4};                              // UPPER LIMIT IN BAR
  double const LOWER_LIMIT{0.0};
  long const THREAD_INTERVAL{1000};
  int const RELAY_CONTROL_DURATION{1};                        // 1 = 1 second
  double MEASUREMENT_INPUT;                                   // declare and initialize PID variables
  double CONTROL_OUTPUT;                                      // declare and initialize PID variables
  double SETPOINT;                                            // declare and initialize PID variables

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
    0.74f, 
    1.10f
  };

  /*
  * Instantiate object for user's programmable temperature values
  */
  PRESSURE_SETTINGS usersDesiredTemperatures;

  // *************************************************************************************
  // ****************************** BEGIN CONFIGURABLES **********************************
  // *************************************************************************************
  /*
  * PRESSURE_SENSOR_INPUT_PIN = Set which DIGITIAL pin your PRESSURE probe's signal wire is connected to.
  * RELAY_CONTROL_OUTPUT_PIN = Set which DIGITIAL pin your RELAY's signal wire is connected to.
  * PRESSURE_TRANSDUCER_MAX_PSI = Set the maximum pressure your pressure sensor can read (EX -> if you bought a 30 PSI sensor, this value is 30.0)
  * ATMOSPHERIC_OFFSET = Set the pressure offset of your sensor. 
  *     To get this value, upload the code to your arduino and run it with the serial monitor open and with your pressure sensor attached. 
  *     If the "MEASUREMENT_INPUT" is "0.0" then use 0.0 as the value.
  *     If the reading is anything else (EX -> "MEASUREMENT_INPUT: -0.10"), then enter 0.10
  *
  * These values are critical for the relay control
  */
  float const PRESSURE_SENSOR_INPUT_PIN{A3};
  double const RELAY_CONTROL_OUTPUT_PIN{11};       // PWM compatible output @ 498 Hz (CHECK BEFORE CHANGING)
  const int PRESSURE_TRANSDUCER_MAX_PSI = 60.0;    // Change this value to the maximum value your pressure sensor can read in PSI. Ex: 30 PSI sensor = 30.0
  const double ATMOSPHERIC_OFFSET{0.0};

  /*
  * These values control the overall "tuning" of the PID. 
  * I'd strongly recomment NOT changing these, but if you find the tune needs adjusting, use this resource: 
  *   - https://robotics.stackexchange.com/questions/167/what-are-good-strategies-for-tuning-pid-loops
  */  
  double KP{20};                    // PID constant = P value
  double KI{10};                    // PID constant = I value
  double KD{18};                    // PID constant = D value

  /*
  * Used when converting the temperture probes output voltage to milivolts.
  * 5.0 = 5V input
  * 3.3 = 3.3V input
  */
  double const PIN_VOLTAGE{5.0};

  /*
  * Set which ANALOGUE pins your TEMPERATURE probe's signal wire(s) are connected to
  * 
  * https://learn.adafruit.com/tmp36-temperature-sensor/using-a-temp-sensor
  */
  int const GROUP_TEMP_PIN{A2};

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
  * MILK_LED{6} = the positive side of your LED, set "6" to the pin your LED is wired to
  */
  OneButton buttonMilk(A0, true);
  int const MILK_LED{6};

  /*
  * A1 = to the pin your switch is wired to, set it to whatever ANALOG pin you've wired your switch into
  * ESPRESSO_LED{7} = the positive side of your LED, set "7" to the pin your LED is wired to
  */
  OneButton buttonEspresso(A1, true);
  int const ESPRESSO_LED{7};

  /*
  * 2 = the DT pin of your encoder - Set this value to whatever DIGITAL pin you've used on your Arduino
  * 3 = the CLK pin ofyour encoder - Set this value to whatever DIGITAL pin you've used on your Arduino
  */
  Encoder encoder(3, 2);

  /*
  * Set the screen width and height here. If you bought the OLED listed in the documentation, there's no need
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
  PID pidControl(&MEASUREMENT_INPUT, &CONTROL_OUTPUT, &SETPOINT, KP, KI, KD, DIRECT);
  Relay relay(RELAY_CONTROL_OUTPUT_PIN, RELAY_CONTROL_DURATION);

  void setup() {

    Serial.begin(9600);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    pinMode(MILK_LED, OUTPUT);
    pinMode(ESPRESSO_LED, OUTPUT);
    pinMode(RELAY_CONTROL_OUTPUT_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    buttonMilk.attachClick(handleClickMilk);
    buttonMilk.attachLongPressStart(saveProgramMilk);
    
    buttonEspresso.attachClick(handleClickEspresso);
    buttonEspresso.attachLongPressStart(saveProgramEspresso);

    timerThread.onRun(updateTemps);
    timerThread.setInterval(THREAD_INTERVAL);

    checkEepromState();

    relay.setRelayMode(relayModeAutomatic);
    pidControl.SetMode(AUTOMATIC);
    
    initialize();                                             // initializes state as "espresso" & does user feedback setup

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
  * Checks if save-state exists in memory & writes to it if it doesn't
  */
  static void checkEepromState() {
    PRESSURE_SETTINGS result;

    if (EEPROM.read(EEPROM_PRESSURE_ADDRESS) == 255) {
      EEPROM.put(EEPROM_PRESSURE_ADDRESS, defaultPressures);
    } 
    result = EEPROM.get(EEPROM_PRESSURE_ADDRESS, result);
    usersDesiredTemperatures = result;
  
  }

  /**
  * Saves new default values to EEPROM
  * @param mode integer - the program mode to modify (*ex. Milk)
  * @param setpoint float - the setpoint value to save to memory
  */
  static void setEepromState(int mode, float setpoint) {
    PRESSURE_SETTINGS state;
    state = EEPROM.get(EEPROM_PRESSURE_ADDRESS, state);

    if (mode == 6) {
      usersDesiredTemperatures.milk = setpoint;
    }
    if (mode == 7) {
      usersDesiredTemperatures.espresso = setpoint;
    }

    if (state.espresso != usersDesiredTemperatures.espresso || state.milk != usersDesiredTemperatures.milk) {
      EEPROM.put(EEPROM_PRESSURE_ADDRESS, usersDesiredTemperatures);
    }
    
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

    if (OLD_ENCODER_POSITION < newEncoderPosition) {          // rotating clockwise
          SETPOINT += STEP_SIZE;

          if (SETPOINT > UPPER_LIMIT) {
            SETPOINT = UPPER_LIMIT;
          }

      } else {
        SETPOINT -= STEP_SIZE;

        if (SETPOINT < LOWER_LIMIT) {
          SETPOINT = LOWER_LIMIT;
        }
        
      }

      OLD_ENCODER_POSITION = newEncoderPosition;
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
    SETPOINT = usersDesiredTemperatures.milk;

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
    SETPOINT = usersDesiredTemperatures.espresso;

    if (checkStateLED(ESPRESSO_LED) == LOW) {
      toggleLED(ESPRESSO_LED);
    }
  
    drawActiveMode(CURRENT_MODE);

  }

  /**
  * Saves new programming value for espresso
  */
  static void saveProgramEspresso() {
    setEepromState(7, SETPOINT);
    blinkLED(7);

  }

  /**
  * Saves new programming value for milk
  */
  static void saveProgramMilk() {
    setEepromState(6, SETPOINT);
    blinkLED(6);

  }

  /**
  * Rapidly blinks the LED 
  */
  static void blinkLED(int mode) {
    for (int i = 0; i < 6; i++) {
      toggleLED(mode);
      delay(200);
    } 

  }

  /**
  * Configures operation mode and static gui elements
  */
  static void initialize() {
    CURRENT_MODE = ESPRESSO_LED;
    toggleLED(ESPRESSO_LED);
    
    SETPOINT = usersDesiredTemperatures.espresso;
    
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
  * @param pin integer - the LED's pin number to toggle ON or OFF.
  */
  static void toggleLED(int pin) {
    const boolean state = digitalRead(pin);
    digitalWrite(pin, !state);

  }

  /**
  * Returns the current byte state (HIGH or LOW) of an LED's pin.
  *
  * @param pin integer - the LED's pin number to check.
  * @return the byte code of the given LED's state (HIGH or LOW)
  */
  static byte checkStateLED(int pin) {
    return digitalRead(pin);

  }

  /**
  * Averages the total value of an array (ex. pressure or temp) and returns a single value
  *
  * @param array int[5] - the array[5] with values to average
  * @return the averaged result of arrayToSmooth
  */
  static float smoothValues(int *array) {
    float sum;

    for (int i = 1; i <= 5; i++) {
      sum += array[i];
    }

    sum /= 5;
    return sum;

  }

  /**
  * Measures the temperature of attached temperature probes.
  * Also blinks current mode's LED if PROGRAMMING_MODE = TRUE
  */
  static void updateTemps() {
    digitalWrite(LED_BUILTIN, HIGH); 
    int const group = analogRead(GROUP_TEMP_PIN);
    float const groupTemperature = convertTemperatureUnits(group);
    int const numberOfSmoothedElements = sizeof(TEMPERATURE_SMOOTHING_HOLDER)/sizeof(TEMPERATURE_SMOOTHING_HOLDER[0]);

    if (TEMPERATURE_SMOOTHING_HOLDER_POSITION > numberOfSmoothedElements) {
      TEMPERATURE_SMOOTHING_HOLDER_POSITION = 0;
    } else {
      TEMPERATURE_SMOOTHING_HOLDER[TEMPERATURE_SMOOTHING_HOLDER_POSITION] = groupTemperature;
      TEMPERATURE_SMOOTHING_HOLDER_POSITION++;
    }

    drawTemperatures(smoothValues(TEMPERATURE_SMOOTHING_HOLDER));
    updatePressure();

    if (PROGRAMMING_MODE == 1) {
      toggleLED(CURRENT_MODE);
      drawActiveMode(CURRENT_MODE);
    }

    digitalWrite(LED_BUILTIN, LOW); 

  }

  /**
  * Measures the pressure of attached pressure probe and updates PID values
  */
  static void updatePressure() {
    float dutyCycle = relay.getDutyCyclePercent();
    float sensor = analogRead(PRESSURE_SENSOR_INPUT_PIN);
    float convertedReading = convertPressureUnits(sensor);

    MEASUREMENT_INPUT = convertedReading;

    // COMPUTE PID LEVELS
    pidControl.Compute();
    
    // APPLY PID COMPUTE TO RELAY
    relay.loop();

    if (MEASUREMENT_INPUT > SETPOINT) {
      relay.setDutyCyclePercent(0);   
    } else {
      relay.setDutyCyclePercent(CONTROL_OUTPUT / 255.0);        // Relay library only accepts values between 0 and 1
    }

    // Serial.print(F("SETPOINT: "));
    // Serial.print(SETPOINT);                                   // only displays in BAR in serial monitor
    // Serial.print(F("  |  MEASUREMENT_INPUT: "));
    // Serial.print(MEASUREMENT_INPUT);
    // Serial.print(F("  |  dutyCycle: "));
    // Serial.print(dutyCycle * 100);
    // Serial.println(F("%"));

    drawPressure(convertedReading, PROGRAMMING_MODE);

  }

  /**
  * Converts milivolts to celcius, fahrenheit, or kelvin
  *
  * @param mv integer - milivolts measured from the temperature sensor
  * @return float - the value converted to the desired units
  */
  static float convertTemperatureUnits(int mV) {

    float voltage = mV * 5.0;
    voltage /= 1024.0;

    float celsius = (voltage - 0.5) * 100;
    float fahrenheit = (celsius * 9.0 / 5.0) + 32.0;

    switch(MEASUREMENT_UNIT) {
      case 'C':
        return celsius;
        break;
      case 'F':
        return fahrenheit;
        break;
      case 'K':
        return celsius + 273.15;
        break;
    }

  }

  /**
  * Converts voltage to BAR or PSI
  *
  * @param voltage float - milivolts measurement from the pressure sensor
  * @return float - the value converted to the desired units
  */
  static float convertPressureUnits(float voltage) {

    /*
    * y = mx + b
    * 
    * Credit to Oven's Garage for the formula beneath.
    * See it explained here: https://www.youtube.com/watch?v=UrqPxwsPWGk
    */
    const int pressureZero = 102.4;                           // analog reading of pressure transducer at 0 PSI - if you bought it from the guide, don't change.
    const int pressureMax = 921.6;                            // analog reading of pressure transducer at 60 PSI - if you bought it from the guide, don't change.

    const float result = ((voltage - pressureZero) * PRESSURE_TRANSDUCER_MAX_PSI) / (pressureMax - pressureZero);   // Result is in PSI
    float const PSI = result + ATMOSPHERIC_OFFSET;
    float const BAR = (result / 14.504) + ATMOSPHERIC_OFFSET;

    switch(MEASUREMENT_UNIT) {
      case 'C':
        return BAR;
        break;
      case 'F':
        return PSI;
        break;
      case 'K':
        return BAR;
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

    display.setCursor(30, 54);
    display.println(F("GROUP"));

    drawActiveMode(CURRENT_MODE);

  }

  /**
  * Draws temperature values on screen - called in protothread
  *
  * @param groupTemp integer - the converted value measured from the temperature sensor
  */
  static void drawTemperatures(int groupTemp) {
    display.fillRect(8, 25, 50, 25, BLACK);                   // Clear modes area
    display.setTextColor(WHITE, BLACK);                       // Prepare for overwriting data
    
    display.setTextSize(2);
    display.setCursor(8, 35);
    display.print(groupTemp);
    display.print(MEASUREMENT_UNIT);
    display.setTextSize(1);

  }

  /**
  * Draws pressure values on screen - called in protothread
  *
  * @param pressure float - pressure in units (not mV)
  * @param programmingMode integer - takes global variable PROGRAMMING_MODE
  */
  static void drawPressure(double pressure, int programmingMode) {
    double convertedSetPoint{0};
    convertedSetPoint = SETPOINT * 14.504;                    // Convert target display pressure from BAR to PSI 

    display.fillRect(66, 20, 62, 64, BLACK);
    display.setTextColor(WHITE, BLACK);

    if (CURRENT_MODE == 25) {

      display.setCursor(70, 28);
      display.print(F("TARG "));

      if (MEASUREMENT_UNIT == 'F') {
        display.print(convertedSetPoint);
      } else {
        display.print(SETPOINT);
      }
      
      display.setCursor(70, 48);
      display.print(F("ACTL "));
      display.print(pressure, 2);

    } else {

      display.setTextSize(2);
      display.setCursor(74, 35);

      if (MEASUREMENT_UNIT == 'F') {
        display.print(convertedSetPoint);
      } else {
        display.print(SETPOINT);
      }

      display.setTextSize(1);
      display.setCursor(103, 54);

      if (MEASUREMENT_UNIT == 'F') {
        display.print(F("PSI"));
      } else {
        display.print(F("BAR"));
      }

    }
    
    display.display();

  }

  /**
  * Draws current mode as a filled-in rectangle
  */
  static void drawModes() {
    display.fillRect(0, 0, 128, 14, BLACK);                   // Clear modes area
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
  *
  * @param activeMode integer - the number which represents the current mode in use: 6, 7, 25
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
