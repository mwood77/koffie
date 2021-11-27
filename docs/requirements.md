> [Back to main page](../README.md)

# Parts list and costs

## How much will this cost?
That's hard to answer, but if you purchase everything in t the list beneath, it should come in around **$100 USD / €85**. I've spec'd slightly more expensive parts, so if you substituted some, it'll be even cheaper

## Things I've assumed you already know and own
- A "dumb" espresso machine
- A computer
    - The tutorial is written using a Mac, but a Windows or Linux computer is also fine.
- A spare phone charger & USB cable to power the Arduino
- How to use a multimeter
- How to solder and de-solder
- Some spare wire, typically small gauge
- Rudimentary knowledge of electronics
- Some understanding of code (C++), but you should be able to get by without it if you follow this to the letter.

## Required parts
> You requirements may differ, so this list may or may not be complete depending on what sensors you want to install / your espresso machine.

I've provided links to the parts where I can. Most are sourced from Aliexpress as that's usually the most cost effective option.

#### La Pavoni Europiccola & Professional

- <input type="checkbox" /> [Arduino Nano or other compatible](https://aliexpress.com/item/4000903444456.html). 
    - I chose the option: `Type-C USB`
        - If you choose `Type-C USB` be careful of the driver requirements (plug and play on Mac and Linux. Windows, not guaranteed). **If you don't know what this means, select a mico-USB one instead.**
- <input type="checkbox" /> [Arduino expansion board](https://aliexpress.com/item/4000903444456.html).
    - **This is optional**, but it reduces your soldering workload / allows you to wire in new sensors quickly.
- <input type="checkbox" /> [A solid state relay](https://s.click.aliexpress.com/e/_9h01hI). 
    - Select the `SSR-40 DA` option, **or make sure you choose a solid state relay that can handle roughly 40A and mains voltage (100-240V).**
- <input type="checkbox" /> [An OLED screen](https://nl.aliexpress.com/item/32957309383.html). 
    - An OLED screen has a very wide viewing angle, is high contrast, and is not complex to implement.
- <input type="checkbox" /> [An optical encoder](https://s.click.aliexpress.com/e/_A36Y2U). 
    - Why not a cheapie digital encoder you ask? Because those feel cheap and nasty.
- <input type="checkbox" /> [A knob for the encoder](https://s.click.aliexpress.com/e/_Atgtlq). 
- <input type="checkbox" /> [Two illuminated momentary switches](https://s.click.aliexpress.com/e/_AFDd3K). 
    - We will be using them to quick-select different memory modes ("Brew mode" & "Frothing mode").
        - Select: 
            - Color: `16 Ring LED`
            - Voltage: `3-6V`
            - Size: `Self-reset momentary`
            - Standard: `choose whatever LED colour you'd like. I'd recommend matching it to your OLED.`
            - Quantity: `2`
- <input type="checkbox" /> [A 4-pin power switch](https://s.click.aliexpress.com/e/_A9EgSG). 
    - Double check the voltage and amperage rating!
- <input type="checkbox" /> [An enclosure to mount the electronics](https://s.click.aliexpress.com/e/_AKRz3E).
- <input type="checkbox" /> [A digital temperature sensor](https://s.click.aliexpress.com/e/_A4KEIU).
    - We'll hack this to fit in the joint between the group head and boiler.
- <input type="checkbox" /> [A pressure transducer with a minimum rating of 30 PSI / 2 BAR](https://s.click.aliexpress.com/e/_ALg5YG). 
    - This one is great, as the thread size (1/8 NPT) is compatible with the Europiccola's manometer adapter (the gauge adapter that screws atop the sight glass).
    - If you don't have an existing pressure gauge, or need an adapter for this project:
        - [M12 to 1/8 NPT adapter](https://www.google.com/search?q=europiccola+m12+to+1%2F8+npt)
        - [M11 to 1/8 NPT adapter](https://www.google.com/search?q=europiccola+m11+to+1%2F8+npt)
        - If you don't know what size to buy, remove the "big screw" on top of the sight glass and measure the diameter of the threads.
            - 11mm = M11 
            - 12mm = M12

### Optional Extras
- <input type="checkbox" /> [An infrared temperature sensor](https://nl.aliexpress.com/item/4001255844417.html?). 
    - This can be placed inside the espresso machine and pointed towards the boiler to get the boiler's temperature.