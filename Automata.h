#include <iostream>
#include <map>
#include <set>
#include <utility>
#include <vector>
#include <exception>
#include <queue>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <stack>


namespace Automata_details {

    template <typename T>
    struct SetHash {
        const static size_t m = 1e9 + 7;
        const static size_t p = 13;
        size_t operator()(const std::set<T>& v) const {
            size_t ans = 0;
            auto hasher = std::hash<T>();
            for (const auto& el: v) {
                ans = (ans + (p*hasher(el)) % m) % m;
            }
            return ans;
        }
    };

}


class Automata {

    struct SymbolWrapper {
        char symbol = char();
        bool is_empty = false;

    };

    struct CompWrap {
        bool operator()(const SymbolWrapper& a, const SymbolWrapper& b) const {
            return (a.is_empty && !b.is_empty) || a.symbol < b.symbol;
        }
    };


    struct Node {
        std::map<SymbolWrapper, std::set<size_t>, CompWrap> edges;
        bool is_final;
        std::map<SymbolWrapper, std::set<size_t>, CompWrap> parents;

        explicit Node(bool is_final = false): is_final(is_final) { };

    };

    void dfs(std::set<size_t>& used, size_t cur) {
        if (used.count(cur)) return;
        used.insert(cur);
        for (size_t empty: elements[cur].edges[SymbolWrapper{' ', true}]) {
            dfs(used, empty);
        }
    }

    void prefix_dfs(std::set<size_t>& used, size_t cur) {
        if (used.count(cur)) return;
        used.insert(cur);
        for (auto c: alphabet) {
            for (size_t empty: elements[cur].edges[SymbolWrapper{c}]) {
                dfs(used, empty);
            }
        }
    }

    void add_edge(size_t from_state, size_t to_state, SymbolWrapper symbol) {
        elements[from_state].edges[symbol].emplace(to_state);
        elements[to_state].parents[symbol].emplace(from_state);
    }

    std::map<size_t, Node> elements;
    std::vector<char> alphabet; // so i could make a full automata and iterate over edges
    size_t starting_point;

    void add_vertices(const Automata& another, size_t shift) {
        for (auto& el: another.elements) {
            elements[el.first + shift].is_final = el.second.is_final;
            for (auto& symb: el.second.edges) {
                for (auto& nd: symb.second) {
                    elements[el.first + shift].edges[symb.first].insert(nd + shift);
                }
            }
            for (auto& symb: el.second.parents) {
                for (auto& nd: symb.second) {
                    elements[el.first + shift].parents[symb.first].insert(nd + shift);
                }
            }
        }
    }

    bool belongs_to_alphabet(char c) {
        return std::any_of(alphabet.begin(), alphabet.end(), [c](char x){return x == c;});
    }


public:

    Automata(size_t start_state_name, std::vector<char> alph): alphabet(std::move(alph)),
    starting_point(start_state_name) {
        elements.emplace(start_state_name, false);
    }

    Automata(size_t start_state_name, std::vector<char> alph, const std::string& RegExp):
    Automata(start_state_name, std::move(alph)) {
        if (RegExp.empty()) return;
        std::stack<Automata> operations_stack;
        for (size_t el_n = 0; el_n < RegExp.length(); ++el_n) {
            char el = RegExp[el_n];
            if (el == '.') {
                if (operations_stack.size() < 2) {
                    throw std::invalid_argument("The operation symbol on the place " +
                                                std::to_string(el_n + 1) + " is invalid, "
                                                                           "the Regular Expression is incorrect!\n");
                }
                Automata cur_top = operations_stack.top();
                operations_stack.pop();
                operations_stack.top() *= cur_top;
            } else if (el == '+') {
                if (operations_stack.size() < 2) {
                    throw std::invalid_argument("The operation symbol on the place " +
                                                std::to_string(el_n + 1) + " is invalid, "
                                                                           "the Regular Expression is incorrect!\n");
                }
                Automata cur_top = operations_stack.top();
                operations_stack.pop();
                operations_stack.top() += cur_top;
            } else if (el == '*') {
                if (operations_stack.empty()) {
                    throw std::invalid_argument("The operation symbol on the place " +
                                                std::to_string(el_n + 1) + " is invalid, "
                                                                           "the Regular Expression is incorrect!\n");
                }
                operations_stack.top().Kleene_closure();
            } else if (belongs_to_alphabet(el)){
                Automata new_automata(0, alphabet);
                new_automata.add_vert(1, true);
                new_automata.add_edge(0, 1, el);
                operations_stack.push(std::move(new_automata));
            } else {
                throw std::invalid_argument("The symbol on the place " +
                std::to_string(el_n + 1) + " is invalid, "
                                           "it must belong to the alphabet or the set of operands {*, +, .}\n");
            }
        }
        if (operations_stack.size() != 1) {
            throw std::invalid_argument("The string is invalid: it does not set the Expression"
                                        " because does not use all the operands or empty");
        }
        *this = operations_stack.top();
    }

    void Kleene_closure() {
        size_t new_start = elements.rbegin()->first + 1;
        add_vert(new_start, true);
        add_empty_edge(new_start, starting_point);
        for (auto& el: elements) {
            if (el.second.is_final) {
                add_empty_edge(el.first, starting_point);
            }
        }
        starting_point = new_start;
    }

    Automata& operator *=(const Automata& another) {
        size_t shift = elements.rbegin()->first + 1;
        add_vertices(another, shift);
        for (auto& el: elements) {
            if (el.second.is_final && el.first < shift) {
                el.second.is_final = false;
                add_empty_edge(el.first, another.starting_point + shift);
            }
        }
        return *this;
    }

    Automata& operator +=(const Automata& another) {
        size_t shift = elements.rbegin()->first + 1;
        add_vertices(another, shift);
        size_t els_in_another = another.elements.rbegin()->first + 1;

        add_vert(els_in_another + shift);
        add_empty_edge(els_in_another + shift, starting_point);
        starting_point = els_in_another + shift;
        add_empty_edge(starting_point, another.starting_point + shift);
        return *this;
    }

    void add_edge(size_t from_state, size_t to_state, char symbol) { // check if symbol is in alphabet
        add_edge(from_state, to_state, SymbolWrapper{symbol});
    }

    void add_empty_edge(size_t from_state, size_t to_state) {
        add_edge(from_state, to_state, SymbolWrapper{' ', true});
    }

    [[nodiscard]] size_t get_starting_point() const {
        return starting_point;
    }

    void add_vert(size_t state_name, bool is_final = false) {
        elements.emplace(state_name, is_final);
    }

    void make_final(size_t state_name) { // check if state exists
        elements[state_name].is_final = true;
    }

    void remove_edge(size_t from_state, size_t to_state, char symbol) {
        elements[from_state].edges[SymbolWrapper{symbol}].erase(to_state);
        elements[to_state].parents[SymbolWrapper{symbol}].erase(from_state);
    }

    void remove_empty_edge(size_t from_state, size_t to_state) {
        elements[from_state].edges[SymbolWrapper{' ', true}].erase(to_state);
        elements[to_state].parents[SymbolWrapper{' ', true}].erase(from_state);
    }

    void remove_vert(size_t vert_name) { // check if vert_name is not the starting point

        if (vert_name == starting_point) {
            throw std::invalid_argument("you can't remove the starting point");
        }

        for (auto smb: alphabet) {
            for (auto child: elements[vert_name].edges[SymbolWrapper{smb}]) {
                remove_edge(vert_name, child, smb);
            }
            for (auto parent: elements[vert_name].parents[SymbolWrapper{smb}]) {
                remove_edge(vert_name, parent, smb);
            }
        }

        for (auto child: elements[vert_name].edges[SymbolWrapper{' ', true}]) {
            remove_empty_edge(vert_name, child);
        }

        for (auto parent: elements[vert_name].parents[SymbolWrapper{' ', true}]) {
            remove_empty_edge(vert_name, parent);
        }

        elements.erase(vert_name);
    }

    bool is_final(size_t state_name) {
        return elements[state_name].is_final;
    }

    bool word_recognized(const std::string& word) {
        std::queue<std::pair<size_t, size_t>> to_check;
        to_check.push({starting_point, 0});
        std::pair<size_t, size_t> cur; // {node, word_pos}
        std::vector<std::set<size_t>> visited_when(elements.size());

        while (!to_check.empty()) {
            cur = to_check.front();
            to_check.pop();

            for (auto to: elements[cur.first].edges[SymbolWrapper{' ', true}]) {
                if (visited_when[to].count(cur.second)) continue;
                visited_when[to].insert(cur.second);
                to_check.push({to, cur.second});
            }

            if (cur.second == word.length()) {
                if (elements[cur.first].is_final) return  true;
                else continue;
            }

            for (auto to: elements[cur.first].edges[SymbolWrapper{word[cur.second]}]) {
                to_check.push({to, cur.second + 1});
                visited_when[to].insert(cur.second + 1);
            }

        }
        return false;
    }


    [[nodiscard]] std::map<size_t, std::set<std::pair<size_t, std::string>>> get_edges() const {
        std::map<size_t, std::set<std::pair<size_t, std::string>>> ans;
        for (auto& el: elements) {
            for (auto& word: el.second.edges) {
                for (auto to: word.second) {
                    if (!word.first.is_empty)
                        ans[el.first].insert(std::make_pair(to, std::string(1, word.first.symbol)));
                    else
                        ans[el.first].insert(std::make_pair(to, "empty"));
                }
            }
        }
        return ans;
    }


    void remove_empty_edges() {
        std::set<size_t> used;
        for (auto& el: elements) {
            dfs(used, el.first);
            for (auto empty: used) {
                for (auto word: alphabet) {
                    for (auto to: elements[empty].edges[SymbolWrapper{word}]) {
                        add_edge(el.first, to, word);
                    }
                }
            }
            used.clear();
        }

        for (auto el: elements) { // intentional copy
            for (size_t empty: el.second.edges[SymbolWrapper{' ', true}]) {
                remove_empty_edge(el.first, empty);
            }
        }
    }

    void make_determined() { // does not save the initial numeration (like, obviously)
        remove_empty_edges();
        size_t cur_name = 0;
        Automata new_automata(cur_name, alphabet);

        std::unordered_map<std::set<size_t>, size_t, Automata_details::SetHash<size_t>> set_to_state;
        set_to_state[{starting_point}] = cur_name;
        ++cur_name;
        std::queue<std::set<size_t>> in_progress;
        in_progress.push({starting_point});
        while (!in_progress.empty()) {
            std::set<size_t> cur = in_progress.front();
            in_progress.pop();
            std::set<size_t> can_go;
            bool is_final = false;

            for (auto vert: cur) {
                if (elements[vert].is_final) {
                    is_final = true;
                    break;
                }
            }

            if (is_final)
                new_automata.make_final(set_to_state[cur]);
            for (auto word: alphabet) {
                for (auto el: cur) {
                    for (auto to: elements[el].edges[SymbolWrapper{word}]) {
                        can_go.insert(to);
                    }
                }
                if (!set_to_state.count(can_go)) {
                    set_to_state[can_go] = cur_name;
                    ++cur_name;
                    new_automata.add_vert(cur_name);
                    in_progress.push(can_go);
                }
                new_automata.add_edge(set_to_state[cur], set_to_state[can_go], word);
                can_go.clear();
            }
        }
        *this = new_automata;
    }

    bool is_prefix(const std::string& s) {
        make_determined();
        size_t cur_index = 0;
        size_t cur_state = starting_point;
        while (cur_index < s.length()) {
            cur_state = *elements[cur_state].edges[SymbolWrapper{s[cur_index]}].begin();
            ++cur_index;
        }
        std::set<size_t> visited;
        prefix_dfs(visited, cur_state);

        return std::any_of(visited.begin(), visited.end(), [this](size_t el){ return elements[el].is_final; });

    }

};
