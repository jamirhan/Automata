#include <iostream>
#include "Automata.h"
#include <string>


bool solve(Automata, char, int);

int main() {
    std::string s;
    std::cin >> s;
    char x;
    int n;
    std::cin >> x >> n;
    Automata a(0, {'a', 'b', 'c'}, s);
    std::cout << (solve(a, x, n) ? "YES" : "NO") << '\n';
}
