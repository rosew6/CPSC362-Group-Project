#include <fstream>
#include <iostream>

// "Dictionary" struct for productions
typedef struct {
    char key;
    char value;
} TerminalPair;

typedef struct {
    char head;
    char left;
    char right;
} BinaryPair;

typedef struct {
    std::string query;
    bool accepted;
} QueryPair;

// Linked list for queries
typedef struct Node{
    std::string query;
    struct Node *next;
} Node;

int main(int argc, char* argv[]){
    std::ifstream file_input("G1_textbook_example_7_34.txt");
    if(!file_input) {
        std::cerr << "Unable to open test file: ";
        return 1;
    }

    std::string line;
    std::string start_variable = "";

    // Declaration of our production structures
    int num_terminal;
    int num_binary;
    int num_queries;
    std::vector<TerminalPair> terminal_productions;
    std::vector<BinaryPair> binary_productions;
    std::vector<QueryPair> queries;

    // Read and store the start variable
    file_input >> start_variable;

    // Read all the terminal productions in the file
    file_input >> num_terminal;
    for(int i = 0; i < num_terminal; i++){
        char head, terminal;
        file_input >> head >> terminal;
        // Store the head and terminal in a pair in the vector
        terminal_productions.push_back({head, terminal});
    }

    // Read all the binary productions in the file
    file_input >> num_binary;
    for(int i = 0; i < num_binary; i++){
        char head, left, right;
        file_input >> head >> left >> right;
        // Store the binary production in the binary_productions vector
        binary_productions.push_back({head, left, right});
    }

    // Read all the queries in the file
    file_input >> num_queries;
    for(int i = 0; i < num_queries; i++){
        std::string query;
        file_input >> query;
        // Store the query with a default accepted value of false
        queries.push_back({query, false});
    }

    std::cout << "num_terminal: " << num_terminal << ", num_binary: " << num_binary << ", num_queries: " << num_queries << std::endl;

    // Print terminals
    std::cout << "Terminals recorded from file:" << std::endl;
    for (const TerminalPair& tp : terminal_productions){
        std::cout << tp.key << " " << tp.value << std::endl;
    }

    std::cout << "Binaries recorded from file:" << std::endl;
    for (const BinaryPair& bp : binary_productions){
        std::cout << bp.head << " " << bp.left << " " << bp.right << std::endl;
    }

    std::cout << "Queries recorded from file: " << std::endl;
    for (const QueryPair& qp : queries){
        std::cout << qp.query << std::endl;
    }
}