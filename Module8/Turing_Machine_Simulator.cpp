#include <cstdint>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// CPSC 362 - Module 8
// Deterministic single-tape Turing Machine simulator.
//
// The tape is modeled as a sparse map of signed positions to nonblank symbols.
// Missing positions are interpreted as the machine's blank symbol, which lets
// the head move indefinitely in either direction without a fixed array bound.

enum class SimulationStatus {
    ACCEPTED,
    REJECTED,
    HALTED_NO_TRANSITION,
    STEP_LIMIT,
    INVALID_INPUT,
    INVALID_MACHINE
};

struct TransitionRule {
    std::string currentState;
    char readSymbol = '_';
    std::string nextState;
    char writeSymbol = '_';
    char direction = 'R';
};

struct Machine {
    std::set<std::string> states;
    std::set<char> inputAlphabet;
    std::set<char> tapeAlphabet;
    char blankSymbol = '_';
    std::string startState;
    std::string acceptState;
    std::string rejectState;
    std::vector<TransitionRule> transitions;
};

struct Configuration {
    std::size_t step = 0;
    std::string state;
    std::int64_t headPosition = 0;
    char scannedSymbol = '_';
    std::map<std::int64_t, char> nonblankTape;
};

struct SimulationResult {
    SimulationStatus status = SimulationStatus::INVALID_MACHINE;
    std::size_t stepsExecuted = 0;
    std::string currentState;
    std::int64_t headPosition = 0;
    std::map<std::int64_t, char> finalNonblankTape;
    std::string message;
    std::vector<Configuration> trace;
};

using TransitionKey = std::pair<std::string, char>;
using TransitionIndex = std::map<TransitionKey, const TransitionRule*>;

std::string statusToString(SimulationStatus status) {
    switch (status) {
        case SimulationStatus::ACCEPTED:
            return "ACCEPTED";
        case SimulationStatus::REJECTED:
            return "REJECTED";
        case SimulationStatus::HALTED_NO_TRANSITION:
            return "HALTED_NO_TRANSITION";
        case SimulationStatus::STEP_LIMIT:
            return "STEP_LIMIT";
        case SimulationStatus::INVALID_INPUT:
            return "INVALID_INPUT";
        case SimulationStatus::INVALID_MACHINE:
            return "INVALID_MACHINE";
    }
    return "UNKNOWN";
}

char readTape(const std::map<std::int64_t, char>& tape,
              std::int64_t position,
              char blankSymbol) {
    const auto it = tape.find(position);
    return (it == tape.end()) ? blankSymbol : it->second;
}

void writeTape(std::map<std::int64_t, char>& tape,
               std::int64_t position,
               char symbol,
               char blankSymbol) {
    if (symbol == blankSymbol) {
        tape.erase(position);
    } else {
        tape[position] = symbol;
    }
}

Configuration makeConfiguration(std::size_t step,
                                const std::string& state,
                                std::int64_t headPosition,
                                const std::map<std::int64_t, char>& tape,
                                char blankSymbol) {
    Configuration config;
    config.step = step;
    config.state = state;
    config.headPosition = headPosition;
    config.scannedSymbol = readTape(tape, headPosition, blankSymbol);
    config.nonblankTape = tape;
    return config;
}

bool validateMachine(const Machine& machine,
                     TransitionIndex& index,
                     std::string& error) {
    index.clear();

    if (machine.states.empty()) {
        error = "The machine must define at least one state.";
        return false;
    }
    if (machine.tapeAlphabet.empty()) {
        error = "The tape alphabet must not be empty.";
        return false;
    }
