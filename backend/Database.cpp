#include "Database.h"
#include <stdexcept>
#include <sqlite3.h>

Database::Database(const std::string &path) {
    if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db_ ? db_ : nullptr);
        if (db_) sqlite3_close(db_);
        throw std::runtime_error("Failed to open DB: " + err);
    }
}

Database::~Database() {
    if (db_) sqlite3_close(db_);
}

void Database::initSchema() {
    const char* sql = "CREATE TABLE IF NOT EXISTS entries (id TEXT PRIMARY KEY, payload TEXT NOT NULL, thumbnail TEXT, created_at TEXT);";
    char* err = nullptr;
    if (sqlite3_exec(db_, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::string e = err ? err : "unknown";
        sqlite3_free(err);
        throw std::runtime_error("Failed to init schema: " + e);
    }
}
