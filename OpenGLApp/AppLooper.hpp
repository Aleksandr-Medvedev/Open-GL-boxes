//
//  AppLooper.hpp
//  OpenGLApp
//
//  Created by Aleksandr Medvedev on 12/3/18.
//  Copyright © 2018 Aleksandr Medvedev. All rights reserved.
//

#ifndef AppLooper_hpp
#define AppLooper_hpp

#include <vector>

class GLFWwindow;

class Drawer {
public:
    virtual void onDraw() = 0;
    virtual void onWindowCreated(float width, float height) = 0;
    virtual void onSpacePressed() = 0;
    virtual void onDragMouse(float xOrigin, float yOrigin, float x, float y) = 0;
};
 
class AppLooper {
    GLFWwindow *window;
    std::vector<Drawer *> *drawers;
    bool isMouseDragging;
    float xDragginOrigin, yDraggingOrigin;
    AppLooper() {
        isMouseDragging = false;
        drawers = new std::vector<Drawer *>(0);
    };

public:
    static AppLooper *const shared();
    int initWindow();
    int run();
    std::vector<Drawer *>::iterator addDrawer(Drawer *drawer);
    void removeDrawer(std::vector<Drawer *>::iterator position);
    void onLeftButtonMousePressed(float x, float y);
    void onLeftButtonMouseReleased(float x, float y);
};

#endif /* AppLooper_hpp */
