#pragma once

#include "Database.h"
#include "Model.h"

#include <optional>
#include <string>
#include <vector>

class Repository {
public:
    explicit Repository(Database &database);

    void initSchema();
    void saveEntry(const Entry &entry);
    std::optional<Entry> getEntry(const std::string &id);
    std::vector<Entry> listEntries();

private:
    Database &database_;
};
