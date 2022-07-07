#include "blockData.hpp"

dataDeliverer::dataDeliverer() {
    functions.push_back(&deliverDefaultData);
    names.emplace_back("default");
    functions.push_back(&deliverFurnaceData);
    names.emplace_back("furnace");
}

void FurnaceData::save(std::vector<char>& data, unsigned long& index) {
    *(int*)&data[index] = burn_time;
    index += 4;
    *(int*)&data[index] = heat;
    index += 4;
    *(int*)&data[index] = heated_items.stack;
    index += 4;
    *(int*)&data[index] = heated_items.type->id;
    index += 4;
    *(int*)&data[index] = fuel.stack;
    index += 4;
    *(int*)&data[index] = fuel.type->id;
    index += 4;
}

void FurnaceData::load(const char*& iter) {
    burn_time = *(int*)iter;
    iter += 4;
    heat = *(int*)iter;
    iter += 4;
    heated_items.stack = *(int*)iter;
    iter += 4;
    heated_items.type->id = *(int*)iter;
    iter += 4;
    fuel.stack = *(int*)iter;
    iter += 4;
    fuel.type->id = *(int*)iter;
    iter += 4;
}