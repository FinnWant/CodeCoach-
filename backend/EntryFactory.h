#pragma once

#include "Model.h"

#include <optional>
#include <string>
#include <vector>

namespace codecoach {

Entry makeScratchEntry(std::string id, std::string title,
                       ScratchPayload payload,
                       std::optional<std::string> author = std::nullopt,
                       std::optional<std::string> description = std::nullopt,
                       std::vector<std::string> tags = {},
                       std::optional<std::string> createdAt = std::nullopt);

Entry makePixelPadEntry(std::string id, std::string title,
                        PixelPadPayload payload,
                        std::optional<std::string> author = std::nullopt,
                        std::optional<std::string> description = std::nullopt,
                        std::vector<std::string> tags = {},
                        std::optional<std::string> createdAt = std::nullopt);

Entry makeUnityEntry(std::string id, std::string title, UnityPayload payload,
                     std::optional<std::string> author = std::nullopt,
                     std::optional<std::string> description = std::nullopt,
                     std::vector<std::string> tags = {},
                     std::optional<std::string> createdAt = std::nullopt);

} // namespace codecoach
