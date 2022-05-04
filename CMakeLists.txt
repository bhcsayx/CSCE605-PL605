cmake_minimum_required(VERSION 3.10)
project(pl605)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -g")

set(FRONTEND_DIR ${PROJECT_SOURCE_DIR}/frontend)
set(IR_DIR ${PROJECT_SOURCE_DIR}/IR)
set(GRAPHVIZ_DIR ${PROJECT_SOURCE_DIR}/graphviz)
set(OPTIM_DIR ${PROJECT_SOURCE_DIR}/optimization)
set(BACKEND_DIR ${PROJECT_SOURCE_DIR}/backend)

add_subdirectory(frontend)
add_subdirectory(IR)
add_subdirectory(graphviz)
add_subdirectory(optimization)
add_subdirectory(backend)

include_directories(${PROJECT_SOURCE_DIR}
    ${FRONTEND_DIR}
    ${IR_DIR}
    ${GRAPHVIZ_DIR}
    ${OPTIM_DIR}
    ${BACKEND_DIR}
)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} frontend)
target_link_libraries(${PROJECT_NAME} IR)
target_link_libraries(${PROJECT_NAME} graphviz)
target_link_libraries(${PROJECT_NAME} optimization)
target_link_libraries(${PROJECT_NAME} backend)