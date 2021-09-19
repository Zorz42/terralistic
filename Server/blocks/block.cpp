#include <cassert>
#include "serverBlocks.hpp"

ServerBlock ServerBlocks::getBlock(unsigned short x, unsigned short y) {
    assert(y >= 0 && y < height && x >= 0 && x < width);
    return {x, y, &blocks[y * width + x], this};
}

const BlockInfo& ServerBlock::getBlockInfo() {
    return ::getBlockInfo(block_data->block_type);
}

void ServerBlock::setTypeDirectly(BlockType block_type) {
    assert((int)block_type >= 0 && block_type < BlockType::NUM_BLOCKS);
    block_data->block_type = block_type;
}

void ServerBlock::setType(BlockType block_type) {
    if(block_type != block_data->block_type) {
        ServerBlockChangeEvent event(*this, block_type);
        event.call();
        
        if(event.cancelled)
            return;
        
        blocks->removeNaturalLight(x);
        setTypeDirectly(block_type);
        blocks->setNaturalLight(x);
        
        update();
        updateNeighbors();
    }
}

void ServerBlock::updateNeighbors() {
    if(x != 0)
        blocks->getBlock(x - 1, y).update();
    if(x != blocks->getWidth() - 1)
        blocks->getBlock(x + 1, y).update();
    if(y != 0)
        blocks->getBlock(x, y - 1).update();
    if(y != blocks->getHeight() - 1)
        blocks->getBlock(x, y + 1).update();
}

void ServerBlock::setBreakProgress(unsigned short ms) {
    block_data->break_progress = ms;
    unsigned char stage = (unsigned char)((float)getBreakProgress() / (float)getBlockInfo().break_time * 9.f);
    if(stage != getBreakStage()) {
        ServerBlockBreakStageChangeEvent event(*this, stage);
        event.call();
        
        if(event.cancelled)
            return;
        
        block_data->break_stage = stage;
    }
}

void ServerBlock::update() {
    ServerBlockUpdateEvent event(*this);
    event.call();
    
    if(event.cancelled)
        return;
    
    if(getBlockInfo().only_on_floor && blocks->getBlock(x, (unsigned short)(y + 1)).getBlockInfo().transparent)
        breakBlock();
    scheduleLightUpdate();
    scheduleLiquidUpdate();
}

void ServerBlock::breakBlock() {
    ServerBlockBreakEvent event(*this);
    event.call();
    
    if(event.cancelled)
        return;
    
    setType(BlockType::AIR);
    setBreakProgress(0);
}
