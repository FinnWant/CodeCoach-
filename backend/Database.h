#pragma once

#include <sqlite3.h>
#include <string>

class Database {
public:
  explicit Database(const std::string &path);
  ~Database();

  void initSchema();
  sqlite3 *connection() const;

private:
  sqlite3 *db_ = nullptr;
};
