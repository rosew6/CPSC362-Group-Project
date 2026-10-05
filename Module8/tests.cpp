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

void check(const std::string& name, bool expected, bool actual){
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

    return 0;
}