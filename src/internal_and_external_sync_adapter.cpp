#include "internal_and_external_sync_adapter.h"

void sync_both_rtc_source() {
    RtcTimeContainer int_time;
    ModuleAdapter::get_internal_rtc_time(int_time);
    // check if internal rtc is valid, if not synchronize internal RTC with external RTC
    if (int_time.isTimeValid() == false) {
        RtcTimeContainer ext_time;
        ModuleAdapter::get_external_rtc_time(ext_time);
        ModuleAdapter::set_internal_rtc_time(ext_time);
    }
}

void ModuleAdapter::init_inter_and_exter_sync_rtc_module(void)
{
    init_external_rtc_module();
    init_internal_rtc_module();
    sync_both_rtc_source();
}

void ModuleAdapter::get_inter_and_exter_sync_rtc_time(ProjectTypes::RTC_Time &time_from_external_rtc)
{
    get_internal_rtc_time(time_from_external_rtc);
}

void ModuleAdapter::set_inter_and_exter_sync_rtc_time(const ProjectTypes::RTC_Time &time_to_set)
{
    set_external_rtc_time(time_to_set);
    set_internal_rtc_time(time_to_set);
}
