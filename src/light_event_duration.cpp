#include "light_event_duration.h"
#include <etl/array.h>

enum class Season
{
    Winter = 0,
    SpringAutumn = 1,
    Summer = 2
};

// Array with event duration times for each season
// dimming before event, dimming after event, active before event, active after event
constexpr etl::array<ControlLogic::BulbEventDurationTime, 3> bulb_event_duration_times = {{
    {0, 30, 60, 30}, // 0: winter
    {0, 20, 30, 30},  // 1: spring and autumn
    {0, 15, 15, 30},  // 2: summer
}};

ControlLogic::BulbEventDurationTime ControlLogic::getEventDurationTime(const uint8_t &month)
{
    // Default event duration time
    auto event_duration_time = bulb_event_duration_times[static_cast<size_t>(Season::Winter)];
    // Get the current season event duration time
    switch (month)
    {
        case 0 ... 1:
        case 10 ... 11: // From January to February and from November to December apply winter event duration time
            event_duration_time = bulb_event_duration_times[static_cast<size_t>(Season::Winter)];
            break;
        case 2 ... 3:
        case 8 ... 9: // From March to April and from September to October apply spring and autumn event duration time
            event_duration_time = bulb_event_duration_times[static_cast<size_t>(Season::SpringAutumn)];
            break;
        case 4 ... 7:   // From May to August apply summer event duration time
            event_duration_time = bulb_event_duration_times[static_cast<size_t>(Season::Summer)];
            break;
        default:
            break;
    }
    return event_duration_time;
}