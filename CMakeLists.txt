cmake_minimum_required(VERSION 3.14)

project(vvv)

set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(PYBIND11_DIR ${LIBS_DIR}/pybind11)
set(IMGUI_DIR ${LIBS_DIR}/imgui)#

find_package(PythonLibs REQUIRED)

add_subdirectory(${PYBIND11_DIR})
add_subdirectory(${LIBS_DIR}/glfw)
add_subdirectory(${LIBS_DIR}/glad)

add_executable(${CMAKE_PROJECT_NAME} 
    src/main.cpp
    src/matrix.cpp	
    src/interface.cpp	
    src/win.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
	${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
    )

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC 
    ${PYTHON_INCLUDE_DIRS} 
    ${PYBIND11_DIR}/include
    ${LIBS_DIR}/glfw/include 
    ${LIBS_DIR}/glad/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    )

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC 
    ${PYTHON_LIBRARIES}
    glfw
    glad
    ) 