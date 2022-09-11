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
        total_weight += w;

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

    Weight find_weight(const T& t) const
    {
        auto it = std::find(elements.begin(), elements.end(), t);
        if (it == elements.end())
        {
            return 0;
        }
        else
        {
            auto i = std::distance(elements.begin(), it);
            return weights[i] / total_weight;
        }
    }

    size_t size_bytes() const
    {
        size_t size = sizeof(WeightedVector<T>);
        size += sizeof(T) * elements.size();
        size += sizeof(Weight) * weights.size();
        size += sizeof(double) * weights.size();
        return size;
    }
    
private:

    std::vector<T> elements;
    std::vector<Weight> weights;
    Weight total_weight = 0;
    std::discrete_distribution<> distribution;
};

}
