#include <array>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

struct TerminalRule { char head, terminal; };
struct BinaryRule { char head, left, right; };

bool accepts(const std::string& word, char start,
             const std::vector<TerminalRule>& terminals,
             const std::vector<BinaryRule>& binaries) {
    const std::size_t n = word.size();
    if (n == 0) return false;
    std::vector<std::vector<std::set<char>>> table(n, std::vector<std::set<char>>(n));
    for (std::size_t i = 0; i < n; ++i)
        for (const auto& rule : terminals)
            if (rule.terminal == word[i]) table[i][i].insert(rule.head);

    for (std::size_t length = 2; length <= n; ++length) {
        for (std::size_t i = 0; i + length <= n; ++i) {
            const std::size_t j = i + length - 1;
            for (std::size_t k = i; k < j; ++k)
                for (const auto& rule : binaries)
                    if (table[i][k].count(rule.left) && table[k + 1][j].count(rule.right))
                        table[i][j].insert(rule.head);
        }
    }
    return table[0][n - 1].count(start) != 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " input_file\n";
        return 1;
    }
    std::ifstream input(argv[1]);
    if (!input) {
        std::cerr << "Unable to open input file: " << argv[1] << '\n';
        return 1;
    }
    char start;
    int terminal_count, binary_count, query_count;
    if (!(input >> start >> terminal_count) || terminal_count < 0) {
        std::cerr << "Invalid input file\n"; return 1;
    }
    std::vector<TerminalRule> terminals;
    for (int i = 0; i < terminal_count; ++i) {
        TerminalRule rule;
        if (!(input >> rule.head >> rule.terminal)) { std::cerr << "Invalid terminal rule\n"; return 1; }
        terminals.push_back(rule);
    }
    if (!(input >> binary_count) || binary_count < 0) { std::cerr << "Invalid binary count\n"; return 1; }
    std::vector<BinaryRule> binaries;
    for (int i = 0; i < binary_count; ++i) {
        BinaryRule rule;
        if (!(input >> rule.head >> rule.left >> rule.right)) { std::cerr << "Invalid binary rule\n"; return 1; }
        binaries.push_back(rule);
    }
    if (!(input >> query_count) || query_count < 0) { std::cerr << "Invalid query count\n"; return 1; }
    for (int i = 0; i < query_count; ++i) {
        std::string query;
        if (!(input >> query)) { std::cerr << "Missing query\n"; return 1; }
        std::cout << query << ": " << (accepts(query, start, terminals, binaries) ? "ACCEPT" : "REJECT") << '\n';
    }
    return 0;
}
