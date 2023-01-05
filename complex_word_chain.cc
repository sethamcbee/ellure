/**
 * @file complex_word_chain.cc
 */

#include "complex_word_chain.h"

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

        if (word == "Got" || word == "got")
        {
            word += "ta";
            return word;
        }

        // Else, whole word.
        return word;
    }
    while (1);
}

std::string ComplexWordChain::get_word_trigrams()
{
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
