//
//  blocks.hpp
//  Terralistic
//
//  Created by Jakob Zorz on 22/06/2021.
//

#ifndef blocks_hpp
#define blocks_hpp

#include <string>
#include "serverNetworking.hpp"
#include "packets.hpp"
#include "properties.hpp"

#define BLOCK_WIDTH 16
#define MAX_LIGHT 100

class blocks;

struct blockData {
    explicit blockData(blockType block_id=blockType::AIR, liquidType liquid_id=liquidType::EMPTY) : block_id(block_id), liquid_id(liquid_id) {}

    blockType block_id;
    liquidType liquid_id = liquidType::EMPTY;
    bool light_source = false, update_light = true;
    unsigned short break_progress = 0;
    unsigned char break_stage = 0, liquid_level = 0, light_level = 0;
    unsigned int when_to_update_liquid = 1;
    flowDirection flow_direction = flowDirection::NONE;
};

class block {
    blockData* block_data = nullptr;
    unsigned short x{}, y{};
    blocks* parent_map{};
    serverNetworkingManager* manager{};

    void syncWithClient();
    void updateNeighbors();
public:
    block(unsigned short x, unsigned short y, blockData* block_data, blocks* parent_map, serverNetworkingManager* manager) : x(x), y(y), block_data(block_data), parent_map(parent_map), manager(manager) {}
    block() = default;

    void update();
    void setType(blockType block_id, bool process=true);
    void setType(liquidType liquid_id, bool process=true);
    void setType(blockType block_id, liquidType liquid_id, bool process=true);
    void setBreakProgress(unsigned short ms);
    void lightUpdate();
    void liquidUpdate();
    void setLightSource(unsigned char power);
    void removeLightSource();
    void setLightLevel(unsigned char light_level);

    inline bool refersToABlock() { return block_data != nullptr; }

    inline bool isLightSource() { return block_data->light_source; }
    inline unsigned char getLightLevel() { return block_data->light_level; }
    inline unsigned short getBreakProgress() { return block_data->break_progress; }
    inline unsigned char getBreakStage() { return block_data->break_stage; }
    inline blockType getType() { return block_data->block_id; }
    inline liquidType getLiquidType() { return block_data->liquid_id; }
    inline void scheduleLightUpdate() { block_data->update_light = true; }
    inline bool hasScheduledLightUpdate() { return block_data->update_light; }
    inline bool canUpdateLiquid() { return block_data->when_to_update_liquid != 0 && (unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() > block_data->when_to_update_liquid; }
    void setLiquidLevel(unsigned char level);
    inline unsigned char getLiquidLevel() { return block_data->liquid_level; }
    inline flowDirection getFlowDirection() { return block_data->flow_direction; }
    inline void setFlowDirection(flowDirection flow_direction) { block_data->flow_direction = flow_direction; }

    [[nodiscard]] inline unsigned short getX() const { return x; }
    [[nodiscard]] inline unsigned short getY() const { return y; }
    
    const uniqueBlock& getUniqueBlock();
    const uniqueLiquid& getUniqueLiquid();
};

class blocks {
    serverNetworkingManager* manager;
    blockData *block_arr = nullptr;
    unsigned short width{}, height{};
    
public:
    blocks(serverNetworkingManager* manager_) : manager(manager_) {}
    
    block getBlock(unsigned short x, unsigned short y);
    
    void createWorld(unsigned short width, unsigned short height);
    void setNaturalLight();
    
    [[maybe_unused]] void removeNaturalLight(unsigned short x);
    void setNaturalLight(unsigned short x);
    
    biome *biomes = nullptr;
    
    int getSpawnX() const;
    int getSpawnY();
    
    void saveTo(std::string path);
    void loadFrom(std::string path);
    
    inline unsigned short getHeight() { return height; }
    inline unsigned short getWidth() { return width; }
    
    ~blocks();
};

#endif /* blocks_hpp */