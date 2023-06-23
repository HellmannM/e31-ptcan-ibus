#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <cstring>

static const auto startTime = std::chrono::high_resolution_clock::now();
uint32_t millis() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count(); }

