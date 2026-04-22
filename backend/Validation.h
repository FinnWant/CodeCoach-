#pragma once

#include "Model.h"

#include <string>

namespace codecoach {

bool validateScratchPayload(const ScratchPayload &payload, std::string &error);
bool validatePixelPadPayload(const PixelPadPayload &payload,
                             std::string &error);
bool validateUnityPayload(const UnityPayload &payload, std::string &error);
bool validateEntry(const Entry &entry, std::string &error);

} // namespace codecoach
