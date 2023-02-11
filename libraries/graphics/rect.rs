use super::color::Color;
use super::vertex_buffer::DrawMode;
use super::GraphicsContext;
use crate::libraries::graphics::{FloatPos, FloatSize};

/// This is a rectangle shape.
#[derive(Clone, Copy)]
pub struct Rect {
    pub pos: FloatPos,
    pub size: FloatSize,
}

impl Rect {
    /// Creates a new rectangle.
    #[must_use]
    pub const fn new(pos: FloatPos, size: FloatSize) -> Self {
        Self { pos, size }
    }

    /// Renders the rectangle on the screen.
    pub fn render(&self, graphics: &GraphicsContext, color: Color) {
        if color.a == 0 {
            return;
        }

        let mut transform = graphics.renderer.normalization_transform.clone();
        transform.translate(self.pos);
        transform.stretch((self.size.0, self.size.1));

        // Safety: We are using a valid shader.
        unsafe {
            gl::UniformMatrix3fv(
                graphics.renderer.passthrough_shader.transform_matrix,
                1,
                gl::FALSE,
                &transform.matrix[0],
            );
            gl::Uniform4f(
                graphics.renderer.passthrough_shader.global_color,
                color.r as f32 / 255.0,
                color.g as f32 / 255.0,
                color.b as f32 / 255.0,
                color.a as f32 / 255.0,
            );
            gl::Uniform1i(graphics.renderer.passthrough_shader.has_texture, 0);
        }

        graphics
            .renderer
            .passthrough_shader
            .rect_vertex_buffer
            .draw(false, DrawMode::Triangles);
    }

    /// Renders the rectangle outline on the screen.
    pub fn render_outline(&self, graphics: &GraphicsContext, color: Color) {
        if color.a == 0 {
            return;
        }

        let mut transform = graphics.renderer.normalization_transform.clone();
        transform.translate(self.pos);
        transform.stretch((self.size.0, self.size.1));

        // Safety: We are using a valid shader.
        unsafe {
            gl::UniformMatrix3fv(
                graphics.renderer.passthrough_shader.transform_matrix,
                1,
                gl::FALSE,
                &transform.matrix[0],
            );
            gl::Uniform4f(
                graphics.renderer.passthrough_shader.global_color,
                color.r as f32 / 255.0,
                color.g as f32 / 255.0,
                color.b as f32 / 255.0,
                color.a as f32 / 255.0,
            );
            gl::Uniform1i(graphics.renderer.passthrough_shader.has_texture, 0);
        }

        graphics
            .renderer
            .passthrough_shader
            .rect_outline_vertex_buffer
            .draw(false, DrawMode::Lines);
    }
}