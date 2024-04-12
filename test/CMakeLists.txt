project(test_document_base)

set( ${PROJECT_NAME}_SOURCES
        test_document_json.cpp
        test_document_t.cpp
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        CONAN_PKG::catch2
        document_library
        generaty_library
)
