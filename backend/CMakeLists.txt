add_library(backend_lib
    Database.cpp
)

target_include_directories(backend_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(SQLite3 REQUIRED)
if(TARGET SQLite::SQLite3)
  target_link_libraries(backend_lib PRIVATE SQLite::SQLite3)
endif()

find_package(nlohmann_json CONFIG QUIET)
if(nlohmann_json_FOUND)
  target_link_libraries(backend_lib PRIVATE nlohmann_json::nlohmann_json)
  target_compile_definitions(backend_lib PRIVATE HAVE_NLOHMANN_JSON)
endif()

set_target_properties(backend_lib PROPERTIES
  PUBLIC_HEADER "Database.h;Model.h"
)
