# DocoPL
**Doco, interpreted programming language written in C++.**
### Structure
Doco aims to become a PL with grammar similar to Python and C/C++.
It will support structures in a similar fashion to Python's classes, arrays, vectors, lists, a full math library, import system, support for Python libraries and the ability to run Python and C++ code.
### Development status
* Lexer completed.
* Parser structure completed.
* Interpreter development has not yet started.

#### Lexer
Full support to recognize strings, arrays, int, float, keywords, operators, identifiers, etc.
It can be extended. 

To shut off the console debug output comment out `display_toks(result_tok);` in `Lex()` inside `lexer.h` file.
