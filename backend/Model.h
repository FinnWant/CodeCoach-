#pragma once

// Minimal model header for future JSON <-> C++ types

#include <string>
#include <nlohmann/json.hpp>

struct Entry {
    std::string id;
    nlohmann::json payload;
};
