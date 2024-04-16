use crate::client::game::tls_client::{AuthenticationState, TlsClient};
use crate::client::global_settings::GlobalSettings;
use crate::client::menus::{Menu, SettingsMenu};
use crate::client::settings::Settings;
use crate::libraries::graphics as gfx;
use crate::shared::tls_client::ConnectionState;
use crate::shared::versions::VERSION;
use gfx::{BaseUiElement, UiElement};
use std::cell::RefCell;
use std::rc::Rc;

use super::{LoginMenu, MultiplayerSelector, SingleplayerSelector};

pub enum SecondaryMenu {
    None,
    SingleMenu((RefCell<Box<dyn Menu>>, usize)),
    Transition {
        top_menu: (RefCell<Box<dyn Menu>>, usize),
        bottom_menu: (RefCell<Box<dyn Menu>>, usize),
        direction: bool, //true is down
        transition_state: f32,
        top_menu_container: gfx::Container,
        bottom_menu_container: gfx::Container,
    },
}

impl SecondaryMenu {
    pub fn render(&mut self, graphics: &mut gfx::GraphicsContext, parent_container: &gfx::Container) {
        match self {
            Self::None => {}
            Self::SingleMenu((menu, _id)) => {
                menu.get_mut().render(graphics, parent_container);
            }
            Self::Transition {
                top_menu: (top_menu, _id_top),
                bottom_menu: (bottom_menu, _id_bot),
                top_menu_container,
                bottom_menu_container,
                ..
            } => {
                top_menu.get_mut().render(graphics, top_menu_container);
                bottom_menu.get_mut().render(graphics, bottom_menu_container);
            }
        }
    }

    pub fn update(&mut self, graphics: &mut gfx::GraphicsContext, parent_container: &gfx::Container) {
        let mut stop_transition = false;
        match self {
            Self::None => {}
            Self::SingleMenu((menu, _id)) => {
                menu.get_mut().update(graphics, parent_container);
            }
            Self::Transition {
                top_menu: (top_menu, _id_top),
                bottom_menu: (bottom_menu, _id_bot),
                direction,
                transition_state,
                top_menu_container,
                bottom_menu_container,
            } => {
                *transition_state += (1.0 - *transition_state) / 15.0;
                if *transition_state > 0.999 {
                    stop_transition = true;
                }
                let offset = if *direction {
                    -graphics.get_window_size().1 * (*transition_state)
                } else {
                    -graphics.get_window_size().1 * (1.0 - *transition_state)
                };
                top_menu_container.rect.pos.1 = parent_container.rect.pos.1 + offset;
                bottom_menu_container.rect.pos.1 = parent_container.rect.pos.1 + offset + graphics.get_window_size().1;
                top_menu_container.update(graphics, parent_container);
                bottom_menu_container.update(graphics, parent_container);
                top_menu.get_mut().update(graphics, top_menu_container);
                bottom_menu.get_mut().update(graphics, bottom_menu_container);
            }
        }
        if stop_transition {
            if let Self::Transition { top_menu, bottom_menu, direction, .. } = std::mem::replace(self, Self::None) {
                *self = Self::SingleMenu(if direction { bottom_menu } else { top_menu });
            }
        }
    }

    pub fn on_event(&mut self, graphics: &mut gfx::GraphicsContext, event: &gfx::Event, parent_container: &gfx::Container) -> bool {
        match self {
            Self::None => false,
            Self::SingleMenu((menu, _id)) => menu.get_mut().on_event(graphics, event, parent_container),
            Self::Transition {
                top_menu: (top_menu, _id_top),
                bottom_menu: (bottom_menu, _id_bot),
                ..
            } => top_menu.get_mut().on_event(graphics, event, parent_container) || bottom_menu.get_mut().on_event(graphics, event, parent_container),
        }
    }

    pub fn switch_to(&mut self, new_menu: (RefCell<Box<dyn Menu>>, usize), graphics: &gfx::GraphicsContext, parent_container: &gfx::Container) {
        match std::mem::replace(self, Self::None) {
            Self::None => {
                *self = Self::SingleMenu(new_menu);
            }
            Self::SingleMenu((menu, id)) => {
                let (top_menu, bottom_menu, direction) = if id < new_menu.1 { ((menu, id), new_menu, true) } else { (new_menu, (menu, id), false) };
                *self = Self::Transition {
                    top_menu,
                    bottom_menu,
                    transition_state: 0.0,
                    direction,
                    bottom_menu_container: gfx::Container::new(graphics, parent_container.rect.pos, parent_container.rect.size, parent_container.orientation, None),
                    top_menu_container: gfx::Container::new(graphics, parent_container.rect.pos, parent_container.rect.size, parent_container.orientation, None),
                }
            }
            Self::Transition { top_menu, bottom_menu, direction, .. } => {
                let switching_to = if direction { bottom_menu } else { top_menu };
                if new_menu.1 == switching_to.1 {
                    *self = Self::SingleMenu(switching_to);
                } else {
                    let (top_menu, bottom_menu, direction) = if switching_to.1 < new_menu.1 {
                        (switching_to, new_menu, true)
                    } else {
                        (new_menu, switching_to, false)
                    };
                    *self = Self::Transition {
                        top_menu,
                        bottom_menu,
                        transition_state: 0.0,
                        direction,
                        bottom_menu_container: gfx::Container::new(graphics, parent_container.rect.pos, parent_container.rect.size, parent_container.orientation, None),
                        top_menu_container: gfx::Container::new(graphics, parent_container.rect.pos, parent_container.rect.size, parent_container.orientation, None),
                    }
                }
            }
        }
    }

    pub fn should_close(&self) -> bool {
        match self {
            Self::None => false,
            Self::SingleMenu((menu, _id)) => menu.borrow_mut().should_close(),
            Self::Transition { top_menu, bottom_menu, .. } => top_menu.0.borrow_mut().should_close() || bottom_menu.0.borrow_mut().should_close(),
        }
    }

    pub fn open_secondary_menu(
        &mut self,
        graphics: &mut gfx::GraphicsContext,
        menu_index: usize,
        settings: Rc<RefCell<Settings>>,
        global_settings: Rc<RefCell<GlobalSettings>>,
        menu_back_timer: std::time::Instant,
        menu_back: &dyn UiElement,
    ) -> bool {
        let menu: RefCell<Box<dyn Menu>> = match menu_index {
            0 => RefCell::new(Box::new(LoginMenu::new(graphics))),
            1 => RefCell::new(Box::new(SingleplayerSelector::new(graphics, settings, global_settings, menu_back_timer))),
            2 => {
                let res = MultiplayerSelector::new(graphics, menu_back_timer, settings, global_settings);
                if let Ok(menu) = res {
                    RefCell::new(Box::new(menu))
                } else {
                    *self = Self::None;
                    return false;
                }
            }
            3 => {
                let mut menu = SettingsMenu::new(settings, global_settings);
                menu.init(graphics, &menu_back.get_container(graphics, &gfx::Container::default(graphics)));
                RefCell::new(Box::new(menu))
            }
            usize::MAX => {
                graphics.close_window();
                return false;
            }
            _ => {
                println!("menu doesn't exist");
                *self = Self::None;
                return false;
            }
        };

        if !matches!(self, SecondaryMenu::SingleMenu((_, curr_menu_index)) if *curr_menu_index == menu_index) {
            self.switch_to((menu, menu_index), graphics, &menu_back.get_container(graphics, &gfx::Container::default(graphics)));
        }

        return true;
    }

    fn get_tls_status_color(tls_client: &Option<TlsClient>) -> gfx::Color {
        tls_client.as_ref().map_or_else(
            || gfx::Color::new(255, 0, 0, 255),
            |client| match &client.get_connection_state() {
                ConnectionState::CONNECTING(_) => gfx::Color::new(255, 255, 0, 255),
                _ => match client.get_authentication_state() {
                    AuthenticationState::AUTHENTICATING => gfx::Color::new(255, 255, 0, 255),
                    AuthenticationState::AUTHENTICATED => gfx::Color::new(0, 255, 0, 255),
                    _ => gfx::Color::new(255, 0, 0, 255),
                },
            },
        )
    }
}

pub struct MainMenu {
    title: gfx::Sprite,
    debug_title: gfx::Sprite,
    version: gfx::Sprite,
    singleplayer_button: gfx::Button,
    multiplayer_button: gfx::Button,
    settings_button: gfx::Button,
    mods_button: gfx::Button,
    exit_button: gfx::Button,
}

impl MainMenu {
    pub fn new(graphics: &gfx::GraphicsContext, open_menu: &Rc<RefCell<Option<usize>>>) -> Self {
        let copied_menu = open_menu.clone();
        let mut singleplayer_button = gfx::Button::new(move || *RefCell::borrow_mut(&copied_menu) = Some(1));
        singleplayer_button.scale = 3.0;
        singleplayer_button.texture = gfx::Texture::load_from_surface(&graphics.font.create_text_surface("Singleplayer", None));
        singleplayer_button.orientation = gfx::CENTER;

        let copied_menu = open_menu.clone();
        let mut multiplayer_button = gfx::Button::new(move || *RefCell::borrow_mut(&copied_menu) = Some(2));
        multiplayer_button.scale = 3.0;
        multiplayer_button.texture = gfx::Texture::load_from_surface(&graphics.font.create_text_surface("Multiplayer", None));
        multiplayer_button.orientation = gfx::CENTER;

        let copied_menu = open_menu.clone();
        let mut settings_button = gfx::Button::new(move || *RefCell::borrow_mut(&copied_menu) = Some(3));
        settings_button.scale = 3.0;
        settings_button.texture = gfx::Texture::load_from_surface(&graphics.font.create_text_surface("Settings", None));
        settings_button.orientation = gfx::CENTER;

        let copied_menu = open_menu.clone();
        let mut mods_button = gfx::Button::new(move || *RefCell::borrow_mut(&copied_menu) = Some(4));
        mods_button.scale = 3.0;
        mods_button.texture = gfx::Texture::load_from_surface(&graphics.font.create_text_surface("Mods", None));
        mods_button.orientation = gfx::CENTER;

        let copied_menu = open_menu.clone();
        let mut exit_button = gfx::Button::new(move || *RefCell::borrow_mut(&copied_menu) = Some(usize::MAX));
        exit_button.scale = 3.0;
        exit_button.texture = gfx::Texture::load_from_surface(&graphics.font.create_text_surface("Exit", None));
        exit_button.orientation = gfx::CENTER;

        let mut debug_title = gfx::Sprite::new();
        debug_title.set_texture(gfx::Texture::load_from_surface(&graphics.font.create_text_surface("DEBUG MODE", None)));
        debug_title.color = gfx::DARK_GREY;
        debug_title.orientation = gfx::TOP;
        debug_title.scale = 2.0;
        debug_title.pos.1 = gfx::SPACING / 4.0;

        let mut title = gfx::Sprite::new();
        title.set_texture(gfx::Texture::load_from_surface(&graphics.font.create_text_surface("Terralistic", None)));
        title.scale = 4.0;
        title.orientation = gfx::TOP;
        title.pos.1 = debug_title.pos.1 + debug_title.get_size().1 + gfx::SPACING / 2.0;

        let mut version = gfx::Sprite::new();
        version.set_texture(gfx::Texture::load_from_surface(&graphics.font.create_text_surface(VERSION, None)));
        version.color = gfx::GREY;
        version.orientation = gfx::BOTTOM;
        version.scale = 2.0;
        version.pos.1 = -5.0;

        {
            let buttons = [&mut singleplayer_button, &mut multiplayer_button, &mut settings_button, &mut mods_button, &mut exit_button];

            // buttons are on top of another on the center of the screen
            // their combined height is centered on the screen

            let mut total_height = 0.0;
            for button in &buttons {
                total_height += button.get_size().1;
            }
            let mut current_y = -total_height / 2.0 + buttons[0].get_size().1;
            for button in buttons {
                button.pos.1 = current_y;
                current_y += button.get_size().1;
            }
        }
        Self {
            title,
            debug_title,
            version,
            singleplayer_button,
            multiplayer_button,
            settings_button,
            mods_button,
            exit_button,
        }
    }
}

impl UiElement for MainMenu {
    fn get_sub_elements(&self) -> Vec<&dyn BaseUiElement> {
        vec![
            &self.title,
            #[cfg(debug_assertions)]
            &self.debug_title,
            &self.version,
            &self.singleplayer_button,
            &self.multiplayer_button,
            &self.settings_button,
            &self.mods_button,
            &self.exit_button,
        ]
    }

    fn get_sub_elements_mut(&mut self) -> Vec<&mut dyn BaseUiElement> {
        vec![
            &mut self.title,
            #[cfg(debug_assertions)]
            &mut self.debug_title,
            &mut self.version,
            &mut self.singleplayer_button,
            &mut self.multiplayer_button,
            &mut self.settings_button,
            &mut self.mods_button,
            &mut self.exit_button,
        ]
    }

    fn get_container(&self, graphics: &gfx::GraphicsContext, parent_container: &gfx::Container) -> gfx::Container {
        gfx::Container::new(graphics, parent_container.rect.pos, parent_container.rect.size, parent_container.orientation, None)
    }
}
