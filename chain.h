/**
 * @file chain.h
 */

#pragma once

#include <map>
#include <random>
#include <vector>

#include "weighted_vector.h"

namespace Ellure
{

static const Weight default_weight{1.0};

template <class T>
struct ChainState
{
    T element;
    WeightedVector<int> sequences;
};

template <class T>
class Chain
{
public:

    void insert(const T& cur, const T& next)
    {
        // Check if current state exists.
        int i_cur;
        auto it_cur = state_map.find(cur);
        if (it_cur != state_map.end())
        {
            i_cur = it_cur->second;
        }
        else
        {
            // Add new state.
            i_cur = states.size();
            state_map[cur] = i_cur;

            ChainState<T> new_state;
            new_state.element = cur;
            states.push_back(new_state);
        }

        // Check if next state exists.
        int i_next;
        auto it_next = state_map.find(next);
        if (it_next != state_map.end())
        {
            i_next = it_next->second;
        }
        else
        {
            // Add new state.
            i_next = states.size();
            state_map[next] = i_next;

            ChainState<T> new_state;
            new_state.element = next;
            states.push_back(new_state);
        }

        // Add new link to chain.
        states[i_cur].sequences.insert(i_next, default_weight);
        ++links;
    }

    void prepare()
    {
        states.shrink_to_fit();
        
        for (auto& s : states)
        {
            s.sequences.prepare();
        }
    }

    const T& get()
    {
        const auto& ret = states[state].element;
        
        // Advance internal state.
        state = states[state].sequences.get(rng);

        return ret;
    }

    void reset()
    {
        size_t max = states.size();
        state = rand() % max;
    }

private:

    std::mt19937 rng{(long unsigned int)time(NULL)};
    int state = 0;
    int links = 0;
    std::map<T, int> state_map;
    std::vector<ChainState<T>> states;
};

}
