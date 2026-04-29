#include "TextContainer.hh"

#include <cassert>
#include <string>
#include <vector>

std::vector<std::string> collect_words(char* const buffer) {
    TextContainer text(buffer);
    std::vector<std::string> words;

    const char* word = nullptr;
    while ((word = text.GetNextWord()) != nullptr) {
        words.emplace_back(word);
    }

    return words;
}

int main() {
    {
        char text[] = "one, two-three";
        const std::vector<std::string> expected = {"one", "two", "three"};
        assert(collect_words(text) == expected);
    }

    {
        char text[] = "\xEF\xBB\xBF"
                      "\xE2\x80\x9C"
                      "hello"
                      "\xE2\x80\x9D"
                      "\xE2\x80\x94"
                      "world"
                      "\xE2\x80\x9D";
        const std::vector<std::string> expected = {"hello", "world"};
        assert(collect_words(text) == expected);
    }

    {
        char text[] = "\xD0\x9F\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82"
                      "\xE2\x80\x94"
                      "\xD0\xBC\xD0\xB8\xD1\x80"
                      " "
                      "\xC2\xAB"
                      "\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"
                      "\xC2\xBB";
        const std::vector<std::string> expected = {
            "\xD0\x9F\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82",
            "\xD0\xBC\xD0\xB8\xD1\x80",
            "\xD1\x82\xD0\xB5\xD1\x81\xD1\x82",
        };
        assert(collect_words(text) == expected);
    }

    {
        char text[] = "\xE2\x80\x9C"
                      "\xE2\x80\x9D"
                      "\xE2\x80\x94";
        assert(collect_words(text).empty());
    }

    {
        char text[] = "\xFF"
                      "\x80"
                      "safe"
                      "\xC0\xAF"
                      "word";
        const std::vector<std::string> expected = {"safe", "word"};
        assert(collect_words(text) == expected);
    }

    return 0;
}
