find_package(SQLite3 REQUIRED)
find_package(nlohmann_json CONFIG QUIET)

# --- main app ---
add_executable(codecoach main.cpp)
target_include_directories(codecoach PRIVATE ${CMAKE_SOURCE_DIR}/backend)
if(TARGET SQLite::SQLite3)
  target_link_libraries(codecoach PRIVATE SQLite::SQLite3)
endif()
if(nlohmann_json_FOUND)
  target_link_libraries(codecoach PRIVATE nlohmann_json::nlohmann_json)
  target_compile_definitions(codecoach PRIVATE HAVE_NLOHMANN_JSON)
endif()
target_link_libraries(codecoach PRIVATE backend_lib)

# --- analyze_image CLI ---
add_executable(analyze_image analyze_image.cpp)
target_include_directories(analyze_image PRIVATE ${CMAKE_SOURCE_DIR}/backend)
if(nlohmann_json_FOUND)
  target_link_libraries(analyze_image PRIVATE nlohmann_json::nlohmann_json)
  target_compile_definitions(analyze_image PRIVATE HAVE_NLOHMANN_JSON)
endif()
target_link_libraries(analyze_image PRIVATE backend_lib)
