#include <fstream>
#include <vector>
#include <string>
#include "worldGenerator.hpp"
#include "biomes.hpp"
#include "cmath"

int WorldGenerator::generateWorld(unsigned short world_width, unsigned short world_height, unsigned int seed) {
    siv::PerlinNoise noise(seed);
    surface_height = new unsigned short[world_width];
    blocks->create(world_width, world_height);
    liquids->create();
    biomes->create();

    loadAssets();
    if(seed == 1000) {
        generateStructureWorld();
    } else {
        generating_total = blocks->getWidth() * 2;
        loadBiomes();
        generateDeafultWorld(noise);
    }
    delete[] surface_height;
    return 0;
}

double turbulence(double x, double y, double size, siv::PerlinNoise& noise) {
    double value = 0, initialSize = size;

    while(size >= 8) {
        value += noise.noise2D(x / size, y / size) * size;
        size /= 2.0;
    }

    return value / initialSize;
}

int WorldGenerator::heatGeneratorInt(unsigned int x, siv::PerlinNoise &noise) {
    int biome_heat = (noise.noise1D((float)x / 2000.0 + 0.125) + 1.0) * 1.5;
    return biome_heat == 3 ? 2 : biome_heat;
}

int WorldGenerator::heightGeneratorInt(unsigned int x, siv::PerlinNoise& noise) {
    if (x < 100 || x > blocks->getWidth() - 100)
        return 0;
    else if (x < 150 || x > blocks->getWidth() - 150)
        return 1;
    else {
        int biome_heat = (noise.noise1D((float)x / 600.0 + 0.001) + 1) * 1.5 + 1;
        return std::min(biome_heat, 3);
    }
}

void WorldGenerator::generateBiomes(unsigned int x, siv::PerlinNoise& noise) {
    int biome_heat = heatGeneratorInt(x, noise);
    int biome_height = heightGeneratorInt(x, noise);
    biomes->biomes[x] = (BiomeType)((biome_heat * 4) + biome_height);
}

void WorldGenerator::terrainGenerator(int x, siv::PerlinNoise& noise) {
    generateSurface(x, noise);
    for(auto &checking_structure : loaded_biomes[(int)biomes->biomes[x]].structure_chances){
        if((noise.noise2D((float)x + 0.5, (float)surface_height[x] + 0.5) + 1) * checking_structure.chance_on_each_block <= 2 && x > checking_structure.x_of_last_instance + checking_structure.least_distance_between_instances) {
            structurePositions.emplace_back(structurePosition(checking_structure.structure_name +
                                         std::to_string((int)((noise.noise2D((float)x - 0.5, (float)surface_height[x] - 0.5) + 1) / 2 * checking_structure.unique_structures_of_type)),
                                         x, surface_height[x] - 1));
            checking_structure.x_of_last_instance = x;
        }
    }
}

void WorldGenerator::calculateHeight(siv::PerlinNoise& noise) {
    int biome_blend = 20;
    float divide_at_end;
    unsigned short *no_blend_height = new unsigned short[blocks->getWidth()];
    for(int current_slice = 0; current_slice < blocks->getWidth(); current_slice++) {
        no_blend_height[current_slice] = loaded_biomes[(int) biomes->biomes[current_slice]].surface_height;
    }

    for(int current_slice = 0; current_slice < blocks->getWidth(); current_slice++) {
        divide_at_end = 0;
        surface_height[current_slice] = 0;
        unsigned short variation = 0;
        for (int i = std::max(0, current_slice - biome_blend); i < std::min(blocks->getWidth() - 1, current_slice + biome_blend); i++) {
            surface_height[current_slice] += no_blend_height[i] * (1 - (float)std::abs(current_slice - i) / biome_blend);
            variation += loaded_biomes[(int) biomes->biomes[i]].surface_height_variation * (1 - (float)std::abs(current_slice - i) / biome_blend);
            divide_at_end += (1 - (float)std::abs(current_slice - i) / biome_blend);
        }
        surface_height[current_slice] /= divide_at_end;
        variation /= divide_at_end;
        surface_height[current_slice] += turbulence(current_slice + 0.003, 0, 64, noise) * variation;
    }
    delete[] no_blend_height;
}

void WorldGenerator::generateSurface(unsigned int x, siv::PerlinNoise &noise) {
    int generate_from = std::max(blocks->getHeight() / 3 * 2, (int)surface_height[x]);
    for(unsigned int y = generate_from; y > 0; y--){
        unsigned int changed_x = std::max(std::min((int)(x + noise.noise2D(x + 0.5, y + 0.5) * 8), (int)blocks->getWidth()), 0);
        switch ((int)biomes->biomes[changed_x]) {
            case 0:
                generateBlockIcyOcean(x, y, noise);
                break;
            case 1:
                generateBlockSnowyPlains(x, y, noise);
                break;
            case 2:
                generateBlockSnowyHills(x, y, noise);
                break;
            case 3:
                generateBlockSnowyMountains(x, y, noise);
                break;
            case 4:
                generateBlockOcean(x, y, noise);
                break;
            case 5:
                generateBlockPlains(x, y, noise);
                break;
            case 6:
                generateBlockHills(x, y, noise);
                break;
            case 7:
                generateBlockMountains(x, y, noise);
                break;
            case 8:
                generateBlockWarmOcean(x, y, noise);
                break;
            case 9:
                generateBlockDesert(x, y, noise);
                break;
            case 10:
                generateBlockSavanaHills(x, y, noise);
                break;
            case 11:
                generateBlockSavanaMountains(x, y, noise);
                break;
        }
    }
}

void WorldGenerator::generateBlockIcyOcean(unsigned int x, unsigned int y, siv::PerlinNoise &noise) {
    if(y <= surface_height[x])
        blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::STONE_BLOCK);
    else if(y > blocks->getHeight() / 3 * 2 - noise.noise1D((float)x / 4 + 0.125) - 2)
        blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::ICE);
    else{
        liquids->setLiquidTypeSilently(x, blocks->getHeight() - y, LiquidType::WATER);
        liquids->setLiquidLevelSilently(x, blocks->getHeight() - y, 255);
    }
}

void WorldGenerator::generateBlockSnowyPlains(unsigned int x, unsigned int y, siv::PerlinNoise &noise) {
    if(y <= surface_height[x]){
        if(y < surface_height[x] + (noise.noise1D((float)x / 3 + 0.15) * 1.8) - 20)
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::STONE_BLOCK);
        else if(y < surface_height[x] + (noise.noise1D((float)x / 3 + 0.15) * 0.5) - 5)
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::DIRT);
        else if(y == surface_height[x] + ceil(noise.noise1D((float)x / 3 + 0.15) * 0.5) - 5)
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::SNOWY_GRASS_BLOCK);
        else
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::SNOW_BLOCK);
    }else
        blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::ICE);
}

void WorldGenerator::generateBlockSnowyHills(unsigned int x, unsigned int y, siv::PerlinNoise &noise) {
    if(y <= surface_height[x]){
        if(y < surface_height[x] + (noise.noise1D((float)x / 3 + 0.15) * 1.8) - 20)
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::STONE_BLOCK);
        else if(y < surface_height[x] + (noise.noise1D((float)x / 3 + 0.15) * 0.5) - 5)
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::DIRT);
        else if(y == surface_height[x] + ceil(noise.noise1D((float)x / 3 + 0.15) * 0.5) - 5)
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::SNOWY_GRASS_BLOCK);
        else
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::SNOW_BLOCK);
    }else
        blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::ICE);
}

void WorldGenerator::generateBlockSnowyMountains(unsigned int x, unsigned int y, siv::PerlinNoise &noise) {
    if(y <= surface_height[x]){
        if(y < surface_height[x] + noise.noise1D((float)x / 3 + 0.15) * 0.5 - 5)
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::STONE_BLOCK);
        else
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::SNOW_BLOCK);
    }else
        blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::ICE);
}

void WorldGenerator::generateBlockOcean(unsigned int x, unsigned int y, siv::PerlinNoise &noise) {
    if(y <= surface_height[x])
        blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::STONE_BLOCK);
    else{
        liquids->setLiquidTypeSilently(x, blocks->getHeight() - y, LiquidType::WATER);
        liquids->setLiquidLevelSilently(x, blocks->getHeight() - y, 255);
    }
}

void WorldGenerator::generateBlockPlains(unsigned int x, unsigned int y, siv::PerlinNoise &noise) {
    if(y <= surface_height[x]){
        if(y < surface_height[x] + (noise.noise1D((float)x / 3 + 0.15) * 1.8) - 15)
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::STONE_BLOCK);
        else if(y < surface_height[x])
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::DIRT);
        else
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::GRASS_BLOCK);
    }else
    if(y < blocks->getHeight() / 3 * 2){
        liquids->setLiquidTypeSilently(x, blocks->getHeight() - y, LiquidType::WATER);
        liquids->setLiquidLevelSilently(x, blocks->getHeight() - y, 255);
    }
}

void WorldGenerator::generateBlockHills(unsigned int x, unsigned int y, siv::PerlinNoise &noise) {
    if(y <= surface_height[x]){
        if(y < surface_height[x] + (noise.noise1D((float)x / 3 + 0.15) * 1.8) - 15)
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::STONE_BLOCK);
        else if(y < surface_height[x])
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::DIRT);
        else
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::GRASS_BLOCK);
    }else
    if(y < blocks->getHeight() / 3 * 2){
        liquids->setLiquidTypeSilently(x, blocks->getHeight() - y, LiquidType::WATER);
        liquids->setLiquidLevelSilently(x, blocks->getHeight() - y, 255);
    }
}

void WorldGenerator::generateBlockMountains(unsigned int x, unsigned int y, siv::PerlinNoise &noise) {
    if(y <= surface_height[x])
        blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::STONE_BLOCK);
    else {
        liquids->setLiquidTypeSilently(x, blocks->getHeight() - y, LiquidType::WATER);
        liquids->setLiquidLevelSilently(x, blocks->getHeight() - y, 255);
    }
}

void WorldGenerator::generateBlockWarmOcean(unsigned int x, unsigned int y, siv::PerlinNoise &noise) {
    if(y <= surface_height[x])
        blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::STONE_BLOCK);
    else{
        liquids->setLiquidTypeSilently(x, blocks->getHeight() - y, LiquidType::WATER);
        liquids->setLiquidLevelSilently(x, blocks->getHeight() - y, 255);
    }
}

void WorldGenerator::generateBlockDesert(unsigned int x, unsigned int y, siv::PerlinNoise &noise) {
    if(y <= surface_height[x]){
        if(y < surface_height[x] + (noise.noise1D((float)x / 3 + 0.15) * 1.8) - 15)
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::STONE_BLOCK);
        else
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::SAND);
    }else{
        liquids->setLiquidTypeSilently(x, blocks->getHeight() - y, LiquidType::WATER);
        liquids->setLiquidLevelSilently(x, blocks->getHeight() - y, 255);
    }
}

void WorldGenerator::generateBlockSavanaHills(unsigned int x, unsigned int y, siv::PerlinNoise &noise) {
    if(y <= surface_height[x]){
        if(y < surface_height[x] + (noise.noise1D((float)x / 3 + 0.15) * 1.8) - 15)
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::STONE_BLOCK);
        else if(y < surface_height[x])
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::DIRT);
        else
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::GRASS_BLOCK);
    }else
    if(y < blocks->getHeight() / 3 * 2){
        liquids->setLiquidTypeSilently(x, blocks->getHeight() - y, LiquidType::WATER);
        liquids->setLiquidLevelSilently(x, blocks->getHeight() - y, 255);
    }
}

void WorldGenerator::generateBlockSavanaMountains(unsigned int x, unsigned int y, siv::PerlinNoise &noise) {
    if(y <= surface_height[x]){
        if(y < surface_height[x] + (noise.noise1D((float)x / 3 + 0.15) * 1.8) - 25)
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::STONE_BLOCK);
        else if(y < surface_height[x])
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::DIRT);
        else
            blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::GRASS_BLOCK);
    }else
    if(y < blocks->getHeight() / 3 * 2){
        liquids->setLiquidTypeSilently(x, blocks->getHeight() - y, LiquidType::WATER);
        liquids->setLiquidLevelSilently(x, blocks->getHeight() - y, 255);
    }
}

void WorldGenerator::generateCaves(siv::PerlinNoise &noise) {
    for(unsigned int x = 0; x < blocks->getWidth(); x++) {
        for (unsigned int y = surface_height[x]; y > 0; --y) {
            float value = turbulence((double)x / 2, (double)y, 64, noise) * std::min(((float)blocks->getHeight() / 3 * 2 - y) / 300, (float)1);
            if (value > 0.3) {
                blocks->setBlockTypeSilently(x, blocks->getHeight() - y, BlockType::AIR);
                if (y == surface_height[x])
                    surface_height--;
            }else {
                value = turbulence((double) x / 4 + blocks->getWidth() * 3, (double)y / 2 + blocks->getHeight() * 3, 64, noise);
                int multiply = std::min((float)1, std::max((float)0, (float)(y - blocks->getHeight() / 3 * 2 + 10) / 100));
                if (value > -0.05 * multiply * multiply && value < 0.05 * multiply * multiply) {
                    blocks->setBlockTypeSilently(x, blocks->getHeight() - y - 1, BlockType::AIR);
                    if (y == surface_height[x])
                        surface_height--;
                }
            }
        }
        generating_current++;
    }
}

void WorldGenerator::generateCaveLakes() {
    for(int i = 0; i < 10000; i++){
        int this_random = (int)random();
        unsigned short x = this_random % blocks->getWidth();
        unsigned short y = blocks->getHeight() - (this_random % blocks->getHeight() / 3 * 2) - 1;
        if(blocks->getBlockType(x, y) == BlockType::AIR){
            while(y < blocks->getHeight() - 1 && blocks->getBlockType(x, y + 1) == BlockType::AIR)
                y++;
            generateLakeRecursively(x, y);
        }else
            continue;
    }
}

void WorldGenerator::generateLakeRecursively(int x, int y) {
    liquids->setLiquidTypeSilently(x, y, LiquidType::WATER);
    liquids->setLiquidLevelSilently(x, y, 255);
    if(y != blocks->getHeight() - 1 && blocks->getBlockType(x, y + 1) == BlockType::AIR && liquids->getLiquidType(x, y + 1) == LiquidType::EMPTY)
        generateLakeRecursively(x, y + 1);
    if(x != 0 && blocks->getBlockType(x - 1, y) == BlockType::AIR && liquids->getLiquidType(x - 1, y) == LiquidType::EMPTY)
        generateLakeRecursively(x - 1, y);
    if(x != blocks->getWidth() - 1 && blocks->getBlockType(x + 1, y) == BlockType::AIR && liquids->getLiquidType(x + 1, y) == LiquidType::EMPTY)
        generateLakeRecursively(x + 1, y);
}

void WorldGenerator::loadAssets() {
    std::ifstream structureFile;
    structureFile.open(resource_path + "/Structures.asset", std::ios::in);

    structureFile.seekg(0, std::ios::end);
    int size = (int)structureFile.tellg();
    char* assetData = new char[size];
    structureFile.seekg(0, std::ios::beg);
    structureFile.read(assetData, size);

    int counter = 0;
    int previousEnd = 0;
    while (counter < size - 1) {
        std::string name;
        int nameSize = (unsigned char)assetData[counter];
        counter++;
        while (counter - previousEnd <= nameSize) {
            name += assetData[counter];
            counter++;
        }
        int x_size = (unsigned char)assetData[counter];
        counter++;
        int y_size = (unsigned char)assetData[counter];
        counter++;
        int y_offset = (unsigned char)assetData[counter];
        counter++;
        auto *blocks_ = new BlockType[x_size * y_size];
        for (int i = 0; i < x_size * y_size; i++) {
            blocks_[i] = (BlockType)assetData[counter];
            counter++;
        }
        structures.emplace_back(name, x_size, y_size, y_offset, blocks_);
        previousEnd = counter;
    }

    structureFile.close();
    delete[] assetData;
}

void WorldGenerator::generateStructure(const std::string& name, int x, int y) {
    for (auto & structure : structures) {
        if (name == structure.name) {
            x -= structure.x_size / 2;
            y += structure.y_offset;
            for(int j = 0; j < structure.y_size * structure.x_size; j++)
                if(structure.blocks[j] != BlockType::NOTHING)
                    blocks->setBlockTypeSilently((unsigned short)(x + j % structure.x_size), (unsigned short)(blocks->getHeight() - y + (j - j % structure.x_size) / structure.x_size) - structure.y_size - 1, structure.blocks[j]);
            break;
        }
    }

}

void WorldGenerator::generateStructureWorld() {
    generateFlatTerrain();
    generateStructuresForStrWorld();
}

void WorldGenerator::generateFlatTerrain() {
    for (int x = 0; x < blocks->getWidth(); x++) {
        biomes->biomes[x] = BiomeType::PLAINS;
    }
    for (int x = 0; x < blocks->getWidth(); x++) {
        for (int y = 0; y < blocks->getHeight(); y++) {
            if (y <= 324) {//generates surface
                blocks->setBlockTypeSilently((unsigned short)x, blocks->getHeight() - y - 1, BlockType::DIRT);
            }else if(y == 325)
                blocks->setBlockTypeSilently((unsigned short)x, blocks->getHeight() - y - 1, BlockType::GRASS_BLOCK);
        }
    }
}

void WorldGenerator::generateStructuresForStrWorld() {
    int x = 0;
    while(x < blocks->getWidth()){
        for (auto & structure : structures) {
            if(structure.y_size + x >= blocks->getWidth())
                return;
            for(int j = 0; j < structure.y_size * structure.x_size; j++)
                if(structure.blocks[j] != BlockType::NOTHING)
                    blocks->setBlockTypeSilently((unsigned short)(x + j % structure.x_size), (unsigned short)(blocks->getHeight() - 326 + (j - j % structure.x_size) / structure.x_size) - structure.y_size, structure.blocks[j]);
            x += structure.x_size + 1;
        }
    }
}

void WorldGenerator::generateDeafultWorld(siv::PerlinNoise& noise) {
    for (int x = 0; x < blocks->getWidth(); x++) {
        generateBiomes(x, noise);
    }
    calculateHeight(noise);
    for (int x = 0; x < blocks->getWidth(); x++) {
        terrainGenerator(x, noise);
        generating_current++;
    }
    generateCaves(noise);
    generateCaveLakes();
    for (const structurePosition& i : structurePositions) {
        generateStructure(i.name, i.x, i.y);
    }
}

void WorldGenerator::loadBiomes() {
    loaded_biomes.push_back(Biome(BiomeType::ICY_SEAS, blocks->getHeight() / 3 * 2 - 50, 10,
                                  {}));
    loaded_biomes.push_back(Biome(BiomeType::SNOWY_TUNDRA, blocks->getHeight() / 3 * 2 + 20, 4,
                                   {}));
    loaded_biomes.push_back(Biome(BiomeType::COLD_HILLS, blocks->getHeight() / 3 * 2 + 29, 15,
                                   {}));
    loaded_biomes.push_back(Biome(BiomeType::SNOWY_MOUNTAINS, blocks->getHeight() / 3 * 2 + 70, 37,
                                  {}));
    loaded_biomes.push_back(Biome(BiomeType::SEA, blocks->getHeight() / 3 * 2 - 50, 10,
                                  {}));
    loaded_biomes.push_back(Biome(BiomeType::PLAINS, blocks->getHeight() / 3 * 2 + 22, 4,
                                  {StructureChance("tree_", 5, 20, 2)
                                  }));
    loaded_biomes.push_back(Biome(BiomeType::FOREST, blocks->getHeight() / 3 * 2 + 23, 10,
                                  {StructureChance("tree_", 3, 6, 2)}));
    loaded_biomes.push_back(Biome(BiomeType::MOUNTAINS, blocks->getHeight() / 3 * 2 + 64, 33,
                                  {}));
    loaded_biomes.push_back(Biome(BiomeType::WARM_OCEAN, blocks->getHeight() / 3 * 2 - 50, 10,
                                  {}));
    loaded_biomes.push_back(Biome(BiomeType::DESERT, blocks->getHeight() / 6 * 4 + 22, 4,
                                  {}));
    loaded_biomes.push_back(Biome(BiomeType::SAVANA, blocks->getHeight() / 3 * 2 + 26, 10,
                                  {}));
    loaded_biomes.push_back(Biome(BiomeType::SAVANA_MOUNTAINS, blocks->getHeight() / 3 * 2 + 50, 25,
                                  {}));
}
