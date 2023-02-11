use crate::libraries::graphics as gfx;
use crate::libraries::graphics::{FloatPos, GraphicsContext};

/**
Camera is a struct that handles the camera position.
 */

pub struct Camera {
    target_position_x: f32,
    target_position_y: f32,
    position_x: f32,
    position_y: f32,
}

impl Camera {
    pub const fn new() -> Self {
        Self {
            target_position_x: 0.0,
            target_position_y: 0.0,
            position_x: 0.0,
            position_y: 0.0,
        }
    }

    pub fn set_position(&mut self, x: f32, y: f32) {
        self.target_position_x = x;
        self.target_position_y = y;
    }

    pub const fn get_position(&self) -> FloatPos {
        FloatPos(self.position_x, self.position_y)
    }

    pub fn update_ms(&mut self, graphics: &mut GraphicsContext) {
        self.position_x += (self.target_position_x - self.position_x) * 0.005;
        self.position_y += (self.target_position_y - self.position_y) * 0.005;

        if graphics.renderer.get_key_state(gfx::Key::W) {
            self.target_position_y -= 2.0;
        }

        if graphics.renderer.get_key_state(gfx::Key::S) {
            self.target_position_y += 2.0;
        }

        if graphics.renderer.get_key_state(gfx::Key::A) {
            self.target_position_x -= 2.0;
        }

        if graphics.renderer.get_key_state(gfx::Key::D) {
            self.target_position_x += 2.0;
        }
    }

    /**
    This function gets the position of the top left corner of the screen in world coordinates.
     */
    pub fn get_top_left(&self, graphics: &mut GraphicsContext) -> (f32, f32) {
        let width = graphics.renderer.get_window_size().0;
        let height = graphics.renderer.get_window_size().1;
        (
            self.position_x - width / 2.0,
            self.position_y - height / 2.0,
        )
    }

    /**
    This function gets the position of the bottom right corner of the screen in world coordinates.
     */
    pub fn get_bottom_right(&self, graphics: &mut GraphicsContext) -> (f32, f32) {
        let width = graphics.renderer.get_window_size().0;
        let height = graphics.renderer.get_window_size().1;
        (
            self.position_x + width / 2.0,
            self.position_y + height / 2.0,
        )
    }
}