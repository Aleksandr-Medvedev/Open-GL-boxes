//
//  MainDrawer.cpp
//  OpenGLApp
//
//  Created by Aleksandr Medvedev on 12/3/18.
//  Copyright © 2018 Aleksandr Medvedev. All rights reserved.
//

#include <string>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include "BMPLoader.hpp"
#include "GLFW_Bridge.h"
#include "Shaders.hpp"
#include "MainDrawer.hpp"

#pragma mark - Lifecycle

MainDrawer::MainDrawer() {
    xModelRotationAngle = 0;
    xModelRotationSpeed = 0;
    xModelRotationAcceleration = 0;
    xViewRotateAngle = 0;
    yViewRotateAngle = 0;
    secondsTicker = 0;
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
    glAttachShader(program, loadShader(GL_VERTEX_SHADER, vertexShader));
    glAttachShader(program, loadShader(GL_FRAGMENT_SHADER, fragmentShader));
    glBindFragDataLocation(program, 0, "outColor");
    glLinkProgram(program);
    glUseProgram(program);
    unsigned int verteciesStride = 8 * sizeof(float);
    GLint vertexPositionAttributeLocation = glGetAttribLocation(program, "iPosition");
    glVertexAttribPointer(vertexPositionAttributeLocation, 3, GL_FLOAT, GL_FALSE, verteciesStride, 0);
    glEnableVertexAttribArray(vertexPositionAttributeLocation);
    GLint vertexColorAttributeLocation = glGetAttribLocation(program, "iColor");
    glVertexAttribPointer(vertexColorAttributeLocation, 3, GL_FLOAT, GL_FALSE, verteciesStride,
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(vertexColorAttributeLocation);
    GLint vertexTextureCoordinateAttributeLocation = glGetAttribLocation(program, "iTexturePosition");
    glVertexAttribPointer(vertexTextureCoordinateAttributeLocation, 2, GL_FLOAT, GL_FALSE, verteciesStride,
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(vertexTextureCoordinateAttributeLocation);
    return program;
}

#pragma mark - Drawer

void MainDrawer::onWindowCreated(float width, float height) {
    glClearColor(0, 0, 0, 1);
    initVerticesBuffer();
    program = loadProgram();
    bindTexture(program, "Odysseybalancing.bmp", "uEveShipsTexture", 0);
    bindTexture(program, "space.bmp", "uSpaceTexture", 1);
    screenAspect = width / height;
}

void MainDrawer::onDraw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLint millis = currentTimeMillis();
    bindTime(millis);
    bindProjectionMatrix(1.0);
    bindViewMatrix(2.0, 2.0, 8.0, xViewRotateAngle, yViewRotateAngle);
    bindModelMatrix(millis, getXModelRotation(), 1.8, 1);
    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
    bindModelMatrix(-millis, -getXModelRotation(), -1.8, -1);
    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
}

#pragma mark User events

void MainDrawer::onSpacePressed() {
    std::cout << "space pressed" << std::endl;
    if (xModelRotationAcceleration < 10) {
        xModelRotationAcceleration += 1;
    }
}

void MainDrawer::onDragMouse(float xO, float yO, float x, float y) {
    static float const fullAngle = 360;
    yViewRotateAngle = (x - xO) / 800 * fullAngle;
    xViewRotateAngle = (y - yO) / 800 * fullAngle;
}

#pragma mark - Uniforms

void MainDrawer::bindTexture(GLuint program, char const* imagePath, char const* uniform, unsigned index) {
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

void MainDrawer::bindTime(GLint milliseconds) {
    glUniform1f(glGetUniformLocation(program, "uTime"), static_cast<GLfloat>(milliseconds));
}

#define MGL_CURRENT_MILLIS(accuracy) std::chrono::duration_cast<std::chrono::milliseconds>( \
                                            std::chrono::system_clock::now().time_since_epoch()).count() >> accuracy

#pragma mark MVP matricies

void MainDrawer::bindProjectionMatrix(float zoom) {
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f * zoom), screenAspect, 1.0f, 16.0f);
    const int uniformLocation = glGetUniformLocation(program, "uProjectionMatrix");
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}

void MainDrawer::bindViewMatrix(float x, float y, float z, float xAngleDegrees, float yAngleDegrees) {
    glm::vec3 origin = glm::vec3(x, y, z);
    origin = glm::rotateX(origin, glm::radians(xAngleDegrees));
    origin = glm::rotateY(origin, glm::radians(yAngleDegrees));
    glm::mat4 viewMatrix = glm::lookAt(origin, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    const int uniformLocation = glGetUniformLocation(program, "uViewMatrix");
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
}

void MainDrawer::bindModelMatrix(float zRotation, float xRotation, float yTranslate, float scaleFactor) {
    if (xModelRotationAcceleration != 0) {
        std::cout << std::fixed << std::setprecision(2) << xRotation << std::endl;
    }
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, yTranslate, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(zRotation) / 4, glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(xRotation), glm::vec3(1.0f, 0.0f, 0.0f));
    float scale = (std::sin(abs(zRotation) * M_PI / 180) / 4 + 1);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(scale, scale * scaleFactor, scale));
    glUniformMatrix4fv(glGetUniformLocation(program, "uModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

#pragma mark - Private helpers

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

float MainDrawer::getXModelRotation() {
    xModelRotationSpeed += xModelRotationAcceleration;
    if (xModelRotationSpeed <= 0.001f) {
        secondsTicker = 0;
        xModelRotationAcceleration = 0;
        xModelRotationSpeed = 0;
    } else {
        long long const currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        if (!secondsTicker) {
            secondsTicker = currentTime;
        }
        xModelRotationAcceleration = -xModelRotationSpeed / 1000;

        secondsTicker = currentTime;
    }
    xModelRotationAngle += xModelRotationSpeed;
    return xModelRotationAngle;
}

void MainDrawer::initVerticesBuffer() {
    GLuint vertexAO;
    glGenVertexArrays(1, &vertexAO);
    glBindVertexArray(vertexAO);

    float vertices[] = {
        0.5f, 0.5f, 0.5f,       1.0f, 0.5f, 0.5f,   1.0f, 0.0f,
        0.5f, 0.5f, -0.5f,      0.5f, 1.0f, 0.5f,   1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f,     0.5f, 0.5f, 1.0f,   0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f,     1.0f, 0.5f, 0.5f,   1.0f, 0.0f,
        0.5f, -0.5f, 0.5f,      0.5f, 1.0f, 0.5f,   1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,     0.5f, 0.5f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,   0.0f, 0.0f,

    };
    GLuint verticesBO;
    glGenBuffers(1, &verticesBO);
    glBindBuffer(GL_ARRAY_BUFFER, verticesBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint elements[] = {
        3, 1, 0,
        3, 2, 1,
        4, 7, 6,
        6, 5, 4,
        1, 7, 6,
        1, 2, 7,
        0, 1, 5,
        1, 6, 5,
        4, 3, 0,
        4, 0, 5
    };
    GLuint elementsBO;
    glGenBuffers(1, &elementsBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
}

GLint MainDrawer::currentTimeMillis() {
    static const long long timeMomemnt = MGL_CURRENT_MILLIS(5);
    long long millis = MGL_CURRENT_MILLIS(5);
    return ~(1UL << (sizeof(GLint) * 8 - 1)) & static_cast<GLint>(millis - timeMomemnt);
}

void printArray(float *array, int count) {
   std::cout << std::fixed;
   std::cout << std::setprecision(2);
   for (int i = 0; i < count; i++) {
       std::cout << array[i] << '\t';
   }
   std::cout << std::endl;
}

void printMat4(glm::mat4 &mat) {
   std::cout << std::fixed;
   std::cout << std::setprecision(2);
   for (int i = 0; i < mat.length(); ++i) {
       glm::vec4 vector = mat[i];
       for (int j = 0; j < vector.length(); ++j) {
           std::cout << vector[j] << '\t';
       }
       std::cout << std::endl;
   }
}
