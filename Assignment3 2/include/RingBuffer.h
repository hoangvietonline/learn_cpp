#ifndef SDP_RING_BUFFER_H
#define SDP_RING_BUFFER_H

#include "Errors.h"
#include <array>
#include <type_traits>
#include <cstddef>

namespace sdp::ds
{
    template <typename T> struct IsNumeric : std::is_arithmetic<T> {};
    template <> struct IsNumeric<std::string> : std::false_type {};

    template <typename T, std::size_t N>
    class RingBuffer
    {
        static_assert(N > 0, "Capacity must be positive");
        std::array<T, N> data{};
        std::size_t next = 0, count = 0;

    public:
        void push(const T &value)
        {
            data[next] = value;
            next = (next + 1) % N;
            if (count < N) ++count;
        }

        std::size_t size() const { return count; }
        bool empty() const { return count == 0; }

        double average() const
        {
            static_assert(IsNumeric<T>::value, "average requires arithmetic type");
            if (empty()) throw EmptyContainerError("Ring buffer is empty");
            double sum = 0;
            for (std::size_t i = 0; i < count; ++i) sum += data[i];
            return sum / static_cast<double>(count);
        }
    };
}
#endif
