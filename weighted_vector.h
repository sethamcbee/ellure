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

    Weight find_weight(const T& t)
    {
        auto it = std::find(elements.begin(), elements.end(), t);
        if (it == elements.end())
        {
            return 0;
        }
        else
        {
            auto i = std::distance(elements.begin(), it);
            return weights[i];
        }
    }

    Weight find_weight_normalized(const T& t) const
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

    void scale(Weight multiplier)
    {
        for (auto& w : weights)
        {
            w = w * multiplier;
        }
        total_weight = total_weight * multiplier;
        prepare();
    }

    void merge(const WeightedVector<T>& other)
    {
        for (size_t i = 0; i < other.elements.size(); ++i)
        {
            const auto& t = other.elements[i];
            const auto& w = other.weights[i];
            insert(t, w);
        }
        prepare();
    }

    size_t size() const
    {
        return elements.size();
    }

    size_t size_bytes() const
    {
        size_t size = sizeof(WeightedVector<T>);
        size += sizeof(T) * elements.size();
        size += sizeof(Weight) * weights.size();
        size += sizeof(double) * weights.size();
        return size;
    }

    std::vector<T> elements;
    std::vector<Weight> weights;
    Weight total_weight = 0;
    std::discrete_distribution<> distribution;
};

}
