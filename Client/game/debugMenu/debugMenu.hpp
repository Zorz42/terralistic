#pragma once
#include "clientPlayers.hpp"

class DebugMenu : public ClientModule {
    bool debug_menu_open = false;
    gfx::Sprite fps_text, coords_text;
    ClientPlayers* player_handler;
    int fps_count = 0;
    ClientBlocks* blocks;
    gfx::Timer timer;
    
    void updateFpsText();
    void updateCoordsText();
    
    void init() override;
    void update(float frame_length) override;
    void render() override;
    bool onKeyDown(gfx::Key key) override;
public:
    DebugMenu(ClientPlayers* player_handler, ClientBlocks* blocks) : player_handler(player_handler), blocks(blocks) {}
};
