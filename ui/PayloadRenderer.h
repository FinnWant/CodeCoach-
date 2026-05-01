#pragma once

#include <QString>
#include <nlohmann/json.hpp>

namespace PayloadRenderer {

// Returns a human-readable ASCII visual of the payload suited to the platform.
// Scratch  → stacked block scripts per sprite
// PixelPad → class banner + method boxes
// Unity    → GameObject hierarchy with component code
QString render(const std::string &platform, const nlohmann::json &payload);

} // namespace PayloadRenderer
