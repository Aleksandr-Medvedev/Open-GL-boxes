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
    void onWindowCreated(float width, float height) override;
    void onSpacePressed() override;
    void onDragMouse(float xOrigin, float yOrigin, float x, float y) override;
    void initVerticesBuffer();
    void bindTexture(GLuint program, char const* imagePath, char const* uniform, unsigned index);
    void bindModelMatrix(float zRotation, float xyRotation, float yTranslate, float scaleFactor);
    void bindViewMatrix(float x, float y, float z, float xAngleDegrees, float yAngleDegrees);
    void bindProjectionMatrix(float zoom);
    void bindTime(GLint milliseconds);
    float getXModelRotation();
    char const *getGlErrorMessage();
    GLint currentTimeMillis();
    GLuint loadShader(GLenum type, const char *source);
    GLuint loadProgram();
    int program;
    long long secondsTicker;
    float screenAspect;
    float xModelRotationAcceleration;
    float xModelRotationSpeed;
    float xModelRotationAngle;
    float xViewRotateAngle;
    float yViewRotateAngle;
public:
    MainDrawer();
};

#endif /* MainDrawer_hpp */
