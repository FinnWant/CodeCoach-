add_library(backend_lib
    Database.cpp
    Repository.cpp
    EntryFactory.cpp
    Validation.cpp
    ImageAnalyzer.cpp
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

find_path(TESSERACT_INCLUDE_DIR tesseract/baseapi.h REQUIRED)
find_library(TESSERACT_LIB tesseract REQUIRED)
find_path(LEPTONICA_INCLUDE_DIR leptonica/allheaders.h REQUIRED)
find_library(LEPTONICA_LIB lept REQUIRED)

target_include_directories(backend_lib PRIVATE
    ${TESSERACT_INCLUDE_DIR}
    ${LEPTONICA_INCLUDE_DIR}
)
target_link_libraries(backend_lib PRIVATE
    ${TESSERACT_LIB}
    ${LEPTONICA_LIB}
)

set_target_properties(backend_lib PROPERTIES
  PUBLIC_HEADER "Database.h;Model.h;Repository.h;EntryFactory.h;Validation.h;ImageAnalyzer.h"
)
