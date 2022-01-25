# Koffie ☕
Koffie is an open source project which allows you to programatically set and adjust the boiler pressure of your espresso machine in real time.

Too much reading? [Watch an overview of the device here!](https://www.youtube.com/watch?v=FLI8BKzOYMY)

### Current development (features requests and bugs)
You can find the task board here, which shows work in progress: 
- [View our project board](https://github.com/mwood77/koffie/projects/1)

### Found a bug?
- [Report it here - please make sure to include the what & how](https://github.com/mwood77/koffie/issues/new)

### Want to contribute? Great!
- Fork it and create a PR!

## Read first!
- This proceedure is 100% reversible
- There are some passive safety limitations built into the software, which include:
    - You have a maximum boiler pressure of 1.4 BAR
- This project was designed/developed on a pre-millenium La Pavoni Europiccola (double rocker; power switch + hi/low)
    - Your implementation may be different than what's documented here, but with some simple probing of your espresso machine circuitry, you should be able to implement this easily.

# User guide
> By default, koffie boots into espresso mode

## Automatic Mode
- Tap either button to toggle between programming modes:
    - Espresso (low pressure)
    - Milk (high pressure)
- The screen will update to show you what mode you're currently in, in addition to the LEDs (if equipped).

## Manual Mode
- Turning the rotary encoder (knob) will toggle Koffie into Manual mode.
    - The pressure readout will show a **Target (TARG:)** and **Actual (ACTL:)** values. 
    - Turning the encoder will either increase or decrease the **target value**. Koffie will then take action to intercept the target value.
- The screen will update to show you what mode you're currently in, in addition to the LEDs (if equipped).

## Programming Mode
In addition to Automatic and Manual operation, you may also overwrite the default pressure values in Koffie's memory. Do set custom values do the following:
1. Turn on Koffie (your espresso machine does not need to be on).
1. Turn the encoder to swtich into **Manual Mode**
1. Set the **Target (TARG:)** to your desired pressure level.
1. Press and hold either button until the LED flashes (approx. 2 seconds)
    - Your new value is now stored in memory.
1. Tap the same button to go back to automatic mode and check that value on screen is the value you saved. If not, repeat the steps above.

# Build and install your own Koffie
This section has been split across multiple pages to make the project easier to digest.
1. [Design document](docs/design.md) **<- START HERE**
1. [Parts list and costs](docs/requirements.md)
1. [Circuit diagrams](docs/circuits.md)
1. [Code Configurables](docs/code_configurables.md) **<- READ THIS BEFORE UPLOADING THE CODE TO YOUR ARDUINO**

### Installation (Machine specific)
- La Pavoni
    1. [Europiccola](docs/machines/europiccola.md)
