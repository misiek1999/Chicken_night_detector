#include "error_manager.h"
#include "etl/algorithm.h"

namespace SystemControl {
    size_t ErrorManager::getErrorCount() const {
        return error_set_.size();
    }

    void ErrorManager::setError(const ErrorCode &error_code) {
        error_set_.insert(error_code);
    }

    bool ErrorManager::checkIsAnyError() const{
        return !error_set_.empty();
    }

    bool ErrorManager::checkIsError(const ErrorCode &error_code) const {
        return error_set_.find(error_code) != error_set_.end();
    }

    bool ErrorManager::checkIsCriticalError() const {
        return etl::any_of(error_set_.begin(), error_set_.end(), [](const ErrorCode &error_code) {
            return static_cast<ErrorCodeType>(error_code) >= kCrititalErrorCodeFirstNumber;
        });
    }

    bool ErrorManager::checkIsRtcError() const {
        return  checkIsError(ErrorCode::kRtcTimeNotSet) ||
                checkIsError(ErrorCode::kRtcTimeNotUpdated) ||
                checkIsError(ErrorCode::kI2CModuleConnectionFailed);
    }

    bool ErrorManager::resetError(const ErrorCode &error_code) {
        return error_set_.erase(error_code);
    }

    void ErrorManager::resetAllError() {
        error_set_.clear();
    }

    etl::unordered_set<ErrorCode, ErrorManager::kErrorCodeSetSize> ErrorManager::getErrorSet() const {
        return error_set_;
    }

} // namespace SystemControl
