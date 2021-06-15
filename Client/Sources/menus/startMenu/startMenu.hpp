//
//  startMenu.hpp
//  Terralistic
//
//  Created by Jakob Zorz on 06/07/2020.
//

#ifndef startMenu_hpp
#define startMenu_hpp

#undef main

#ifdef _WIN32
#include "graphics.hpp"
#else

#ifdef DEVELOPER_MODE
#include <Graphics_Debug/graphics.hpp>
#else
#include <Graphics/graphics.hpp>
#endif

#endif

class startMenu : public gfx::scene {
    gfx::button singleplayer_button, multiplayer_button, exit_button;
    gfx::sprite title;
    gfx::image background;
    gfx::rect back_rect;
public:
    void init() override;
    void onKeyDown(gfx::key key) override;
    void render() override;
};

#endif /* startMenu_hpp */
