use crate::menus::background_rect::BackgroundRect;
use graphics::GraphicsContext;
use graphics as gfx;
use crate::game::private_world::run_private_world;


/**this function runs the world creation menu.*/
pub fn run_world_creation(graphics: &mut GraphicsContext, menu_back: &mut dyn BackgroundRect) {
    let mut title = gfx::Sprite::new();
    title.scale = 3.0;
    title.texture = gfx::Texture::load_from_surface(&graphics.font.create_text_surface(String::from("Create a new world:")));
    title.y = gfx::SPACING;
    title.orientation = gfx::TOP;

    let mut buttons_container = gfx::Container::new(0, 0, 0, 0,  gfx::BOTTOM);

    let mut back_button = gfx::Button::new();
    back_button.scale = 3.0;
    back_button.texture = gfx::Texture::load_from_surface(&graphics.font.create_text_surface(String::from("Back")));

    let mut create_button = gfx::Button::new();
    create_button.scale = 3.0;
    create_button.texture = gfx::Texture::load_from_surface(&graphics.font.create_text_surface(String::from("Create world")));
    create_button.x = back_button.get_width() + gfx::SPACING;

    buttons_container.rect.w = back_button.get_width() + create_button.get_width() + gfx::SPACING;
    buttons_container.rect.h = back_button.get_height();
    buttons_container.rect.y = -gfx::SPACING;

    let mut world_name_input = gfx::TextInput::new(graphics);
    world_name_input.scale = 3.0;
    world_name_input.set_hint(graphics, String::from("World name"));
    world_name_input.orientation = gfx::CENTER;
    world_name_input.selected = true;

    world_name_input.text_processing = Some(Box::new(|text: char| {
        // this closure only accepts letters, numbers and _ symbol
        if text.is_alphanumeric() || text == '_' {
            return Some(text);
        }
        None
    }));

    //this is where the menu is drawn
    'render_loop: while graphics.renderer.is_window_open() {
        while let Some(event) = graphics.renderer.get_event() {//sorts out the events
            world_name_input.on_event(&event, graphics, None);
            match event {
                gfx::Event::KeyRelease(key) => {
                    if key == gfx::Key::MouseLeft {
                        if back_button.is_hovered(graphics, Some(&buttons_container)) {
                            break 'render_loop;
                        }
                        if create_button.is_hovered(graphics, Some(&buttons_container)) {
                            run_private_world(graphics, menu_back);
                        }
                    }
                }
                _ => {}
            }
        }
        menu_back.set_back_rect_width(700);

        menu_back.render_back(graphics);

        //render input fields

        buttons_container.update(graphics, Some(&menu_back.get_back_rect_container()));

        title.render(graphics, Some(&menu_back.get_back_rect_container()));
        back_button.render(graphics, Some(&buttons_container));

        create_button.render(graphics, Some(&buttons_container));

        world_name_input.render(graphics, Some(&menu_back.get_back_rect_container()));

        graphics.renderer.update_window();
    }
}