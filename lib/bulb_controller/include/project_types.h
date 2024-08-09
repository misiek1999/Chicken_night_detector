#ifndef  PROJECT_TYPES_H_
#define  PROJECT_TYPES_H_
#include <cinttypes>

namespace ProjectTypes {
    // internal project type aliases
    using GPIO_t = uint16_t;
    using time_ms_t = uint32_t;
    using time_us_t = uint32_t;
    using time_minute_t = int32_t;
    using abs_min_past_midnight_t = uint32_t;
    using time_zone_t = int8_t;
    using latitude_t = float;
    using longitude_t = float;
    using req_t = float;
    using analog_signal_t = uint16_t;
}   // namespace ProjectTypes

#endif // PROJECT_TYPES_H_
