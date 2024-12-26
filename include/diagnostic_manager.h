#pragma once

#include <Wire.h>
#include <etl/array.h>
#include "error_manager.h"
#include "Log.h"

namespace Diagnostic
{
    class DiagnosticManager
    {
        DiagnosticManager();
    public:
        // Singleton instance methods
        static DiagnosticManager * getInstance() {
            static DiagnosticManager instance;
            return &instance;
        }
        ~DiagnosticManager() = default;
        DiagnosticManager(const DiagnosticManager &) = delete;
        DiagnosticManager & operator=(const DiagnosticManager &) = delete;
        DiagnosticManager(DiagnosticManager &&) = delete;
        DiagnosticManager & operator=(DiagnosticManager &&) = delete;

        /*
            @details perform diagnostic routine
        */
        void performDiagnostic();
    private:
        TwoWire & _wire;
        SystemControl::ErrorManager & _error_manager;

        bool checkI2cDevices();
    };

} // namespace DiagnosticManager
