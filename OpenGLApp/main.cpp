//
//  main.cpp
//  OpenGLApp
//
//  Created by Aleksandr Medvedev on 12/3/18.
//  Copyright © 2018 Aleksandr Medvedev. All rights reserved.
//

#include "AppLooper.hpp"
#include "MainDrawer.hpp"

int main(int argc, const char *argv[]) {
    AppLooper *looper = AppLooper::shared();
    looper->addDrawer(new MainDrawer());
    looper->initWindow();
    return AppLooper::shared()->run();
}
