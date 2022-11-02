cmake_minimum_required(VERSION 3.16.2)

option(SUX_TESTING "Build tests and test tools" OFF)

get_directory_property(SUX_HAS_PARENT PARENT_DIRECTORY)
if(NOT SUX_HAS_PARENT)
    if(NOT CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain.cmake CACHE FILEPATH "" FORCE)
    endif()

    set(HUNTER_URL "https://github.com/cpp-pm/hunter/archive/v0.24.1.tar.gz")
    set(HUNTER_SHA1 "4942227a6e6f5e64414c55b97ef98609de199d18")

    if(SUX_TESTING)
        set(HUNTER_PACKAGES GTest)
    endif()

    include(FetchContent)
    FetchContent_Declare(SetupHunter GIT_REPOSITORY https://github.com/cpp-pm/gate)
    FetchContent_MakeAvailable(SetupHunter)
endif()

project(sux)

include(GNUInstallDirs)

add_library(sux INTERFACE)

target_include_directories(
        sux
        INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(sux INTERFACE cxx_std_17)

if(SUX_TESTING)
    add_subdirectory(benchmark)
    add_subdirectory(test)
endif()
