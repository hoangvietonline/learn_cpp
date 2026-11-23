#ifndef SDP_ALGORITHMS_H
#define SDP_ALGORITHMS_H

#include <vector>
#include <functional>
#include <cstddef>

namespace sdp::algo
{
    template <typename T, typename Compare>
    void mergeRange(std::vector<T> &values, std::vector<T> &buffer,
                    std::size_t first, std::size_t last, Compare compare)
    {
        if (last - first < 2) return;
        const auto mid = first + (last - first) / 2;
        mergeRange(values, buffer, first, mid, compare);
        mergeRange(values, buffer, mid, last, compare);
        auto i = first, j = mid;
        for (auto k = first; k < last; ++k)
        {
            if (i < mid && (j == last || !compare(values[j], values[i])))
                buffer[k] = values[i++];
            else
                buffer[k] = values[j++];
        }
        for (auto k = first; k < last; ++k) values[k] = buffer[k];
    }

    template <typename T, typename Compare = std::less<T>>
    void mergeSort(std::vector<T> &values, Compare compare = {})
    {
        auto buffer = values;
        mergeRange(values, buffer, 0, values.size(), compare);
    }

    // Return values.size() if not found; input must be sorted by compare.
    template <typename T, typename Compare = std::less<T>>
    std::size_t binarySearch(const std::vector<T> &values, const T &key,
                             Compare compare = {})
    {
        std::size_t first = 0, last = values.size();
        while (first < last)
        {
            const auto mid = first + (last - first) / 2;
            if (compare(values[mid], key)) first = mid + 1;
            else if (compare(key, values[mid])) last = mid;
            else return mid;
        }
        return values.size();
    }
}
#endif
