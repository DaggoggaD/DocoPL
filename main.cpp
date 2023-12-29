using namespace std;
#include "Lexer.h"

void func(int* a, int* b) {
    *a = 2;
    *b = 3;
}

int main() {
    vector<string> readingResult = Lex("test.doco");
    return 0;
}