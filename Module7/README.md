# CPSC362-Group-Project
Group repository for CPSC 362 automata projects

## Module 7.3: Context-Free Language Membership Checker

This C++17 program uses the Cocke–Younger–Kasami (CYK) algorithm to check whether each nonempty query string belongs to a context-free grammar already in Chomsky normal form (CNF). It reads the grammar and queries from a text file and prints `query: ACCEPT` or `query: REJECT` for each query in order.

### Compile

```bash
g++ -std=c++17 -Wall -Wextra -pedantic Context_Free_Language_Checker.cpp -o cfl_checker
```

### Run

```bash
./cfl_checker G1_textbook_example_7_34.txt
./cfl_checker G2_anbn.txt
./cfl_checker G1_group_tests.txt
```

Run these commands from the `Module7` folder. On Windows, use `cfl_checker.exe` in place of `./cfl_checker`.

### Test files

- `G1_textbook_example_7_34.txt`: textbook grammar and supplied queries; `baaba` is accepted.
- `G2_anbn.txt`: supplied tests for strings of the form aⁿbⁿ, where n is at least 1.
- `G1_group_tests.txt`: the G1 tests plus two group-created queries, `aaa` (accepted) and `bbb` (rejected).

The program expects one input filename as its command-line argument. Each file lists the start variable, terminal rules, binary rules, and nonempty queries with their respective counts. It does not convert grammars to CNF.

Group Roles:
__________________________________

Project Manager/Coordinator: **Wyatt Rose**
Responsibilities: Oversee the project timeline, ensure tasks are on track, and facilitate communication among team members.
Skills: Strong organizational and communication skills.

Lead Developer: **Aaron Littlejohn**
Responsibilities: Take charge of the main coding tasks, ensure code quality, and integrate different parts of the project.
Skills: Strong programming skills and experience with the relevant programming language (Python or C++).

Researcher/Analyst: **Cameron Long**
Responsibilities: Dive into the theoretical aspects, ensure the implementation aligns with computational theory, and assist with complex problem-solving.
Skills: Good understanding of computational models and theory.

Tester/Debugger: **Jacob Gainley**
Responsibilities: Develop and run test cases, identify bugs, and work with the developer to resolve issues.
Skills: Detail-oriented with a knack for problem-solving and debugging.

Documentation Specialist: **Raymond Claudio**
Responsibilities: Document the code, write user guides, and prepare the final report or presentation.
Skills: Strong writing skills and attention to detail.
