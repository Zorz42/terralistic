//
//  main.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 19/02/2021.
//

#include "startMenu.hpp"
#include "fileManager.hpp"
#include "playerHandler.hpp"
#include "config.hpp"
#include "properties.hpp"
#include "textures.hpp"
#include "server.hpp"

#include <iostream>

#ifdef _WIN32
#define main Terralistic_main
int main(int argc, char **argv);
extern "C" int SDL_main(int argc, char **argv) {
    return main(argc, argv);
}
#endif

int main(int argc, char **argv) {
    // initialize graphics and set resource path, which is a part of file loading in graphics
    
    Packet test(PacketType::DISCONNECT, 11);
    test << (std::string)"1234567890";
    std::cout << "Packet contents: \"" << test.get<std::string>() << "\"" << std::endl;
    
    gfx::init(1000, 600);
    gfx::resource_path = fileManager::getResourcePath(argv[0]);
    gfx::loadFont("pixel_font.ttf", 8);
    gfx::setWindowMinimumSize(gfx::getWindowWidth(), gfx::getWindowHeight());

    serverInit();
    fileManager::init();
    config = ConfigFile(fileManager::getDataPath() + "/config.txt");
    initProperties();
    loadTextures();
    
    gfx::runScene(new startMenu());

    gfx::quit();

    return 0;
}
