add_executable(codecoach main.cpp)

target_include_directories(codecoach PRIVATE ${CMAKE_SOURCE_DIR}/backend)

find_package(SQLite3 REQUIRED)
if(TARGET SQLite::SQLite3)
  target_link_libraries(codecoach PRIVATE SQLite::SQLite3)
endif()

find_package(nlohmann_json CONFIG QUIET)
if(nlohmann_json_FOUND)
  target_link_libraries(codecoach PRIVATE nlohmann_json::nlohmann_json)
  target_compile_definitions(codecoach PRIVATE HAVE_NLOHMANN_JSON)
endif()

target_link_libraries(codecoach PRIVATE backend_lib)
