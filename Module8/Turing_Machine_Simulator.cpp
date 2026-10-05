        {"q0", '_', "q0", '_', 'R'}
    };
    SimulationResult loopResult = simulate(loopingMachine, "", 3, true);
    tests.push_back({"TM-04", "Stop a nonhalting computation at the step limit",
                     SimulationStatus::STEP_LIMIT, loopResult,
                     loopResult.stepsExecuted == 3 && loopResult.headPosition == 3});

    // TM-05: input validation occurs before execution.
    Machine inputMachine = makeBaseMachine();
    inputMachine.inputAlphabet = {'0'};
    inputMachine.tapeAlphabet = {'0', '_'};
    inputMachine.transitions = {
        {"q0", '0', "qaccept", '0', 'R'}
    };
    tests.push_back({"TM-05", "Reject a symbol outside the input alphabet",
                     SimulationStatus::INVALID_INPUT,
                     simulate(inputMachine, "01", 20, true), true});

    // TM-06: duplicate state-symbol transition key.
    Machine duplicateMachine = makeBaseMachine();
    duplicateMachine.inputAlphabet.clear();
    duplicateMachine.transitions = {
        {"q0", '_', "qaccept", '_', 'R'},
        {"q0", '_', "qreject", '_', 'L'}
    };
    tests.push_back({"TM-06", "Reject duplicate deterministic transition keys",
                     SimulationStatus::INVALID_MACHINE,
                     simulate(duplicateMachine, "", 20, true), true});

    // TM-07: invalid move direction.
    Machine directionMachine = makeBaseMachine();
    directionMachine.inputAlphabet.clear();
    directionMachine.transitions = {
        {"q0", '_', "qaccept", '_', 'S'}
    };
    tests.push_back({"TM-07", "Reject a transition direction other than L or R",
                     SimulationStatus::INVALID_MACHINE,
                     simulate(directionMachine, "", 20, true), true});

    // TM-08: demonstrate unbounded movement to the left of position zero.
    Machine leftMachine = makeBaseMachine();
    leftMachine.inputAlphabet.clear();
    leftMachine.transitions = {
        {"q0", '_', "q1", '_', 'L'},
        {"q1", '_', "q2", 'X', 'R'},
        {"q2", '_', "qaccept", '_', 'L'}
    };
    SimulationResult leftResult = simulate(leftMachine, "", 20, true);
    const auto leftCell = leftResult.finalNonblankTape.find(-1);
    const bool leftCellOk = leftCell != leftResult.finalNonblankTape.end() &&
                            leftCell->second == 'X';
    tests.push_back({"TM-08", "Use a negative tape position to prove left extension",
                     SimulationStatus::ACCEPTED, leftResult, leftCellOk});

    // TM-09: demonstrate writing beyond the right end of the supplied input.
    Machine rightMachine = makeBaseMachine();
    rightMachine.inputAlphabet = {'0'};
    rightMachine.transitions = {
        {"q0", '0', "q1", '0', 'R'},
        {"q1", '_', "qaccept", 'X', 'L'}
    };
    SimulationResult rightResult = simulate(rightMachine, "0", 20, true);
    const auto rightCell = rightResult.finalNonblankTape.find(1);
    const bool rightCellOk = rightCell != rightResult.finalNonblankTape.end() &&
                             rightCell->second == 'X';
    tests.push_back({"TM-09", "Write beyond the supplied input on the right",
                     SimulationStatus::ACCEPTED, rightResult, rightCellOk});

    // TM-10: writing the blank symbol removes a sparse tape cell.
    Machine eraseMachine = makeBaseMachine();
    eraseMachine.inputAlphabet = {'0'};
    eraseMachine.tapeAlphabet = {'0', '_'};
    eraseMachine.transitions = {
        {"q0", '0', "qaccept", '_', 'R'}
    };
    SimulationResult eraseResult = simulate(eraseMachine, "0", 20, true);
    tests.push_back({"TM-10", "Erase a cell by writing the blank symbol",
                     SimulationStatus::ACCEPTED, eraseResult,
                     eraseResult.finalNonblankTape.empty()});

    // TM-11: blank symbol must be in the tape alphabet.
    Machine blankMachine = makeBaseMachine();
    blankMachine.inputAlphabet = {'0'};
    blankMachine.tapeAlphabet = {'0'};
    blankMachine.transitions.clear();
    tests.push_back({"TM-11", "Reject a machine whose tape alphabet lacks blank",
                     SimulationStatus::INVALID_MACHINE,
                     simulate(blankMachine, "0", 20, true), true});

    // TM-12: every input symbol must also be a tape symbol.
    Machine subsetMachine = makeBaseMachine();
    subsetMachine.inputAlphabet = {'0', '1'};
    subsetMachine.tapeAlphabet = {'0', '_'};
    subsetMachine.transitions.clear();
    tests.push_back({"TM-12", "Reject an input alphabet not contained in tape alphabet",
                     SimulationStatus::INVALID_MACHINE,
                     simulate(subsetMachine, "", 20, true), true});

    // TM-13: start state must be declared.
    Machine startMachine = makeBaseMachine();
    startMachine.startState = "missing";
    startMachine.transitions.clear();
    tests.push_back({"TM-13", "Reject an undeclared start state",
                     SimulationStatus::INVALID_MACHINE,
                     simulate(startMachine, "", 20, true), true});

    std::cout << "CPSC 362 - Module 8 Turing Machine Simulator Verification\n"
              << "========================================================\n\n";

    std::size_t passed = 0;
    for (const VerificationCase& testCase : tests) {
        printCase(testCase);
        const bool casePass =
            testCase.result.status == testCase.expectedStatus &&
            testCase.extraCondition;
        if (casePass) {
            ++passed;
        }
    }

    std::cout << "Trace evidence for TM-01 (initial configuration plus each transition):\n";
    printTrace(tests[0].result.trace);
    std::cout << '\n';

    std::cout << "Trace evidence for TM-08 (negative tape index):\n";
    printTrace(leftResult.trace);
    std::cout << '\n';

    std::cout << "Summary: " << passed << "/" << tests.size()
              << " verification cases passed.\n";

    return (passed == tests.size()) ? 0 : 1;
}
        {"q0", '_', "q0", '_', 'R'}
    };
    SimulationResult loopResult = simulate(loopingMachine, "", 3, true);
    tests.push_back({"TM-04", "Stop a nonhalting computation at the step limit",
                     SimulationStatus::STEP_LIMIT, loopResult,
                     loopResult.stepsExecuted == 3 && loopResult.headPosition == 3});

    // TM-05: input validation occurs before execution.
    Machine inputMachine = makeBaseMachine();
    inputMachine.inputAlphabet = {'0'};
    inputMachine.tapeAlphabet = {'0', '_'};
    inputMachine.transitions = {
        {"q0", '0', "qaccept", '0', 'R'}
    };
    tests.push_back({"TM-05", "Reject a symbol outside the input alphabet",
                     SimulationStatus::INVALID_INPUT,
                     simulate(inputMachine, "01", 20, true), true});

    // TM-06: duplicate state-symbol transition key.
    Machine duplicateMachine = makeBaseMachine();
    duplicateMachine.inputAlphabet.clear();
    duplicateMachine.transitions = {
        {"q0", '_', "qaccept", '_', 'R'},
        {"q0", '_', "qreject", '_', 'L'}
    };
    tests.push_back({"TM-06", "Reject duplicate deterministic transition keys",
                     SimulationStatus::INVALID_MACHINE,
                     simulate(duplicateMachine, "", 20, true), true});

    // TM-07: invalid move direction.
    Machine directionMachine = makeBaseMachine();
    directionMachine.inputAlphabet.clear();
    directionMachine.transitions = {
        {"q0", '_', "qaccept", '_', 'S'}
    };
    tests.push_back({"TM-07", "Reject a transition direction other than L or R",
                     SimulationStatus::INVALID_MACHINE,
                     simulate(directionMachine, "", 20, true), true});

    // TM-08: demonstrate unbounded movement to the left of position zero.
    Machine leftMachine = makeBaseMachine();
    leftMachine.inputAlphabet.clear();
    leftMachine.transitions = {
        {"q0", '_', "q1", '_', 'L'},
        {"q1", '_', "q2", 'X', 'R'},
        {"q2", '_', "qaccept", '_', 'L'}
    };
    SimulationResult leftResult = simulate(leftMachine, "", 20, true);
    const auto leftCell = leftResult.finalNonblankTape.find(-1);
    const bool leftCellOk = leftCell != leftResult.finalNonblankTape.end() &&
                            leftCell->second == 'X';
    tests.push_back({"TM-08", "Use a negative tape position to prove left extension",
                     SimulationStatus::ACCEPTED, leftResult, leftCellOk});

    // TM-09: demonstrate writing beyond the right end of the supplied input.
    Machine rightMachine = makeBaseMachine();
    rightMachine.inputAlphabet = {'0'};
    rightMachine.transitions = {
        {"q0", '0', "q1", '0', 'R'},
        {"q1", '_', "qaccept", 'X', 'L'}
    };
    SimulationResult rightResult = simulate(rightMachine, "0", 20, true);
    const auto rightCell = rightResult.finalNonblankTape.find(1);
    const bool rightCellOk = rightCell != rightResult.finalNonblankTape.end() &&
                             rightCell->second == 'X';
    tests.push_back({"TM-09", "Write beyond the supplied input on the right",
                     SimulationStatus::ACCEPTED, rightResult, rightCellOk});

    // TM-10: writing the blank symbol removes a sparse tape cell.
    Machine eraseMachine = makeBaseMachine();
    eraseMachine.inputAlphabet = {'0'};
    eraseMachine.tapeAlphabet = {'0', '_'};
    eraseMachine.transitions = {
        {"q0", '0', "qaccept", '_', 'R'}
    };
    SimulationResult eraseResult = simulate(eraseMachine, "0", 20, true);
    tests.push_back({"TM-10", "Erase a cell by writing the blank symbol",
                     SimulationStatus::ACCEPTED, eraseResult,
                     eraseResult.finalNonblankTape.empty()});

    // TM-11: blank symbol must be in the tape alphabet.
    Machine blankMachine = makeBaseMachine();
    blankMachine.inputAlphabet = {'0'};
    blankMachine.tapeAlphabet = {'0'};
    blankMachine.transitions.clear();
    tests.push_back({"TM-11", "Reject a machine whose tape alphabet lacks blank",
                     SimulationStatus::INVALID_MACHINE,
                     simulate(blankMachine, "0", 20, true), true});

    // TM-12: every input symbol must also be a tape symbol.
    Machine subsetMachine = makeBaseMachine();
    subsetMachine.inputAlphabet = {'0', '1'};
    subsetMachine.tapeAlphabet = {'0', '_'};
    subsetMachine.transitions.clear();
    tests.push_back({"TM-12", "Reject an input alphabet not contained in tape alphabet",
                     SimulationStatus::INVALID_MACHINE,
                     simulate(subsetMachine, "", 20, true), true});

    // TM-13: start state must be declared.
    Machine startMachine = makeBaseMachine();
    startMachine.startState = "missing";
    startMachine.transitions.clear();
    tests.push_back({"TM-13", "Reject an undeclared start state",
                     SimulationStatus::INVALID_MACHINE,
                     simulate(startMachine, "", 20, true), true});

    std::cout << "CPSC 362 - Module 8 Turing Machine Simulator Verification\n"
              << "========================================================\n\n";

    std::size_t passed = 0;
    for (const VerificationCase& testCase : tests) {
        printCase(testCase);
        const bool casePass =
            testCase.result.status == testCase.expectedStatus &&
            testCase.extraCondition;
        if (casePass) {
            ++passed;
        }
    }

    std::cout << "Trace evidence for TM-01 (initial configuration plus each transition):\n";
    printTrace(tests[0].result.trace);
    std::cout << '\n';

    std::cout << "Trace evidence for TM-08 (negative tape index):\n";
    printTrace(leftResult.trace);
    std::cout << '\n';

    std::cout << "Summary: " << passed << "/" << tests.size()
              << " verification cases passed.\n";

    return (passed == tests.size()) ? 0 : 1;
}
