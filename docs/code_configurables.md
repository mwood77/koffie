> [Back to main page](../README.md)

# Code configurables

## Prerequisites
If you've never used/programmed an Arduino before, I'd strongly suggest you start here:
1. [Arduino - Getting Started](https://www.arduino.cc/en/Guide)
1. [Arduino Nano Quickstart](https://docs.arduino.cc/hardware/nano)

There are a number of files but there are only a few things you **might** need to change. 
You can find these configurable values located between the following headers in `koffie.ino`

<hr>
Please read the comments closesly and do not change any of the other code unless you understand what is going on. If you can see an improvement point, please raise a PR!
<hr>

```
 File: ~/koffie/koffie.ino


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

## Arduino Libraries Required
Most of the required libraries are included, or are automatically downloaded on the first compilation.

If the Arduino IDE (2.0) fails to compile the code, it's probably unable to find a library. Please make sure you've downloaded the following (slightly obscure) libraries:

1. **Encoder** - by Paul Stoffregen
    - Version 1.4.2
1. **OneButton** - by Matthias Hertel
    - Version 2.0.4
1. **Adafruit SSD1306** - by Adafruit
    - Version 2.5.1
    - Make sure to install all of the required dependencies

## Helpful hints
1. Make sure to turn on the option `Show verbose output during: [x] compile`
    - This is found under **Arduino IDE** -> **Preferences** on macOS
    - This is found under **File** -> **Preferences** on Windows
    - This will give a lot of output during compile, but it makes it much easier to diagnose what went wrong.
