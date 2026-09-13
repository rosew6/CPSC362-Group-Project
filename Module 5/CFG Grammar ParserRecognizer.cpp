#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>
#include <stdexcept>

struct Grammar {
    std::set<char> variables;
    std::set<char> terminals;
    std::map<char, std::vector<std::string>> productions;
    char start{};
};

enum class Outcome {
    Accepted,
    Rejected,
    Inconclusive,
    InvalidGrammar,
    InvalidInput
};

struct SearchNode {
    std::string form;
    int steps{};
    std::vector<std::string> path;
};

struct SearchResult {
    Outcome outcome{};
    std::string reason;
    std::vector<std::string> derivation;
};

struct Bounds {
    int maxProductionSteps = 40;
    std::size_t maxVisitedConfigurations = 200000;
    std::size_t maxSententialFormSymbols = 128;
};

std::string displayForm(const std::string& form) {
    return form.empty() ? "eps" : form;
}

bool isVariable(const Grammar& grammar, char symbol) {
    return grammar.variables.find(symbol) != grammar.variables.end();
}

bool validateGrammar(const Grammar& grammar, std::string& reason) {
    if (grammar.start == '\0' || !isVariable(grammar, grammar.start)) {
        reason = "The start symbol is not in the variable set.";
        return false;
    }

    for (const char variable : grammar.variables) {
        if (!std::isupper(static_cast<unsigned char>(variable))) {
            reason = "Every variable must be one uppercase character.";
            return false;
        }
    }

    for (const char terminal : grammar.terminals) {
        if (std::isupper(static_cast<unsigned char>(terminal))) {
            reason = "Every terminal must be one lowercase character.";
            return false;
        }
    }

    for (const char symbol : grammar.variables) {
        if (grammar.terminals.find(symbol) != grammar.terminals.end()) {
            reason = "A symbol is declared as both a variable and a terminal.";
            return false;
        }
    }

    for (std::map<char, std::vector<std::string>>::const_iterator it = grammar.productions.begin();
        it != grammar.productions.end(); ++it) {
        const char head = it->first;
        const std::vector<std::string>& bodies = it->second;

        if (!isVariable(grammar, head)) {
            reason = std::string("Production head '") + head + "' is not a declared variable.";
            return false;
        }

        for (const std::string& body : bodies) {
            for (const char symbol : body) {
                if (!isVariable(grammar, symbol) &&
                    grammar.terminals.find(symbol) == grammar.terminals.end()) {
                    reason = std::string("Production body contains undeclared symbol '") + symbol + "'.";
                    return false;
                }
            }
        }
    }

    return true;
}

bool validateInput(const Grammar& grammar, const std::string& input, std::string& reason) {
    for (const char symbol : input) {
        if (grammar.terminals.find(symbol) == grammar.terminals.end()) {
            reason = std::string("Input contains a character outside the terminal alphabet: '") + symbol + "'.";
            return false;
        }
    }
    return true;
}

std::size_t countVariables(const Grammar& grammar, const std::string& form) {
    std::size_t count = 0;
    for (const char symbol : form) {
        if (isVariable(grammar, symbol)) {
            ++count;
        }
    }
    return count;
}

bool fixedTerminalsMatchInput(const Grammar& grammar,
    const std::string& form,
    const std::string& input) {
    // Terminals before the first variable must match the input prefix.
    std::size_t firstVariable = form.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    while (firstVariable != std::string::npos &&
        !isVariable(grammar, form[firstVariable])) {
        firstVariable = form.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ", firstVariable + 1);
    }

    if (firstVariable == std::string::npos) {
        return form == input;
    }

    for (std::size_t i = 0; i < firstVariable; ++i) {
        if (i >= input.size() || form[i] != input[i]) {
            return false;
        }
    }

    // Terminals after the last variable must match the input suffix.
    std::size_t lastVariable = std::string::npos;
    for (std::size_t i = form.size(); i > 0; --i) {
        const std::size_t index = i - 1;
        if (isVariable(grammar, form[index])) {
            lastVariable = index;
            break;
        }
    }

    if (lastVariable == std::string::npos) {
        return form == input;
    }

    const std::size_t suffixLength = form.size() - lastVariable - 1;
    if (suffixLength > input.size()) {
        return false;
    }

    const std::size_t inputSuffixStart = input.size() - suffixLength;
    for (std::size_t i = 0; i < suffixLength; ++i) {
        if (form[lastVariable + 1 + i] != input[inputSuffixStart + i]) {
            return false;
        }
    }

    return true;
}

std::size_t minimumYieldLengthForVariable(const Grammar& grammar,
    char variable,
    std::map<char, std::size_t>& memo,
    std::set<char>& visiting) {
    const auto memoIt = memo.find(variable);
    if (memoIt != memo.end()) {
        return memoIt->second;
    }

    if (visiting.find(variable) != visiting.end()) {
        return std::numeric_limits<std::size_t>::max() / 4;
    }

    visiting.insert(variable);

    std::size_t best = std::numeric_limits<std::size_t>::max() / 4;
    const auto productionIt = grammar.productions.find(variable);
    if (productionIt != grammar.productions.end()) {
        for (const std::string& body : productionIt->second) {
            std::size_t length = 0;
            bool finite = true;
            for (const char symbol : body) {
                if (isVariable(grammar, symbol)) {
                    const std::size_t child = minimumYieldLengthForVariable(
                        grammar, symbol, memo, visiting);
                    if (child >= std::numeric_limits<std::size_t>::max() / 8) {
                        finite = false;
                        break;
                    }
                    length += child;
                }
                else {
                    ++length;
                }
            }
            if (finite) {
                best = std::min(best, length);
            }
        }
    }

    visiting.erase(variable);
    memo[variable] = best;
    return best;
}

std::size_t minimumYieldLength(const Grammar& grammar, const std::string& form) {
    std::map<char, std::size_t> memo;
    std::size_t total = 0;
    for (const char symbol : form) {
        if (isVariable(grammar, symbol)) {
            std::set<char> visiting;
            const std::size_t value = minimumYieldLengthForVariable(grammar, symbol, memo, visiting);
            if (value >= std::numeric_limits<std::size_t>::max() / 8) {
                return std::numeric_limits<std::size_t>::max() / 4;
            }
            total += value;
        }
        else {
            ++total;
        }
    }
    return total;
}

bool canPrune(const Grammar& grammar,
    const std::string& form,
    const std::string& input) {
    // Safe pruning rules from the tutorial.
    if (!fixedTerminalsMatchInput(grammar, form, input)) {
        return true;
    }

    std::size_t fixedTerminalCount = 0;
    for (const char symbol : form) {
        if (!isVariable(grammar, symbol)) {
            ++fixedTerminalCount;
        }
    }
    if (fixedTerminalCount > input.size()) {
        return true;
    }

    const std::size_t minimumLength = minimumYieldLength(grammar, form);
    if (minimumLength > input.size()) {
        return true;
    }

    return false;
}

std::string makeStateKey(const std::string& form) {
    return form;
}

SearchResult recognize(const Grammar& grammar,
    const std::string& input,
    const Bounds& bounds) {
    std::string reason;
    if (!validateGrammar(grammar, reason)) {
        return { Outcome::InvalidGrammar, reason, {} };
    }

    if (!validateInput(grammar, input, reason)) {
        return { Outcome::InvalidInput, reason, {} };
    }

    std::queue<SearchNode> frontier;
    std::unordered_set<std::string> visited;

    SearchNode startNode;
    startNode.form = std::string(1, grammar.start);
    startNode.steps = 0;
    startNode.path.push_back(startNode.form);

    frontier.push(startNode);
    visited.insert(makeStateKey(startNode.form));

    bool safetyBoundHit = false;

    while (!frontier.empty()) {
        SearchNode current = std::move(frontier.front());
        frontier.pop();

        if (current.form.size() > bounds.maxSententialFormSymbols) {
            safetyBoundHit = true;
            continue;
        }

        if (canPrune(grammar, current.form, input)) {
            continue;
        }

        if (countVariables(grammar, current.form) == 0) {
            if (current.form == input) {
                return { Outcome::Accepted, "The search found a leftmost derivation.", current.path };
            }
            continue;
        }

        const std::size_t leftmostVariablePosition = [&]() {
            for (std::size_t i = 0; i < current.form.size(); ++i) {
                if (isVariable(grammar, current.form[i])) {
                    return i;
                }
            }
            return current.form.size();
            }();

        if (leftmostVariablePosition == current.form.size()) {
            continue;
        }

        if (current.steps >= bounds.maxProductionSteps) {
            safetyBoundHit = true;
            continue;
        }

        const char variable = current.form[leftmostVariablePosition];
        const auto productionIt = grammar.productions.find(variable);
        if (productionIt == grammar.productions.end()) {
            continue;
        }

        for (const std::string& body : productionIt->second) {
            std::string next = current.form.substr(0, leftmostVariablePosition);
            next += body;
            next += current.form.substr(leftmostVariablePosition + 1);

            if (next.size() > bounds.maxSententialFormSymbols) {
                safetyBoundHit = true;
                continue;
            }

            if (canPrune(grammar, next, input)) {
                continue;
            }

            const std::string key = makeStateKey(next);
            if (visited.find(key) != visited.end()) {
                continue;
            }

            if (visited.size() >= bounds.maxVisitedConfigurations) {
                safetyBoundHit = true;
                continue;
            }

            SearchNode child;
            child.form = std::move(next);
            child.steps = current.steps + 1;
            child.path = current.path;
            child.path.push_back(child.form);

            visited.insert(key);
            frontier.push(std::move(child));
        }
    }

    if (safetyBoundHit) {
        return { Outcome::Inconclusive,
                "The search reached a safety limit before proving validity.",
                {} };
    }

    return { Outcome::Rejected,
            "All reachable leftmost derivations were exhausted, no inputs generated.",
            {} };
}

std::string outcomeLabel(Outcome outcome) {
    switch (outcome) {
    case Outcome::Accepted: return "Accepted";
    case Outcome::Rejected: return "Rejected";
    case Outcome::Inconclusive: return "Inconclusive";
    case Outcome::InvalidGrammar: return "Invalid grammar";
    case Outcome::InvalidInput: return "Invalid input";
    }
    return "Unknown";
}

void printResult(const std::string& input, const SearchResult& result) {
    std::cout << "Input: " << displayForm(input) << "\n";
    std::cout << "Result: " << outcomeLabel(result.outcome) << "\n";

    if (result.outcome == Outcome::Accepted) {
        std::cout << "Derivation:\n";
        for (const std::string& form : result.derivation) {
            std::cout << displayForm(form) << "\n";
        }
    }
    else {
        std::cout << "Reason: " << result.reason << "\n";
    }
    std::cout << "\n";
}

bool parseCharacterSet(const std::string& line,
    std::set<char>& destination,
    std::string& error) {
    destination.clear();

    // Accept either "ab" or "a b" (and similarly for variables).
    for (std::string::const_iterator it = line.begin();
        it != line.end(); ++it) {
        const unsigned char c = static_cast<unsigned char>(*it);
        if (!std::isspace(c)) {
            destination.insert(*it);
        }
    }

    if (destination.empty()) {
        error = "The symbol set cannot be empty.";
        return false;
    }

    return true;
}

bool parseProductions(Grammar& grammar) {
    std::cout << "Enter productions one per line in the form Head -> body | body.\n";
    std::cout << "Use eps for the empty body. When complete press Enter then type DONE and press enter to continue.\n";

    std::string line;
    while (true) {
        std::cout << "Production: ";
        if (!std::getline(std::cin, line)) {
            return false;
        }

        if (line == "DONE") {
            break;
        }

        const std::size_t arrow = line.find("->");
        if (arrow == std::string::npos) {
            std::cout << "Invalid production format. Example: P -> eps | 0P0 | 1P1\n";
            continue;
        }

        std::string headPart = line.substr(0, arrow);
        std::string bodyPart = line.substr(arrow + 2);

        auto trim = [](std::string text) {
            const auto notSpace = [](unsigned char c) { return !std::isspace(c); };
            text.erase(text.begin(), std::find_if(text.begin(), text.end(), notSpace));
            text.erase(std::find_if(text.rbegin(), text.rend(), notSpace).base(), text.end());
            return text;
            };

        headPart = trim(headPart);
        bodyPart = trim(bodyPart);

        if (headPart.size() != 1) {
            std::cout << "Invalid production head. The head must be one character.\n";
            continue;
        }

        const char head = headPart[0];
        std::vector<std::string> bodies;
        std::stringstream alternatives(bodyPart);
        std::string alternative;

        while (std::getline(alternatives, alternative, '|')) {
            alternative = trim(alternative);
            if (alternative == "eps") {
                alternative.clear();
            }
            bodies.push_back(alternative);
        }

        auto& destination = grammar.productions[head];
        destination.insert(destination.end(), bodies.begin(), bodies.end());
    }

    return true;
}

bool readGrammar(Grammar& grammar) {
    std::string line;
    std::string error;

    std::cout << "Variables: ";
    if (!std::getline(std::cin, line)) return false;
    if (!parseCharacterSet(line, grammar.variables, error)) {
        std::cout << error << "\n";
        return false;
    }

    std::cout << "Terminals: ";
    if (!std::getline(std::cin, line)) return false;
    if (!parseCharacterSet(line, grammar.terminals, error)) {
        std::cout << error << "\n";
        return false;
    }

    std::cout << "Start variable: ";
    if (!std::getline(std::cin, line)) return false;
    if (line.size() != 1) {
        std::cout << "The start variable must be one character.\n";
        return false;
    }
    grammar.start = line[0];

    if (!parseProductions(grammar)) {
        return false;
    }

    return true;
}

std::vector<std::string> readInputStrings() {
    std::vector<std::string> inputs;

    std::cout << "Input strings (separate with spaces; use eps for empty string): ";

    std::string line;
    std::getline(std::cin, line);

    std::stringstream parser(line);
    std::string token;

    while (parser >> token) {
        if (token == "eps") {
            inputs.push_back("");
        }
        else {
            inputs.push_back(token);
        }
    }

    return inputs;
}

bool readYesNo(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string answer;
        std::getline(std::cin, answer);
        if (answer == "y" || answer == "Y" || answer == "yes" || answer == "YES") return true;
        if (answer == "n" || answer == "N" || answer == "no" || answer == "NO") return false;
        std::cout << "Please enter y or n.\n";
    }
}

int main() {
    std::cout << "CFG Parser/Recognizer\n\n";

    while (true) {
        Grammar grammar;
        if (!readGrammar(grammar)) {
            if (!std::cin) {
                std::cout << "\nInput stream closed. Exiting.\n";
                return 0;
            }
            std::cout << "Incomplete grammar entry.\n";
        }

        std::string grammarReason;
        if (!validateGrammar(grammar, grammarReason)) {
            std::cout << "Invalid grammar\n";
            std::cout << "Reason: " << grammarReason << "\n\n";
        }
        else {
            std::cout << "Grammar: Correctly formatted and entered\n\n";
        }

        Bounds bounds;
        std::cout << "Maximum production steps [40]: Press enter to use 40. ";
        std::string line;
        std::getline(std::cin, line);
        if (!line.empty()) {
            try {
                bounds.maxProductionSteps = std::stoi(line);
                if (bounds.maxProductionSteps < 0) throw std::out_of_range("negative");
            }
            catch (...) {
                std::cout << "Invalid value. Using 40.\n";
                bounds.maxProductionSteps = 40;
            }
        }

        while (true) {
            const std::vector<std::string> inputs = readInputStrings();

            if (inputs.empty()) {
                std::cout << "No input strings were entered. Please enter at least one string.\n\n";
                continue;
            }

            std::cout << "\n==================== RESULTS ====================\n\n";

            for (std::vector<std::string>::const_iterator it = inputs.begin();
                it != inputs.end(); ++it) {
                const SearchResult result = recognize(grammar, *it, bounds);
                printResult(*it, result);
            }

            std::cout << "==================================================\n\n";

            if (!readYesNo("Test another batch with the same grammar? (y/n): ")) {
                break;
            }

            std::cout << "\n";
        }

        if (!readYesNo("Enter a new grammar? (y/n): ")) {
            std::cout << "Goodbye.\n";
            break;
        }
        std::cout << "\n";
    }

    return 0;
}

