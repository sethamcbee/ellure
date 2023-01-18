/**
 * @file complex_word_chain.cc
 */

#include "complex_word_chain.h"

#include "gui/editor.h"
#include "pos.h"

extern std::string lapos_main(const std::string& input);

namespace Ellure
{

ComplexWordChain::ComplexWordChain()
{
    // Do nothing.
}

ComplexWordChain::ComplexWordChain(const std::vector<std::string>& input)
{
    construct_unigrams(input);
    construct_bigrams(input);
    construct_trigrams(input);
}

Weight ComplexWordChain::get_size()
{
    return unigrams.links;
}

void ComplexWordChain::scale(Weight multiplier)
{
    unigrams.scale(multiplier);
    bigrams.scale(multiplier);
    trigrams.scale(multiplier);

    unigram_beginnings.scale(multiplier);
    bigram_beginnings.scale(multiplier);
    trigram_beginnings.scale(multiplier);
}

void ComplexWordChain::merge(const ComplexWordChain& other)
{
    unigrams.merge(other.unigrams);
    bigrams.merge(other.bigrams);
    trigrams.merge(other.trigrams);

    unigram_beginnings.merge(other.unigram_beginnings);
    bigram_beginnings.merge(other.bigram_beginnings);
    trigram_beginnings.merge(other.trigram_beginnings);
}

std::string ComplexWordChain::get_word_state(const LineState& state)
{
    // Get current state.
    size_t current_i = state.tokens.size() - 2;
    const auto& current_word = state.tokens[current_i];
    std::string pos_0;
    std::string pos_1;
    if (state.pos.size() == 2)
    {
        pos_0 = "[START]";
        pos_1 = "[START]";
    }
    else if (state.pos.size() == 3)
    {
        pos_0 = "[START]";
        pos_1 = state.pos[current_i - 1];
    }
    else
    {
        pos_0 = state.pos[current_i - 2];
        pos_1 = state.pos[current_i - 1];
    }
    Value2 current_v2{pos_1, current_word};
    Value3 current_v3{pos_0, pos_1, current_word};

    // Get n-gram possibilities.
    int uni_max = unigrams.get_options(current_word);
    int bi_max = uni_max + bigrams.get_options(current_v2);
    int tri_max = bi_max + trigrams.get_options(current_v3);

    // Determine which mode we'll use to generate a word.
    int roll = rand() % tri_max;
    if (current_word == "[START]")
    {
        bigrams.set_state(bigram_beginnings.get(rng));
        return get_word_bigrams();
    }
    else if (roll < uni_max)
    {
        if (current_word == "[START]")
        {
            unigrams.set_state(unigram_beginnings.get(rng));
        }
        else
        {
            unigrams.set_state(current_word);
            get_word_unigrams();
        }
        return get_word_unigrams();
    }
    else if (roll < bi_max)
    {
        bigrams.set_state(current_v2);
        get_word_bigrams();
        return get_word_bigrams();
    }
    else
    {
        // roll <= tri_max
        trigrams.set_state(current_v3);
        get_word_trigrams();
        return get_word_trigrams();
    }
}

std::string ComplexWordChain::get_line_state(const LineState& state)
{
    int n = 0;
    std::string last;
    std::string output;
    do
    {
        ++n;
        if (n > 20)
        {
            return output;
        }

        auto whole_line = state.line + output;
        auto start = whole_line.c_str();
        auto end = start + whole_line.size();
        LineState running_state{start, end};
        auto word = get_word_state(running_state);

        if (word == "[END]")
        {
            return output;
        }

        if (word == "[START]")
        {
            continue;
        }

        if (word == "'s"
            || word == "'m"
            || word == "'d"
            || word == "'ll"
            || word == "'re"
            || word == "'ve"
            || word == "n't")
        {
            output += word;
            last = word;
            continue;
        }
        if (last == "Gim" || last == "gim" || last == "Lem" || last == "lem")
        {
            if (word == "me")
            {
                output += word;
                last = word;
                continue;
            }
        }
        if (last == "Gon" || last == "gon" || last == "Wan" || last == "wan")
        {
            if (word == "na")
            {
                output += word;
                last = word;
                continue;
            }
        }
        if (last == "Got" || last == "got")
        {
            if (word == "ta")
            {
                output += word;
                last = word;
                continue;
            }
        }

        if (word != "," && output != "")
        {
            output += " ";
        }
        output += word;
    }
    while (1);

    return output;
}

std::string ComplexWordChain::get_line_unigrams()
{
    std::string last;
    std::string output;
    do
    {
        auto t = unigrams.get();

        if (t == "[END]")
        {
            unigrams.set_state(unigram_beginnings.get(rng));
            return output;
        }

        if (t == "[START]")
        {
            continue;
        }

        if (t == "'s"
            || t == "'m"
            || t == "'d"
            || t == "'ll"
            || t == "'re"
            || t == "'ve"
            || t == "n't")
        {
            output += t;
            last = t;
            continue;
        }
        if (last == "Gim" || last == "gim" || last == "Lem" || last == "lem")
        {
            if (t == "me")
            {
                output += t;
                last = t;
                continue;
            }
        }
        if (last == "Gon" || last == "gon" || last == "Wan" || last == "wan")
        {
            if (t == "na")
            {
                output += t;
                last = t;
                continue;
            }
        }
        if (last == "Got" || last == "got")
        {
            if (t == "ta")
            {
                output += t;
                last = t;
                continue;
            }
        }

        if (t != "," && output != "")
        {
            output += " ";
        }
        output += t;

        // Alpha.
        if (!(rand() % 20))
        {
            output += ',';
            unigrams.set_state(unigram_beginnings.get(rng));
        }

        last = t;
    }
    while (1);

    unigrams.set_state(unigram_beginnings.get(rng));
    return output;
}

std::string ComplexWordChain::get_line_bigrams()
{
    std::string last;
    std::string output;
    do
    {
        const auto& pair = bigrams.get();
        auto t = std::get<1>(pair);

        if (t == "[END]")
        {
            bigrams.set_state(bigram_beginnings.get(rng));
            return output;
        }

        if (t == "[START]")
        {
            continue;
        }

        if (t == "'s"
            || t == "'m"
            || t == "'d"
            || t == "'ll"
            || t == "'re"
            || t == "'ve"
            || t == "n't")
        {
            output += t;
            last = t;
            continue;
        }
        if (last == "Gim" || last == "gim" || last == "Lem" || last == "lem")
        {
            if (t == "me")
            {
                output += t;
                last = t;
                continue;
            }
        }
        if (last == "Gon" || last == "gon" || last == "Wan" || last == "wan")
        {
            if (t == "na")
            {
                output += t;
                last = t;
                continue;
            }
        }
        if (last == "Got" || last == "got")
        {
            if (t == "ta")
            {
                output += t;
                last = t;
                continue;
            }
        }

        if (t != "," && output != "")
        {
            output += " ";
        }
        output += t;

        // Alpha.
        if (!(rand() % 20))
        {
            output += ',';
            bigrams.set_state(bigram_beginnings.get(rng));
        }

        last = t;
    }
    while (1);

    bigrams.set_state(bigram_beginnings.get(rng));
    return output;
}

std::string ComplexWordChain::get_line_trigrams()
{
    std::string last;
    std::string output;
    do
    {
        const auto& pair = trigrams.get();
        auto t = std::get<2>(pair);

        if (t == "[END]")
        {
            trigrams.set_state(trigram_beginnings.get(rng));
            return output;
        }

        if (t == "[START]")
        {
            continue;
        }

        if (t == "'s"
            || t == "'m"
            || t == "'d"
            || t == "'ll"
            || t == "'re"
            || t == "'ve"
            || t == "n't")
        {
            output += t;
            last = t;
            continue;
        }
        if (last == "Gim" || last == "gim" || last == "Lem" || last == "lem")
        {
            if (t == "me")
            {
                output += t;
                last = t;
                continue;
            }
        }
        if (last == "Gon" || last == "gon" || last == "Wan" || last == "wan")
        {
            if (t == "na")
            {
                output += t;
                last = t;
                continue;
            }
        }
        if (last == "Got" || last == "got")
        {
            if (t == "ta")
            {
                output += t;
                last = t;
                continue;
            }
        }

        if (t != "," && output != "")
        {
            output += " ";
        }
        output += t;

        // Alpha.
        if (!(rand() % 20))
        {
            output += ',';
            trigrams.set_state(trigram_beginnings.get(rng));
        }

        last = t;
    }
    while (1);

    trigrams.set_state(trigram_beginnings.get(rng));
    return output;
}

std::string ComplexWordChain::get_word_unigrams()
{
    do
    {
        auto word = unigrams.get();

        if (word == "[END]")
        {
            unigrams.set_state(unigram_beginnings.get(rng));
            continue;
        }

        if (word == "[START]")
        {
            continue;
        }

        if (word == "Gim" || word == "gim" || word == "Lem" || word == "lem")
        {
            word += "me";
            return word;
        }

        if (word == "Gon" || word == "gon" || word == "Wan" || word == "wan")
        {
            word += "na";
            return word;
        }

        // Else, whole word.
        return word;
    }
    while (1);
}

std::string ComplexWordChain::get_word_bigrams()
{
    do
    {
        auto pair = bigrams.get();
        auto word = std::get<1>(pair);

        if (word == "[END]")
        {
            bigrams.set_state(bigram_beginnings.get(rng));
            return word;
        }

        if (word == "[START]")
        {
            continue;
        }

        if (word == "Gim" || word == "gim" || word == "Lem" || word == "lem")
        {
            word += "me";
            return word;
        }

        if (word == "Gon" || word == "gon" || word == "Wan" || word == "wan")
        {
            word += "na";
            return word;
        }

        // Else, whole word.
        return word;
    }
    while (1);
}

std::string ComplexWordChain::get_word_trigrams()
{
    do
    {
        auto tuple = trigrams.get();
        auto word = std::get<2>(tuple);

        if (word == "[END]")
        {
            trigrams.set_state(trigram_beginnings.get(rng));
            return word;
        }

        if (word == "[START]")
        {
            continue;
        }

        if (word == "Gim" || word == "gim" || word == "Lem" || word == "lem")
        {
            word += "me";
            return word;
        }

        if (word == "Gon" || word == "gon" || word == "Wan" || word == "wan")
        {
            word += "na";
            return word;
        }

        // Else, whole word.
        return word;
    }
    while (1);
}

void ComplexWordChain::construct_unigrams(const std::vector<std::string>& input)
{
    for (size_t start = 0; start < input.size(); ++start)
    {
        auto end = start + 1;
        while (input[end] != "[END]")
        {
            ++end;
        }
        if (end == start + 1)
        {
            // Skip empty lines.
            ++start;
            continue;
        }

        Weight weight{default_weight};
        bool first = true;
        for (size_t i = start; i < end; ++i)
        {
            unigrams.insert(input[i], input[i + 1], weight);
            if (first)
            {
                first = false;
                unigram_beginnings.insert(input[i], 1.0);
            }
        }
    }

    unigrams.prepare();
    unigrams.reset();
    unigram_beginnings.prepare();
}

void ComplexWordChain::construct_bigrams(const std::vector<std::string>& input)
{
    // Associate each word with the POS preceding it.
    for (size_t start = 0; start < input.size(); ++start)
    {
        auto end = start + 1;
        while (input[end] != "[END]")
        {
            ++end;
        }
        if (end == start + 1)
        {
            // Skip empty lines.
            ++start;
            continue;
        }

        // Build line for lapos.
        std::string line{input[start + 1]};
        for (size_t i = start + 2; i < end; ++i)
        {
            if (input[i] != ",")
            {
                line += " ";
            }
            line += input[i];
        }
        auto pos_str = lapos_main(line);
        POS pos{pos_str};
        const auto& pos_data = pos.get_data();

        // Build chain, including START and END tokens.
        Weight weight{1.0 / pos_data.size()};
        bool first = true;
        for (size_t i = 1; i < pos_data.size() - 1; ++i)
        {
            auto gram0 = std::tuple{pos_data[i - 1], input[start + i]};
            auto gram1 = std::tuple{pos_data[i], input[start + i + 1]};
            bigrams.insert(gram0, gram1, weight);
            if (first)
            {
                first = false;
                bigram_beginnings.insert(gram0, 1.0);
            }
        }

        // Iterate to next line.
        start = end;
    }

    bigrams.prepare();
    bigrams.reset();
    bigram_beginnings.prepare();
}

void ComplexWordChain::construct_trigrams(const std::vector<std::string>& input)
{
    // Associate each word with the prior two parts of speech.
    for (size_t start = 0; start < input.size(); ++start)
    {
        auto end = start + 1;
        while (input[end] != "[END]")
        {
            ++end;
        }
        if (end == start + 1)
        {
            // Skip empty lines.
            ++start;
            continue;
        }

        // Build line for lapos.
        std::string line{input[start + 1]};
        for (size_t i = start + 2; i < end; ++i)
        {
            if (input[i] != ",")
            {
                line += " ";
            }
            line += input[i];
        }
        auto pos_str = lapos_main(line);
        POS pos{pos_str};
        const auto& pos_data = pos.get_data();

        // Check for short line.
        Weight weight{default_weight};
        if (end - start < 3) // START{0} he{1} died{2} END{3}
        {
            auto gram0 = std::tuple{"[START]", "[START]", input[start + 1]};
            auto gram1 = std::tuple{"[START]", input[start + 1], "[END]"};
            trigrams.insert(gram0, gram1, weight);
            trigram_beginnings.insert(gram0, 1.0);
            start = end;
            continue;
        }

        // Build padded starting trigram.
        auto gram_start = std::tuple{"[START]", "[START]", input[start + 1]};
        auto gram_second = std::tuple{"[START]", pos_data[1], input[start + 2]};
        trigrams.insert(gram_start, gram_second, weight);
        trigram_beginnings.insert(gram_start, 1.0);

        // Build chain, including START and END tokens.
        for (size_t i = 2; i < pos_data.size() - 1; ++i)
        {
            auto gram0 = std::tuple{pos_data[i - 2], pos_data[i - 1], input[start + i]};
            auto gram1 = std::tuple{pos_data[i - 1], pos_data[i], input[start + i + 1]};
            trigrams.insert(gram0, gram1, weight);
        }

        // Iterate to next line.
        start = end;
    }

    trigrams.prepare();
    trigrams.reset();
    trigram_beginnings.prepare();
}

}
