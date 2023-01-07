mod networking;
mod mod_manager;
mod blocks;
mod world_generator;

use shared::mod_manager::GameMod;
use shared_mut::SharedMut;
use events::EventManager;
use crate::blocks::ServerBlocks;
use crate::mod_manager::ServerModManager;
use crate::networking::ServerNetworking;
use crate::world_generator::WorldGenerator;

#[derive(Clone, Copy, PartialEq, Eq)]
pub enum ServerState {
    Starting,
    Running,
    Stopping,
    Stopped,
}

pub struct Server {
    tps_limit: f64,
    running: SharedMut<bool>,
    server_state: SharedMut<ServerState>,
    events: EventManager,
    networking: ServerNetworking,
    mods: ServerModManager,
    blocks: ServerBlocks,
}

impl Server {
    pub fn new(running: SharedMut<bool>, server_state: SharedMut<ServerState>, port: u16) -> Self {
        Self {
            tps_limit: 20.0,
            running,
            server_state,
            events: EventManager::new(),
            networking: ServerNetworking::new(port),
            mods: ServerModManager::new(),
            blocks: ServerBlocks::new(),
        }
    }

    pub fn start(&mut self) {
        println!("Starting server...");
        let timer = std::time::Instant::now();
        *self.server_state.borrow() = ServerState::Starting;

        // load base game mod
        let base_mod = GameMod::from_bytes(include_bytes!("../../BaseGame/BaseGame.mod").to_vec());
        self.mods.mod_manager.add_mod(base_mod);

        // init modules
        self.networking.init();
        self.blocks.init(&mut self.mods.mod_manager);

        let mut generator = WorldGenerator::new();
        generator.init(&mut self.mods.mod_manager);

        self.mods.init();
        for game_mod in self.mods.mod_manager.mods_mut() {
            game_mod.call_function::<(), ()>("init_server", ()).unwrap();
        }

        generator.generate(&mut self.blocks.blocks, &mut self.mods.mod_manager, 4400, 1200, 423657);

        // start server loop
        println!("Server started in {}ms", timer.elapsed().as_millis());
        *self.server_state.borrow() = ServerState::Running;
        let mut last_time = std::time::Instant::now();
        loop {
            let delta_time = last_time.elapsed().as_secs_f64();
            last_time = std::time::Instant::now();

            // update modules
            self.networking.update(&mut self.events);
            self.mods.update();

            // handle events
            while let Some(event) = self.events.pop_event() {
                self.blocks.on_event(&event, &mut self.networking);
                self.networking.on_event(&event);
            }

            if !*self.running.borrow() {
                break;
            }

            // sleep
            let sleep_time = 1.0 / self.tps_limit - delta_time;
            if sleep_time > 0.0 {
                std::thread::sleep(std::time::Duration::from_secs_f64(sleep_time));
            }
        }

        *self.server_state.borrow() = ServerState::Stopping;

        // stop modules
        self.networking.stop();
        self.mods.stop();

        *self.server_state.borrow() = ServerState::Stopped;
        println!("Server stopped.");
    }
}