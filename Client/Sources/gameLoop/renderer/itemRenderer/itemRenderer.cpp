//
//  itemRenderer.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 20/02/2021.
//

#include "core.hpp"

#include "itemRenderer.hpp"
#include "playerHandler.hpp"

static itemRenderer::uniqueRenderItem* unique_render_items;

itemRenderer::uniqueRenderItem& itemRenderer::getUniqueRenderItem(unsigned short id) {
    return unique_render_items[id];
}

void itemRenderer::render() {
    for(itemEngine::item& i : itemEngine::items)
        gfx::render(unique_render_items[i.getItemId()].texture, short(i.x / 100 - playerHandler::view_x + gfx::getWindowWidth() / 2), short(i.y / 100 - playerHandler::view_y + gfx::getWindowHeight() / 2));
}

INIT_SCRIPT
    INIT_ASSERT(itemEngine::unique_items.size());
    unique_render_items = new itemRenderer::uniqueRenderItem[itemEngine::unique_items.size()];
    for(int i = 0; i < itemEngine::unique_items.size(); i++) {
        unique_render_items[i].texture.setTexture(itemEngine::unique_items[i].name == "nothing" ? nullptr : gfx::loadImageFile("texturePack/items/" + itemEngine::unique_items[i].name + ".png"));
        unique_render_items[i].texture.scale = 2;
        unique_render_items[i].text_texture.setTexture(gfx::renderText(itemEngine::unique_items[i].name, {255, 255, 255}));
        unique_render_items[i].text_texture.scale = 2;
    }
INIT_SCRIPT_END
