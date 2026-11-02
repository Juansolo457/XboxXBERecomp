# The SDK's tests. Built on their own, not by a game: cmake -S tests -B out/tests -G Ninja, then ctest --test-dir out/tests
cmake_minimum_required(VERSION 3.25)
project(XboxXBERecompTests LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "the SDK builds with Clang: pass -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++")
endif()
add_compile_options(-march=x86-64-v2)

get_filename_component(XBE_SDK_DIR ${CMAKE_CURRENT_LIST_DIR}/.. ABSOLUTE)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
enable_testing()

include(FetchContent)
# the versions cmake/XbeRecomp.cmake takes; that file also builds the game's window, input and audio, which tests skip
set(ZYDIS_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(ZYDIS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ZYDIS_BUILD_DOXYGEN OFF CACHE BOOL "" FORCE)
set(ZYDIS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(zydis GIT_REPOSITORY https://github.com/zyantific/zydis.git GIT_TAG v4.1.1 GIT_SHALLOW TRUE)
FetchContent_Declare(tomlplusplus GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git GIT_TAG v3.4.0 GIT_SHALLOW TRUE)
# the single header only
FetchContent_Declare(doctest GIT_REPOSITORY https://github.com/doctest/doctest.git GIT_TAG v2.4.11 GIT_SHALLOW TRUE
    SOURCE_SUBDIR no_cmake_build)
FetchContent_MakeAvailable(zydis tomlplusplus doctest)
add_library(doctest_header INTERFACE)
target_include_directories(doctest_header SYSTEM INTERFACE ${doctest_SOURCE_DIR})

file(GLOB XBE_RECOMP_SOURCES CONFIGURE_DEPENDS ${XBE_SDK_DIR}/tools/recomp/*.cpp)
add_executable(xbe_recomp ${XBE_RECOMP_SOURCES})
target_include_directories(xbe_recomp PRIVATE ${XBE_SDK_DIR}/runtime ${XBE_SDK_DIR}/tools/recomp)
target_compile_definitions(xbe_recomp PRIVATE _CRT_SECURE_NO_WARNINGS)
target_link_libraries(xbe_recomp PRIVATE Zydis tomlplusplus::tomlplusplus)
if(WIN32)
    target_link_options(xbe_recomp PRIVATE -fuse-ld=lld)
endif()

add_subdirectory(recomp)
add_subdirectory(runtime)

add_subdirectory(${XBE_SDK_DIR}/tools/perf/bench perf_bench)
add_test(NAME perf_tools COMMAND ${Python3_EXECUTABLE} ${XBE_SDK_DIR}/tools/perf/test_perf_tools.py)
