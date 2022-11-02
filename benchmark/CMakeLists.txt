find_package(GTest CONFIG REQUIRED)

# recsplit benchmark tests
set(SUX_RECSPLIT_DUMP recsplit_dump)
add_executable(${SUX_RECSPLIT_DUMP} ${CMAKE_SOURCE_DIR}/benchmark/function/recsplit_dump.cpp)
target_include_directories(${SUX_RECSPLIT_DUMP} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${SUX_RECSPLIT_DUMP} GTest::gtest)

set(SUX_RECSPLIT_DUMP128 recsplit_dump128)
add_executable(${SUX_RECSPLIT_DUMP128} ${CMAKE_SOURCE_DIR}/benchmark/function/recsplit_dump128.cpp)
target_include_directories(${SUX_RECSPLIT_DUMP128} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${SUX_RECSPLIT_DUMP128} GTest::gtest)

set(SUX_RECSPLIT_LOAD recsplit_load)
add_executable(${SUX_RECSPLIT_LOAD} ${CMAKE_SOURCE_DIR}/benchmark/function/recsplit_load.cpp)
target_include_directories(${SUX_RECSPLIT_LOAD} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${SUX_RECSPLIT_LOAD} GTest::gtest)

set(SUX_RECSPLIT_LOAD128 recsplit_load128)
add_executable(${SUX_RECSPLIT_LOAD128} ${CMAKE_SOURCE_DIR}/benchmark/function/recsplit_load128.cpp)
target_include_directories(${SUX_RECSPLIT_LOAD128} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${SUX_RECSPLIT_LOAD128} GTest::gtest)
