cmake_minimum_required(VERSION 3.14)

if(NOT TARGET yapb)
    project(yapb-fuzzers LANGUAGES C)
    find_package(yapb REQUIRED)
    set(YAPB_LIB yapb::yapb)
else()
    set(YAPB_LIB yapb)
endif()

add_executable(fuzz_parse fuzzer_parse.c)
target_link_libraries(fuzz_parse PRIVATE ${YAPB_LIB})
target_compile_options(fuzz_parse PRIVATE -fsanitize=fuzzer,address -O1 -fno-omit-frame-pointer)
target_link_libraries(fuzz_parse PRIVATE -fsanitize=fuzzer,address)

add_executable(gen_corpus gen_corpus.c)
target_link_libraries(gen_corpus PRIVATE ${YAPB_LIB})
