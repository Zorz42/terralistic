extern crate alloc;

use alloc::sync::Arc;
use std::sync::{Mutex, PoisonError};

use crate::client::game::chat::ClientChat;
use crate::client::game::debug_menu::DebugMenu;
use anyhow::{bail, Result};

use crate::client::game::entities::ClientEntities;
use crate::client::game::framerate_measurer::FramerateMeasurer;
use crate::client::game::inventory::ClientInventory;
use crate::client::game::items::ClientItems;
use crate::client::game::lights::ClientLights;
use crate::client::game::pause_menu::PauseMenu;
use crate::client::game::players::ClientPlayers;
use crate::client::menus::{run_loading_screen, BackgroundRect};
use crate::libraries::events;
use crate::libraries::events::EventManager;
use crate::libraries::graphics::GraphicsContext;
use crate::shared::entities::PositionComponent;

use super::background::Background;
use super::block_selector::BlockSelector;
use super::blocks::ClientBlocks;
use super::camera::Camera;
use super::mod_manager::ClientModManager;
use super::networking::ClientNetworking;
use super::walls::ClientWalls;

#[allow(clippy::too_many_lines)]
/// # Errors
/// If the game basically crashes
pub fn run_game(
    graphics: &mut GraphicsContext,
    menu_back: &mut dyn BackgroundRect,
    server_port: u16,
    server_address: String,
    player_name: &str,
) -> Result<()> {
    // load base game mod
    let mut pre_events = EventManager::new();
    let mut networking = ClientNetworking::new(server_port, server_address);
    networking.init(player_name.to_owned())?;
    while networking.is_welcoming() {
        // wait 1 ms
        std::thread::sleep(core::time::Duration::from_millis(1));
    }

    networking.update(&mut pre_events)?;
    networking.start_receiving();

    let timer = std::time::Instant::now();

    let loading_text = Arc::new(Mutex::new("Loading".to_owned()));
    let loading_text2 = loading_text.clone();

    let init_thread = std::thread::spawn(move || {
        let mut temp_fn = || -> Result<(ClientModManager, ClientBlocks, ClientWalls, ClientEntities, ClientItems)> {
            *loading_text2.lock().unwrap_or_else(PoisonError::into_inner) = "Loading mods".to_owned();
            let mut mods = ClientModManager::new();
            let mut blocks = ClientBlocks::new();
            let mut walls = ClientWalls::new(&mut blocks.get_blocks());
            let mut entities = ClientEntities::new();
            let mut items = ClientItems::new();

            while let Some(event) = pre_events.pop_event() {
                mods.on_event(&event)?;
                blocks.on_event(&event, &mut pre_events, &mut mods.mod_manager)?;
                walls.on_event(&event)?;
                items.on_event(&event, &mut entities.entities, &mut pre_events);
            }

            blocks.init(&mut mods.mod_manager)?;
            walls.init(&mut mods.mod_manager)?;
            items.init(&mut mods.mod_manager)?;

            *loading_text2.lock().unwrap_or_else(PoisonError::into_inner) = "Initializing mods".to_owned();
            mods.init()?;

            anyhow::Ok((mods, blocks, walls, entities, items))
        };
        // if the init fails, we clear the loading text so the error can be displayed
        let result = temp_fn();
        loading_text2
            .lock()
            .unwrap_or_else(PoisonError::into_inner)
            .clear();
        result
    });

    run_loading_screen(graphics, menu_back, &loading_text);

    let result = init_thread.join();
    let Ok(result) = result else { bail!("Failed to join init thread"); };
    let result = result?;

    let mut mods = result.0;
    let mut blocks = result.1;
    let mut walls = result.2;
    let mut entities = result.3;
    let mut items = result.4;

    let mut background = Background::new();
    let mut inventory = ClientInventory::new();
    let mut lights = ClientLights::new();
    let mut events = EventManager::new();
    let mut camera = Camera::new();
    let mut players = ClientPlayers::new(player_name);
    let mut block_selector = BlockSelector::new();
    let mut pause_menu = PauseMenu::new();
    let mut debug_menu = DebugMenu::new();
    let mut framerate_measurer = FramerateMeasurer::new();
    let mut chat = ClientChat::new(graphics);

    background.init()?;
    inventory.init();
    lights.init(&blocks.get_blocks())?;

    blocks.load_resources(&mut mods.mod_manager)?;
    walls.load_resources(&mut mods.mod_manager)?;
    items.load_resources(&mut mods.mod_manager)?;
    camera.load_resources(graphics);
    players.load_resources(&mut mods.mod_manager)?;

    pause_menu.init(graphics);
    debug_menu.init();
    chat.init();

    // print the time it took to initialize
    println!("Game joined in {}ms", timer.elapsed().as_millis());

    'main_loop: while graphics.renderer.is_window_open() {
        framerate_measurer.update();

        while let Some(event) = graphics.renderer.get_event() {
            events.push_event(events::Event::new(event));
        }

        graphics.renderer.block_key_states = chat.is_selected();

        networking.update(&mut events)?;
        mods.update()?;
        blocks.update(framerate_measurer.get_delta_time(), &mut events)?;
        walls.update(framerate_measurer.get_delta_time(), &mut events)?;

        if let Some(main_player) = players.get_main_player() {
            let player_pos = entities
                .entities
                .ecs
                .get::<&PositionComponent>(main_player)?;

            camera.set_position(player_pos.x(), player_pos.y());
        }

        while framerate_measurer.has_5ms_passed() {
            camera.update_ms(graphics);
            players.update(
                graphics,
                &mut entities.entities,
                &mut networking,
                &blocks.get_blocks(),
            )?;
            entities.entities.update_entities_ms(&blocks.get_blocks());
        }

        background.render(graphics, &camera);
        walls.render(graphics, &camera)?;
        blocks.render(graphics, &camera)?;
        players.render(graphics, &mut entities.entities, &camera);
        items.render(graphics, &camera, &mut entities.entities)?;
        lights.render(graphics, &camera, &blocks.get_blocks(), &mut events)?;
        camera.render(graphics);
        block_selector.render(graphics, &mut networking, &camera)?;
        inventory.render(graphics, &items, &mut networking)?;
        chat.render(graphics);

        pause_menu.render(graphics);

        debug_menu.render(
            graphics,
            &[
                format!("FPS: {}", framerate_measurer.get_fps()),
                format!("{:.2} ms max", framerate_measurer.get_max_frame_time()),
                format!("{:.2} ms avg", framerate_measurer.get_avg_frame_time()),
            ],
        );

        while let Some(event) = events.pop_event() {
            if chat.on_event(&event, graphics, &mut networking)? {
                continue;
            }
            inventory.on_event(&event, &mut networking)?;
            mods.on_event(&event)?;
            blocks.on_event(&event, &mut events, &mut mods.mod_manager)?;
            walls.on_event(&event)?;
            entities.on_event(&event)?;
            items.on_event(&event, &mut entities.entities, &mut events);
            block_selector.on_event(graphics, &mut networking, &camera, &event)?;
            players.on_event(&event, &mut entities.entities);
            lights.on_event(&event, &blocks.get_blocks())?;
            camera.on_event(&event);
            if pause_menu.on_event(&event, graphics) {
                break 'main_loop;
            }
            debug_menu.on_event(&event);
        }

        framerate_measurer.update_post_render();

        graphics.renderer.update_window();
    }

    networking.stop()?;
    mods.stop()?;

    Ok(())
}
