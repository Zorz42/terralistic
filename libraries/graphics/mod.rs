mod position;
pub use position::{FloatPos, FloatSize, IntPos, IntSize};

mod color;
pub use color::Color;

mod theme;
pub use theme::{
    BLACK, BLUR, BORDER_COLOR, BUTTON_PADDING, DARK_GREY, GREY, LIGHT_GREY, SHADOW_INTENSITY,
    SPACING, TEXT_INPUT_WIDTH, TRANSPARENCY, TRANSPARENT, WHITE,
};

mod transformation;

mod vertex_buffer;

mod blend_mode;
pub use blend_mode::{set_blend_mode, BlendMode};

mod shaders;

mod surface;
pub use surface::Surface;

mod blur;

mod shadow;

mod passthrough_shader;

mod renderer;
use renderer::Renderer;

mod events;
pub use events::{Event, Key};

mod rect;
pub use rect::Rect;

mod texture;
pub use texture::Texture;

mod text;
pub use text::Font;

mod container;
pub use container::{
    Container, Orientation, BOTTOM, BOTTOM_LEFT, BOTTOM_RIGHT, CENTER, LEFT, RIGHT, TOP, TOP_LEFT,
    TOP_RIGHT,
};

mod render_rect;
pub use render_rect::RenderRect;

mod button;
pub use button::Button;

mod text_input;
pub use text_input::TextInput;

mod sprite;
pub use sprite::Sprite;

mod texture_atlas;
pub use texture_atlas::TextureAtlas;

mod rect_array;
pub use rect_array::RectArray;

/*
A struct that will be passed all
around the functions that need drawing
*/
pub struct GraphicsContext {
    pub renderer: Renderer,
    pub font: Font,
}

use anyhow::Result;

/// Initializes the graphics context.
/// # Errors
/// - If the renderer fails to initialize.
/// - If the font fails to initialize.
pub fn init(
    window_width: u32,
    window_height: u32,
    window_title: &str,
    default_font_data: &[u8],
) -> Result<GraphicsContext> {
    Ok(GraphicsContext {
        renderer: Renderer::new(window_width, window_height, window_title)?,
        font: Font::new(default_font_data)?,
    })
}