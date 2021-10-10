#ifndef worldGenerator_hpp
#define worldGenerator_hpp

#include <string>
#include <utility>
#include "blocks.hpp"
#include "SimplexNoise.h"
#include "biomes.hpp"
#include "liquids.hpp"


struct structure {
    std::string name;
    int x_size, y_size, y_offset;
    BlockType* blocks;
    structure(std::string cname, int x, int y, int offset, BlockType* cBlocks) : name(std::move(cname)), x_size(x), y_size(y), y_offset(offset), blocks(cBlocks) {}
};

struct structurePosition {
    std::string name;
    int x, y;
    structurePosition(std::string cname, int cx, int cy) : name(std::move(cname)), x(cx), y(cy) {}
};

class WorldGenerator {
    Blocks* blocks;
    Biomes* biomes;
    Liquids* liquids;

    std::vector<structure> structures;
    std::vector<structurePosition> structurePositions;

    void biomeGeneratorSwitch(unsigned int x, SimplexNoise& noise);
    int calculateHeight(int x, SimplexNoise& noise);
    //static int heightGeneratorInt(unsigned int x, SimplexNoise& noise);
    void terrainGenerator(int x, SimplexNoise& noise);
    void generateSurface(int x, int surface_height, SimplexNoise& noise);
    void generateStructureWorld();
    void generateFlatTerrain();
    void generateStructuresForStrWorld();
    void updateBlocks();
    void generateDeafultWorld(SimplexNoise& noise);
    int heightGeneratorInt(unsigned int x, SimplexNoise& noise);
    static int heatGeneratorInt(unsigned int x, SimplexNoise& noise);
    void loadBiomes();

    //void generateOakTree(int x, int y);
    //void generateAccaciaTree(int x, int y);
    void generateStructure(const std::string& name, int x, int y);

    void loadAssets();

    std::string resource_path;

    unsigned int generating_current = 0, generating_total = 1;

public:
    WorldGenerator(Blocks* blocks, Liquids* liquids, Biomes* biomes, std::string resource_path) : blocks(blocks), liquids(liquids), biomes(biomes), resource_path(std::move(resource_path)) {}

    unsigned int getGeneratingCurrent() const { return generating_current; }
    unsigned int getGeneratingTotal() const { return generating_total; }

    int generateWorld(unsigned short world_width, unsigned short world_height, unsigned int seed);
};

#endif
