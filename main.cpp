#include <iostream>
#include "Automata.h"
#include <string>

int main() {
    std::string s;
    std::cin >> s;
    char x;
    int n;
    std::cin >> x >> n;
    Automata a(0, {'a', 'b', 'c'}, s);
    std::cout << (solve(a, x, n)?"YES":"NO") << '\n';
}
