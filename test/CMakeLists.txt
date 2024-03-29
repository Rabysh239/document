project(test_document_base)

include(FetchContent)

set( ${PROJECT_NAME}_SOURCES
        generaty.cpp
        test_document_json.cpp
        test_document_t.cpp
)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0
)

FetchContent_MakeAvailable(Catch2)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        Catch2::Catch2WithMain
        document_library
)
