#include <cmath>
#include "rect.hpp"
#include "glfwAbstraction.hpp"
#include "exception.hpp"
#include "blur.hpp"
#include "shadow.hpp"

float approach(float object, int target, int smooth_factor) {
    if(std::abs(object - target) < 1 || object == target)
        return target;
    return object + (target - object) / smooth_factor;
}

void gfx::Rect::render() {
    if(first_time) {
        first_time = false;
        jumpToTarget();
    }
    
    RectShape target_rect = getTranslatedRect();
    
    while(ms_counter < approach_timer.getTimeElapsed()) {
        ms_counter++;
        
        render_x = approach(render_x, target_rect.x, smooth_factor * 10);
        render_y = approach(render_y, target_rect.y, smooth_factor * 10);
        render_w = approach(render_w, target_rect.w, smooth_factor * 10);
        render_h = approach(render_h, target_rect.h, smooth_factor * 10);
    }
    
    RectShape rect = {(int)render_x, (int)render_y, (int)render_w, (int)render_h};

    if(blur_radius && blur_enabled)
        gfx::blurRectangle(rect, blur_radius, window_texture, window_texture_back, getWindowWidth(), getWindowHeight(), normalization_transform);
    
    if(shadow_intensity)
        gfx::drawShadow(rect, shadow_intensity);
    
    rect.render(fill_color);
    rect.renderOutline(border_color);
}

void gfx::Rect::jumpToTarget() {
    render_y = y;
    render_x = x;
    render_w = w;
    render_h = h;
}
