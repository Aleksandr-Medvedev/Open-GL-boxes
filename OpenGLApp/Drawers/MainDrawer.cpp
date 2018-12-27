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
#include "BMPLoader.hpp"
#include "MainDrawer.hpp"
#include "GLFW_Bridge.h"
#include <OpenGL/gl3ext.h>

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
                                       uniform mat4 uMVPMatrix;
                                       void main() {
                                           gl_Position = uMVPMatrix * vec4(iPosition, 0.0, 1.0);
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
                                           if (vTexturePosition.y < 0.5) {
                                               texturePositionBuffer.y = 1 - texturePositionBuffer.y;
                                               texturePositionBuffer.x = texturePositionBuffer.x + sin(texturePositionBuffer.y * 128 + uTime / 4) * 0.02;
                                           }
                                           vec4 eveShipsColor = texture(uEveShipsTexture, texturePositionBuffer);
                                           vec4 spaceColor = texture(uSpaceTexture, texturePositionBuffer);
                                           float showDegree = sin(radians(uTime * 4)) / 2 + 0.5;
                                           outColor = mix(eveShipsColor, spaceColor, showDegree) * vec4(vColor, 1);
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
        -0.5f, 0.5f,    1.0f, 0.5f, 0.5f,   0.0f, 1.0f,
        0.5f, 0.5f,     0.5f, 1.0f, 0.5f,   1.0f, 1.0f,
        0.5f, -0.5f,    0.5f, 0.5f, 1.0f,   1.0f, 0.0f,
        -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f
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
    .count() >> 5;
    long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
        .count() >> 5;
    return ~(1UL << (sizeof(GLint) * 8 - 1)) & static_cast<GLint>(millis - timeMomemnt);
}

void MainDrawer::bindMVPMatrix(float zRotation, float xRotation) {
    glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(zRotation) / 4, glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(xRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    float scaleFactor = std::sin(zRotation * M_PI / 180) / 4 + 1.5f;
    modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleFactor, scaleFactor, 1.0f));
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0, 0, 4.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), screenAspect, 1.0f, 8.0f);
    glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
    glUniformMatrix4fv(glGetUniformLocation(program, "uMVPMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
}

void MainDrawer::bindTime(GLint milliseconds) {
    glUniform1f(glGetUniformLocation(program, "uTime"), static_cast<GLfloat>(milliseconds));
}

float MainDrawer::getXRotation() {
    xRotationSpeed += xRotationAcceleration;
    if (xRotationSpeed <= 0) {
        secondsTicker = 0;
        xRotationAcceleration = 0;
        xRotationSpeed = 0;
    } else {
        long long const currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        if (!secondsTicker) {
            secondsTicker = currentTime;
        }
        xRotationAcceleration = -xRotationSpeed / 1000;

        secondsTicker = currentTime;
    }
    xRotationAngle += xRotationSpeed;
    return xRotationAngle;
}

void MainDrawer::onDraw() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    GLint millis = currentTimeMillis();
    bindTime(millis);
    bindMVPMatrix(millis, getXRotation());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void MainDrawer::onWindowCreated(float width, float height) {
    xRotationSpeed = 0;
    secondsTicker = 0;
    xRotationAcceleration = 0;
    initVerticesBuffer();
    program = loadProgram();
    attachTexture(program, "Odysseybalancing.bmp", "uEveShipsTexture", 0);
    attachTexture(program, "space.bmp", "uSpaceTexture", 1);
    screenAspect = width / height;
}

void MainDrawer::onSpacePressed() {
    std::cout << "space pressed" << std::endl;
    if (xRotationAcceleration < 10) {
        xRotationAcceleration += 1;
    }
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
