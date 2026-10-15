#pragma once
#include "Config.hpp"
#include <atomic>

void BackendLoop(Config& cfg,
                 std::atomic<bool>& shutdown,
                 std::atomic<double>& outFps,
                 std::atomic<int>& outTargets,
                 std::atomic<bool>& outLocked);
