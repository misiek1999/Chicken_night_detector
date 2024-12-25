#include "error_manager.h"
#include "etl/algorithm.h"

namespace SystemControl {

    void ErrorManager::setError(const ErrorCode &error_code) {
        error_set_.insert(error_code);
    }

    bool ErrorManager::checkIsError() const{
        return !error_set_.empty();
    }

    bool ErrorManager::checkIsCriticalError() const {
        return etl::any_of(error_set_.begin(), error_set_.end(), [](const ErrorCode &error_code) {
            return static_cast<ErrorCodeType>(error_code) >= kCrititalErrorCodeFirstNumber;
        });
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
