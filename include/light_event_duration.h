#pragma once

#include <ctime>
#include "project_types.h"

namespace ControlLogic {

struct BulbEventDurationTime {
    ProjectTypes::time_minute_t dimming_before_event;
    ProjectTypes::time_minute_t dimming_after_event;
    ProjectTypes::time_minute_t active_before_event;
    ProjectTypes::time_minute_t active_after_event;
};

BulbEventDurationTime getEventDurationTime(const uint8_t &month);

}   // namespace ControlLogic
