cmake_minimum_required(VERSION 3.10)

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
