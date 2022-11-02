find_package(GTest CONFIG REQUIRED)

# bits unit tests
set(SUX_BITS_TEST bits_unittest)
add_executable(${SUX_BITS_TEST} ${CMAKE_SOURCE_DIR}/test/bits/test.cpp)
target_include_directories(${SUX_BITS_TEST} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${SUX_BITS_TEST} GTest::gtest)

# function unit tests
set(SUX_FUNCTION_TEST function_unittest)
add_executable(${SUX_FUNCTION_TEST} ${CMAKE_SOURCE_DIR}/test/function/test.cpp)
target_include_directories(${SUX_FUNCTION_TEST} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${SUX_FUNCTION_TEST} GTest::gtest)

# util unit tests
set(SUX_UTIL_TEST util_unittest)
add_executable(${SUX_UTIL_TEST} ${CMAKE_SOURCE_DIR}/test/util/test.cpp)
target_include_directories(${SUX_UTIL_TEST} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${SUX_UTIL_TEST} GTest::gtest)
