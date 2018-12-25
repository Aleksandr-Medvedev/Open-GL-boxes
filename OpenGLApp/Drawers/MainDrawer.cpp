//
//  MainDrawer.cpp
//  OpenGLApp
//
//  Created by Aleksandr Medvedev on 12/3/18.
//  Copyright © 2018 Aleksandr Medvedev. All rights reserved.
//

#include <string>
#include <iostream>
#include <chrono>
#include <math.h>
#include <limits>
#include <bitset>
#include "BMPLoader.hpp"
#include "MainDrawer.hpp"
#include "GLFW_Bridge.h"

MainDrawer::MainDrawer() {
}

GLuint MainDrawer::loadShader(GLenum type, const char *source) {
    GLuint shader = glCreateShader(type);
    GLchar const *sources[1] = { source };
    glShaderSource(shader, 1, sources, NULL);
    glCompileShader(shader);
    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    char compileMessage[512];
    glGetShaderInfoLog(shader, sizeof(compileMessage), NULL, compileMessage);
    std::cout << compileMessage << std::endl;
    if (compileStatus == GL_TRUE) {
        return shader;
    } else {
        glDeleteShader(shader);
        return -1;
    }

}

GLuint MainDrawer::loadProgram() {
    GLuint program = glCreateProgram();
    glAttachShader(program, loadShader(GL_VERTEX_SHADER, R"glsl(
                                       #version 150
                                       in vec2 iPosition;
                                       in vec3 iColor;
                                       in vec2 iTexturePosition;
                                       out vec3 vColor;
                                       out vec2 vTexturePosition;
                                       void main() {
                                           gl_Position = vec4(iPosition.x, iPosition.y, 0.0, 1.0);
                                           vColor = iColor;
                                           vTexturePosition = iTexturePosition;
                                       })glsl"));
    glAttachShader(program, loadShader(GL_FRAGMENT_SHADER, R"glsl(
                                       #version 150
                                       in vec3 vColor;
                                       in vec2 vTexturePosition;
                                       out vec4 outColor;
                                       uniform sampler2D uEveShipsTexture;
                                       uniform sampler2D uSpaceTexture;
                                       uniform float uTime;
                                       void main() {
                                           vec2 texturePositionBuffer = vTexturePosition;
                                           if (vTexturePosition.y > 0.5) {
                                               texturePositionBuffer.y = 1 - texturePositionBuffer.y;
                                           } else {
                                               texturePositionBuffer.x = texturePositionBuffer.x + sin(texturePositionBuffer.y * 128) * 0.02;
                                           }
                                           vec4 eveShipsColor = texture(uEveShipsTexture, texturePositionBuffer);
                                           vec4 spaceColor = texture(uSpaceTexture, texturePositionBuffer);
                                           outColor = mix(eveShipsColor, spaceColor, sin(radians(uTime * 8)) / 2 + 0.5);
                                       })glsl"));
    glBindFragDataLocation(program, 0, "outColor");
    glLinkProgram(program);
    glUseProgram(program);
    unsigned int verteciesStride = 7 * sizeof(float);
    GLint vertexPositionAttributeLocation = glGetAttribLocation(program, "iPosition");
    glVertexAttribPointer(vertexPositionAttributeLocation, 2, GL_FLOAT, GL_FALSE, verteciesStride, 0);
    glEnableVertexAttribArray(vertexPositionAttributeLocation);
    GLint vertexColorAttributeLocation = glGetAttribLocation(program, "iColor");
    glVertexAttribPointer(vertexColorAttributeLocation, 3, GL_FLOAT, GL_FALSE, verteciesStride,
                          (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(vertexColorAttributeLocation);
    GLint vertexTextureCoordinateAttributeLocation = glGetAttribLocation(program, "iTexturePosition");
    glVertexAttribPointer(vertexTextureCoordinateAttributeLocation, 2, GL_FLOAT, GL_FALSE, verteciesStride,
                          (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(vertexTextureCoordinateAttributeLocation);
    return program;
}

void MainDrawer::initVerticesBuffer() {
    GLuint vertexAO;
    glGenVertexArrays(1, &vertexAO);
    glBindVertexArray(vertexAO);

    float vertices[] = {
        -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
        0.5f, 0.5f,     0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        0.5f, -0.5f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
        -0.5f, -0.5f,   0.5f, 0.5f, 0.5f,   0.0f, 0.0f
    };
    GLuint verticesBO;
    glGenBuffers(1, &verticesBO);
    glBindBuffer(GL_ARRAY_BUFFER, verticesBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint elements[] = {
        3, 1, 0,
        3, 1, 2
    };
    GLuint elementsBO;
    glGenBuffers(1, &elementsBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
}

void MainDrawer::attachTexture(GLuint program, char const* imagePath, char const* uniform, unsigned index) {
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    Bitmap *bitmap = BMPLoader::loadImage(imagePath);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap->width, bitmap->height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap->data);
    delete bitmap;
    glUniform1i(glGetUniformLocation(program, uniform), index);
}

char const *MainDrawer::getGlErrorMessage() {
    const int error = glGetError();
    switch (error) {
        case GL_NO_ERROR:
            return "No error\n";
        case GL_INVALID_ENUM:
            return "Invalid enum specified\n";
        case GL_INVALID_VALUE:
            return "Invalid value specified\n";
        case GL_INVALID_OPERATION:
            return "Invalid operation performed\n";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "Invalid framebuffer operation\n";
        case GL_OUT_OF_MEMORY:
            return "Out of memory exception\n";
        case GL_STACK_UNDERFLOW:
            return "Stack underflow exception\n";
        case GL_STACK_OVERFLOW:
            return "Stack overflow exception\n";
        default:
            return "Undefined error us occurred\n";
    }
}

GLint MainDrawer::currentTimeMillis() {
    static const long long timeMomemnt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
    .count() >> 7;
    long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
        .count() >> 7;
    return ~(1UL << (sizeof(GLint) * 8 - 1)) & static_cast<GLint>(millis - timeMomemnt);
}

void MainDrawer::onDraw() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    GLint millis = currentTimeMillis();
    glUniform1f(glGetUniformLocation(program, "uTime"), static_cast<GLfloat>(millis));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void MainDrawer::onWindowCreated() {
    initVerticesBuffer();
    program = loadProgram();
    attachTexture(program, "Odysseybalancing.bmp", "uEveShipsTexture", 0);
    attachTexture(program, "space.bmp", "uSpaceTexture", 1);
}
