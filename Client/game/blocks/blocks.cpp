#include "clientBlocks.hpp"
#include "platform_folders.h"

void ClientBlocks::create(unsigned short map_width, unsigned short map_height, const std::vector<char>& map_data) {
    width = map_width;
    height = map_height;
    blocks = new ClientMapBlock[width * height];
    
    int* map_data_iter = (int*)&map_data[0];
    ClientMapBlock* map_iter = blocks;
    
    for(int y = 0; y < height; y++)
        for(int x = 0; x < width; x++) {
            int data = *map_data_iter++;
            
            *map_iter++ = ClientMapBlock((BlockType)(data & 0xff), (LiquidType)(data >> 8 & 0xff), data >> 16 & 0xff, data >> 24 & 0xff);
        }
    
    view_x = map_width * BLOCK_WIDTH;
}

void ClientBlocks::onEvent(ClientPacketEvent &event) {
    switch(event.packet_type) {
        case PacketType::BLOCK_CHANGE: {
            unsigned short x, y;
            unsigned char block_type;
            event.packet >> x >> y >> block_type;
            
            ClientBlock curr_block = getBlock(x, y);
            curr_block.setType((BlockType)block_type, curr_block.getLiquidType());
            break;
        }
        case PacketType::LIGHT_CHANGE: {
            unsigned short x, y;
            unsigned char light_level;
            event.packet >> x >> y >> light_level;
            
            getBlock(x, y).setLightLevel(light_level);
            break;
        }
        case PacketType::LIQUID_CHANGE: {
            unsigned short x, y;
            unsigned char liquid_type, liquid_level;
            event.packet >> x >> y >> liquid_type >> liquid_level;
            
            ClientBlock curr_block = getBlock(x, y);
            curr_block.setType(curr_block.getBlockType(), (LiquidType)liquid_type);
            curr_block.setLiquidLevel(liquid_level);
            break;
        }
        case PacketType::BLOCK_PROGRESS_CHANGE: {
            unsigned char stage;
            unsigned short x, y;
            event.packet >> x >> y >> stage;
            getBlock(x, y).setBreakStage(stage);
            break;
        }
        default:;
    }
}

ClientBlocks::~ClientBlocks() {
    delete[] blocks;
}
