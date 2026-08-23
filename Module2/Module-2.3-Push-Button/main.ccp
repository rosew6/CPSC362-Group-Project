#include <iostream>
#include <string>

// Represents the two possible states of the push-button system.
enum class State {
    Off,
    On
};

// Determines the next state based on the current state and button input.
State transition(State currentState, bool buttonPressed) {
    if (buttonPressed) {
        return State::On;
    }

    return currentState;
}

// Converts a State value into text for console output.
std::string stateToString(State state) {
    return state == State::On ? "On" : "Off";
}

int main() {
    State currentState = State::Off;
    int buttonPressCount = 0;
    std::string input;

    std::cout << "Push-Button System Simulator\n";
    std::cout << "Enter 1 to press the button.\n";
    std::cout << "Enter 0 for no button press.\n";
    std::cout << "Enter q to quit the simulation.\n\n";
    std::cout << "Initial State: " << stateToString(currentState) << '\n';

    while (true) {
        std::cout << "\nInput (1, 0, or q): ";
        std::getline(std::cin, input);

        if (input == "q" || input == "Q") {
            break;
        }

        if (input != "0" && input != "1") {
            std::cout << "Invalid input. Please enter 1, 0, or q.\n";
            continue;
        }

        bool buttonPressed = (input == "1");

        if (buttonPressed) {
            ++buttonPressCount;
        }

        currentState = transition(currentState, buttonPressed);

        std::cout << "Current State: "
                  << stateToString(currentState) << '\n';
        std::cout << "Button Press Count: "
                  << buttonPressCount << '\n';
    }

    std::cout << "\nSimulation ended.\n";
    std::cout << "Total Button Presses: "
              << buttonPressCount << '\n';

    return 0;
}
