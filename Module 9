
#include <cctype>
#include <iostream>
#include <map>
#include <string>

enum class Classification {
    Decidable,
    Undecidable,
    Unknown
};

enum class Recognizability {
    Recursive,
    ReNotRecursive,
    NonRE,
    NotStated,
    Unknown
};

struct CatalogEntry {
    std::string id;
    std::string title;
    Classification classification;
    Recognizability recognizability;
    std::string reason;
    std::string textbookCitation;
};

std::string ToString(Classification value) {
    switch (value) {
    case Classification::Decidable:
        return "DECIDABLE";
    case Classification::Undecidable:
        return "UNDECIDABLE";
    case Classification::Unknown:
        return "UNKNOWN";
    }
    return "UNKNOWN";
}

std::string ToString(Recognizability value) {
    switch (value) {
    case Recognizability::Recursive:
        return "RECURSIVE";
    case Recognizability::ReNotRecursive:
        return "RE_NOT_RECURSIVE";
    case Recognizability::NonRE:
        return "NON_RE";
    case Recognizability::NotStated:
        return "NOT_STATED_IN_CATALOG";
    case Recognizability::Unknown:
        return "UNKNOWN";
    }
    return "UNKNOWN";
}

// Normalize identifiers only. Do not interpret natural language input.
std::string NormalizeIdentifier(const std::string& input) {
    std::string result;

    for (unsigned char ch : input) {
        if (ch == ' ' || ch == '-') {
            result += '_';
        }
        else {
            result += static_cast<char>(std::toupper(ch));
        }
    }

    return result;
}

std::map<std::string, CatalogEntry> BuildCatalog() {
    using C = Classification;
    using R = Recognizability;

    std::map<std::string, CatalogEntry> catalog;

    const CatalogEntry entries[] = {
        {
            "TM_HAS_FIVE_STATES",
            "Does the Turing machine have exactly five states?",
            C::Decidable,
            R::Recursive,
            "The finite encoding of the machine can be inspected "
            "and its states counted, so the answer can always be "
            "determined.",
            "Hopcroft, Motwani, and Ullman, 3rd ed., "
            "Section 9.3.4, p. 399, Example 9.12."
        },
        {
            "TM_SOME_INPUT_AT_LEAST_FIVE_MOVES",
            "Does some input cause the machine to make at least "
            "five moves?",
            C::Decidable,
            R::Recursive,
            "Only finitely many relevant tape patterns need to be "
            "checked for the first five moves, making the property "
            "decidable.",
            "Hopcroft, Motwani, and Ullman, 3rd ed., "
            "Section 9.3.4, p. 399, Example 9.12."
        },
        {
            "TM_ACCEPTS_INPUT",
            "Does a given Turing machine accept a given input?",
            C::Undecidable,
            R::ReNotRecursive,
            "The universal language L_u consists of encoded machine "
            "and input pairs for which the machine accepts. "
            "It is recursively enumerable but not recursive, "
            "so no algorithm decides every instance.",
            "Hopcroft, Motwani, and Ullman, 3rd ed., "
            "Sections 9.2.3-9.2.4, pp. 387-390, Theorem 9.6."
        },
        {
            "TM_HALTS_ON_INPUT",
            "Does a given Turing machine halt on a given input?",
            C::Undecidable,
            R::ReNotRecursive,
            "Simulation can recognize a yes-instance when the "
            "machine halts, but it may run forever when the "
            "machine does not halt. No algorithm decides all pairs.",
            "Hopcroft, Motwani, and Ullman, 3rd ed., "
            "Section 9.2.4, p. 390, box 'The Halting Problem' "
            "and Exercise 9.2.1."
        },
        {
            "TM_LANGUAGE_EMPTY",
            "Is the language accepted by a given Turing machine empty?",
            C::Undecidable,
            R::NonRE,
            "The problem asks whether a machine accepts no strings. "
            "The textbook's empty-language problem is not "
            "recursively enumerable and therefore is not decidable.",
            "Hopcroft, Motwani, and Ullman, 3rd ed., "
            "Section 9.3.2, pp. 394-397, Theorems 9.8-9.10."
        },
        {
            "TM_LANGUAGE_FINITE",
            "Is the language accepted by a given Turing machine finite?",
            C::Undecidable,
            R::NotStated,
            "Having a finite accepted language is a nontrivial "
            "property of the language recognized by a Turing "
            "machine. Rice's theorem establishes that this "
            "property is undecidable. Stronger recognizability "
            "details are not required by this catalog.",
            "Hopcroft, Motwani, and Ullman, 3rd ed., "
            "Sections 9.3.3-9.3.4, pp. 397-399, Theorem 9.11 "
            "and the list on p. 399."
        },
        {
            "PCP_HAS_MATCH",
            "Does a Post Correspondence Problem instance have "
            "a matching sequence?",
            C::Undecidable,
            R::NotStated,
            "The general problem asks whether any nonempty finite "
            "sequence of tile indices produces equal concatenations "
            "of the top and bottom strings. The general PCP "
            "decision problem is undecidable.",
            "Hopcroft, Motwani, and Ullman, 3rd ed., "
            "Section 9.4.1, pp. 401-403; Section 9.4.3, "
            "pp. 407-412, Theorem 9.19."
        },
        {
            "CFG_AMBIGUOUS",
            "Is a given context-free grammar ambiguous?",
            C::Undecidable,
            R::NotStated,
            "The general CFG ambiguity problem is undecidable. "
            "The textbook establishes this result using a "
            "reduction from the Post Correspondence Problem.",
            "Hopcroft, Motwani, and Ullman, 3rd ed., "
            "Section 9.5.2, pp. 413-415, Theorem 9.20."
        }
    };

    for (const CatalogEntry& entry : entries) {
        catalog.emplace(entry.id, entry);
    }

    return catalog;
}

void PrintEntry(const CatalogEntry& entry) {
    std::cout << "id=" << entry.id << '\n'
        << "title=" << entry.title << '\n'
        << "classification="
        << ToString(entry.classification) << '\n'
        << "recognizability="
        << ToString(entry.recognizability) << '\n'
        << "reason=" << entry.reason << '\n'
        << "textbook=" << entry.textbookCitation << '\n';
}

void ClassifyIdentifier(
    const std::map<std::string, CatalogEntry>& catalog,
    const std::string& input
) {
    const std::string normalized = NormalizeIdentifier(input);
    const auto found = catalog.find(normalized);

    if (found != catalog.end()) {
        PrintEntry(found->second);
        return;
    }

    std::cout << "id=" << normalized << '\n'
        << "title=Not in the fixed catalog\n"
        << "classification=UNKNOWN\n"
        << "recognizability=UNKNOWN\n"
        << "reason=The identifier is not in the fixed catalog. "
        "Manual review is required.\n"
        << "textbook=No catalog citation available; "
        "manual review is required.\n";
}

void ListIdentifiers(
    const std::map<std::string, CatalogEntry>& catalog
) {
    std::cout << "Available catalog identifiers:\n";

    for (const auto& item : catalog) {
        std::cout << item.first << '\n';
    }
}

int main(int argc, char* argv[]) {
    const auto catalog = BuildCatalog();

    // Preserve command-line functionality.
    if (argc >= 2) {
        const std::string command = argv[1];

        if (command == "--list") {
            ListIdentifiers(catalog);
            return 0;
        }

        if (command == "--classify") {
            if (argc < 3) {
                std::cerr
                    << "Usage: decidability_catalog.exe "
                    << "--classify \"CATALOG_ID\"\n";
                return 1;
            }

            std::string input = argv[2];

            // Preserve additional command-line words.
            for (int i = 3; i < argc; ++i) {
                input += ' ';
                input += argv[i];
            }

            ClassifyIdentifier(catalog, input);
            return 0;
        }

        std::cerr
            << "Unknown command. Use --list or --classify.\n";
        return 1;
    }

    // Interactive mode.
    std::cout << "Decidability Case-Study Catalog\n"
        << "Enter a catalog identifier to classify it.\n"
        << "Type --list to display available identifiers.\n"
        << "Type exit to quit the program.\n\n";

    std::string input;

    // Continue accepting input until exit or end-of-input.
    while (true) {
        std::cout << "Input: ";

        if (!std::getline(std::cin, input)) {
            break;
        }

        // Allow exit in any capitalization.
        std::string command = input;
        for (char& ch : command) {
            ch = static_cast<char>(
                std::tolower(static_cast<unsigned char>(ch))
                );
        }

        if (command == "exit") {
            std::cout << "Exiting catalog. Goodbye!\n";
            break;
        }

        if (input == "--list") {
            ListIdentifiers(catalog);
            std::cout << '\n';
            continue;
        }

        // Classify this entry, then prompt for another.
        ClassifyIdentifier(catalog, input);
        std::cout << '\n';
    }

    return 0;
}
