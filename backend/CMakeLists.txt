project(backend)

aux_source_directory(${PROJECT_SOURCE_DIR} FE_FILES)

add_library(${PROJECT_NAME} STATIC ${FE_FILES})

include_directories(
    ${PROJECT_SOURCE_DIR}
)