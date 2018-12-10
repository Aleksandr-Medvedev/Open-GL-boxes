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
    for (Drawer *drawer: *drawers) {
        if (drawer) {
            drawer->onWindowCreated();
        }
    }
    return 0;
}

int AppLooper::run() {
    if (!window && initWindow() != 0) {
        // TODO: throw an error
    }

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        for (Drawer *drawer: *drawers) {
            if (drawer) {
                drawer->onDraw();
            }
        }
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
