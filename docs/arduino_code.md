> [Back to main page](../README.md)

# Arduino code

## Prerequisites
If you've never used/programmed an Arduino before, I'd strongly suggest you start here:
1. [Arduino - Getting Started](https://www.arduino.cc/en/Guide)
1. [Arduino Nano Quickstart](https://docs.arduino.cc/hardware/nano)

There is a whole lot going on, but there are only a few things you **might** need to change.

Anything that's configurable is located between the headers.
```
// *************************************************************************************
// ****************************** BEGIN CONFIGURABLES **********************************
// *************************************************************************************
 
...
...
...

// *************************************************************************************
// ******************************* END CONFIGURABLES ***********************************
// *************************************************************************************
```

## Please read the comments closesly and do not change any of the other code, unless you understand what's happenning in it. 
If you followed the instructions to the letter, you shouldn't need to change a thing.

```cpp

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
* Used when converting the temperture probes output voltage to milivolts.
* 5.0 = 5V input
* 3.3 = 3.3V input
*/
double const PIN_VOLTAGE{5.0};

/*
* Set which ANALOG pins your TEMPERATURE probe's signal wire(s) are connected to
* 
* https://learn.adafruit.com/tmp36-temperature-sensor/using-a-temp-sensor
*/
int const GROUP_TEMP_PIN{A2};
int const BOILER_TEMP_PIN{A3};

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
int const OLED_RESET{-1};           // Reset pin 

// *************************************************************************************
// ******************************* END CONFIGURABLES ***********************************
// *************************************************************************************
```