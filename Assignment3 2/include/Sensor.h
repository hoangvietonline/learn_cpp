#ifndef SDP_SENSOR_H
#define SDP_SENSOR_H

#include <memory>
#include <mutex>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <utility>

namespace sdp
{
    struct Sample
    {
        long long timestamp = 0;
        std::size_t line = 0;
        std::string id, type;
        double value = 0;
    };

    struct Statistics
    {
        std::string id, type, group;
        std::size_t count = 0;
        double minimum = 0, maximum = 0, average = 0, movingAverage = 0;
        std::vector<Sample> alarms;
    };

    class Logger
    {
        std::mutex mutex;
    public:
        void log(const std::string &message);
    };

    class Group;

    class Sensor
    {
        std::string id, type;
        std::weak_ptr<Group> group;
        std::vector<Sample> samples;
        std::mutex mutex;
    public:
        Sensor(std::string id, std::string type, const std::shared_ptr<Group> &group);
        void add(Sample sample);
        Statistics finalize();
    };

    class Group
    {
        std::string name;
        // Only modified while the registry mutex is held.
        std::vector<std::shared_ptr<Sensor>> sensors;
    public:
        explicit Group(std::string name) : name(std::move(name)) {}
        const std::string &getName() const { return name; }
        void add(const std::shared_ptr<Sensor> &sensor) { sensors.push_back(sensor); }
    };

    class SensorRegistry
    {
        std::map<std::string, std::shared_ptr<Sensor>> sensors;
        std::unordered_map<std::string, std::shared_ptr<Group>> groups;
        std::mutex mutex;
    public:
        void add(Sample sample);
        std::vector<Statistics> finalize(Logger &logger);
    };

    bool validRange(const Sample &sample);
}
#endif
