project(benchmark)

cmake_policy(SET CMP0048 NEW)

set( ${PROJECT_NAME}_SOURCES
        benchmark.cpp
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        document_library
        generaty_library
        CONAN_PKG::benchmark
)
