#include "EntryFactory.h"

namespace codecoach {

Entry makeScratchEntry(std::string id, std::string title,
                       ScratchPayload payload,
                       std::optional<std::string> author,
                       std::optional<std::string> description,
                       std::vector<std::string> tags,
                       std::optional<std::string> createdAt) {
  Entry entry;
  entry.id = std::move(id);
  entry.title = std::move(title);
  entry.platform = "scratch";
  entry.author = std::move(author);
  entry.description = std::move(description);
  entry.tags = std::move(tags);
  entry.payload = std::move(payload);

  if (entry.payload.contains("visual") &&
      entry.payload["visual"].contains("thumbnail")) {
    entry.thumbnail = entry.payload["visual"]["thumbnail"].get<std::string>();
  }
  if (createdAt) {
    entry.createdAt = std::move(createdAt);
  } else if (entry.payload.contains("metadata") &&
             entry.payload["metadata"].contains("created_at")) {
    entry.createdAt =
        entry.payload["metadata"]["created_at"].get<std::string>();
  }
  if (entry.payload.contains("metadata") &&
      entry.payload["metadata"].contains("source_image")) {
    entry.sourceImage =
        entry.payload["metadata"]["source_image"].get<std::string>();
  }

  return entry;
}

Entry makePixelPadEntry(std::string id, std::string title,
                        PixelPadPayload payload,
                        std::optional<std::string> author,
                        std::optional<std::string> description,
                        std::vector<std::string> tags,
                        std::optional<std::string> createdAt) {
  Entry entry;
  entry.id = std::move(id);
  entry.title = std::move(title);
  entry.platform = "pixelpad";
  entry.author = std::move(author);
  entry.description = std::move(description);
  entry.tags = std::move(tags);
  entry.payload = std::move(payload);

  if (createdAt) {
    entry.createdAt = std::move(createdAt);
  } else if (entry.payload.contains("metadata") &&
             entry.payload["metadata"].contains("created_at")) {
    entry.createdAt =
        entry.payload["metadata"]["created_at"].get<std::string>();
  }
  if (entry.payload.contains("metadata") &&
      entry.payload["metadata"].contains("source_image")) {
    entry.sourceImage =
        entry.payload["metadata"]["source_image"].get<std::string>();
  }

  return entry;
}

Entry makeUnityEntry(std::string id, std::string title, UnityPayload payload,
                     std::optional<std::string> author,
                     std::optional<std::string> description,
                     std::vector<std::string> tags,
                     std::optional<std::string> createdAt) {
  Entry entry;
  entry.id = std::move(id);
  entry.title = std::move(title);
  entry.platform = "unity";
  entry.author = std::move(author);
  entry.description = std::move(description);
  entry.tags = std::move(tags);
  entry.payload = std::move(payload);

  if (createdAt) {
    entry.createdAt = std::move(createdAt);
  } else if (entry.payload.contains("metadata") &&
             entry.payload["metadata"].contains("created_at")) {
    entry.createdAt =
        entry.payload["metadata"]["created_at"].get<std::string>();
  }
  if (entry.payload.contains("metadata") &&
      entry.payload["metadata"].contains("source_image")) {
    entry.sourceImage =
        entry.payload["metadata"]["source_image"].get<std::string>();
  }

  return entry;
}

} // namespace codecoach
