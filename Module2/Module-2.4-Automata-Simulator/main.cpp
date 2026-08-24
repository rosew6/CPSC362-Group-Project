#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <utility>

// Specifies whether the automaton is deterministic or nondeterministic.
enum class AutomataType {
    DFA,
    NFA
};

// Stores the complete definition of a DFA or NFA.
struct Automata {
    AutomataType type;
    std::set<std::string> states;
    std::set<char> alphabet;
    std::map<std::pair<std::string, char>,
             std::set<std::string>> transitions;
    std::string startState;
    std::set<std::string> acceptingStates;
};

// Reads an integer within a required range.
int readInteger(const std::string& prompt, int minimum, int maximum) {
    int value;

    while (true) {
        std::cout << prompt;

        if (std::cin >> value && value >= minimum && value <= maximum) {
            return value;
        }

        std::cout << "Invalid input. Enter a number from "
                  << minimum << " to " << maximum << ".\n";

        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Reads a state name and verifies that it exists.
std::string readExistingState(
    const std::set<std::string>& states,
    const std::string& prompt) {

    std::string state;

    while (true) {
        std::cout << prompt;
        std::cin >> state;

        if (states.count(state) > 0) {
            return state;
        }

        std::cout << "Unknown state. Enter one of the defined states.\n";
    }
}

// Prints a set of states in a readable format.
void printStateSet(const std::set<std::string>& states) {
    std::cout << "{";

    bool first = true;

    for (const std::string& state : states) {
        if (!first) {
            std::cout << ", ";
        }

        std::cout << state;
        first = false;
    }

    std::cout << "}";
}

// Collects and validates the complete automaton definition.
Automata inputAutomata() {
    Automata automata;
    std::string typeInput;

    while (true) {
        std::cout << "Enter automata type (DFA or NFA): ";
        std::cin >> typeInput;

        if (typeInput == "DFA" || typeInput == "dfa") {
            automata.type = AutomataType::DFA;
            break;
        }

        if (typeInput == "NFA" || typeInput == "nfa") {
            automata.type = AutomataType::NFA;
            break;
        }

        std::cout << "Invalid type. Enter DFA or NFA.\n";
    }

    int numberOfStates =
        readInteger("Enter number of states: ", 1, 100);

    std::cout << "Enter " << numberOfStates
              << " unique state names.\n";

    while (static_cast<int>(automata.states.size()) < numberOfStates) {
        std::string state;

        std::cout << "State "
                  << automata.states.size() + 1 << ": ";
        std::cin >> state;

        if (!automata.states.insert(state).second) {
            std::cout << "That state already exists.\n";
        }
    }

    int numberOfSymbols =
        readInteger("Enter number of alphabet symbols: ", 1, 50);

    std::cout << "Enter " << numberOfSymbols
              << " unique single-character symbols.\n";

    while (static_cast<int>(automata.alphabet.size()) <
           numberOfSymbols) {

        std::string symbolInput;

        std::cout << "Symbol "
                  << automata.alphabet.size() + 1 << ": ";
        std::cin >> symbolInput;

        if (symbolInput.size() != 1) {
            std::cout << "Enter exactly one character.\n";
            continue;
        }

        if (!automata.alphabet.insert(symbolInput[0]).second) {
            std::cout << "That symbol already exists.\n";
        }
    }

    automata.startState = readExistingState(
        automata.states, "Enter the start state: ");

    int numberOfAccepting = readInteger(
        "Enter number of accepting states: ",
        0,
        numberOfStates);

    while (static_cast<int>(automata.acceptingStates.size()) <
           numberOfAccepting) {

        std::string state = readExistingState(
            automata.states, "Enter an accepting state: ");

        if (!automata.acceptingStates.insert(state).second) {
            std::cout << "That accepting state was already entered.\n";
        }
    }

    std::cout << "\nDefine the transition table.\n";

    for (const std::string& currentState : automata.states) {
        for (char symbol : automata.alphabet) {
            std::pair<std::string, char> key =
                {currentState, symbol};

            if (automata.type == AutomataType::DFA) {
                std::string prompt =
                    "Transition from " + currentState +
                    " on '" + std::string(1, symbol) + "': ";

                std::string nextState =
                    readExistingState(automata.states, prompt);

                automata.transitions[key].insert(nextState);
            } else {
                std::string prompt =
                    "Number of transitions from " +
                    currentState + " on '" +
                    std::string(1, symbol) + "': ";

                int numberOfTargets = readInteger(
                    prompt, 0, numberOfStates);

                while (static_cast<int>(
                           automata.transitions[key].size()) <
                       numberOfTargets) {

                    std::string nextState = readExistingState(
                        automata.states,
                        "Enter a destination state: ");

                    if (!automata.transitions[key]
                             .insert(nextState).second) {
                        std::cout
                            << "That destination was already entered.\n";
                    }
                }
            }
        }
    }

    return automata;
}

// Verifies that every character belongs to the automaton's alphabet.
bool inputUsesAlphabet(
    const Automata& automata,
    const std::string& input) {

    for (char symbol : input) {
        if (automata.alphabet.count(symbol) == 0) {
            std::cout << "Input contains invalid symbol '"
                      << symbol << "'.\n";
            return false;
        }
    }

    return true;
}

// Processes a string deterministically and prints its transitions.
bool processDFA(
    const Automata& automata,
    const std::string& input) {

    std::string currentState = automata.startState;

    std::cout << "Start state: " << currentState << '\n';

    for (char symbol : input) {
        auto transition = automata.transitions.find(
            {currentState, symbol});

        if (transition == automata.transitions.end() ||
            transition->second.empty()) {

            std::cout << "No valid transition from "
                      << currentState << " on '" << symbol << "'.\n";
            return false;
        }

        std::string nextState = *transition->second.begin();

        std::cout << currentState << " --" << symbol
                  << "--> " << nextState << '\n';

        currentState = nextState;
    }

    std::cout << "Final state: " << currentState << '\n';

    return automata.acceptingStates.count(currentState) > 0;
}

// Processes all possible NFA states and prints each transition step.
bool processNFA(
    const Automata& automata,
    const std::string& input) {

    std::set<std::string> currentStates = {automata.startState};

    std::cout << "Start states: ";
    printStateSet(currentStates);
    std::cout << '\n';

    for (char symbol : input) {
        std::set<std::string> nextStates;

        for (const std::string& currentState : currentStates) {
            auto transition = automata.transitions.find(
                {currentState, symbol});

            if (transition != automata.transitions.end()) {
                nextStates.insert(
                    transition->second.begin(),
                    transition->second.end());
            }
        }

        std::cout << "On '" << symbol << "': ";
        printStateSet(currentStates);
        std::cout << " -> ";
        printStateSet(nextStates);
        std::cout << '\n';

        currentStates = nextStates;

        if (currentStates.empty()) {
            std::cout << "No active states remain.\n";
            return false;
        }
    }

    std::cout << "Final states: ";
    printStateSet(currentStates);
    std::cout << '\n';

    for (const std::string& state : currentStates) {
        if (automata.acceptingStates.count(state) > 0) {
            return true;
        }
    }

    return false;
}

int main() {
    std::cout << "Finite Automata Simulator\n";
    std::cout << "=========================\n\n";

    Automata automata = inputAutomata();

    // Additional feature: process multiple strings without rebuilding
    // the automaton. Enter QUIT to finish.
    while (true) {
        std::string input;

        std::cout
            << "\nEnter a string to process "
            << "(EMPTY for the empty string or QUIT to exit): ";

        std::cin >> input;

        if (input == "QUIT" || input == "quit") {
            break;
        }

        if (input == "EMPTY" || input == "empty") {
            input.clear();
        }

        if (!inputUsesAlphabet(automata, input)) {
            continue;
        }

        bool accepted;

        if (automata.type == AutomataType::DFA) {
            accepted = processDFA(automata, input);
        } else {
            accepted = processNFA(automata, input);
        }

        std::cout << "Result: String is "
                  << (accepted ? "Accepted" : "Not Accepted")
                  << ".\n";
    }

    std::cout << "\nSimulator closed.\n";

    return 0;
}
