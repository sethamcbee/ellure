/**
 * @file complex_word_chain.h
 */

#pragma once

#include <string>
#include <vector>

#include "chain.h"

namespace Ellure
{

struct LineState;

class ComplexWordChain
{
public:

    // string1 = current word
    using Value = std::string;

    // string1 = Previous POS, string2 = current word
    using Value2 = std::tuple<std::string, std::string>;

    // string1 = POS - 2, string2 = POS - 1, string3 = current word
    using Value3 = std::tuple<std::string, std::string, std::string>;

    ComplexWordChain();
    ComplexWordChain(const std::vector<std::string>& input);

    Weight get_size();

    void scale(Weight multiplier);

    void merge(const ComplexWordChain& other);

    std::string get_word_state(const LineState& state);
    std::string get_line_state(const LineState& state);

    std::string get_line_unigrams();
    std::string get_line_bigrams();
    std::string get_line_trigrams();

    std::string get_word_unigrams();
    std::string get_word_bigrams();
    std::string get_word_trigrams();

private:

    std::mt19937 rng{(long unsigned int)time(NULL)};
    Chain<Value> unigrams;
    Chain<Value2> bigrams;
    Chain<Value3> trigrams;
    WeightedVector<Value> unigram_beginnings;
    WeightedVector<Value2> bigram_beginnings;
    WeightedVector<Value3> trigram_beginnings;

    void construct_unigrams(const std::vector<std::string>& input);
    void construct_bigrams(const std::vector<std::string>& input);
    void construct_trigrams(const std::vector<std::string>& input);
};

}
