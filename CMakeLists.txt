cmake_minimum_required(VERSION 3.11)

project(binview
    VERSION 1.0
    LANGUAGES CXX
)

add_library(binview INTERFACE)

target_include_directories(binview
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(binview
    INTERFACE cxx_std_11
)

option(BINVIEW_BUILD_TESTS "Build tests" ON)

if(BINVIEW_BUILD_TESTS)
    enable_testing()
    add_executable(binview_tests
        tests/test.cpp
    )

    target_link_libraries(binview_tests
        PRIVATE binview
    )

    add_test(
        NAME binview
        COMMAND binview_tests
    )
endif()

option(BINVIEW_BUILD_BENCHMARKS "Build benchmarks" ON)

if(BINVIEW_BUILD_BENCHMARKS)
    include(FetchContent)

    FetchContent_Declare(
        nanobench
        GIT_REPOSITORY https://github.com/martinus/nanobench.git
        GIT_TAG v4.3.11
        GIT_SHALLOW TRUE
    )
    FetchContent_GetProperties(nanobench)
    if(NOT nanobench_POPULATED)
        FetchContent_Populate(nanobench)
    endif()

    add_library(binview_nanobench STATIC
        ${nanobench_SOURCE_DIR}/src/test/app/nanobench.cpp
    )
    target_include_directories(binview_nanobench
        PUBLIC ${nanobench_SOURCE_DIR}/src/include
    )
    target_compile_features(binview_nanobench
        PUBLIC cxx_std_11
    )

    add_executable(binview_runtime_benchmarks
        benchmarks/runtime.cpp
    )
    target_link_libraries(binview_runtime_benchmarks
        PRIVATE binview binview_nanobench
    )

    add_library(binview_compile_time_benchmark OBJECT
        benchmarks/compile_time.cpp
    )
    target_link_libraries(binview_compile_time_benchmark
        PRIVATE binview
    )
endif()
