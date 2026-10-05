#pragma once
#include <cstddef>
#include <string>
#include <utility>
#include <iostream>
#include <set>
#include <map>
#include <vector>

enum class Move { Left, Right };
 
struct Transition {
    std::string nextState;
    char writeSymbol;
    Move move;
};
 
struct Machine {
    std::set<std::string> states;
    std::set<char> inputAlphabet;
    std::set<char> tapeAlphabet;
    char blank = '_';
    std::string startState;
    std::string acceptState;
    std::string rejectState;
    std::map<std::pair<std::string, char>, Transition> transitions;
    bool hasDuplicate = false;
};

enum class Status {ACCEPTED, REJECTED, HALTED_NO_TRANSITION, STEP_LIMIT, INVALID_INPUT, INVALID_MACHINE};

struct SimulationResult {
    Status status = Status::INVALID_MACHINE;
    std::size_t steps = 0;
    std::string state;
    long long head = 0;
    std::map<long long, char> tape;
    std::string message;
    std::vector<std::string> trace;
};

std::string statusToString(Status status);
bool addTransition(Machine& m, std::string from, char read, std::string to, char write, Move mv);
char readCell(const std::map<long long, char> &tape, long long pos, char blank);
void writeCell(std::map<long long, char> &tape, long long pos, char sym, char blank);
bool validateMachine(const Machine& m);
SimulationResult simulate(const Machine& m, const std::string& input,
                          std::size_t stepLimit, bool captureTrace = false);
