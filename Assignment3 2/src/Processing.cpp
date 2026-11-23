#include "Processing.h"
#include "ThreadSafeQueue.h"
#include "Algorithms.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <thread>
#include <exception>

namespace sdp
{
    namespace algorithms = algo;

    namespace io
    {
        Sample parse(const std::string &line, std::size_t number)
        {
            auto fail = [number] { return ParseError("Line " + std::to_string(number) + ": invalid sample"); };
            std::vector<std::string> fields;
            std::size_t start = 0;
            for (;;)
            {
                const auto end = line.find(';', start);
                fields.push_back(line.substr(start, end == std::string::npos ? end : end - start));
                if (end == std::string::npos) break;
                start = end + 1;
            }
            if (fields.size() != 4 || std::find_if(fields.begin(), fields.end(),
                    [](const std::string &field) { return field.empty(); }) != fields.end())
                throw fail();
            if (!fields.back().empty() && fields.back().back() == '\r') fields.back().pop_back();
            if (fields[1].empty() || fields[1].find_first_of(",\"\r\n") != std::string::npos ||
                (fields[2] != "TEMP" && fields[2] != "VOLT" && fields[2] != "SPEED"))
                throw fail();
            try
            {
                std::size_t used;
                const auto timestamp = std::stoll(fields[0], &used);
                if (timestamp < 0 || used != fields[0].size()) throw fail();
                const double value = std::stod(fields[3], &used);
                if (!std::isfinite(value) || used != fields[3].size()) throw fail();
                return {timestamp, number, fields[1], fields[2], value};
            }
            catch (const std::invalid_argument &) { throw fail(); }
            catch (const std::out_of_range &) { throw fail(); }
        }

        void writeReport(const Report &report, std::ostream &out)
        {
            out << "id,type,group,count,min,max,average,moving_average,alarms\n";
            out << std::fixed << std::setprecision(2);
            for (const auto &s : report.sensors)
                out << s.id << ',' << s.type << ',' << s.group << ',' << s.count << ','
                    << s.minimum << ',' << s.maximum << ',' << s.average << ','
                    << s.movingAverage << ',' << s.alarms.size() << '\n';
            out << "total,valid,invalid\n" << report.total << ',' << report.valid << ',' << report.invalid << '\n';
        }
    }

    Report process(const std::string &filename, unsigned workers)
    {
        ThreadSafeQueue<Sample> queue;
        const auto registry = std::make_unique<SensorRegistry>();
        const auto logger = std::make_shared<Logger>();
        std::atomic<std::size_t> total{0}, valid{0}, invalid{0};
        std::mutex errorMutex;
        std::exception_ptr error;
        const auto capture = [&] {
            {
                std::lock_guard<std::mutex> lock(errorMutex);
                if (!error) error = std::current_exception();
            }
            queue.close();
        };
        std::vector<std::thread> threads;
        try
        {
            for (unsigned i = 0; i < std::max(1u, workers); ++i)
            {
                threads.emplace_back([&, logger] {
                    try
                    {
                        Sample sample;
                        while (queue.pop(sample))
                        {
                            if (!validRange(sample))
                            {
                                ++invalid;
                                logger->log("Line " + std::to_string(sample.line) + ": out of range");
                                continue;
                            }
                            registry->add(std::move(sample));
                            ++valid;
                        }
                    }
                    catch (...) { capture(); }
                });
            }
            threads.emplace_back([&, logger] {
                try
                {
                    std::ifstream input(filename);
                    if (!input) throw FileError("Cannot open input file: " + filename);
                    std::unordered_map<std::string, std::string> types;
                    std::string line;
                    while (std::getline(input, line))
                    {
                        const auto number = ++total;
                        try
                        {
                            auto sample = io::parse(line, number);
                            // Bind an ID to its first valid type in file order, not worker order.
                            if (validRange(sample))
                            {
                                const auto entry = types.emplace(sample.id, sample.type);
                                if (entry.first->second != sample.type)
                                    throw ParseError("Line " + std::to_string(number) + ": conflicting sensor type");
                            }
                            queue.push(std::move(sample));
                        }
                        catch (const ParseError &e)
                        {
                            ++invalid;
                            logger->log(e.what());
                        }
                    }
                    if (input.bad()) throw FileError("Error reading input: " + filename);
                    queue.close();
                }
                catch (...) { capture(); }
            });
        }
        catch (...)
        {
            queue.close();
            for (auto &thread : threads) thread.join();
            throw;
        }
        for (auto &thread : threads) thread.join();
        if (error) std::rethrow_exception(error);
        Report report{registry->finalize(*logger), total.load(), valid.load(), invalid.load()};
        const auto compare = [](const Statistics &a, const Statistics &b) {
            return a.average != b.average ? a.average > b.average : a.id < b.id;
        };
        auto expected = report.sensors;
        algorithms::mergeSort(report.sensors, compare);
        std::sort(expected.begin(), expected.end(), compare);
        for (std::size_t i = 0; i < expected.size(); ++i)
            if (expected[i].id != report.sensors[i].id) throw Error("Sort verification failed");
        return report;
    }

    int run(int argc, char **argv)
    {
        try
        {
            const std::string filename = argc > 1 ? argv[1] : "data/sensor_log.txt";
            unsigned workers = std::max(1u, std::thread::hardware_concurrency());
            if (argc > 2)
            {
                const std::string arg = argv[2];
                std::size_t used;
                if (arg.empty() || arg.find_first_not_of("0123456789") != std::string::npos)
                    throw Error("Worker count must be a non-negative integer");
                const auto value = std::stoul(arg, &used);
                if (used != arg.size() || value > std::numeric_limits<unsigned>::max())
                    throw Error("Invalid worker count");
                workers = std::max(1u, static_cast<unsigned>(value));
            }
            const auto start = std::chrono::steady_clock::now();
            const auto report = process(filename, workers);
            std::ofstream file("report.csv");
            if (!file) throw FileError("Cannot create report.csv");
            io::writeReport(report, file);
            file.close();
            if (!file) throw FileError("Cannot write report.csv");
            io::writeReport(report, std::cout);
            for (const auto &sensor : report.sensors)
                for (const auto &alarm : sensor.alarms)
                    std::cout << "ALARM " << alarm.id << " timestamp=" << alarm.timestamp
                              << " value=" << alarm.value << '\n';
            const auto alarmSensors = std::count_if(report.sensors.begin(), report.sensors.end(),
                [](const Statistics &s) { return !s.alarms.empty(); });
            std::cout << "Sensors with alarms: " << alarmSensors << '\n';
            std::cout << "Workers: " << workers << ", processing time (ms): "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::steady_clock::now() - start).count() << '\n';
            // Separate ID-sorted index; the CSV remains sorted by average.
            auto byId = report.sensors;
            const auto compareId = [](const Statistics &a, const Statistics &b) { return a.id < b.id; };
            algorithms::mergeSort(byId, compareId);
            std::cout << "Enter sensor ID (empty to quit): ";
            std::string id;
            while (std::getline(std::cin, id) && !id.empty())
            {
                Statistics key;
                key.id = id;
                const auto index = algorithms::binarySearch(byId, key, compareId);
                if (index == byId.size()) std::cout << "Not found: " << id << '\n';
                else
                {
                    const auto found = byId.begin() + index;
                    std::cout << "Found: " << found->id << ", count=" << found->count
                              << ", min=" << found->minimum << ", max=" << found->maximum
                              << ", average=" << found->average << ", moving average=" << found->movingAverage
                              << ", alarms=" << found->alarms.size() << '\n';
                }
                std::cout << "Enter sensor ID (empty to quit): ";
            }
            return 0;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << '\n';
            return 1;
        }
    }
}
