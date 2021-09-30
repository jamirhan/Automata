#include "Automata.h"
#include <gtest/gtest.h>


void out(Automata& b) {
    auto s = b.get_edges();

    for (auto& from: s) {
        std::cout << from.first;
        if (b.is_final(from.first)) {
            std::cout << " (final)";
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


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
