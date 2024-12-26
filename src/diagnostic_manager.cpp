#include "diagnostic_manager.h"
#include <Arduino.h>

static constexpr const auto kTimeBetweenPrintMs = 10000u; // 10 second
static auto last_print_time = 0u;

// I2C devices used in the system
constexpr auto list_of_i2c_devices = etl::make_array<uint8_t> (
    0x68
);

Diagnostic::DiagnosticManager::DiagnosticManager():
    _wire(Wire),
    _error_manager(*SystemControl::ErrorManager::getInstance()) {
}

void Diagnostic::DiagnosticManager::performDiagnostic() {
    bool result = true;
    // check i2c devices
    if (checkI2cDevices()) {
        LOG_DEBUG("I2C devices are OK");
    } else {
        result = false;
        LOG_DEBUG("I2C devices are not OK");
    }

    if (result) {
        LOG_DEBUG("Diagnostic is OK");
    } else {
        LOG_DEBUG("Diagnostic is not OK");
    }
}

bool Diagnostic::DiagnosticManager::checkI2cDevices() {
    auto status = true;
    for (const auto& device : list_of_i2c_devices) {
        _wire.beginTransmission(device);
        auto error = _wire.endTransmission();
        if (error != 0) {
            // print this log every 10s to avoid console spam
            if (millis() - last_print_time > kTimeBetweenPrintMs) {
                last_print_time = millis();
                LOG_WARNING("I2C device: %u not found, error %d", device, error);
            }
            _error_manager.setError(SystemControl::ErrorCode::kI2CModuleConnectionFailed);
            status = false;
            break;
        }
    }
    if (status) {
        LOG_DEBUG("All I2C devices are OK");
        if (_error_manager.checkIsError(SystemControl::ErrorCode::kI2CModuleConnectionFailed)) {
            if (_error_manager.resetError(SystemControl::ErrorCode::kI2CModuleConnectionFailed)) {
                LOG_INFO("clear I2C module connection error");
            } else {
                LOG_WARNING("I2C module connection error is not cleared");
            }
        }
    }
    return status;
}
