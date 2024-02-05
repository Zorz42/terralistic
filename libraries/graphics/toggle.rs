use crate::libraries::graphics as gfx;
use gfx::theme::GFX_DEFAULT_BUTTON_BORDER_COLOR;

/// A Toggle is a rectangle with 2 states.
/// It can be clicked to toggle between them.
pub struct Toggle {
    pub pos: gfx::FloatPos,
    pub size: gfx::FloatSize,
    pub orientation: gfx::Orientation,
    pub padding: f32,
    pub scale: f32,
    pub left_color: gfx::Color,
    pub right_color: gfx::Color,
    pub border_color: gfx::Color,
    pub button_color: gfx::Color,
    pub toggled: bool,
    pub hovered: bool,
    toggle_progress: f32,
    hover_progress: f32,
    timer: std::time::Instant,
    timer_counter: u32,
}

impl Toggle {
    /// Creates a new button.
    #[must_use]
    pub fn new() -> Self {
        Self {
            pos: gfx::FloatPos(0.0, 0.0),
            size: gfx::FloatSize(82.0, 50.0),
            orientation: gfx::TOP_LEFT,
            padding: 5.0,
            scale: 1.0,
            left_color: gfx::Color::new(210, 0, 0, 255),
            right_color: gfx::Color::new(0, 210, 0, 255),
            border_color: GFX_DEFAULT_BUTTON_BORDER_COLOR,
            button_color: gfx::WHITE,
            toggled: false,
            hovered: false,
            toggle_progress: 0.0,
            hover_progress: 0.0,
            timer: std::time::Instant::now(),
            timer_counter: 0,
        }
    }

    /// returns the size.
    #[must_use]
    pub const fn get_size(&self) -> gfx::FloatSize {
        self.size
    }

    /// Generates the container for the toggle.
    #[must_use]
    pub fn get_container(&self, graphics: &gfx::GraphicsContext, parent_container: Option<&gfx::Container>) -> gfx::Container {
        gfx::Container::new(graphics, self.pos, self.get_size(), self.orientation, parent_container)
    }

    /// Checks if the toggle is hovered with a mouse.
    #[must_use]
    pub fn is_hovered(&self, graphics: &gfx::GraphicsContext, parent_container: Option<&gfx::Container>) -> bool {
        let container = self.get_container(graphics, parent_container);
        let rect = container.get_absolute_rect();
        let mouse_pos = graphics.get_mouse_pos();
        rect.contains(mouse_pos)
    }

    pub fn on_event(&mut self, event: &gfx::Event, graphics: &gfx::GraphicsContext, parent_container: Option<&gfx::Container>) -> bool {
        if let gfx::Event::KeyRelease(gfx::Key::MouseLeft, ..) = event {
            if self.is_hovered(graphics, parent_container) {
                self.toggled = !self.toggled;
                return true;
            }
        }
        false
    }

    /// Renders the toggle.
    pub fn render(&mut self, graphics: &gfx::GraphicsContext, parent_container: Option<&gfx::Container>) {
        self.hovered = self.is_hovered(graphics, parent_container);
        let mut container = self.get_container(graphics, parent_container);
        let float_size_padding = gfx::FloatSize(self.padding, self.padding);
        let toggle_target = if self.toggled { 1.0 } else { 0.0 };
        let hover_target = if self.is_hovered(graphics, parent_container) { 1.0 } else { 0.0 };

        while self.timer_counter < self.timer.elapsed().as_millis() as u32 {
            self.toggle_progress += (toggle_target - self.toggle_progress) / 40.0;
            if (toggle_target - self.toggle_progress).abs() <= 0.01 {
                self.toggle_progress = toggle_target;
            }
            self.hover_progress += (hover_target - self.hover_progress) / 40.0;
            if (hover_target - self.hover_progress).abs() <= 0.01 {
                self.hover_progress = hover_target;
            }
            self.timer_counter += 1;
        }

        let fill_color = gfx::interpolate_colors(self.left_color, self.right_color, self.toggle_progress);
        let fill_color = gfx::interpolate_colors(
            gfx::Color {
                r: (fill_color.r as f32 * 0.8) as u8,
                g: (fill_color.g as f32 * 0.8) as u8,
                b: (fill_color.b as f32 * 0.8) as u8,
                a: 255,
            },
            fill_color,
            self.hover_progress,
        );

        container.rect.render(graphics, self.border_color);
        container.rect.size = container.rect.size - float_size_padding - float_size_padding;
        container.update(graphics, parent_container);
        container.get_absolute_rect().render(graphics, fill_color);
        let size = gfx::FloatSize(container.rect.size.1 - 2.0 * self.padding, container.rect.size.1 - 2.0 * self.padding);

        let position = gfx::FloatPos(
            self.padding * (1.0 - self.toggle_progress) + (container.rect.size.0 - size.0 - self.padding) * self.toggle_progress,
            0.0,
        );

        let button = gfx::container::Container::new(graphics, position, size, gfx::LEFT, Some(&container));
        button.get_absolute_rect().render(graphics, self.button_color);
    }
}
