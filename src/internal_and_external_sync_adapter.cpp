#include "internal_and_external_sync_adapter.h"

//TODO: implement this functions
void sync_both_rtc_source() {
}

void ModuleAdapter::init_inter_and_exter_sync_rtc_module() {
    init_external_rtc_module();
    init_internal_rtc_module();
    sync_both_rtc_source();
}

void ModuleAdapter::get_inter_and_exter_sync_rtc_time(std::time_t &time) {
    (void)time;
}

void ModuleAdapter::set_inter_and_exter_sync_rtc_time(const std::time_t &time) {
    (void)time;
}
