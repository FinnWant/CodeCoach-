#include "Validation.h"

#include <nlohmann/json.hpp>
#include <string>

namespace codecoach {

static bool isNonEmpty(const std::string &value) { return !value.empty(); }

bool validateScratchPayload(const ScratchPayload &payload, std::string &error) {
  if (payload.visual.stage.empty()) {
    error = "Scratch visual stage is empty.";
    return false;
  }

  for (const auto &sprite : payload.visual.stage) {
    if (!isNonEmpty(sprite.sprite)) {
      error = "Scratch stage sprite name is required.";
      return false;
    }
    if (sprite.scripts.empty()) {
      error = "Scratch stage sprite must contain at least one script.";
      return false;
    }
    for (const auto &script : sprite.scripts) {
      if (!isNonEmpty(script.when)) {
        error = "Scratch script trigger is required.";
        return false;
      }
      if (script.blocks.empty()) {
        error = "Scratch script must contain at least one block.";
        return false;
      }
      for (const auto &block : script.blocks) {
        if (!isNonEmpty(block)) {
          error = "Scratch block text must not be empty.";
          return false;
        }
      }
    }
  }

  return true;
}

bool validatePixelPadPayload(const PixelPadPayload &payload,
                             std::string &error) {
  if (payload.classes.empty()) {
    error = "PixelPad payload must include at least one class.";
    return false;
  }

  for (const auto &klass : payload.classes) {
    if (!isNonEmpty(klass.name)) {
      error = "PixelPad class name is required.";
      return false;
    }
    if (!isNonEmpty(klass.type)) {
      error = "PixelPad class type is required.";
      return false;
    }
    for (const auto &method : klass.methods) {
      if (!isNonEmpty(method.name)) {
        error = "PixelPad method name is required.";
        return false;
      }
      if (!isNonEmpty(method.code)) {
        error = "PixelPad method code is required.";
        return false;
      }
    }
  }

  if (payload.assets && payload.assets->sprites.empty()) {
    error = "PixelPad assets must include at least one sprite if assets are "
            "provided.";
    return false;
  }

  return true;
}

bool validateUnityPayload(const UnityPayload &payload, std::string &error) {
  if (payload.objects.empty()) {
    error = "Unity payload must include at least one object.";
    return false;
  }

  for (const auto &object : payload.objects) {
    if (!isNonEmpty(object.name)) {
      error = "Unity object name is required.";
      return false;
    }
    if (object.components.empty()) {
      error = "Unity object must include at least one component.";
      return false;
    }
    for (const auto &component : object.components) {
      if (!isNonEmpty(component.type)) {
        error = "Unity component type is required.";
        return false;
      }
    }
  }

  return true;
}

bool validateEntry(const Entry &entry, std::string &error) {
  if (entry.id.empty()) {
    error = "Entry id is required.";
    return false;
  }
  if (entry.title.empty()) {
    error = "Entry title is required.";
    return false;
  }
  if (entry.platform.empty()) {
    error = "Entry platform is required.";
    return false;
  }

  try {
    if (entry.platform == "scratch") {
      ScratchPayload payload = entry.payload.get<ScratchPayload>();
      return validateScratchPayload(payload, error);
    }
    if (entry.platform == "pixelpad") {
      PixelPadPayload payload = entry.payload.get<PixelPadPayload>();
      return validatePixelPadPayload(payload, error);
    }
    if (entry.platform == "unity") {
      UnityPayload payload = entry.payload.get<UnityPayload>();
      return validateUnityPayload(payload, error);
    }
  } catch (const nlohmann::json::exception &ex) {
    error = std::string("Payload JSON validation failed: ") + ex.what();
    return false;
  }

  error = "Unsupported platform: " + entry.platform;
  return false;
}

} // namespace codecoach
