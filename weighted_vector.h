/**
 * @file weighted_vector.h
 */

#pragma once

#include <algorithm>
#include <random>
#include <vector>

namespace Ellure
{

typedef double Weight;

template <class T>
class WeightedVector
{
public:

    void insert(const T& t, Weight w)
    {
        totalWeight += w;

        for (size_t i = 0; i < elements.size(); ++i)
        {
            if (elements[i] == t)
            {
                weights[i] += w;
                return;
            }
        }
        // Else, add new element.
        elements.push_back(t);
        weights.push_back(w);
    }

    void prepare()
    {
        std::discrete_distribution<> newDist(weights.begin(), weights.end());
        distribution = newDist;

        elements.shrink_to_fit();
        weights.shrink_to_fit();
    }

    template <class G>
    const T& get(G& g)
    {
        return elements[distribution(g)];
    }

private:

    std::vector<T> elements;
    std::vector<Weight> weights;
    Weight totalWeight = 0;
    std::discrete_distribution<> distribution;
};

}
