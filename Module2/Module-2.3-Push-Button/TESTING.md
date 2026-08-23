# Push-Button System Test Plan

## Tester

Jacob Gainley — Tester/Debugger

## Purpose

This test plan verifies that the push-button system starts in the correct
state, processes valid button inputs correctly, handles invalid input, and
continues running without crashing.

## Planned Test Cases

| Test ID | Starting State | Input | Expected Result |
|---|---|---|---|
| TC-01 | Program start | None | The initial state is displayed as Off. |
| TC-02 | Off | 1 | The system transitions to On. |
| TC-03 | On | 1 | The system remains On. |
| TC-04 | Off | 0 | The system remains Off. |
| TC-05 | On | 0 | The system remains On. |
| TC-06 | Either state | Invalid number | The program rejects the input without crashing. |
| TC-07 | Either state | Letter or symbol | The program rejects the input without entering an infinite error loop. |
| TC-08 | Either state | Exit command | The program terminates normally. |

## Test Environment

- Language: C++
- Compiler: To be recorded when testing is performed
- Operating system: Windows
- Source file tested: `main.cpp`

## Test Results

Testing will be performed after the initial program implementation is added.
Actual results, pass/fail status, and discovered defects will be recorded here.
