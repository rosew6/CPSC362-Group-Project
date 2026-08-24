# Finite Automata Simulator Test Plan

## Tester

Jacob Gainley — Tester/Debugger

## Purpose

This test plan verifies DFA and NFA creation, state-transition processing,
string acceptance, input validation, empty-string handling, and normal program
termination.

## DFA Test Configuration

The DFA accepts binary strings ending in `1`.

| Component | Definition |
|---|---|
| States | `q0`, `q1` |
| Alphabet | `0`, `1` |
| Start state | `q0` |
| Accepting state | `q1` |
| q0 on 0 | q0 |
| q0 on 1 | q1 |
| q1 on 0 | q0 |
| q1 on 1 | q1 |

## Planned DFA Tests

| Test ID | Input | Expected Result |
|---|---|---|
| DFA-01 | `1` | Accepted |
| DFA-02 | `101` | Accepted |
| DFA-03 | `100` | Not Accepted |
| DFA-04 | `EMPTY` | Not Accepted |
| DFA-05 | `102` | Invalid symbol is reported |
| DFA-06 | `QUIT` | Simulator closes normally |

## NFA Test Configuration

The NFA accepts binary strings containing at least one `1`.

| Component | Definition |
|---|---|
| States | `q0`, `q1` |
| Alphabet | `0`, `1` |
| Start state | `q0` |
| Accepting state | `q1` |
| q0 on 0 | q0 |
| q0 on 1 | q0 and q1 |
| q1 on 0 | q1 |
| q1 on 1 | q1 |

## Planned NFA Tests

| Test ID | Input | Expected Result |
|---|---|---|
| NFA-01 | `000` | Not Accepted |
| NFA-02 | `001` | Accepted |
| NFA-03 | `1010` | Accepted |
| NFA-04 | `EMPTY` | Not Accepted |
| NFA-05 | `12` | Invalid symbol is reported |
| NFA-06 | `QUIT` | Simulator closes normally |

## Validation Tests

| Test ID | Invalid Entry | Expected Behavior |
|---|---|---|
| VAL-01 | Automata type other than DFA or NFA | Entry is rejected and requested again |
| VAL-02 | Duplicate state name | Duplicate is rejected |
| VAL-03 | Alphabet entry with multiple characters | Entry is rejected |
| VAL-04 | Undefined start state | Entry is rejected |
| VAL-05 | Undefined accepting state | Entry is rejected |
| VAL-06 | Undefined transition destination | Entry is rejected |
| VAL-07 | Numeric entry outside allowed range | Entry is rejected |

## Test Environment

- Language: C++17
- Compiler: To be recorded after testing
- Environment: GitHub Codespaces
- Source file: `main.cpp`

## Test Results

Testing will be performed after compilation. Actual results, pass/fail status,
and any discovered defects will be recorded in this section.
