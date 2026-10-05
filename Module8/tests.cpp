#include "tm.h"

Machine makeValidMachine() {
    Machine m;
    m.states = {"q0", "q_accept", "q_reject"};
    m.inputAlphabet = {'0', '1'};
    m.tapeAlphabet = {'0', '1', 'B'};
    m.blank = 'B';
    m.startState = "q0";
    m.acceptState = "q_accept";
    m.rejectState = "q_reject";

    addTransition(m, "q0", '0', "q_accept", '0', Move::Right);
    addTransition(m, "q0", '1', "q_reject", '1', Move::Right);
    return m;
}

int failures = 0;

void check(const std::string& name, bool expected, bool actual){
    if (expected != actual) ++failures;
    std::cout << (expected == actual ? "PASS " : "FAIL ") << name << ": expected " << expected << ", got " << actual << std::endl;
}

int main() {
        
    // Transition testing
    // Machine m;
    // bool first = addTransition(m, "q0", '0', "q2", 'X', Move::Right);
    // bool second = addTransition(m, "q0", '0', "q2", 'X', Move::Left);

    // std::cout << "first add: " << first << "\n";
    // std::cout << "second add: " << second << "\n";

    // Tape testing
    // std::map<long long, char> tape;
    // char blank = 'B';
    
    // // Write a value and read back the negative position
    // writeCell(tape, -3, 'X', blank);
    // std::cout << "Test 1: expected X, got " << readCell(tape, -3, blank) << std::endl;

    // // Try to read a cell that was never written to
    // std::cout << "Test 2: expected B, got " << readCell(tape, 5, blank) << std::endl;

    // // Try to write a blank to erase the cell
    // std::size_t sizeBefore = tape.size();
    // writeCell(tape, 0, 'A', blank);
    // std::cout << "Test 3a: expected size " << sizeBefore + 1 << ", got " << tape.size() << std::endl;
    // writeCell(tape, 0, blank, blank);
    // std::cout << "Test 3b: expected size " << sizeBefore << ", got " << tape.size() << std::endl;

    // // Reading does not add entries
    // std::size_t before = tape.size();
    // readCell(tape, 100, blank);
    // std::cout << "Test 4: expected size " << before << ", got " << tape.size() << std::endl;

    // ValidateMachine Testing
    std::cout << std::boolalpha;
    check("valid machine", true, validateMachine(makeValidMachine()));

    Machine m1 = makeValidMachine();
    m1.startState = "q9";
    check("bad start state", false, validateMachine(m1));

    Machine m2 = makeValidMachine();
    m2.rejectState = m2.acceptState;
    check("accept == reject", false, validateMachine(m2));

    Machine m3 = makeValidMachine();
    m3.tapeAlphabet.erase('B');
    check("blank missing from tape", false, validateMachine(m3));

    Machine m4 = makeValidMachine();
    m4.inputAlphabet.insert('B');
    check("blank in input alphabet", false, validateMachine(m4));

    Machine m5 = makeValidMachine();
    m5.inputAlphabet.insert('2');
    check("input symbol not in tape", false, validateMachine(m5));

    Machine m6 = makeValidMachine();
    addTransition(m6, "q0", 'B', "q9", 'B', Move::Left);
    check("transition to unknown state", false, validateMachine(m6));

    Machine m7 = makeValidMachine();
    addTransition(m7, "q0", 'B', "q0", 'Z', Move::Left);
    check("transition writes unknown symbol", false, validateMachine(m7));

    Machine m8 = makeValidMachine();
    addTransition(m8, "q0", '0', "q0", '1', Move::Left);
    check("duplicate transition", false, validateMachine(m8));

    Machine m = makeValidMachine();
    auto accepted = simulate(m, "0", 1, true);
    check("accept at step limit", true, accepted.status == Status::ACCEPTED);
    check("trace includes initial and final", true, accepted.trace.size() == 2);
    check("explicit rejection", true, simulate(m, "1", 10).status == Status::REJECTED);
    check("invalid input", true, simulate(m, "2", 10).status == Status::INVALID_INPUT);
    check("missing transition", true, simulate(m, "", 10).status == Status::HALTED_NO_TRANSITION);
    check("zero step limit", true, simulate(m, "0", 0).status == Status::STEP_LIMIT);
    Machine loop = makeValidMachine();
    loop.transitions.clear();
    addTransition(loop, "q0", 'B', "q0", 'B', Move::Left);
    auto limited = simulate(loop, "", 3, true);
    check("nonhalting step limit", true, limited.status == Status::STEP_LIMIT && limited.steps == 3 && limited.head == -3);
    Machine left = makeValidMachine();
    left.states.insert("q1");
    left.tapeAlphabet.insert('X');
    left.transitions.clear();
    addTransition(left, "q0", 'B', "q1", 'B', Move::Left);
    addTransition(left, "q1", 'B', "q_accept", 'X', Move::Right);
    auto extended = simulate(left, "", 10);
    check("write at negative position", true, extended.status == Status::ACCEPTED && readCell(extended.tape, -1, 'B') == 'X');
    Machine right = makeValidMachine();
    right.states.insert("q1");
    right.tapeAlphabet.insert('X');
    right.transitions.clear();
    addTransition(right, "q0", '0', "q1", '0', Move::Right);
    addTransition(right, "q1", 'B', "q_accept", 'X', Move::Left);
    auto extendedRight = simulate(right, "0", 10);
    check("write beyond input on right", true, readCell(extendedRight.tape, 1, 'B') == 'X');
    std::map<long long, char> tape;
    writeCell(tape, -3, 'X', 'B');
    check("read negative cell", true, readCell(tape, -3, 'B') == 'X');
    check("missing cell is blank", true, readCell(tape, 100, 'B') == 'B' && tape.size() == 1);
    writeCell(tape, -3, 'B', 'B');
    check("blank erases cell", true, tape.empty());
    check("invalid machine simulation", true, simulate(m8, "0", 10).status == Status::INVALID_MACHINE);
    Machine badMove = makeValidMachine();
    badMove.transitions.begin()->second.move = static_cast<Move>(99);
    check("invalid direction", false, validateMachine(badMove));
    std::cout << "Failures: " << failures << '\n';
    return failures == 0 ? 0 : 1;
}
