//
//  Shaders.cpp
//  OpenGLApp
//
//  Created by Aleksandr Medvedev on 12/29/18.
//  Copyright © 2018 Aleksandr Medvedev. All rights reserved.
//

#include <string>
#include "Shaders.hpp"

const char *const vertexShader = R"glsl(
    #version 150
    in vec3 iPosition;
    in vec3 iColor;
    in vec2 iTexturePosition;
    out vec3 vColor;
    out vec2 vTexturePosition;
    uniform mat4 uModelMatrix;
    uniform mat4 uViewMatrix;
    uniform mat4 uProjectionMatrix;
    void main() {
        gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(iPosition, 1.0);
        vColor = iColor;
        vTexturePosition = iTexturePosition;
    }
)glsl";

const char *const fragmentShader = R"glsl(
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
    }
)glsl";
