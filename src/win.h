
#pragma once

#include <glad/glad.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <misc/cpp/imgui_stdlib.h>

class win
{
private:
    /* data */
public:
    win(/* args */);
    ~win();

    int init(int width, int height);
    bool loop();
    void pre_render();
    void imgui_render();
    void opengl_render();
    void swap_buffers();

    GLFWwindow* window;



};

