> [Back to main page](../README.md)

# Design document
The purpose of this document is:
- To outline what functionality the PID, "koffie," has
- How "koffie" behaves when iteracted with by a human
- How "koffie" interacts with an espresso machine

## Program modes
1. Espresso - low pressure for extracting espresso
1. Milk - high pressure for frothing milk
1. Manual - "on the fly" pressure adjustment
1. Programming - modifier which adjusts the save-state for Espresso or Milk modes
1. Reset - returns Espresso and Milk modes back to default values

## Controls
- 2 momentary buttons which are used to set and access stored values
    - Button 1:
        - **Momentary press:**
            1. deactivate current program
            1. activate "Espresso Mode" & call-up pressure for extracting espresso
        - **Long press (3 seconds):** 
            - enter or exit programming mode for extracting espresso
    - Button 2:
        - **Momentary press:** 
            1. deactivate current program
            1. activate "Milk Mode" & call-up pressure for frothing milk
        - **Long press (3 seconds):** 
            - enter or exit programming mode for frothing milk
    - Button 1 + 2:
        - **Long press (3 seconds):** 
            1. Change measurement units
- 1 multi-function rotary encoder
    - If in Espresso or Milk modes:
        - deactivate current program
        - activate "Manual Mode," and enable on-the-fly boiler pressure adjustment
    - If in programming mode:
        - adjusts memory setting for relevant program
    
## GUI
#### Block diagram of interface

![Block diagram of koffie interface](images/koffie_block_diagram_gui.jpeg)

#### Populated block diagram interface
|  |  |
| - | - |
| ![Block diagram of koffie interface](images/populated_koffie_block_diagram_gui.jpeg) | - A signle mode is illuminated when activated <br/> - When in "programming mode," the corresponding parent mode will flash <br/> - Temperatures are displayed, if available <br/> - if no probe is connected, `N/C` is displayed |


## Bonus features / nice to haves
- Configurable to display units in Science & Frankenstien