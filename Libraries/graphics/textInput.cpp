#include "graphics-internal.hpp"
void gfx::TextInput::setText(const std::string& text_) {
    text = text_;
    renderText(text, text_color);
}

unsigned short gfx::TextInput::getWidth() const {
    return (width + 2 * margin) * scale;
}

gfx::TextInput::TextInput() {
    margin = 3;
    back_rect.shadow_intensity = GFX_DEFAULT_TEXT_BOX_SHADOW_INTENSITY;
}

void gfx::TextInput::setBlurIntensity(float blur_intensity) {
    back_rect.blur_intensity = blur_intensity;
}

void gfx::TextInput::render(unsigned short mouse_x, unsigned short mouse_y) {
    RectShape rect = getTranslatedRect();
    back_rect.setX(rect.x);
    back_rect.setY(rect.y);
    back_rect.setWidth(rect.w);
    back_rect.setHeight(rect.h);
    back_rect.fill_color = isHovered(mouse_x, mouse_y) ? hover_color : def_color;
    back_rect.render();
    
    rect.x += margin * scale;
    rect.y += margin * scale;
    rect.w = getTextureWidth() * scale;
    rect.h -= margin * 2 * scale;
    short x;
    unsigned short w;
    if (rect.w > width * scale) {
        x = rect.w / scale - width;
        w = width;
    }
    else {
        x = 0;
        w = rect.w / scale;
    }
    
    Image::render(scale, rect.x, rect.y, {x, 0, w, (unsigned short)(rect.h / scale)});
    if (active)
        RectShape(rect.x + (rect.w > width * scale ? width * scale : rect.w ), rect.y, scale, rect.h).render(text_color);
        
}

void gfx::TextInput::setBorderColor(Color color) {
    back_rect.border_color = color;
}