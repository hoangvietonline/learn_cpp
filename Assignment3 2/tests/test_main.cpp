#include "Algorithms.h"
#include "RingBuffer.h"
#include "ThreadSafeQueue.h"
#include "Processing.h"
#include <cassert>
#include <iostream>
#include <queue>
#include <thread>

namespace sdp
{
    void test()
    {
        ds::LinkedQueue<int> queue;
        assert(queue.empty());
        try { queue.pop(); assert(false); }
        catch (const EmptyContainerError &) {}
        queue.push(5);
        assert(queue.front() == 5 && queue.size() == 1);
        queue.push(7);
        assert(queue.pop() == 5 && queue.pop() == 7 && queue.empty());
        ds::LinkedQueue<std::string> strings;
        strings.push("hello");
        assert(strings.pop() == "hello");
        ds::RingBuffer<double, 3> ring;
        try { ring.average(); assert(false); }
        catch (const EmptyContainerError &) {}
        ring.push(1); assert(ring.average() == 1);
        ring.push(2); ring.push(3); assert(ring.average() == 2);
        ring.push(4); assert(ring.size() == 3 && ring.average() == 3);
        ds::RingBuffer<int, 2> integerRing;
        integerRing.push(3); integerRing.push(5);
        assert(integerRing.average() == 4);
        ds::RingBuffer<double, 10> lastTen;
        for (int i = 1; i <= 12; ++i) lastTen.push(i);
        assert(lastTen.average() == 7.5);
        std::vector<int> numbers{3, 1, 2, 2};
        algo::mergeSort(numbers);
        assert((numbers == std::vector<int>{1, 2, 2, 3}));
        assert(numbers[algo::binarySearch(numbers, 2)] == 2);
        assert(algo::binarySearch(numbers, 7) == numbers.size());
        std::vector<double> doubles{1.5, 4.2, 2.1};
        algo::mergeSort(doubles, std::greater<double>{});
        assert(doubles.front() == 4.2);
        struct Item { int key; };
        auto byKey = [](const Item &a, const Item &b) { return a.key < b.key; };
        std::vector<Item> items{{5}, {2}};
        algo::mergeSort(items, byKey);
        assert(algo::binarySearch(items, Item{5}, byKey) == 1);
        ThreadSafeQueue<int> shared;
        std::queue<int> reference;
        for (int i = 0; i < 1000; ++i) reference.push(i);
        std::thread producer([&shared] {
            for (int i = 0; i < 1000; ++i) shared.push(i);
            shared.close();
        });
        int value;
        while (shared.pop(value))
        {
            assert(!reference.empty() && value == reference.front());
            reference.pop();
        }
        producer.join();
        assert(reference.empty() && !shared.pop(value));
        try { shared.push(1); assert(false); }
        catch (const Error &) {}
        ThreadSafeQueue<int> emptyQueue;
        std::thread consumer([&emptyQueue] { int x; assert(!emptyQueue.pop(x)); });
        emptyQueue.close();
        consumer.join();
        auto group = std::make_shared<Group>("SPEED");
        std::weak_ptr<Group> weakGroup = group;
        auto sensor = std::make_shared<Sensor>("SPD", "SPEED", group);
        group->add(sensor);
        assert(group.use_count() == 1 && sensor.use_count() == 2);
        group.reset();
        assert(weakGroup.expired() && sensor.use_count() == 1);
        // Equal timestamps must be ordered by input line, not arrival order.
        auto speedGroup = std::make_shared<Group>("SPEED");
        Sensor speed("SPD", "SPEED", speedGroup);
        speed.add({100, 3, "SPD", "SPEED", 40});
        speed.add({100, 2, "SPD", "SPEED", 100});
        speed.add({100, 1, "SPD", "SPEED", 40});
        const auto stats = speed.finalize();
        assert(stats.alarms.size() == 2 && stats.alarms[0].line == 2 && stats.alarms[1].line == 3);
        assert(io::parse("1000;TEMP_01;TEMP;36.5", 1).value == 36.5);
        try { io::parse("1;TEMP;TEMP;abc", 2); assert(false); }
        catch (const ParseError &) {}
        std::cout << "All unit tests passed\n";
    }
}

int main() { sdp::test(); }
