#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <stack>
#include <stdexcept>

struct State {
    int id;
    bool is_accepting;
    std::map<char, std::set<int>> transitions;
};

struct NFA {
    std::vector<State> states;
    int start_state;
    int accepting_state;
};

NFA construct_nfa_from_regular_expression(
    const std::string& regular_expression);
NFA handle_union(NFA nfa1, NFA nfa2);
NFA handle_concatenation(NFA nfa1, NFA nfa2);
NFA handle_asterick(NFA nfa);

bool match_string(
    const NFA& nfa,
    const std::string& input_string);

int main() {
    std::string regular_expression;
    std::string input_string;
    std::cout << "Enter regular expression: ";
    std::cin >> regular_expression;
    std::cout << "Enter string to test: ";
    std::cin >> input_string;

    try {
        NFA nfa =
            construct_nfa_from_regular_expression(
                regular_expression);
        bool result =match_string(nfa, input_string);
        std::cout << "string is " << (result ? "Accepted" : "Not Accepted") << " by Regular Expression." << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    return 0;
}


NFA construct_nfa_from_regular_expression(
    const std::string& regular_expression) {
    if (regular_expression.empty()) {
        throw std::runtime_error("Invalid regular expression.");
    }
    std::string formatted_regular_expression;
    // Adds a period where concatenation happens
    for (size_t i = 0; i < regular_expression.size(); i++) {
        formatted_regular_expression += regular_expression[i];
        if (i + 1 < regular_expression.size()) {
            char current = regular_expression[i];
            char next = regular_expression[i + 1];
            bool current_is_character = current != '|' && current != '*' && current != '(' && current != ')' && current != '.';
            bool next_is_character = next != '|' && next != '*' && next != '(' && next != ')' && next != '.';
            if ((current_is_character || current == ')' || current == '*') && (next_is_character || next == '(')) {
                formatted_regular_expression += '.';
            }
        }
    }

    std::string postfix_expression;
    std::stack<char> operators;

    // Converts the regular expression
    for (char c : formatted_regular_expression) {
        bool is_character = c != '|' && c != '*' && c != '(' && c != ')' && c != '.';
        if (is_character) {
            postfix_expression += c;
        }
        else if (c == '(') {
            operators.push(c);
        }
        else if (c == ')') {
            while (!operators.empty() &&
                operators.top() != '(') {
                postfix_expression += operators.top();
                operators.pop();
            }
            if (operators.empty()) {
                throw std::runtime_error("Parentheses do not match.");
            }
            operators.pop();
        }
        else if (c == '*') {
            postfix_expression += c;
        }

        else if (c == '|' ||
            c == '.') {
            int current_priority;
            if (c == '|') {
                current_priority = 1;
            }
            else {
                current_priority = 2;
            }
            while (!operators.empty() &&
                operators.top() != '(') {
                int top_priority = 0;
                if (operators.top() == '|') {
                    top_priority = 1;
                }
                else if (operators.top() == '.') {
                    top_priority = 2;
                }
                if (top_priority <
                    current_priority) {
                    break;
                }
                postfix_expression +=
                    operators.top();
                operators.pop();
            }
            operators.push(c);
        }
    }
    while (!operators.empty()) {
        if (operators.top() == '(') {
            throw std::runtime_error(
                "Parentheses do not match.");
        }
        postfix_expression +=
            operators.top();
        operators.pop();
    }

    std::stack<NFA> nfa_stack;

    // Builds the NFA from the converted regular expression
    for (char c :postfix_expression) {
        bool is_character = c != '|' && c != '*' && c != '.';
        if (is_character) {
            NFA nfa;
            State state0;
            state0.id = 0;
            state0.is_accepting = false;
            State state1;
            state1.id = 1;
            state1.is_accepting = true;
            state0.transitions[c].insert(1);
            nfa.states.push_back(state0);
            nfa.states.push_back(state1);
            nfa.start_state = 0;
            nfa.accepting_state = 1;
            nfa_stack.push(nfa);
        }

        else if (c == '.') {
            if (nfa_stack.size() < 2) {
                throw std::runtime_error(
                    "error line 163.");
            }
            NFA nfa2 =
                nfa_stack.top();
            nfa_stack.pop();

            NFA nfa1 = nfa_stack.top();
            nfa_stack.pop();
            nfa_stack.push(handle_concatenation( nfa1,nfa2));
        }
        else if (c == '|') {
            if (nfa_stack.size() < 2) {
                throw std::runtime_error("Invalid union.");
            }
            NFA nfa2 = nfa_stack.top();
            nfa_stack.pop();
            NFA nfa1 = nfa_stack.top();
            nfa_stack.pop();
            nfa_stack.push(handle_union( nfa1, nfa2));
        }
        else if (c == '*') {
            if (nfa_stack.empty()) {
                throw std::runtime_error(
                    "Invalid asterick.");
            }
            NFA nfa = nfa_stack.top();
            nfa_stack.pop();
            nfa_stack.push(handle_asterick( nfa));
        }
    }
    if (nfa_stack.size() != 1) {
        throw std::runtime_error(
            "Invalid regular expression.");
    }
    return nfa_stack.top();
}


NFA handle_union(
    NFA nfa1,
    NFA nfa2) {
    NFA result;
    State start;
    start.id = 0;
    start.is_accepting = false;
    result.states.push_back(start);
    int offset1 = 1;
    for (State state :nfa1.states) {
        State new_state;
        new_state.id = state.id + offset1;
        new_state.is_accepting =false;
        for (auto transition :
            state.transitions) {
            for (int next :
            transition.second) {
                new_state.transitions[ transition.first].insert(next + offset1);
            }
        }
        result.states.push_back(
            new_state);
    }
    int offset2 =static_cast<int>(result.states.size());
    for (State state :nfa2.states) {
        State new_state;
        new_state.id =state.id + offset2;
        new_state.is_accepting = false;
        for (auto transition :
            state.transitions) {
            for (int next : transition.second) {
                new_state.transitions[transition.first].insert(next + offset2);
            }
        }
        result.states.push_back(
            new_state);
    }
    int accepting_id =static_cast<int>(result.states.size());
    State accepting_state;
    accepting_state.id =accepting_id;
    accepting_state.is_accepting =true;
    result.states.push_back(accepting_state);
    int start1 =nfa1.start_state + offset1;
    int accept1 =nfa1.accepting_state + offset1;
    int start2 =nfa2.start_state +offset2;
    int accept2 =nfa2.accepting_state +offset2;

    // using \0 for epsilon
    result.states[0].transitions['\0'].insert(start1);
    result.states[0].transitions['\0'].insert(start2);
    result.states[accept1].transitions['\0'].insert(accepting_id);
    result.states[accept2].transitions['\0'].insert(accepting_id);
    result.start_state = 0;
    result.accepting_state =accepting_id;
    return result;
}


NFA handle_concatenation(
    NFA nfa1,
    NFA nfa2) {
    NFA result;
    for (State state : nfa1.states) {
        state.is_accepting =false;
        result.states.push_back( state);
    }
    int offset =static_cast<int>(result.states.size());
    for (State state :nfa2.states) {
        State new_state;
        new_state.id =state.id + offset;
        new_state.is_accepting =state.is_accepting;
        for (auto transition :state.transitions) {

            for (int next :transition.second) {
                new_state.transitions[transition.first].insert(next + offset);
            }
        }
        result.states.push_back(new_state);
    }
    int first_accept =nfa1.accepting_state;
    int second_start =nfa2.start_state + offset;
    int second_accept =nfa2.accepting_state + offset;
    result.states[first_accept].transitions['\0'].insert(second_start);
    result.start_state =nfa1.start_state;
    result.accepting_state =second_accept;
    result.states[second_accept].is_accepting = true;
    return result;
}


NFA handle_asterick(
    NFA nfa) {
    NFA result;
    State start;
    start.id = 0;
    start.is_accepting = false;
    result.states.push_back(start);
    int offset = 1;
    for (State state :nfa.states) {
        State new_state;
        new_state.id = state.id + offset;
        new_state.is_accepting = false;
        for (auto transition : state.transitions) {
            for (int next : transition.second) {
                new_state.transitions[transition.first].insert(next + offset);
            }
        }
        result.states.push_back(new_state);
    }
    int accepting_id =static_cast<int>(result.states.size());
    State accepting_state;
    accepting_state.id =accepting_id;
    accepting_state.is_accepting =true;
    result.states.push_back(accepting_state);
    int old_start =nfa.start_state +offset;
    int old_accept =nfa.accepting_state +offset;

    // Can skip the expression completely
    result.states[0].transitions['\0'].insert(accepting_id);

    // Starts the expression
    result.states[0].transitions['\0'].insert(old_start);

    // Repeats the expression
    result.states[old_accept].transitions['\0'].insert(old_start);

    // Finishes the expression
    result.states[old_accept].transitions['\0'].insert(accepting_id);
   
    result.start_state = 0;
    result.accepting_state =accepting_id;
    return result;
}


bool match_string(const NFA& nfa,const std::string& input_string) {
    std::set<int> current_states;
    current_states.insert(nfa.start_state);
    // Follows epsilon transitions before input
    bool changed = true;
    while (changed) {
        changed = false;
        std::set<int> temp =current_states;
        for (int state : temp) {
            auto transition =nfa.states[state].transitions.find('\0');

            if (transition !=nfa.states[state].transitions.end()) {
                for (int next :transition->second) {
                    if (current_states.insert(next).second) {
                        changed = true;
                    }
                }
            }
        }
    }

    // Reads each character from the input string
    for (char c :input_string) {
        std::set<int> next_states;
        for (int state :current_states) {
            auto transition = nfa.states[state].transitions.find(c);
            if (transition != nfa.states[state].transitions.end()) {
                next_states.insert(transition->second.begin(),transition->second.end());
            }
        }
        current_states =next_states;
        // Follows epsilon transitions again
        changed = true;
        while (changed) {
            changed = false;
            std::set<int> temp =current_states;
            for (int state :
            temp) {
                auto transition =nfa.states[state].transitions.find('\0');
                if (transition !=nfa.states[state].transitions.end()) {
                    for (int next :transition->second) {
                        if (current_states.insert(next).second) {
                            changed = true;
                        }
                    }
                }
            }
        }

        if (current_states.empty()) {
            return false;
        }
    }

    return current_states.count(nfa.accepting_state ) > 0;
}