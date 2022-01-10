> [Back to main page](../README.md)

# Parts list and costs

## How much will this cost?
That's hard to answer, but if you purchase everything in the list beneath, it should come to no more than **$60 USD / €53**. 

## Things I've assumed you already know and own
- A "dumb" espresso machine
- A computer
    - The tutorial is written using a Mac, but a Windows or Linux computer is also fine. The instructions should be interchangable.
- A spare phone charger & USB cable to power the Arduino
- How to use a multimeter
- How to solder and de-solder
- Some spare small-gauge wire
- Rudimentary knowledge of electronics
- Some understanding of code (C++) and/or how to use an Arduino
    - However if you follow the instruction to the letter, you should be able to get by without any previous experience.

## Required parts
> You requirements may differ, so this list may or may not be complete depending on what sensors you want to install / your espresso machine.

I've provided links to the parts where I can. Most are sourced from Aliexpress as that's usually the most cost effective option.

#### La Pavoni Europiccola & Professional
- [ ] [Arduino Nano or other compatible](https://aliexpress.com/item/4000903444456.html). 
    - I chose the option: `Type-C USB`
        - If you choose `Type-C USB` be careful of the driver requirements (plug and play on Mac and Linux. Windows, not guaranteed). **If you don't know what this means, select a mico-USB one instead.**
- [ ] [Arduino expansion board](https://s.click.aliexpress.com/e/_9RjuPG).
    - **This is optional**, but it reduces your soldering workload / allows you to wire in new sensors quickly.
- [ ] [A solid state relay](https://s.click.aliexpress.com/e/_9h01hI). 
    - Select the `SSR-40 DA` option, **or make sure you choose a solid state relay that can handle roughly 40A and mains voltage (100-240V).**
- [ ] [An OLED screen](https://nl.aliexpress.com/item/32957309383.html). 
    - An OLED screen has a very wide viewing angle, is high contrast, and is not complex to implement.
- [ ] [An encoder](https://s.click.aliexpress.com/e/_AZFjme). 
- [ ] [A knob for the encoder](https://s.click.aliexpress.com/e/_Atgtlq). 
    - The better the knob, the better the encoder will feel!
- [ ] [2x illuminated momentary switches](https://s.click.aliexpress.com/e/_AFDd3K). 
    - We will be using them to quick-select different memory modes ("Brew mode" & "Frothing mode").
        - Select: 
            - Color: `16 Ring LED`
            - Voltage: `3-6V`
            - Size: `Self-reset momentary`
            - Standard: `choose whatever LED colour you'd like. I'd recommend matching it to your OLED.`
            - Quantity: `2`
- [ ] [An enclosure to mount the electronics](https://s.click.aliexpress.com/e/_AKRz3E).
- [ ] [2x TMP36 analog temperature sensors](https://s.click.aliexpress.com/e/_AWMTHm).
    - We'll hack this to fit in the joint between the group head and boiler.
- [ ] [2x 4k7 resistors (also known as 4700 ohms or 4.7K resistors)](https://www.google.com/search?q=4k7+resistor).
    - We'll use these on our switch's LEDs.
- [ ] [2x A pressure transducer with a minimum rating of 30 PSI / 2 BAR.](https://s.click.aliexpress.com/e/_ALg5YG). 
    - I use 60PSI sensors in this project, but you can change this variable in the project code.
    - I'd recommend you buy 2 examples as these have a habit of failing.
    - This one is great, as the thread size (1/8 NPT) is compatible with the Europiccola's manometer adapter (the gauge adapter that screws atop the sight glass).
    - If you don't have an existing pressure gauge, or need an adapter for this project:
        - [M12 to 1/8 NPT adapter](https://www.google.com/search?q=europiccola+m12+to+1%2F8+npt)
        - [M11 to 1/8 NPT adapter](https://www.google.com/search?q=europiccola+m11+to+1%2F8+npt)
        - If you don't know what size to buy, remove the "big screw" on top of the sight glass and measure the diameter of the threads.
            - 11mm = M11 
            - 12mm = M12

### Optional Extras
- [ ] [Kapton Tape](https://www.google.com/search?q=kpaton+tape).
    - We'll use this to attach our sensors to the coffee machine (this is heat resistant tape).