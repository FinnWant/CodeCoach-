find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 OPTIONAL_COMPONENTS Svg)
find_package(nlohmann_json CONFIG QUIET)

set(UI_SOURCES
    main_ui.cpp
    MainWindow.cpp
    BrowseWidget.cpp
    AddEntryDialog.cpp
    DetailWidget.cpp
    PayloadRenderer.cpp
    resources.qrc
)

add_executable(codecoach_ui ${UI_SOURCES})

target_include_directories(codecoach_ui PRIVATE
    ${CMAKE_SOURCE_DIR}/backend
)

target_link_libraries(codecoach_ui PRIVATE
    Qt6::Widgets
    backend_lib
)

if(Qt6Svg_FOUND)
    target_link_libraries(codecoach_ui PRIVATE Qt6::Svg)
    target_compile_definitions(codecoach_ui PRIVATE HAVE_QT_SVG)
endif()

if(nlohmann_json_FOUND)
    target_link_libraries(codecoach_ui PRIVATE nlohmann_json::nlohmann_json)
    target_compile_definitions(codecoach_ui PRIVATE HAVE_NLOHMANN_JSON)
endif()
