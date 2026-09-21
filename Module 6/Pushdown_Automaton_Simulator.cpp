#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

struct Transition {
    std::string current_state;
    char input_symbol;  // '\0' means epsilon
    char required_top;
    std::string next_state;
    std::string replacement;
};

struct NPDA {
    std::set<std::string> states;
    std::set<char> input_alphabet;
    std::set<char> stack_alphabet;
    std::string start_state;
    char start_stack_symbol;
    std::set<std::string> accepting_states;
    std::vector<Transition> transitions;
};

struct Configuration {
    std::string state;
    size_t input_position;
    std::string stack;
};

enum class ResultType {
    Accepted,
    Rejected,
    InvalidPDA,
    InvalidInput,
    LimitExceeded
};

struct Limits {
    size_t max_steps = 200;
    size_t max_visited = 200000;
    size_t max_stack_size = 2000;
};

struct SimulationResult {
    ResultType result;
    std::string reason;
    std::vector<Transition> trace;
    size_t number_visited;
};

struct Branch {
    Configuration current;
    size_t steps;
    std::vector<Transition> trace;
};
using ConfigurationKey = std::tuple<std::string, size_t, std::string>;

std::string validate_npda(const NPDA& machine);
std::string validate_input(const NPDA& machine, const std::string& input_string);
bool transition_applies(const Transition& transition, const Configuration& current, const std::string& input_string);
Configuration use_transition(const Transition& transition, const Configuration& current);
SimulationResult run_npda(const NPDA& machine, const std::string& input_string, const Limits& limits = Limits());
std::string result_name(ResultType result);
void print_result(const SimulationResult& result);
NPDA make_zero_n_one_n_npda();
NPDA make_palindrome_npda();
NPDA make_duplicate_transition_npda();
NPDA make_full_input_npda();
NPDA make_epsilon_loop_npda();
NPDA make_stack_limit_npda();
NPDA make_test_machine(int group, const std::string& test_name);
bool read_expected_result(const std::string& text, ResultType& result);
int run_tests(const std::string& file_name);

int main(int argc, char* argv[]) {
    if (argc >= 2 && std::string(argv[1]) == "--test") {
        if (argc > 3) {
            std::cerr << "Usage: Pushdown_Automaton_Simulator --test [test_file]\n";
            return 1;
        }
        std::string test_file = "Pushdown_Automaton_Simulator_Tests.txt";
        if (argc == 3) test_file = argv[2];
        return run_tests(test_file);
    }
    if (argc > 2) {
        std::cerr << "Usage: Pushdown_Automaton_Simulator [input_string]\n";
        return 1;
    }
    std::string input_string;
    if (argc == 2) {
        input_string = argv[1];
    }
    else {
        std::cout << "Enter a string for the 0^n1^n NPDA (use EPS for empty): ";
        std::cin >> input_string;
    }

    if (input_string == "EPS" || input_string == "eps") input_string = "";
    NPDA machine = make_zero_n_one_n_npda();
    SimulationResult result = run_npda(machine, input_string);
    print_result(result);
    return 0;
}

// Checks the NPDA for errors
std::string validate_npda(const NPDA& machine) {
    if (machine.states.count(machine.start_state) == 0) return "Start state is undefined.";
    if (machine.stack_alphabet.count(machine.start_stack_symbol) == 0) return "Start stack symbol is not in the stack alphabet.";

    for (const std::string& state : machine.accepting_states) {
        if (machine.states.count(state) == 0) return "Accepting state is undefined: " + state;
    }

    // Checking every transition in case more than one applies
    for (const Transition& transition : machine.transitions) {
        if (machine.states.count(transition.current_state) == 0) return "Undefined current state: " + transition.current_state;
        if (machine.states.count(transition.next_state) == 0) return "Undefined next state: " + transition.next_state;

        if (transition.input_symbol != '\0' && machine.input_alphabet.count(transition.input_symbol) == 0) {
            return "Transition input symbol is not in the input alphabet.";
        }

        if (machine.stack_alphabet.count(transition.required_top) == 0) {
            return "Required stack symbol is not in the stack alphabet.";
        }

        for (char symbol : transition.replacement) {
            if (machine.stack_alphabet.count(symbol) == 0) return "Replacement symbol is not in the stack alphabet.";
        }
    }

    return "";
}

// Checks the input string
std::string validate_input(const NPDA& machine, const std::string& input_string) {
    for (char symbol : input_string) {
        if (machine.input_alphabet.count(symbol) == 0) return "Input contains a symbol outside the input alphabet.";
    }
    return "";
}

// Checks if a transition can be used
bool transition_applies(const Transition& transition, const Configuration& current, const std::string& input_string) {
    if (transition.current_state != current.state) return false;
    if (current.stack.empty()) return false;
    if (transition.required_top != current.stack.back()) return false;
    if (transition.input_symbol == '\0') return true;
    if (current.input_position >= input_string.size()) return false;

    return input_string[current.input_position] == transition.input_symbol;
}

// Uses a transition and returns the new configuration
Configuration use_transition(const Transition& transition, const Configuration& current) {
    Configuration next = current;
    next.state = transition.next_state;

    if (transition.input_symbol != '\0') next.input_position++;

    next.stack.pop_back();

    // The last character in the string is the top of the stack
    for (auto symbol = transition.replacement.rbegin(); symbol != transition.replacement.rend(); symbol++) {
        next.stack.push_back(*symbol);
    }

    return next;
}

// Runs the NPDA
SimulationResult run_npda(const NPDA& machine, const std::string& input_string, const Limits& limits) {
    std::string error = validate_npda(machine);
    if (!error.empty()) return { ResultType::InvalidPDA, error, {}, 0 };

    error = validate_input(machine, input_string);
    if (!error.empty()) return { ResultType::InvalidInput, error, {}, 0 };

    if (limits.max_visited == 0) {
        return { ResultType::LimitExceeded,"Maximum number of configurations is zero.", {}, 0 };
    }

    if (limits.max_stack_size < 1) {
        return { ResultType::LimitExceeded, "Initial stack is larger than the stack limit.", {}, 0 };
    }

    std::vector<Branch> branches;
    std::set<ConfigurationKey> visited;

    Configuration first = { machine.start_state, 0, std::string(1, machine.start_stack_symbol) };
    branches.push_back({ first, 0, {} });
    visited.insert({ first.state, first.input_position, first.stack });

    bool limit_reached = false;
    std::string limit_reason;
    size_t current_branch = 0;

    while (current_branch < branches.size()) {
        Branch branch = branches[current_branch++];
        Configuration current = branch.current;

        if (current.input_position == input_string.size() && machine.accepting_states.count(current.state) > 0) {
            return { ResultType::Accepted, "The input was used in an accepting state.", branch.trace, visited.size() };
        }

        for (const Transition& transition : machine.transitions) {
            if (!transition_applies(transition, current, input_string)) continue;

            if (branch.steps >= limits.max_steps) {
                limit_reached = true;
                if (limit_reason.empty()) limit_reason = "A branch reached the transition step limit.";
                continue;
            }

            Configuration next = use_transition(transition, current);

            if (next.stack.size() > limits.max_stack_size) {
                limit_reached = true;
                if (limit_reason.empty()) limit_reason = "A branch reached the stack size limit.";
                continue;
            }

            ConfigurationKey key = { next.state, next.input_position, next.stack };
            if (visited.count(key) > 0) continue;

            if (visited.size() >= limits.max_visited) {
                limit_reached = true;
                if (limit_reason.empty()) limit_reason = "The visited configuration limit was reached.";
                continue;
            }

            visited.insert(key);
            Branch new_branch = branch;
            new_branch.current = next;
            new_branch.steps++;
            new_branch.trace.push_back(transition);
            branches.push_back(new_branch);
        }
    }

    if (limit_reached) return { ResultType::LimitExceeded, limit_reason, {}, visited.size() };

    return { ResultType::Rejected,"Every possible branch stopped without accepting.", {}, visited.size() };
}

// Returns the result as text
std::string result_name(ResultType result) {
    if (result == ResultType::Accepted) return "Accepted";
    if (result == ResultType::Rejected) return "Rejected";
    if (result == ResultType::InvalidPDA) return "Invalid PDA";
    if (result == ResultType::InvalidInput) return "Invalid input";
    return "Limit exceeded";
}

// Prints the result and the accepting trace
void print_result(const SimulationResult& result) {
    std::cout << "\nResult:" << result_name(result.result) << '\n';
    std::cout << "Reason: " << result.reason << '\n';
    std::cout << "Configurations Visited: " << result.number_visited << '\n';

    if (result.result != ResultType::Accepted) return;

    std::cout << "\nAccepting Trace:\n";

    if (result.trace.empty()) {
        std::cout << "The starting configuration is accepting.\n";
        return;
    }

    for (const Transition& transition : result.trace) {
        std::string input = transition.input_symbol == '\0' ? "eps" : std::string(1, transition.input_symbol);
        std::string replacement = transition.replacement.empty() ? "eps" : transition.replacement;

        std::cout << "(" << transition.current_state << ", " << input << ", " << transition.required_top << ") -> (";
        std::cout << transition.next_state << ", " << replacement << ")\n";
    }
}

// Makes the NPDA for 0^n1^n
NPDA make_zero_n_one_n_npda() {
    NPDA machine;
    machine.states = { "q_push", "q_pop", "q_accept" };
    machine.input_alphabet = { '0', '1' };
    machine.stack_alphabet = { '0', 'Z' };
    machine.start_state = "q_push";
    machine.start_stack_symbol = 'Z';
    machine.accepting_states = { "q_accept" };

    machine.transitions.push_back({ "q_push", '0', 'Z', "q_push", "0Z" });
    machine.transitions.push_back({ "q_push", '0', '0', "q_push", "00" });
    machine.transitions.push_back({ "q_push", '\0', 'Z', "q_pop", "Z" });
    machine.transitions.push_back({ "q_push", '\0', '0', "q_pop", "0" });
    machine.transitions.push_back({ "q_pop", '1', '0', "q_pop", "" });
    machine.transitions.push_back({ "q_pop", '\0', 'Z', "q_accept", "Z" });

    return machine;
}

// Makes the NPDA for even length binary palindromes
NPDA make_palindrome_npda() {
    NPDA machine;
    machine.states = { "q_push", "q_match", "q_accept" };
    machine.input_alphabet = { '0', '1' };
    machine.stack_alphabet = { '0', '1', 'Z' };
    machine.start_state = "q_push";
    machine.start_stack_symbol = 'Z';
    machine.accepting_states = { "q_accept" };

    for (char top : machine.stack_alphabet) {
        machine.transitions.push_back({ "q_push", '0', top, "q_push", std::string("0") + top });
        machine.transitions.push_back({ "q_push", '1', top, "q_push", std::string("1") + top });
        machine.transitions.push_back({ "q_push", '\0', top, "q_match", std::string(1, top) });
    }

    machine.transitions.push_back({ "q_match", '0', '0', "q_match", "" });
    machine.transitions.push_back({ "q_match", '1', '1', "q_match", "" });
    machine.transitions.push_back({ "q_match", '\0', 'Z', "q_accept", "Z" });

    return machine;
}

// Makes the NPDA with duplicate transitions
NPDA make_duplicate_transition_npda() {
    NPDA machine;
    machine.states = { "q0", "q_dead", "q_accept" };
    machine.input_alphabet = { 'a' };
    machine.stack_alphabet = { 'Z' };
    machine.start_state = "q0";
    machine.start_stack_symbol = 'Z';
    machine.accepting_states = { "q_accept" };
    machine.transitions.push_back({ "q0", 'a', 'Z', "q_dead", "Z" });
    machine.transitions.push_back({ "q0", 'a', 'Z', "q_accept", "Z" });
    return machine;
}

// Makes the NPDA used to test the whole input
NPDA make_full_input_npda() {
    NPDA machine;
    machine.states = {"q0", "q_accept" };
    machine.input_alphabet = { 'a'};
    machine.stack_alphabet = {'Z'};
    machine.start_state = "q0";
    machine.start_stack_symbol ='Z';
    machine.accepting_states = { "q_accept" };
    machine.transitions.push_back({ "q0", 'a', 'Z', "q_accept", "Z" });
    return machine;
}

// Makes the NPDA with an epsilon loop
NPDA make_epsilon_loop_npda() {
    NPDA machine;
    machine.states = { "q0", "q_accept" };
    machine.input_alphabet = {'a' };
    machine.stack_alphabet = {'Z' };
    machine.start_state = "q0";
    machine.start_stack_symbol = 'Z';
    machine.accepting_states = { "q_accept" };
    machine.transitions.push_back({ "q0", '\0', 'Z', "q0", "Z" });
    machine.transitions.push_back({ "q0", 'a', 'Z', "q_accept", "Z" });
    return machine;
}

// Makes the NPDA used to test the stack limit
NPDA make_stack_limit_npda() {
    NPDA machine;
    machine.states = { "q0" };
    machine.input_alphabet = {};
    machine.stack_alphabet = {'A','Z'};
    machine.start_state = "q0";
    machine.start_stack_symbol = 'Z';
    machine.accepting_states = {};
    machine.transitions.push_back({ "q0", '\0', 'Z', "q0", "AZ" });
    machine.transitions.push_back({ "q0", '\0', 'A', "q0", "AA" });
    return machine;
}

// Chooses the machine needed for a test
NPDA make_test_machine(int group, const std::string& test_name) {
    if (group == 1) return make_zero_n_one_n_npda();
    if (group == 2) return make_palindrome_npda();
    if (group == 3) return make_duplicate_transition_npda();
    if (group == 4) return make_full_input_npda();

    if (group == 5) {
        NPDA machine = make_full_input_npda();
        if (test_name == "test2") machine.transitions[0].next_state = "q_missing";
        if (test_name == "test3") machine.start_stack_symbol = 'X';
        return machine;
    }
    if (group == 6) return make_epsilon_loop_npda();
    return make_stack_limit_npda();
}

// Converts a result from the text file
bool read_expected_result(const std::string& text, ResultType& result) {
    if (text == "Accepted") result = ResultType::Accepted;
    else if (text == "Rejected") result = ResultType::Rejected;
    else if (text == "InvalidPDA") result = ResultType::InvalidPDA;
    else if (text == "InvalidInput") result = ResultType::InvalidInput;
    else if (text == "LimitExceeded") result = ResultType::LimitExceeded;
    else return false;
    return true;
}

// Reads and runs all tests from the text file
int run_tests(const std::string& file_name) {
    std::ifstream input(file_name);
    if (!input) {
        std::cerr << "Unable to open test file: " << file_name << '\n';
        return 1;
    }

    std::string line;
    size_t line_number = 0;
    size_t passed = 0;
    size_t total = 0;

    while (std::getline(input, line)) {
        line_number++;
        if (line.empty() || line[0] == '#') continue;

        std::istringstream row(line);
        int group;
        std::string test_name;
        std::string input_string;
        std::string expected_text;
        size_t max_stack = 2000;
        std::string extra;

        if (!(row >> group >> test_name >> input_string >> expected_text)) {
            std::cerr << "Invalid test on line " << line_number << ".\n";
            return 1;
        }

        if (row >> max_stack) {
            if (row >> extra) {
                std::cerr << "Invalid test on line " << line_number << ".\n";
                return 1;
            }
        }
        else {
            row.clear();
            if (row >> extra) {
                std::cerr << "Invalid test on line " << line_number << ".\n";
                return 1;
            }
        }

        if (group < 1 || group > 7) {
            std::cerr << "Invalid group on line " << line_number << ".\n";
            return 1;
        }

        if (group == 5 && test_name != "test1" && test_name != "test2" && test_name != "test3") {
            std::cerr << "Invalid Group 5 test on line " << line_number << ".\n";
            return 1;
        }

        ResultType expected;
        if (!read_expected_result(expected_text, expected)) {
            std::cerr << "Invalid result on line " << line_number << ".\n";
            return 1;
        }

        if (input_string == "EPS" || input_string =="eps") input_string = "";

        NPDA machine = make_test_machine(group, test_name);
        Limits limits;
        limits.max_stack_size = max_stack;
        SimulationResult actual = run_npda(machine, input_string, limits);
        bool correct = actual.result == expected;
        total++;
        if (correct) passed++;

        std::cout << (correct ? "PASS ": "FAIL ");
        std::cout << "Group " << group << " - " << test_name;
        std::cout << " - Expected: " << result_name(expected);
        std::cout << " - Actual: "<< result_name(actual.result) << '\n';
    }
    std::cout << "\nRequired Test Results: " << passed << "/" << total << " passed.\n";
    return passed == total ? 0 : 1;
}
