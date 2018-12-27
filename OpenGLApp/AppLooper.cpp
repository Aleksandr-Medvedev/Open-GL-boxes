//
//  AppLooper.cpp
//  OpenGLApp
//
//  Created by Aleksandr Medvedev on 12/3/18.
//  Copyright © 2018 Aleksandr Medvedev. All rights reserved.
//

#include "AppLooper.hpp"
#include "GLFW_Bridge.h"
#include <vector>
#include <iostream>
#include <chrono>
#include <functional>

AppLooper * AppLooper::shared() {
    static AppLooper *instance = new AppLooper();
    return instance;
}

std::vector<Drawer *>::iterator AppLooper::addDrawer(Drawer *drawer) {
    drawers->push_back(drawer);
    return drawers->end();
}

void AppLooper::removeDrawer(std::vector<Drawer *>::iterator position) {
    drawers->erase(position);
}

int AppLooper::initWindow() {
    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    window = glfwCreateWindow(800, 600, "Meshes", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    std::cout << "OpenGL context is initializaed with the version: " << glGetString(GL_VERSION) << std::endl;
    int *width = new int();
    int *height = new int();
    glfwGetWindowSize(window, width, height);
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        const AppLooper *looper = AppLooper::shared();
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
            for (Drawer *drawer: *looper->drawers) {
                if (drawer) {
                    drawer->onSpacePressed();
                }
            }
        }
    });
    for (Drawer *drawer: *drawers) {
        if (drawer) {
            drawer->onWindowCreated(static_cast<float>(*width), static_cast<float>(*height));
        }
    }
    return 0;
}

int AppLooper::run() {
    if (!window && initWindow() != 0) {
        // TODO: throw an error
    }

    /* Loop until the user closes the window */
    long long lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    while (!glfwWindowShouldClose(window)) {
        for (Drawer *drawer: *drawers) {
            if (drawer) {
                drawer->onDraw();
            }
        }
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();

        long long newTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
//        std::cout << "FPS: " << 1000 / (newTime - lastTime) << std::endl;
        lastTime = newTime;
    }

    glfwTerminate();
    return 0;
}
