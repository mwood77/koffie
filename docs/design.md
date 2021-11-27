> [Back to main page](../README.md)

# Design document
The purpose of this document:
- To outline what functionality the PID has
- How the PID behaves when iteracted with by a human
- How the PID interacts with an espresso machine

## Controls
- 2 momentary buttons used to set and access stored values
    - Button 1:
        - **momentary press:** call-up pressure for extracting espresso
        - **long press (3 seconds):** enter or exit programming mode for extracting espresso

    - Button 2:
        - **momentary press:** call-up pressure for frothing milk
        - **long press (3 seconds):** enter or exit programming mode for frothing milk
- 1 rocker swtich for controlling power state
- 1 multi-function rotary encoder
    - If in programming mode:
        - adjust memory setting for relevant program
    - If in regular operation:
        - on-the-fly boiler pressure adjustment
    
## GUI
> WIP

## Bonus features
> WIP for extra feature implementation, such as group head pressure during extraction.