#include "clientLiquids.hpp"

void ClientLiquids::onEvent(ClientPacketEvent &event) {
    switch(event.packet_type) {
        case PacketType::LIQUID: {
            int x, y;
            unsigned char liquid_type, liquid_level;
            event.packet >> x >> y >> liquid_type >> liquid_level;
            
            setLiquidType(x, y, (LiquidType)liquid_type);
            setLiquidLevel(x, y, liquid_level);
            
            break;
        }
        default:;
    }
}

void ClientLiquids::onEvent(WelcomePacketEvent& event) {
    if(event.packet_type == WelcomePacketType::LIQUIDS) {
        std::vector<char> data = networking->getData();
        loadFromSerial(&data[0]);
    }
}

void ClientLiquids::init() {
    networking->packet_event.addListener(this);
    networking->welcome_packet_event.addListener(this);
}

void ClientLiquids::stop() {
    networking->packet_event.removeListener(this);
    networking->welcome_packet_event.removeListener(this);
}

void ClientLiquids::render() {
    if((blocks->getViewEndX() - blocks->getViewBeginX()) * (blocks->getViewEndY() - blocks->getViewBeginY()) > most_blocks_on_screen) {
        most_blocks_on_screen = (blocks->getViewEndX() -blocks->getViewBeginX()) * (blocks->getViewEndY() - blocks->getViewBeginY());
        liquid_rects.resize(most_blocks_on_screen);
    }
    
    int liquid_index = 0;
    for(unsigned short x = blocks->getViewBeginX(); x < blocks->getViewEndX(); x++)
        for(unsigned short y = blocks->getViewBeginY(); y < blocks->getViewEndY(); y++) {
            int block_x = x * BLOCK_WIDTH * 2 - blocks->view_x + gfx::getWindowWidth() / 2, block_y = y * BLOCK_WIDTH * 2 - blocks->view_y + gfx::getWindowHeight() / 2;

            if(getLiquidType(x, y) != LiquidType::EMPTY) {
                int texture_y = resource_pack->getTextureRectangle(getLiquidType(x, y)).y * 2;
                
                liquid_rects.setTextureCoords(liquid_index, {0, (short)texture_y, BLOCK_WIDTH, BLOCK_WIDTH});
                
                int level = ((int)getLiquidLevel(x, y) + 1) / 8;
                liquid_rects.setRect(liquid_index, {(short)block_x, short(block_y + BLOCK_WIDTH * 2 - level), (short)BLOCK_WIDTH * 2, (unsigned short)level});
                liquid_index++;
            }
        }
    
    if(liquid_index)
        liquid_rects.render(liquid_index, &resource_pack->getLiquidTexture());
}