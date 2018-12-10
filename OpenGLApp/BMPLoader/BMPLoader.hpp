//
//  BMPLoader.hpp
//  OpenGL Findings
//
//  Created by Aleksandr Medvedev on 11/29/18.
//  Copyright © 2018 Aleksandr Medvedev. All rights reserved.
//

#ifndef BMPLoader_hpp
#define BMPLoader_hpp

#include <stdio.h>
#include <strings.h>

struct Bitmap {
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int width, height;
    unsigned int imageSize;   // = width*height*3
    // Actual RGB data
    unsigned char *data;

    Bitmap(unsigned char *header, unsigned dataPos, unsigned width, unsigned height, unsigned imageSize,
           unsigned char *data) : dataPos(dataPos), width(width), height(height), imageSize(imageSize), data(data) {
        memcpy(this->header, header, sizeof(this->header));
    }
};

class BMPLoader {
public:
    static Bitmap *loadImage(const char * imagepath);
};

#endif /* BMPLoader_hpp */
