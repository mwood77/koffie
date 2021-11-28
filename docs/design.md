> [Back to main page](../README.md)

# Design document
The purpose of this document is:
- To outline what functionality the PID, "koffie," has
- How "koffie" behaves when iteracted with by a human
- How "koffie" interacts with an espresso machine

## Program modes
1. Espresso - low pressure for extracting espresso
1. Milk - high pressure for extracting milk
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
- 1 rocker swtich for controlling the power state
- 1 multi-function rotary encoder
    - If in Espresso or Milk modes:
        - Enter "Manual Mode," and enable on-the-fly boiler pressure adjustment
    - If in programming mode:
        - adjusts memory setting for relevant program
    
## GUI
Block diagram of interface

![Block diagram of koffie interface](images/koffie_block_diagram_gui.jpeg)

## Bonus features
> WIP for extra feature implementation, such as group head pressure during extraction.