# Module 2.4: Finite Automata Simulator

## Overview

This C++ program allows a user to define and simulate either a deterministic
finite automaton (DFA) or nondeterministic finite automaton (NFA). The user
provides the states, alphabet, start state, accepting states, and transition
table. The simulator then processes input strings and determines whether each
string is accepted or rejected.

## Supported Automata

### Deterministic Finite Automaton

A DFA has exactly one destination state for each state and alphabet-symbol
combination. The simulator follows one transition path and reports its final
state.

### Nondeterministic Finite Automaton

An NFA can have zero, one, or multiple destination states for a state and
symbol combination. The simulator tracks every active state and accepts the
string when at least one final state is an accepting state.

## Program Features

- User-defined DFA and NFA configurations
- Validated state and alphabet entry
- Validated start and accepting states
- Complete DFA transition-table construction
- Multiple destination states for NFA transitions
- Input-symbol validation
- Step-by-step transition output
- Accepted or rejected result
- Empty-string testing using the `EMPTY` command
- Multiple string tests without rebuilding the automaton
- Graceful exit using the `QUIT` command

## Files

- `main.cpp` — Contains the finite automata simulator
- `README.md` — Explains compilation, usage, and program design
- `TESTING.md` — Contains planned tests and final results

## Compilation

Compile the program with a C++17-compatible compiler:

```bash
g++ -std=c++17 main.cpp -o automata_simulator
