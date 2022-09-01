/**
 * @file weighted_vector.h
 */

#pragma once

#include <algorithm>
#include <random>
#include <vector>

namespace Ellure
{

template <class T>
class WeightedVector
{
public:

    void insert(const T& t)
    {
        ++totalWeight;

        for (size_t i = 0; i < elements.size(); ++i)
        {
            if (elements[i] == t)
            {
                weights[i] += 1;
                return;
            }
        }
        // Else, add new element.
        elements.push_back(t);
        weights.push_back(1);
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
    std::vector<int> weights;
    int totalWeight = 0;
    std::discrete_distribution<> distribution;
};

}
