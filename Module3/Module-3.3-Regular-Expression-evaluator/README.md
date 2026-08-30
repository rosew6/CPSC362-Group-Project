 
Module 3: Regular Expression Evaluator 
 
Title: Regular Expression Evaluator  
Course: CPSC 362 – Computing Theory 
Module: 3 
Group Members: Aron Littlejohn, Wyatt Rose, Cameron Long, Jacob Gainey, Raymond Claudio Date: 20260830 
  
1. Introduction 
•	The purpose of this Regular Expression Evaluator is to solidify understanding of how regular expressions can be converted into a form that a computer can process.   
•	The objective of the assignment was to create a C++ program that evaluates regular expressions and determines whether an input string is accepted. 
•	This project covers data structuring, regular expression processing, NFA construction, and string matching.   
  
2. System Design and Implementation 2.1 Data Representation 
•	The program represents the regular expression as a Nondeterministic Finite Automaton (NFA).  Each state is stored using a 𝑆𝑇𝐴𝑇𝐸 structure containing an ID, acceptance status, and a map of transitions.  The NFA stores all states along with the starting and accepting states.   
•	Explain the enumeration or data structures used. 
2.2 Core Algorithm Implementation 
•	Explain how the core algorithm was implemented. 
•	Describe the specific logic applied by the algorithm. 
•	Include a sample table or diagram showing the program's key processing steps (if applicable). 
Component 	  Input 	    Operation 	           Result 
Literal Match a,a 	  Create transition for a. 	Accepted 
Concatenation ab,ab 	Connect NFAs for a and b 	Accepted 
Union 	      a|b , b Create alternate paths.  	Accepted 
Kleene Star  a*, aaaa Repeat the a transistion  Accepted 
 	 	 		 	
  
3. Program Execution and Results 
3.1 Program Execution 
•	The program takes input to enter a regular expression followed by a string to test.  The expression is then converted into an NFA, and the input string is processed through its transitions, then the program displays the accepted/rejected/error status of the string. 
•	Example Output: 
  <img width="636" height="106" alt="image" src="https://github.com/user-attachments/assets/8f6109c3-35eb-4c81-a898-54d1252eeb6c" />

Example 1: Kleene star mismatch.  
  
4. Additional Features and Enhancements 
• 	Error handling for invalid expressions, mismatched parentheses, invalid unions and Kleen star operations.   
  
5. Challenges and Solutions 
• 	The biggest challenge was handling epsilon transitions when combining NFAs.  This was resolved by using /0 to represent epsilon transitions and repeatedly following them during string evaluation.  Additionally, the detection of invalid regular expressions was fixed through exception handling and descriptive error messages.   
  
6. Conclusion 
	• 	The Regex evaluator shows how regular expressions can be processed using finite automata.   
  
7. References (if applicable) 
• 	Hopcroft, J. E., Motwani, R., & Ullman, J. D. (2007). Introduction to automata theory, languages, and computation (3rd ed.). Pearson Addison Wesley.
  
Appendix (if applicable) 
	• 	 
