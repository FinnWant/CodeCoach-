#include "Repository.h"

#include <stdexcept>

namespace {
std::string serializeTags(const std::vector<std::string> &tags) {
  return nlohmann::json(tags).dump();
}

std::vector<std::string> deserializeTags(const std::string &text) {
  if (text.empty()) {
    return {};
  }
  try {
    return nlohmann::json::parse(text).get<std::vector<std::string>>();
  } catch (...) {
    return {};
  }
}
} // namespace

Repository::Repository(Database &database) : database_(database) {}

void Repository::initSchema() {
  const char *sql = "CREATE TABLE IF NOT EXISTS entries ("
                    "id TEXT PRIMARY KEY,"
                    "title TEXT NOT NULL,"
                    "platform TEXT NOT NULL,"
                    "author TEXT,"
                    "description TEXT,"
                    "tags TEXT,"
                    "payload TEXT NOT NULL,"
                    "thumbnail TEXT,"
                    "created_at TEXT,"
                    "source_image TEXT"
                    ");";

  char *err = nullptr;
  if (sqlite3_exec(database_.connection(), sql, nullptr, nullptr, &err) !=
      SQLITE_OK) {
    std::string e = err ? err : "unknown";
    sqlite3_free(err);
    throw std::runtime_error("Failed to init repository schema: " + e);
  }
}

void Repository::saveEntry(const Entry &entry) {
  const char *sql = "INSERT OR REPLACE INTO entries ("
                    "id,title,platform,author,description,tags,payload,"
                    "thumbnail,created_at,source_image)"
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(database_.connection(), sql, -1, &stmt, nullptr) !=
      SQLITE_OK) {
    throw std::runtime_error("Failed to prepare save statement");
  }

  sqlite3_bind_text(stmt, 1, entry.id.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, entry.title.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, entry.platform.c_str(), -1, SQLITE_TRANSIENT);

  if (entry.author.has_value()) {
    sqlite3_bind_text(stmt, 4, entry.author->c_str(), -1, SQLITE_TRANSIENT);
  } else {
    sqlite3_bind_null(stmt, 4);
  }

  if (entry.description.has_value()) {
    sqlite3_bind_text(stmt, 5, entry.description->c_str(), -1,
                      SQLITE_TRANSIENT);
  } else {
    sqlite3_bind_null(stmt, 5);
  }

  sqlite3_bind_text(stmt, 6, serializeTags(entry.tags).c_str(), -1,
                    SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 7, entry.payload.dump().c_str(), -1,
                    SQLITE_TRANSIENT);

  if (entry.thumbnail.has_value()) {
    sqlite3_bind_text(stmt, 8, entry.thumbnail->c_str(), -1, SQLITE_TRANSIENT);
  } else {
    sqlite3_bind_null(stmt, 8);
  }

  if (entry.createdAt.has_value()) {
    sqlite3_bind_text(stmt, 9, entry.createdAt->c_str(), -1, SQLITE_TRANSIENT);
  } else {
    sqlite3_bind_null(stmt, 9);
  }

  if (entry.sourceImage.has_value()) {
    sqlite3_bind_text(stmt, 10, entry.sourceImage->c_str(), -1,
                      SQLITE_TRANSIENT);
  } else {
    sqlite3_bind_null(stmt, 10);
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    throw std::runtime_error("Failed to execute save statement");
  }

  sqlite3_finalize(stmt);
}

std::optional<Entry> Repository::getEntry(const std::string &id) {
  const char *sql = "SELECT id, title, platform, author, description, tags, "
                    "payload, thumbnail, created_at, source_image "
                    "FROM entries WHERE id = ?;";

  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(database_.connection(), sql, -1, &stmt, nullptr) !=
      SQLITE_OK) {
    throw std::runtime_error("Failed to prepare get statement");
  }

  sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
  std::optional<Entry> result;

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    Entry entry;
    entry.id = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    entry.title = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    entry.platform =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

    if (sqlite3_column_type(stmt, 3) != SQLITE_NULL) {
      entry.author =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    }
    if (sqlite3_column_type(stmt, 4) != SQLITE_NULL) {
      entry.description =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
    }

    entry.tags = deserializeTags(
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5)));
    entry.payload = nlohmann::json::parse(
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6)));

    if (sqlite3_column_type(stmt, 7) != SQLITE_NULL) {
      entry.thumbnail =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 7));
    }
    if (sqlite3_column_type(stmt, 8) != SQLITE_NULL) {
      entry.createdAt =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8));
    }
    if (sqlite3_column_type(stmt, 9) != SQLITE_NULL) {
      entry.sourceImage =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 9));
    }

    result = std::move(entry);
  }

  sqlite3_finalize(stmt);
  return result;
}

std::vector<Entry> Repository::listEntries() {
  const char *sql = "SELECT id, title, platform, author, description, tags, "
                    "payload, thumbnail, created_at, source_image "
                    "FROM entries ORDER BY created_at DESC;";

  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(database_.connection(), sql, -1, &stmt, nullptr) !=
      SQLITE_OK) {
    throw std::runtime_error("Failed to prepare list statement");
  }

  std::vector<Entry> entries;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    Entry entry;
    entry.id = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    entry.title = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    entry.platform =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

    if (sqlite3_column_type(stmt, 3) != SQLITE_NULL) {
      entry.author =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    }
    if (sqlite3_column_type(stmt, 4) != SQLITE_NULL) {
      entry.description =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
    }

    entry.tags = deserializeTags(
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5)));
    entry.payload = nlohmann::json::parse(
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6)));

    if (sqlite3_column_type(stmt, 7) != SQLITE_NULL) {
      entry.thumbnail =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 7));
    }
    if (sqlite3_column_type(stmt, 8) != SQLITE_NULL) {
      entry.createdAt =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8));
    }
    if (sqlite3_column_type(stmt, 9) != SQLITE_NULL) {
      entry.sourceImage =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 9));
    }

    entries.push_back(std::move(entry));
  }

  sqlite3_finalize(stmt);
  return entries;
}
