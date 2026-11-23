#ifndef SDP_PROCESSING_H
#define SDP_PROCESSING_H
#include "Sensor.h"
#include <iosfwd>

namespace sdp
{
    struct Report
    {
        std::vector<Statistics> sensors;
        std::size_t total = 0, valid = 0, invalid = 0;
    };

    namespace io
    {
        Sample parse(const std::string &line, std::size_t number);
        void writeReport(const Report &report, std::ostream &out);
    }

    Report process(const std::string &filename, unsigned workers);
    int run(int argc, char **argv);
}
#endif
