cmake_minimum_required(VERSION 3.28.1)

project(InMemory_DB)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_testing()
add_library(InMemory_DB src/db.cpp inc/cell.hpp src/operator.cpp src/query_parser.cpp src/lexer.cpp
        src/instruction.cpp)
target_include_directories(InMemory_DB PRIVATE inc/)
include_directories(inc/)
include(FetchContent)
fetchcontent_declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(lexer_tests test/lexer_test.cpp)
add_executable(parser_tests test/parser_test.cpp)
add_executable(math_engine_tests test/math_engine_test.cpp)
add_executable(db_tests test/db_tests.cpp)
add_executable(example src/main.cpp)
target_link_libraries(example InMemory_DB)
target_link_libraries(lexer_tests GTest::gtest_main InMemory_DB)
target_link_libraries(db_tests GTest::gtest_main InMemory_DB)
target_link_libraries(parser_tests GTest::gtest_main InMemory_DB)
target_link_libraries(math_engine_tests GTest::gtest_main InMemory_DB)

include(GoogleTest)
gtest_discover_tests(lexer_tests)
gtest_discover_tests(parser_tests)
gtest_discover_tests(math_engine_tests)
gtest_discover_tests(db_tests)

