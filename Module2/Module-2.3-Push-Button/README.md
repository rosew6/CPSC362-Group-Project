# Module 2.3: Push-Button System Simulator

## Overview

This C++ program simulates a basic finite-state push-button system. The system
has two possible states: `Off` and `On`. It begins in the `Off` state and uses
user input to determine whether the button has been pressed.

The project demonstrates state representation, transition logic, input
validation, and a continuous simulation loop.

## State-Transition Rules

| Current State | Button Input | Next State |
|---|---|---|
| Off | Not pressed (`0`) | Off |
| Off | Pressed (`1`) | On |
| On | Not pressed (`0`) | On |
| On | Pressed (`1`) | On |

Pressing the button changes the system from `Off` to `On`. Once the system is
`On`, additional button presses leave it in the `On` state.

## Additional Features

The program includes the following features beyond the basic requirements:

- A counter that records the total number of button presses
- Input validation for unsupported entries
- An exit command that ends the simulation normally
- Clear console instructions and state information

## Files

- `main.cpp` — Contains the push-button simulator
- `TESTING.md` — Contains the test plan and recorded test results
- `README.md` — Explains the program and its use

## Compilation

Compile the program with a C++ compiler such as `g++`:

```bash
g++ main.cpp -o push_button
