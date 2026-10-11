Background
The authoritative source is Hopcroft, Motwani, and Ullman, Introduction to Automata Theory, Languages, and Computation, 3rd edition. Section 9.2.1, printed pages 383–384, defines a decidable problem as one for which a Turing machine always halts and answers correctly. Chapter 9 proves that no such algorithm exists for several problems in this assignment.

System Description

Implement exactly these base identifiers and stored classifications:

Catalog ID	Classification	Recognition detail
TM_HAS_FIVE_STATES	DECIDABLE	RECURSIVE
TM_SOME_INPUT_AT_LEAST_FIVE_MOVES	DECIDABLE	RECURSIVE
TM_ACCEPTS_INPUT	UNDECIDABLE	RE_NOT_RECURSIVE
TM_HALTS_ON_INPUT	UNDECIDABLE	RE_NOT_RECURSIVE
TM_LANGUAGE_EMPTY	UNDECIDABLE	NON_RE
TM_LANGUAGE_FINITE	UNDECIDABLE	NOT_STATED_IN_CATALOG
PCP_HAS_MATCH	UNDECIDABLE	NOT_STATED_IN_CATALOG
CFG_AMBIGUOUS	UNDECIDABLE	NOT_STATED_IN_CATALOG
NOT_STATED_IN_CATALOG does not mean that no stronger result exists. It means this assignment requires only the classification established by the cited textbook passage.

Store the entries in a std::map<std::string, CatalogEntry> or another exact-key lookup structure. Each record must contain the catalog ID, problem title, classification, recognizability, concise reason, and exact textbook citation.

Required Input Behavior
The user selects a catalog ID. You may normalize lowercase to uppercase and spaces or hyphens to underscores. After normalization, perform an exact-key lookup. Do not use substring rules such as if text contains "halt", and do not infer a classification from a paragraph supplied by the user.

If the normalized identifier is not in the fixed catalog, return UNKNOWN for both classification and recognizability and state that the identifier is not in the fixed catalog and requires manual review.

Required Output
For a recognized ID, display:

normalized catalog ID;
problem title;
DECIDABLE or UNDECIDABLE;
recognition detail;
a concise reason; and
the textbook section, printed page, and theorem or example citation stored in the catalog.
Assignment Details
For this assignment, first review the Decidability Case-Study Catalog Step-by-Step Tutorial (DOCX)Download Decidability Case-Study Catalog Step-by-Step Tutorial (DOCX). Next, read through the Decidability Case-Study Catalog Test Cases Programming Assignment (DOCX)Download Decidability Case-Study Catalog Test Cases Programming Assignment (DOCX). Complete the following steps:

Define the classification enums and catalog record.
Create the eight fixed records from the accompanying test-case document.
Normalize the entered identifier without interpreting prose.
Perform an exact lookup.
Return the stored result when found.
Return UNKNOWN and manual-review wording when not found.
Add a command or menu option that lists the available identifiers.
Run every required test and compare the observed fields with the expected results.
Theory Boundaries
A particular easy-looking instance does not make the general decision problem decidable.
A finite timeout does not decide whether an arbitrary Turing machine will halt.
A fixed lookup may report a proven textbook result; it must not invent a reduction.
SAT and 3-SAT are outside this Chapter 9 catalog. Inputs about them return UNKNOWN for manual review. Do not label them undecidable.
Compilation
Compile with warnings enabled:

g++ -std=c++17 -Wall -Wextra -pedantic YourFile.cpp -o decidability_catalog.exe
