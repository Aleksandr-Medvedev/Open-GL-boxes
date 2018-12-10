//
//  MainDrawer.hpp
//  OpenGLApp
//
//  Created by Aleksandr Medvedev on 12/3/18.
//  Copyright © 2018 Aleksandr Medvedev. All rights reserved.
//

#ifndef MainDrawer_hpp
#define MainDrawer_hpp

#include <stdio.h>
#include <OpenGL/gltypes.h>
#include "AppLooper.hpp"

class MainDrawer: public Drawer {
    void onDraw() override;
    void onWindowCreated() override;
    void initVerticesBuffer();
    void attachTexture();
    char const *getGlErrorMessage();
    GLuint loadShader(GLenum type, const char *source);
    GLuint loadProgram();
public:
    MainDrawer();
};

#endif /* MainDrawer_hpp */
