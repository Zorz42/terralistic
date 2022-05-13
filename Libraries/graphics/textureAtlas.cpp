#include "graphics-internal.hpp"

void gfx::TextureAtlas::create(const std::vector<Texture*>& textures) {
    int width = 0, height = 0;
    for(auto texture : textures) {
        height += texture->getTextureHeight();
        width = std::max(width, texture->getTextureWidth());
    }
    texture.createBlankImage(width, height);
    texture.setRenderTarget();
    rects.resize(textures.size());
    int y = 0;
    for(int i = 0; i < textures.size(); i++) {
        textures[i]->render(1, 0, y);
        rects[i] = RectShape(0, y, textures[i]->getTextureWidth(), textures[i]->getTextureHeight());
        y += textures[i]->getTextureHeight();
    }
    resetRenderTarget();
}

gfx::RectShape gfx::TextureAtlas::getRect(int id) {
    return rects[id];
}