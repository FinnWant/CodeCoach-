#pragma once

#include <string>
#include <sqlite3.h>

class Database {
public:
    explicit Database(const std::string &path);
    ~Database();

    void initSchema();

private:
    sqlite3 *db_ = nullptr;
};
