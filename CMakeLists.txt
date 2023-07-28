cmake_minimum_required(VERSION 3.22)

project(perseus VERSION 1.0.1)

set(MAYA_LOCATION $ENV{MAYA_LOCATION})
set(MAYA_VERSION $ENV{REZ_MAYA_VERSION})

file(GLOB SOURCES perseus/source/*.cpp)

add_library(perseus MODULE ${SOURCES})

if(WIN32)
    set(PLUGIN_SUFFIX .mll)
elseif(APPLE)
    set(PLUGIN_SUFFIX .bundle)
else()
    set(PLUGIN_SUFFIX .so)
endif()

if(APPLE)
    set(MAYA_INCLUDE_DIR ${MAYA_LOCATION}/../../include)
    set(MAYA_LIBRARY_DIR ${MAYA_LOCATION}/MacOS)
    # maya 2024 supports arm64
    target_compile_options(perseus PRIVATE -Wno-ignored-attributes)
    set_target_properties(perseus PROPERTIES
        CXX_STANDARD 14
        COMPILE_FLAGS "-arch x86_64"
        LINK_FLAGS "-arch x86_64")
else()
    set(MAYA_INCLUDE_DIR ${MAYA_LOCATION}/include)
    set(MAYA_LIBRARY_DIR ${MAYA_LOCATION}/lib)
endif()

set_target_properties(perseus PROPERTIES
    PREFIX ""
    SUFFIX ${PLUGIN_SUFFIX}
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    POSITION_INDEPENDENT_CODE ON)

target_include_directories(perseus PRIVATE
    "perseus/source"
    "${MAYA_INCLUDE_DIR}")

target_link_directories(perseus PRIVATE
    "${MAYA_LIBRARY_DIR}")

target_link_libraries(perseus PRIVATE
    Foundation
    OpenMaya
    OpenMayaAnim)


install(TARGETS perseus DESTINATION plug-ins)

file(GLOB MELSCRIPTS perseus/modules/perseus/scripts/*.mel)
install(FILES ${MELSCRIPTS} DESTINATION scripts)
