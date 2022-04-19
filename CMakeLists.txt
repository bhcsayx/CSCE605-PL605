cmake_minimum_required(VERSION 3.10)
project(pl605)

add_definitions("-Wall -g")

set(FRONTEND_DIR ${PROJECT_SOURCE_DIR}/frontend)
set(IR_DIR ${PROJECT_SOURCE_DIR}/IR)

add_subdirectory(frontend)
add_subdirectory(IR)

include_directories(${PROJECT_SOURCE_DIR}
    ${FRONTEND_DIR}
    ${IR_DIR}
)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} frontend)
target_link_libraries(${PROJECT_NAME} IR)