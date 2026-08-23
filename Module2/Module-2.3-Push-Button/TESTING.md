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
- Compiler: 'g++' in GitHub Codespaces 
- Operating system: Windows
- Source file tested: `main.cpp`

## Test Results

| Test ID | Actual Result | Status |
|---|---|---|
| TC-01 | Program displayed the initial state as Off. | Pass |
| TC-02 | Input `1` transitioned the system from Off to On. | Pass |
| TC-03 | Input `1` while On kept the system On. | Pass |
| TC-04 | Input `0` while Off kept the system Off. | Pass |
| TC-05 | Input `0` while On kept the system On. | Pass |
| TC-06 | Invalid numeric input `7` displayed an error and the program continued. | Pass |
| TC-07 | Invalid text input `hello` displayed an error and the program continued. | Pass |
| TC-08 | Input `q` ended the program normally and displayed the final press count. | Pass |

## Test Summary

All eight planned test cases passed. The program compiled successfully using
`g++` in GitHub Codespaces. No functional defects were identified during
testing.
