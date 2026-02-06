cmake_minimum_required(VERSION 3.14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(yapb
    VERSION 0.1.0
    DESCRIPTION "Yet Another Protocol Buffer"
    LANGUAGES C
)

# ===== BUILD OPTIONS =====
option(YAPB_BUILD_SHARED "Build shared library instead of static" OFF)
option(YAPB_BUILD_TESTS "Build test cases" ON)
option(YAPB_BUILD_FUZZERS "Build fuzzing targets" OFF)

# ===== C STANDARD =====
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ===== LIBRARY SOURCES =====
set(YAPB_SOURCES
    src/yapb.c
)

set(YAPB_HEADERS
    include/yapb.h
)

# ===== BUILD LIBRARY (STATIC OR SHARED) =====
if(YAPB_BUILD_SHARED)
    add_library(yapb SHARED ${YAPB_SOURCES})
else()
    add_library(yapb STATIC ${YAPB_SOURCES})
endif()

# ===== INCLUDE DIRECTORIES =====
target_include_directories(yapb
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# ===== TARGET PROPERTIES =====
set_target_properties(yapb PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${YAPB_HEADERS}"
)

# ===== INSTALL TARGETS =====
include(GNUInstallDirs)

install(TARGETS yapb
    EXPORT yapbTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yapb
)

install(EXPORT yapbTargets
    FILE yapbTargets.cmake
    NAMESPACE yapb::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/yapb
)

# ===== PACKAGE CONFIG =====
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/yapbConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/yapbConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/yapb
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/yapbConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/yapbConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/yapbConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/yapb
)

# ===== SUBDIRECTORIES =====
if(YAPB_BUILD_FUZZERS)
    add_subdirectory(fuzzers)
endif()

if(YAPB_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
