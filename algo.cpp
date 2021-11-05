#include "Automata.h"


bool solve(Automata a, char x, int k) {
    std::string new_str(k, x);
    return a.is_prefix(new_str);
}
