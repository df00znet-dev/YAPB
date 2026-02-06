cmake_minimum_required(VERSION 3.14)

include(FetchContent)

if(NOT TARGET yapb)
    project(yapb-tests LANGUAGES C)
    find_package(yapb REQUIRED)
    set(YAPB_LIB yapb::yapb)
else()
    set(YAPB_LIB yapb)
endif()

# Fetch munit from GitHub
FetchContent_Declare(
    munit
    GIT_REPOSITORY https://github.com/nemequ/munit.git
    GIT_TAG master
)
FetchContent_MakeAvailable(munit)

# Build munit as a library
add_library(munit STATIC ${munit_SOURCE_DIR}/munit.c)
target_include_directories(munit PUBLIC ${munit_SOURCE_DIR})

# Build test executable
add_executable(test_yapb test_yapb.c)
target_link_libraries(test_yapb PRIVATE ${YAPB_LIB} munit)
add_test(NAME test_yapb COMMAND test_yapb)
