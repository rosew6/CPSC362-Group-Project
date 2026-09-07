#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct DFA {
    std::set<std::string> states;
    std::set<char> alphabet;
    std::string start_state;
    std::set<std::string> accepting_states;
    std::map<std::pair<std::string, char>, std::string> transitions;
};

using StatePair = std::pair<std::string, std::string>;

DFA read_dfa(std::istream& input);
void validate_dfa(const DFA& dfa);
std::set<std::string> find_reachable(const DFA& dfa);
std::set<StatePair> table_filling(const DFA& dfa, const std::set<std::string>& reachable);
std::vector<std::set<std::string>> build_equivalence_classes(const DFA& dfa, const std::set<std::string>& reachable, const std::set<StatePair>& distinguishable);
DFA build_minimized_dfa(const DFA& dfa, const std::vector<std::set<std::string>>& classes);
bool accepts(const DFA& dfa, const std::string& input_string);
void print_dfa(const DFA& dfa);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: dfa_minimizer <dfa-file>\n";
        return 1;
    }

    try {
        std::ifstream input(argv[1]);
        if (!input) throw std::runtime_error("Unable to open DFA file.");

        DFA original = read_dfa(input);
        validate_dfa(original);
        std::set<std::string> reachable = find_reachable(original);

        std::cout << "\nReachable states: ";
        bool first = true;
        for (const std::string& state : reachable) {
            if (!first) std::cout << ", ";
            std::cout << state;
            first = false;
        }
        std::cout << "\nRemoved states: ";
        first = true;
        bool removed = false;
        for (const std::string& state : original.states) {
            if (reachable.count(state) == 0) {
                if (!first) std::cout << ", ";
                std::cout << state;
                first = false;
                removed = true;
            }
        }
        if (!removed) std::cout << "None";
        std::cout << '\n';
        std::set<StatePair> distinguishable = table_filling(original, reachable);
        std::cout << "\nEquivalent pairs:\n";
        std::vector<std::string> reachable_states(reachable.begin(), reachable.end());
        bool pair_found = false;
        for (size_t i = 0; i < reachable_states.size(); i++) {
            for (size_t j = i + 1; j < reachable_states.size(); j++) {
                StatePair pair = { reachable_states[i], reachable_states[j] };
                if (distinguishable.count(pair) == 0) {
                    std::cout << "(" << pair.first << ", " << pair.second << ")\n";
                    pair_found = true;
                }
            }
        }
        if (!pair_found) std::cout << "None\n";
        std::vector<std::set<std::string>> classes = build_equivalence_classes(original, reachable, distinguishable);
        std::cout << "\nEquivalent classes:\n";
        for (const std::set<std::string>& group : classes) {
            std::cout << "{";
            first = true;
            for (const std::string& state : group) {
                if (!first) std::cout << ", ";
                std::cout << state;
                first = false;
            }
            std::cout << "}\n";
        }

        DFA minimized = build_minimized_dfa(original, classes);
        print_dfa(minimized);
        std::string test_string;
        bool all_match = true;
        bool tests_found = false;
        std::cout << "\nTest Strings:\n";
        while (input >> test_string) {
            tests_found = true;
            std::string actual_string = test_string;
            if (test_string == "EPS") actual_string = "";
            bool original_result = accepts(original, actual_string);
            bool minimized_result = accepts(minimized, actual_string);
            std::cout << test_string << " -> Original: " << (original_result ? "Accepted" : "Rejected") << ", Minimized: " << (minimized_result ? "Accepted" : "Rejected");
            if (original_result == minimized_result) std::cout << " - Match\n";
            else {
                std::cout << " - Does Not Match\n";
                all_match = false;
            }
        }
        if (!tests_found) std::cout << "No test strings found.\n";
        else if (all_match) std::cout << "\nAll test results match.\n";
        else std::cout << "\nSome test results do not match.\n";
    }
    catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}

// Reads the DFA from the file
DFA read_dfa(std::istream& input) {
    DFA dfa;
    int number_of_states, number_of_symbols;
    if (!(input >> number_of_states >> number_of_symbols)) throw std::runtime_error("Could not read number of states and symbols.");
    if (number_of_states <= 0) throw std::runtime_error("DFA must have at least one state.");
    if (number_of_symbols <= 0) throw std::runtime_error("DFA must have at least one symbol.");

    // Reads states
    for (int i = 0; i < number_of_states; i++) {
        std::string state;
        if (!(input >> state)) throw std::runtime_error("Not enough states.");
        if (dfa.states.count(state) > 0) throw std::runtime_error("Duplicate state: " + state);
        dfa.states.insert(state);
    }
    std::vector<char> alphabet_order;

    // Reads alphabet
    for (int i = 0; i < number_of_symbols; i++) {
        char symbol;
        if (!(input >> symbol)) throw std::runtime_error("Not enough symbols.");
        if (dfa.alphabet.count(symbol) > 0) throw std::runtime_error("Duplicated alphabet symbol.");
        dfa.alphabet.insert(symbol);
        alphabet_order.push_back(symbol);
    }
    if (!(input >> dfa.start_state)) throw std::runtime_error("Start state was not provided.");
    int number_of_accepting_states;
    if (!(input >> number_of_accepting_states)) throw std::runtime_error("Could not read accepting stats.");
    if (number_of_accepting_states < 0 || number_of_accepting_states > number_of_states) throw std::runtime_error("Invalid number of accepting states.");

    // Reads accepting states
    for (int i = 0; i < number_of_accepting_states; i++) {
        std::string state;
        if (!(input >> state)) throw std::runtime_error("Not enough accepting states.");
        if (dfa.accepting_states.count(state) > 0) throw std::runtime_error("Duplicated accepting state: " + state);
        dfa.accepting_states.insert(state);
    }

    std::set<std::string> rows_read;

    // Reads transition table
    for (int i = 0; i < number_of_states; i++) {
        std::string current_state;
        if (!(input >> current_state)) throw std::runtime_error("Missing transition row.");
        if (dfa.states.count(current_state) == 0) throw std::runtime_error("Undefined state in transition table: " + current_state);
        if (rows_read.count(current_state) > 0) throw std::runtime_error("Duplicate transition row: " + current_state);
        rows_read.insert(current_state);
        for (char symbol : alphabet_order) {
            std::string destination;
            if (!(input >> destination)) throw std::runtime_error("Incomplete transition row: " + current_state);
            dfa.transitions[{current_state, symbol}] = destination;
        }
    }
    return dfa;
}

// Checks DFA for errors
void validate_dfa(const DFA& dfa) {
    if (dfa.states.empty()) throw std::runtime_error("DFA has no states.");
    if (dfa.alphabet.empty()) throw std::runtime_error("DFA alphabet is empty.");
    if (dfa.states.count(dfa.start_state) == 0) throw std::runtime_error("Start state is undefined.");
    for (const std::string& state : dfa.accepting_states) {
        if (dfa.states.count(state) == 0) throw std::runtime_error("Accrpting state in undefined " + state);
    }
    for (const std::string& state : dfa.states) {
        for (char symbol : dfa.alphabet) {
            auto transition = dfa.transitions.find({ state, symbol });
            if (transition == dfa.transitions.end()) throw std::runtime_error("Missing transition from state " + state);
            if (dfa.states.count(transition->second) == 0) throw std::runtime_error("Undefined destination state: " + transition->second);
        }
    }
}

// Finds reachable states
std::set<std::string> find_reachable(const DFA& dfa) {
    std::set<std::string> reachable;
    std::vector<std::string> states_to_check;
    reachable.insert(dfa.start_state);
    states_to_check.push_back(dfa.start_state);

    size_t current = 0;
    while (current < states_to_check.size()) {
        std::string state = states_to_check[current++];
        for (char symbol : dfa.alphabet) {
            std::string next_state = dfa.transitions.at({ state, symbol });
            if (reachable.insert(next_state).second) states_to_check.push_back(next_state);
        }
    }
    return reachable;
}

// Marks distinguishable state pairs
std::set<StatePair> table_filling(const DFA& dfa, const std::set<std::string>& reachable) {
    std::set<StatePair> distinguishable;
    std::vector<std::string> states(reachable.begin(), reachable.end());

    // Marks accepting and non-accepting pairs
    for (size_t i = 0; i < states.size(); i++) {
        for (size_t j = i + 1; j< states.size(); j++) {
            bool first_accepting = dfa.accepting_states.count(states[i]) > 0;
            bool second_accepting = dfa.accepting_states.count(states[j]) > 0;
            if (first_accepting != second_accepting) distinguishable.insert({ states[i], states[j] });
        }
    }

    // Checks the remaining pairs
    bool changed = true;
    while (changed) {
        changed = false;
        for (size_t i = 0; i < states.size(); i++) {
            for (size_t j = i +1; j < states.size(); j++) {
                StatePair current_pair = { states[i], states[j] };
                if (distinguishable.count(current_pair) > 0) continue;

                for (char symbol : dfa.alphabet) {
                    std::string first_next = dfa.transitions.at({ states[i], symbol });
                    std::string second_next = dfa.transitions.at({ states[j], symbol });
                    if (first_next == second_next) continue;
                    StatePair next_pair;
                    if (first_next < second_next) next_pair = { first_next, second_next };
                    else next_pair = { second_next, first_next };
                    if (distinguishable.count(next_pair) > 0) {
                        distinguishable.insert(current_pair);
                        changed = true;
                        break;
                    }
                }
            }
        }
    }
    return distinguishable;
}

// Groups equivalent states
std::vector<std::set<std::string>> build_equivalence_classes(const DFA& dfa, const std::set<std::string>& reachable, const std::set<StatePair>& distinguishable) {
    std::vector<std::set<std::string>> classes;
    std::set<std::string> used;
    for (const std::string& state : reachable) {
        if (used.count(state) >0) continue;
        std::set<std::string> group;
        group.insert(state);
        used.insert(state);
        for (const std::string& other : reachable) {
            if (used.count(other) >0) continue;
            StatePair pair;
            if (state < other) pair = { state, other };
            else pair = { other, state };
            if (distinguishable.count(pair) == 0) {
                group.insert(other);
                used.insert(other);
            }
        }
        classes.push_back(group);
    }
    return classes;
}

// Builds minimized DFA
DFA build_minimized_dfa(const DFA& dfa, const std::vector<std::set<std::string>>& classes){
    DFA minimized;
    minimized.alphabet = dfa.alphabet;
    std::map<std::string, std::string> old_to_new;

    // Makes new states
    for (const std::set<std::string>& group : classes) {
        std::string new_name = "{";
        bool first = true;
        for (const std::string& state : group) {
            if (!first) new_name += ",";
            new_name += state;
            first = false;
        }
        new_name += "}";
        minimized.states.insert(new_name);
        for (const std::string& state : group) old_to_new[state] = new_name;
        for (const std::string& state : group) {
            if (dfa.accepting_states.count(state)> 0) {
                minimized.accepting_states.insert(new_name);
                break;
            }
        }
    }
    minimized.start_state = old_to_new.at(dfa.start_state);

    // Makes new transitions
    for (const std::set<std::string>& group : classes) {
        std::string state = *group.begin();
        std::string new_state = old_to_new.at(state);

        for (char symbol : dfa.alphabet) {
            std::string old_destination = dfa.transitions.at({ state, symbol });
            std::string new_destination = old_to_new.at(old_destination);
            minimized.transitions[{new_state, symbol}] = new_destination;
        }
    }
    return minimized;
}

// Tests a string on the DFA
bool accepts(const DFA& dfa, const std::string& input_string) {
    std::string current_state = dfa.start_state;
    for (char symbol : input_string) {
        if (dfa.alphabet.count(symbol) == 0) return false;
        auto transition = dfa.transitions.find({ current_state, symbol });
        if (transition == dfa.transitions.end()) return false;
        current_state = transition->second;
    }
    return dfa.accepting_states.count(current_state) > 0;
}

// Prints minimized DFA
void print_dfa(const DFA& dfa) {
    std::cout << "\nMinimized DFA\n";
    std::cout << "----------------------------\n";
    std::cout << "States: ";

    bool first = true;
    for (const std::string& state : dfa.states) {
        if (!first) std::cout << ",";
        std::cout << state;
        first = false;
    }
    std::cout << "\nStart State: " << dfa.start_state;
    std::cout << "\nAccepting States: ";
    first = true;
    for (const std::string& state : dfa.accepting_states) {
        if (!first) std::cout << ", ";
        std::cout << state;
        first = false;
    }
    if (dfa.accepting_states.empty()) std::cout << "None";
    std::cout << "\n\nTransition Table:\nState";
    for (char symbol : dfa.alphabet) std::cout << "\t" << symbol;
    std::cout << '\n';
    for (const std::string& state : dfa.states) {
        std::cout << state;
        for (char symbol : dfa.alphabet) std::cout << "\t" << dfa.transitions.at({ state, symbol });
        std::cout <<'\n';
    }
}