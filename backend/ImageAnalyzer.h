#pragma once

#include "Model.h"

#include <string>

namespace codecoach {

struct AnalysisResult {
    std::string rawText;
    std::string platform;   // "scratch", "pixelpad", "unity", "unknown"
    int confidence = 0;     // 0-100, ratio of winning platform score to total
    nlohmann::json payload; // platform-specific payload matching schema
};

// Runs OCR on imagePath, detects platform via heuristics, and extracts
// a structured payload. Throws std::runtime_error on IO or OCR failure.
AnalysisResult analyzeImage(const std::string &imagePath);

} // namespace codecoach
