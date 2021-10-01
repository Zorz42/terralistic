#pragma once

#include "properties.hpp"
#include "events.hpp"

#define INVENTORY_SIZE 20

struct ItemStack {
    ItemStack(ItemType type, unsigned short stack) : type(type), stack(stack) {}
    ItemStack() = default;
    ItemType type = ItemType::NOTHING;
    unsigned short stack = 0;
};


class InventoryItemChangeEvent {
public:
    InventoryItemChangeEvent(char item_pos) : item_pos(item_pos) {}
    char item_pos;
};

class RecipeAvailabilityChangeEvent {};


class Inventory {
    ItemStack mouse_item;
    unsigned int item_counts[(int)ItemType::NUM_ITEMS];
    std::vector<const Recipe*> available_recipes;
    ItemStack inventory_arr[INVENTORY_SIZE];
    bool hasIngredientsForRecipe(const Recipe& recipe);
public:
    Inventory(char*& iter);
    Inventory();
    
    unsigned char selected_slot = 0;
    
    const std::vector<const Recipe*>& getAvailableRecipes();
    void updateAvailableRecipes();
    
    char addItem(ItemType id, int quantity);
    char removeItem(ItemType id, int quantity);
    void setItem(char pos, ItemStack item);
    ItemStack getItem(char pos);
    
    ItemStack getSelectedSlot();
    void swapWithMouseItem(char pos);
    
    unsigned short increaseStack(char pos, unsigned short stack);
    unsigned short decreaseStack(char pos, unsigned short stack);
    
    void serialize(std::vector<char>& serial) const;
    
    EventSender<InventoryItemChangeEvent> item_change_event;
    EventSender<RecipeAvailabilityChangeEvent> recipe_availability_change_event;
};
