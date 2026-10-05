#include "tm.h"
#include <sstream>

std::string statusToString(Status status) {
    switch (status) {
        case Status::ACCEPTED: return "ACCEPTED";
        case Status::REJECTED: return "REJECTED";
        case Status::HALTED_NO_TRANSITION: return "HALTED_NO_TRANSITION";
        case Status::STEP_LIMIT: return "STEP_LIMIT";
        case Status::INVALID_INPUT: return "INVALID_INPUT";
        case Status::INVALID_MACHINE: return "INVALID_MACHINE";
    }
    return "UNKNOWN";
}

bool addTransition(Machine& m, std::string from, char read,
                   std::string to, char write, Move mv) {
    const auto added = m.transitions.emplace(
        std::make_pair(from, read), Transition{to, write, mv});
    if (!added.second) m.hasDuplicate = true;
    return added.second;
}

char readCell(const std::map<long long, char>& tape, long long pos, char blank) {
    const auto it = tape.find(pos);
    return it == tape.end() ? blank : it->second;
}

void writeCell(std::map<long long, char>& tape, long long pos, char sym, char blank) {
    if (sym == blank) tape.erase(pos);
    else tape[pos] = sym;
}

bool validateMachine(const Machine& m) {
    if (m.states.empty() || m.tapeAlphabet.empty() || m.hasDuplicate) return false;
    if (!m.tapeAlphabet.count(m.blank) || m.inputAlphabet.count(m.blank)) return false;
    for (char c : m.inputAlphabet)
        if (!m.tapeAlphabet.count(c)) return false;
    if (!m.states.count(m.startState) || !m.states.count(m.acceptState) ||
        !m.states.count(m.rejectState)) return false;
    if (m.startState == m.acceptState || m.startState == m.rejectState ||
        m.acceptState == m.rejectState) return false;
    for (const auto& entry : m.transitions) {
        const auto& key = entry.first;
        const auto& rule = entry.second;
        if (!m.states.count(key.first) || !m.states.count(rule.nextState) ||
            !m.tapeAlphabet.count(key.second) || !m.tapeAlphabet.count(rule.writeSymbol))
            return false;
        if (rule.move != Move::Left && rule.move != Move::Right) return false;
    }
    return true;
}

static void recordTrace(SimulationResult& r, char blank) {
    std::ostringstream out;
    out << "step=" << r.steps << " state=" << r.state << " head=" << r.head
        << " scan='" << readCell(r.tape, r.head, blank) << "' tape={";
    bool first = true;
    for (const auto& cell : r.tape) {
        if (!first) out << ", ";
        first = false;
        out << cell.first << ":'" << cell.second << "'";
    }
    out << '}';
    r.trace.push_back(out.str());
}

SimulationResult simulate(const Machine& m, const std::string& input,
                          std::size_t stepLimit, bool captureTrace) {
    SimulationResult r;
    r.state = m.startState;
    if (!validateMachine(m)) {
        r.message = "Invalid machine definition.";
        return r;
    }
    for (char c : input) {
        if (!m.inputAlphabet.count(c)) {
            r.status = Status::INVALID_INPUT;
            r.message = "Input symbol is outside the input alphabet.";
            return r;
        }
    }
    for (std::size_t i = 0; i < input.size(); ++i)
        writeCell(r.tape, static_cast<long long>(i), input[i], m.blank);
    if (captureTrace) recordTrace(r, m.blank);
    while (true) {
        if (r.state == m.acceptState) {
            r.status = Status::ACCEPTED;
            r.message = "Entered accept state.";
            break;
        }
        if (r.state == m.rejectState) {
            r.status = Status::REJECTED;
            r.message = "Entered reject state.";
            break;
        }
        if (r.steps >= stepLimit) {
            r.status = Status::STEP_LIMIT;
            r.message = "Transition limit reached.";
            break;
        }
        const auto it = m.transitions.find(
            std::make_pair(r.state, readCell(r.tape, r.head, m.blank)));
        if (it == m.transitions.end()) {
            r.status = Status::HALTED_NO_TRANSITION;
            r.message = "No transition for the current state and symbol.";
            break;
        }
        const Transition& rule = it->second;
        writeCell(r.tape, r.head, rule.writeSymbol, m.blank);
        r.head += rule.move == Move::Left ? -1 : 1;
        r.state = rule.nextState;
        ++r.steps;
        if (captureTrace) recordTrace(r, m.blank);
    }
    return r;
}
