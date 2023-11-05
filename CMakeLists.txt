cmake_minimum_required(VERSION 3.14)

project(vvv)

set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(PYBIND11_DIR ${LIBS_DIR}/pybind11)

find_package(PythonLibs REQUIRED)

add_subdirectory(${PYBIND11_DIR})

add_executable(${CMAKE_PROJECT_NAME} 
    src/main.cpp
    )

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC 
    ${PYTHON_INCLUDE_DIRS} 
    ${PYBIND11_DIR}/include
    )

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC 
    ${PYTHON_LIBRARIES}
    ) 