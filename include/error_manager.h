#pragma once

#include <cstdint>
#include <etl/unordered_set.h>

#define GET_ERROR_CODE_NAME(name) case name : return #name;

namespace SystemControl
{
    using ErrorCodeType = uint32_t;

    constexpr ErrorCodeType kCrititalErrorCodeFirstNumber = 100;

    enum class ErrorCode : ErrorCodeType
    {
        // non critical errors
        kI2CModuleConnectionFailed    =  0,
        kRtcTimeNotSet,
        kRtcTimeNotUpdated,
        kLightSensorError,
        kDoorControlError,

        // critical errors
        kOverCurrent            =  kCrititalErrorCodeFirstNumber,
        kOverVoltage,
        kDoorSensorError,
    };

    constexpr const char* getErrorCodeName(const ErrorCode &error_code)
    {
        switch (error_code)
        {
            GET_ERROR_CODE_NAME(ErrorCode::kI2CModuleConnectionFailed)
            GET_ERROR_CODE_NAME(ErrorCode::kRtcTimeNotSet)
            GET_ERROR_CODE_NAME(ErrorCode::kRtcTimeNotUpdated)
            GET_ERROR_CODE_NAME(ErrorCode::kLightSensorError)
            GET_ERROR_CODE_NAME(ErrorCode::kDoorControlError)
            GET_ERROR_CODE_NAME(ErrorCode::kOverCurrent)
            GET_ERROR_CODE_NAME(ErrorCode::kOverVoltage)
            GET_ERROR_CODE_NAME(ErrorCode::kDoorSensorError)
            default:
                return "Unknown error code";
        }
    }

    class ErrorManager
    {
    private:
        static constexpr size_t kErrorCodeSetSize = 10;
        etl::unordered_set<ErrorCode, kErrorCodeSetSize> error_set_;

    protected:
        ErrorManager() = default;

    public:
        static ErrorManager* getInstance() {
            static ErrorManager instance;
            return &instance;
    }
        ~ErrorManager() = default;
        ErrorManager(const ErrorManager &) = delete;
        ErrorManager &operator=(const ErrorManager &) = delete;
        ErrorManager(ErrorManager &&) = delete;
        ErrorManager &operator=(ErrorManager &&) = delete;


        size_t getErrorCount() const;
        void setError(const ErrorCode &error_code);
        bool checkIsAnyError() const;
        bool checkIsError(const ErrorCode &error_code) const;
        bool checkIsCriticalError() const;
        bool resetError(const ErrorCode &error_code);
        void resetAllError();
        etl::unordered_set<ErrorCode, kErrorCodeSetSize> getErrorSet() const;

    };

} // namespace SystemControl
