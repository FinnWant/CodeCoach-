#pragma once

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

struct ScratchScript {
  std::string when;
  std::vector<std::string> blocks;
};

inline void to_json(nlohmann::json &j, const ScratchScript &script) {
  j = nlohmann::json{{"when", script.when}, {"blocks", script.blocks}};
}

inline void from_json(const nlohmann::json &j, ScratchScript &script) {
  script.when = j.at("when").get<std::string>();
  script.blocks = j.at("blocks").get<std::vector<std::string>>();
}

struct ScratchStageSprite {
  std::string sprite;
  std::vector<ScratchScript> scripts;
};

inline void to_json(nlohmann::json &j, const ScratchStageSprite &sprite) {
  j = nlohmann::json{{"sprite", sprite.sprite}, {"scripts", sprite.scripts}};
}

inline void from_json(const nlohmann::json &j, ScratchStageSprite &sprite) {
  sprite.sprite = j.at("sprite").get<std::string>();
  sprite.scripts = j.at("scripts").get<std::vector<ScratchScript>>();
}

struct ScratchVisual {
  std::optional<std::string> thumbnail;
  std::vector<ScratchStageSprite> stage;
};

inline void to_json(nlohmann::json &j, const ScratchVisual &visual) {
  j = nlohmann::json{};
  if (visual.thumbnail) {
    j["thumbnail"] = *visual.thumbnail;
  }
  if (!visual.stage.empty()) {
    j["stage"] = visual.stage;
  }
}

inline void from_json(const nlohmann::json &j, ScratchVisual &visual) {
  if (j.contains("thumbnail")) {
    visual.thumbnail = j.at("thumbnail").get<std::string>();
  }
  if (j.contains("stage")) {
    visual.stage = j.at("stage").get<std::vector<ScratchStageSprite>>();
  }
}

struct ScratchMetadata {
  std::optional<std::string> created_at;
  std::optional<std::string> source_image;
};

inline void to_json(nlohmann::json &j, const ScratchMetadata &metadata) {
  j = nlohmann::json{};
  if (metadata.created_at) {
    j["created_at"] = *metadata.created_at;
  }
  if (metadata.source_image) {
    j["source_image"] = *metadata.source_image;
  }
}

inline void from_json(const nlohmann::json &j, ScratchMetadata &metadata) {
  if (j.contains("created_at")) {
    metadata.created_at = j.at("created_at").get<std::string>();
  }
  if (j.contains("source_image")) {
    metadata.source_image = j.at("source_image").get<std::string>();
  }
}

struct ScratchPayload {
  ScratchVisual visual;
  std::optional<ScratchMetadata> metadata;
};

inline void to_json(nlohmann::json &j, const ScratchPayload &payload) {
  j = nlohmann::json{{"visual", payload.visual}};
  if (payload.metadata) {
    j["metadata"] = *payload.metadata;
  }
}

inline void from_json(const nlohmann::json &j, ScratchPayload &payload) {
  payload.visual = j.at("visual").get<ScratchVisual>();
  if (j.contains("metadata")) {
    payload.metadata = j.at("metadata").get<ScratchMetadata>();
  }
}

struct PixelPadMethod {
  std::string name;
  std::string code;
};

inline void to_json(nlohmann::json &j, const PixelPadMethod &method) {
  j = nlohmann::json{{"name", method.name}, {"code", method.code}};
}

inline void from_json(const nlohmann::json &j, PixelPadMethod &method) {
  method.name = j.at("name").get<std::string>();
  method.code = j.at("code").get<std::string>();
}

struct PixelPadClass {
  std::string name;
  std::string type;
  std::vector<PixelPadMethod> methods;
};

inline void to_json(nlohmann::json &j, const PixelPadClass &klass) {
  j = nlohmann::json{
      {"name", klass.name}, {"type", klass.type}, {"methods", klass.methods}};
}

inline void from_json(const nlohmann::json &j, PixelPadClass &klass) {
  klass.name = j.at("name").get<std::string>();
  klass.type = j.at("type").get<std::string>();
  klass.methods = j.at("methods").get<std::vector<PixelPadMethod>>();
}

struct PixelPadAssets {
  std::vector<std::string> sprites;
};

inline void to_json(nlohmann::json &j, const PixelPadAssets &assets) {
  j = nlohmann::json{{"sprites", assets.sprites}};
}

inline void from_json(const nlohmann::json &j, PixelPadAssets &assets) {
  assets.sprites = j.at("sprites").get<std::vector<std::string>>();
}

struct PixelPadMetadata {
  std::optional<std::string> language;
  std::optional<std::string> source_image;
};

inline void to_json(nlohmann::json &j, const PixelPadMetadata &metadata) {
  j = nlohmann::json{};
  if (metadata.language) {
    j["language"] = *metadata.language;
  }
  if (metadata.source_image) {
    j["source_image"] = *metadata.source_image;
  }
}

inline void from_json(const nlohmann::json &j, PixelPadMetadata &metadata) {
  if (j.contains("language")) {
    metadata.language = j.at("language").get<std::string>();
  }
  if (j.contains("source_image")) {
    metadata.source_image = j.at("source_image").get<std::string>();
  }
}

struct PixelPadPayload {
  std::vector<PixelPadClass> classes;
  std::optional<PixelPadAssets> assets;
  std::optional<PixelPadMetadata> metadata;
};

inline void to_json(nlohmann::json &j, const PixelPadPayload &payload) {
  j = nlohmann::json{{"classes", payload.classes}};
  if (payload.assets) {
    j["assets"] = *payload.assets;
  }
  if (payload.metadata) {
    j["metadata"] = *payload.metadata;
  }
}

inline void from_json(const nlohmann::json &j, PixelPadPayload &payload) {
  payload.classes = j.at("classes").get<std::vector<PixelPadClass>>();
  if (j.contains("assets")) {
    payload.assets = j.at("assets").get<PixelPadAssets>();
  }
  if (j.contains("metadata")) {
    payload.metadata = j.at("metadata").get<PixelPadMetadata>();
  }
}

struct UnityComponent {
  std::string type;
  std::optional<std::string> code;
};

inline void to_json(nlohmann::json &j, const UnityComponent &component) {
  j = nlohmann::json{{"type", component.type}};
  if (component.code) {
    j["code"] = *component.code;
  }
}

inline void from_json(const nlohmann::json &j, UnityComponent &component) {
  component.type = j.at("type").get<std::string>();
  if (j.contains("code")) {
    component.code = j.at("code").get<std::string>();
  }
}

struct UnityObject {
  std::string name;
  std::vector<UnityComponent> components;
};

inline void to_json(nlohmann::json &j, const UnityObject &object) {
  j = nlohmann::json{{"name", object.name}, {"components", object.components}};
}

inline void from_json(const nlohmann::json &j, UnityObject &object) {
  object.name = j.at("name").get<std::string>();
  object.components = j.at("components").get<std::vector<UnityComponent>>();
}

struct UnityMetadata {
  std::optional<std::string> language;
  std::optional<std::string> source_image;
};

inline void to_json(nlohmann::json &j, const UnityMetadata &metadata) {
  j = nlohmann::json{};
  if (metadata.language) {
    j["language"] = *metadata.language;
  }
  if (metadata.source_image) {
    j["source_image"] = *metadata.source_image;
  }
}

inline void from_json(const nlohmann::json &j, UnityMetadata &metadata) {
  if (j.contains("language")) {
    metadata.language = j.at("language").get<std::string>();
  }
  if (j.contains("source_image")) {
    metadata.source_image = j.at("source_image").get<std::string>();
  }
}

struct UnityPayload {
  std::vector<UnityObject> objects;
  std::optional<UnityMetadata> metadata;
};

inline void to_json(nlohmann::json &j, const UnityPayload &payload) {
  j = nlohmann::json{{"objects", payload.objects}};
  if (payload.metadata) {
    j["metadata"] = *payload.metadata;
  }
}

inline void from_json(const nlohmann::json &j, UnityPayload &payload) {
  payload.objects = j.at("objects").get<std::vector<UnityObject>>();
  if (j.contains("metadata")) {
    payload.metadata = j.at("metadata").get<UnityMetadata>();
  }
}

struct Entry {
  std::string id;
  std::string title;
  std::string platform;
  std::optional<std::string> author;
  std::optional<std::string> description;
  std::vector<std::string> tags;
  nlohmann::json payload;
  std::optional<std::string> thumbnail;
  std::optional<std::string> createdAt;
  std::optional<std::string> sourceImage;
};

inline void to_json(nlohmann::json &j, const Entry &entry) {
  j = nlohmann::json{{"id", entry.id},
                     {"title", entry.title},
                     {"platform", entry.platform},
                     {"payload", entry.payload}};

  if (entry.author) {
    j["author"] = *entry.author;
  }
  if (entry.description) {
    j["description"] = *entry.description;
  }
  if (!entry.tags.empty()) {
    j["tags"] = entry.tags;
  }
  if (entry.thumbnail) {
    j["thumbnail"] = *entry.thumbnail;
  }
  if (entry.createdAt) {
    j["created_at"] = *entry.createdAt;
  }
  if (entry.sourceImage) {
    j["source_image"] = *entry.sourceImage;
  }
}

inline void from_json(const nlohmann::json &j, Entry &entry) {
  entry.id = j.at("id").get<std::string>();
  entry.title = j.at("title").get<std::string>();
  entry.platform = j.at("platform").get<std::string>();
  entry.payload = j.at("payload");

  if (j.contains("author")) {
    entry.author = j.at("author").get<std::string>();
  }
  if (j.contains("description")) {
    entry.description = j.at("description").get<std::string>();
  }
  if (j.contains("tags")) {
    entry.tags = j.at("tags").get<std::vector<std::string>>();
  }
  if (j.contains("thumbnail")) {
    entry.thumbnail = j.at("thumbnail").get<std::string>();
  }
  if (j.contains("created_at")) {
    entry.createdAt = j.at("created_at").get<std::string>();
  }
  if (j.contains("source_image")) {
    entry.sourceImage = j.at("source_image").get<std::string>();
  }
}
