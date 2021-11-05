#include "Automata.h"
#include <gtest/gtest.h>

bool solve(Automata, char, int);

void out(Automata& b) {
    auto s = b.get_edges();

    for (auto& from: s) {
        std::cout << from.first;
        if (b.is_final(from.first)) {
            std::cout << " (final)";
        }
        if (from.first == b.get_starting_point()) {
            std::cout << " (starting)";
        }
        std::cout << ":\n";
        for (auto& p: from.second) {
            std::cout << '{' << p.first << ", " << p.second << '}' << " ";
        }
        std::cout << '\n';
    }
}

TEST(basic_operations, BASIC) {
    Automata a(0, {'a', 'b'});
    a.add_edge(0, 0, 'a');
    a.add_edge(0, 0, 'b');
    a.make_final(0);

    EXPECT_TRUE(a.word_recognized("abbbabbabbaaabbbbabbbbbbb"));
    EXPECT_TRUE(a.word_recognized("ababaaaaaaaabbbbbbb"));

    a.remove_edge(0, 0, 'a');
    EXPECT_FALSE(a.word_recognized("abab"));

    a.add_empty_edge(0, 0);

    EXPECT_FALSE(a.word_recognized("bbbba"));

    EXPECT_TRUE(a.word_recognized(""));

    Automata b(0, {'a', 'b'});

    b.add_vert(1);
    b.add_vert(2, true);

    b.add_empty_edge(0, 1);
    b.add_empty_edge(1, 2);
    b.add_empty_edge(2, 0);
    b.add_empty_edge(1, 1);

    EXPECT_TRUE(b.word_recognized(""));
    {
        Automata b(0, {'a', 'b'});

        b.add_vert(1);
        b.add_vert(2);
        b.add_vert(3);

        b.add_empty_edge(0, 1);
        b.add_empty_edge(0, 2);
        b.add_empty_edge(0, 3);

        b.add_edge(1, 3, 'b');
        b.make_final(3);
        b.remove_empty_edges();

        EXPECT_TRUE(b.word_recognized("b"));
    }

}


TEST(determination, DET_TEST) {
    Automata a(0, {'a', 'b', 'c'});
    for (int i = 1; i <= 3; ++i) {
        a.add_vert(i, true);
        a.add_edge(0, i, 'a');
        a.add_edge(i, i, 'a' + i - 1);
    }

    EXPECT_TRUE(a.word_recognized("aaaaaaaaaa"));
    EXPECT_TRUE(a.word_recognized(("abbbbbbbbbb")));
    EXPECT_FALSE(a.word_recognized("aab"));
    EXPECT_TRUE(a.word_recognized("accccccc"));
    EXPECT_FALSE(a.word_recognized(("cccccccc")));

    a.remove_empty_edges();

    a.add_vert(100);
    a.remove_vert(100);
    EXPECT_THROW(a.remove_vert(a.get_starting_point()), std::invalid_argument);
    auto els = a.get_edges();

    EXPECT_TRUE(a.word_recognized("aaaaaaaaaa"));
    EXPECT_TRUE(a.word_recognized(("abbbbbbbbbb")));
    EXPECT_FALSE(a.word_recognized("aab"));
    EXPECT_TRUE(a.word_recognized("accccccc"));
    EXPECT_FALSE(a.word_recognized(("cccccccc")));

    Automata b(0, {'a', 'b'});
    for (int i = 1; i <= 3; ++i) {
        b.add_vert(i, false);
        b.add_empty_edge(i - 1, i);
    }
    b.add_edge(3, 0, 'a');
    b.make_final(0);

    EXPECT_TRUE(b.word_recognized("aaaaaa"));
    EXPECT_TRUE(b.word_recognized("a"));
    EXPECT_FALSE(b.word_recognized("ab"));

    b.remove_empty_edges();

    EXPECT_TRUE(b.word_recognized("aaaaaa"));
    EXPECT_TRUE(b.word_recognized("a"));
    EXPECT_FALSE(b.word_recognized("ab"));

    b.make_determined();

    a.make_determined();

    EXPECT_TRUE(a.word_recognized("aaaaaaaaaa"));
    EXPECT_TRUE(a.word_recognized(("abbbbbbbbbb")));
    EXPECT_FALSE(a.word_recognized("aab"));
    EXPECT_TRUE(a.word_recognized("accccccc"));
    EXPECT_FALSE(a.word_recognized(("cccccccc")));

}

TEST(REGEXP, PARSER) {
    EXPECT_THROW(Automata(0, {'a', 'b', 'c'}, "a*b+a"), std::invalid_argument);
    EXPECT_THROW(Automata(0, {'a', 'b', 'c'}, "+a"), std::invalid_argument);
    EXPECT_THROW(Automata(0, {'a', 'b', 'c'}, "*b+a"), std::invalid_argument);
    EXPECT_THROW(Automata(0, {'a', 'b', 'c'}, "a.b+a"), std::invalid_argument);
    Automata a(0, {'a', 'b', 'c'}, "a*b*.");
    EXPECT_TRUE(a.word_recognized("aaaabb"));
    Automata b(0, {'a', 'b', 'c'}, "");
    EXPECT_FALSE(b.word_recognized(""));
    EXPECT_FALSE(b.word_recognized("a"));
    Automata c(0, {'a', 'b'}, "ab.*");
    EXPECT_TRUE(c.word_recognized("abababab"));
    EXPECT_TRUE(c.word_recognized(""));
    EXPECT_FALSE(c.word_recognized("aabababa"));
    Automata d(0, {'a', 'b'}, "b*a.b*.a.b*.*");
    EXPECT_FALSE(d.word_recognized("aaa"));
    EXPECT_FALSE(d.word_recognized("aaabababaabb"));
    EXPECT_TRUE(d.word_recognized("aabbaa"));
    EXPECT_TRUE(d.word_recognized("abababa"));
    Automata a1(0, {'a', 'b', 'c'}, "ab+c.aba.*.bac.+.+*");
    EXPECT_TRUE(a1.word_recognized("acab"));
    EXPECT_FALSE(a1.word_recognized("aab"));
    EXPECT_TRUE(a1.word_recognized("bcabaac"));
    EXPECT_TRUE(a1.word_recognized("ab"));
    a1.make_determined();
    EXPECT_TRUE(a1.word_recognized("acab"));
    EXPECT_FALSE(a1.word_recognized("aab"));
    EXPECT_TRUE(a1.word_recognized("bcabaac"));
}


TEST(TASK3_TEST, PREFIX_TEST) {
    Automata a1(0, {'a', 'b', 'c'}, "ab+c.aba.*.bac.+.+*");
    EXPECT_TRUE(a1.is_prefix("ac"));
    EXPECT_TRUE(a1.is_prefix("a"));
    EXPECT_TRUE(a1.is_prefix(""));
    EXPECT_TRUE(a1.is_prefix("ab"));
    EXPECT_TRUE(a1.is_prefix("bca"));
    EXPECT_TRUE(a1.is_prefix("a"));
    Automata a(0, {'a', 'b', 'c'}, "a*b*.");
    EXPECT_TRUE(a.is_prefix("aaaa"));
    Automata b(0, {'a', 'b', 'c'}, "");
    EXPECT_FALSE(b.is_prefix(""));
    EXPECT_FALSE(b.is_prefix("a"));
    Automata c(0, {'a', 'b'}, "ab.*");
    EXPECT_TRUE(c.is_prefix("abababa"));
    EXPECT_TRUE(c.is_prefix(""));
    EXPECT_FALSE(c.is_prefix("aabab"));
}


TEST(TASK3_TEST, ALGO) {
    Automata a1(0, {'a', 'b', 'c'}, "ab+c.aba.*.bac.+.+*");
    Automata a2(0, {'a', 'b', 'c'}, "a*b*.");
    Automata a5(0, {'a', 'b', 'c'}, "ab.c+*ca+*.c.");
    Automata a6(0, {'a', 'b', 'c'}, "a*");

    EXPECT_TRUE(solve(a1, 'a', 2));
    for (int i = 1; i < 100; ++i) {
        EXPECT_TRUE(solve(a6, 'a', i));
        EXPECT_FALSE(solve(a6, 'b', i));
    }

    EXPECT_TRUE(solve(a2, 'b', 3));
    EXPECT_FALSE(solve(a2, 'c', 1));
    EXPECT_TRUE(solve(a2, 'c', 0));

    EXPECT_TRUE(solve(a5, 'c', 10));
    EXPECT_FALSE(solve(a5, 'b', 1));
    EXPECT_TRUE(solve(a5, 'a', 1));
    EXPECT_TRUE(solve(a5, 'a', 2));
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

}
