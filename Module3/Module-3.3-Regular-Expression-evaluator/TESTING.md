# Regular Expression Evaluator Testing Report

## Tester

Jacob Gainley

## Testing Objective

The purpose of this testing was to verify that the regular expression
evaluator correctly processes literal characters, concatenation, union,
Kleene star operations, grouped expressions, mismatched inputs, and
invalid regular expressions.

## Test Environment

- Language: C++
- Compiler standard: C++17
- Source file tested: main.cpp

## Test Results

| Test | Regular Expression | Input String | Expected Result | Actual Result | Status |
|---|---|---|---|---|---|
| Literal match | `a` | `a` | Accepted | Accepted | Pass |
| Literal mismatch | `a` | `b` | Not Accepted | Not Accepted | Pass |
| Concatenation match | `ab` | `ab` | Accepted | Accepted | Pass |
| Incomplete concatenation | `ab` | `a` | Not Accepted | Not Accepted | Pass |
| Union first option | `a\|b` | `a` | Accepted | Accepted | Pass |
| Union second option | `a\|b` | `b` | Accepted | Accepted | Pass |
| Union mismatch | `a\|b` | `c` | Not Accepted | Not Accepted | Pass |
| Kleene star repetition | `a*` | `aaaa` | Accepted | Accepted | Pass |
| Kleene star mismatch | `a*` | `b` | Not Accepted | Not Accepted | Pass |
| Grouped union | `a(b\|c)` | `ac` | Accepted | Accepted | Pass |
| Grouped union mismatch | `a(b\|c)` | `ad` | Not Accepted | Not Accepted | Pass |
| Complex expression | `(a\|b)*ab` | `abab` | Accepted | Accepted | Pass |
| Complex expression mismatch | `(a\|b)*ab` | `aba` | Not Accepted | Not Accepted | Pass |
| Repeated group | `(ab)*` | `abab` | Accepted | Accepted | Pass |
| Missing closing parenthesis | `(a` | `a` | Error | Error | Pass |
| Missing opening parenthesis | `a)` | `a` | Error | Error | Pass |
| Invalid union | `a\|` | `a` | Error | Error | Pass |
| Invalid leading star | `*a` | `a` | Error | Error | Pass |
| Empty group | `()` | `a` | Error | Error | Pass |

## Issues Identified

### Empty-string input

The evaluator cannot currently test an empty input string because it reads
the input using:

`std::cin >> input_string;`

Expressions such as `a*` and `(ab)*` should accept an empty string, but the
program does not provide a way to enter one.

A possible solution is to allow the user to enter `EMPTY` and convert that
value to an empty string before testing.

### Unclear error message

An invalid concatenation can produce the message `error line 163.` This
should be replaced with a descriptive message such as
`Invalid concatenation.`

### Repeated Kleene star

The expression `a**` is accepted and operates like `a*`. The group should
decide whether repeated Kleene stars are valid or should produce an error.

## Conclusion

All completed functional tests passed. The evaluator correctly processes
the required literal, concatenation, union, grouping, and Kleene star
operations. The main issue identified is the inability to test an empty
input string. Minor improvements are also recommended for error-message
clarity and repeated-star validation.
