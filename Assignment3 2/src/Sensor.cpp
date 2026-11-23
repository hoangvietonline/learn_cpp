#include "Sensor.h"
#include "RingBuffer.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <tuple>

namespace sdp
{
    void Logger::log(const std::string &message)
    {
        std::lock_guard<std::mutex> lock(mutex);
        std::cerr << message << '\n';
    }

    bool validRange(const Sample &s)
    {
        if (!std::isfinite(s.value)) return false;
        if (s.type == "TEMP") return s.value >= -40 && s.value <= 150;
        if (s.type == "VOLT") return s.value >= 0 && s.value <= 18;
        if (s.type == "SPEED") return s.value >= 0 && s.value <= 300;
        return false;
    }

    Sensor::Sensor(std::string id, std::string type, const std::shared_ptr<Group> &group)
        : id(std::move(id)), type(std::move(type)), group(group) {}

    void Sensor::add(Sample sample)
    {
        std::lock_guard<std::mutex> lock(mutex);
        samples.push_back(std::move(sample));
    }

    Statistics Sensor::finalize()
    {
        std::lock_guard<std::mutex> lock(mutex);
        std::sort(samples.begin(), samples.end(), [](const Sample &a, const Sample &b) {
            return std::tie(a.timestamp, a.line) < std::tie(b.timestamp, b.line);
        });
        Statistics result;
        result.id = id;
        result.type = type;
        if (const auto owner = group.lock()) result.group = owner->getName();
        result.count = samples.size();
        if (samples.empty()) return result;
        result.minimum = result.maximum = samples.front().value;
        ds::RingBuffer<double, 10> window;
        double previous = samples.front().value;
        for (const auto &sample : samples)
        {
            result.minimum = std::min(result.minimum, sample.value);
            result.maximum = std::max(result.maximum, sample.value);
            window.push(sample.value);
            const bool alarm = (type == "TEMP" && sample.value > 105) ||
                (type == "VOLT" && (sample.value < 9 || sample.value > 16)) ||
                (type == "SPEED" && std::abs(sample.value - previous) > 50);
            if (alarm) result.alarms.push_back(sample);
            previous = sample.value;
        }
        result.average = std::accumulate(samples.begin(), samples.end(), 0.0,
            [](double sum, const Sample &s) { return sum + s.value; }) / result.count;
        result.movingAverage = window.average();
        return result;
    }

    void SensorRegistry::add(Sample sample)
    {
        std::shared_ptr<Sensor> sensor;
        {
            std::lock_guard<std::mutex> lock(mutex);
            auto found = sensors.find(sample.id);
            if (found == sensors.end())
            {
                auto &group = groups[sample.type];
                if (!group) group = std::make_shared<Group>(sample.type);
                sensor = std::make_shared<Sensor>(sample.id, sample.type, group);
                sensors.emplace(sample.id, sensor);
                group->add(sensor);
            }
            else sensor = found->second;
        }
        // Registry lock is released before taking the per-sensor lock.
        sensor->add(std::move(sample));
    }

    std::vector<Statistics> SensorRegistry::finalize(Logger &logger)
    {
        std::vector<std::shared_ptr<Sensor>> snapshot;
        {
            std::lock_guard<std::mutex> lock(mutex);
            for (const auto &entry : sensors) snapshot.push_back(entry.second);
        }
        std::vector<Statistics> results;
        for (const auto &sensor : snapshot)
        {
            auto stats = sensor->finalize();
            logger.log("Ownership " + stats.id + ": sensor use_count=" +
                       std::to_string(sensor.use_count()) + " (registry, group, snapshot)");
            results.push_back(std::move(stats));
        }
        return results;
    }
}
