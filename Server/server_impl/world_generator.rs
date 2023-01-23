use std::collections::HashMap;
use noise::{NoiseFn, Perlin};
use rand::{RngCore, SeedableRng};
use rlua::prelude::{LuaUserData};
use rlua::UserDataMethods;
use shared::blocks::{BlockId, Blocks};
use shared::mod_manager::ModManager;
use shared_mut::SharedMut;

fn turbulence(noise: &Perlin, x: f64, y: f64) -> f64 {
    let mut value = 0.0;
    let mut size = 1.0;

    for _ in 0..3 {
        value += noise.get([x / size, y / size]) * size;
        size /= 2.0;
    }

    value / 2.0
}

fn convolve(array: &Vec<f64>, size: i32) -> Vec<f64> {
    let mut result = Vec::new();

    for i in 0..array.len() {
        let mut sum = 0.0;
        let left_index = i32::max(i as i32 - size / 2, 0);
        let right_index = i32::min(i as i32 + size / 2, array.len() as i32 - 1);
        for j in left_index..right_index {
            sum += array[j as usize];
        }
        result.push(sum / (right_index - left_index) as f64);
    }

    result
}

pub struct WorldGenerator {
    biomes: SharedMut<Vec<Biome>>,
    total_tasks: i32,
    current_task: i32,
    status_text: SharedMut<String>,
}

impl WorldGenerator {
    pub fn new() -> Self {
        Self {
            biomes: SharedMut::new(Vec::new()),
            total_tasks: 0,
            current_task: 0,
            status_text: SharedMut::new(String::new()),
        }
    }

    pub fn init(&mut self, mods: &mut ModManager) {
        mods.add_global_function("new_biome", move |_, _: ()| {
            Ok(Biome::new())
        });

        let biomes = self.biomes.clone();
        mods.add_global_function("register_biome", move |_, biome: Biome| {
            biomes.borrow().push(biome);
            Ok(biomes.borrow().len() - 1)
        });

        // lua function connect_biomes(biome1, biome2, weight) takes two biome ids and a weight and connects them
        // the weight is how likely it is to go from biome1 to biome2 (and vice versa)
        let biomes = self.biomes.clone();
        mods.add_global_function("connect_biomes", move |_, (biome1, biome2, weight): (i32, i32, i32)| {
            biomes.borrow()[biome1 as usize].adjacent_biomes.push((weight, biome2));
            biomes.borrow()[biome2 as usize].adjacent_biomes.push((weight, biome1));
            Ok(())
        });
    }

    fn next_task(&mut self) {
        self.current_task += 1;
        *self.status_text.borrow() = format!("Generating world {}%", (self.current_task as f32 / self.total_tasks as f32 * 100.0) as i32);
    }

    pub fn generate(&mut self, blocks: &mut Blocks, mods: &mut ModManager, min_width: i32, height: i32, seed: u64, status_text: SharedMut<String>) {
        // create a random number generator with seed
        let mut rng = rand::rngs::StdRng::seed_from_u64(seed);

        if self.biomes.borrow().len() == 0 {
            panic!("No biomes were added! Cannot generate world!");
        }

        let mut min_heights = Vec::new();
        let mut max_heights = Vec::new();
        let mut biome_ids = Vec::new();

        let mut width = 0;

        // walk on the graph of biomes
        // initial biome is random
        let mut curr_biome = rand::random::<i32>().abs() % self.biomes.borrow().len() as i32;
        while width < min_width {
            // determine the width of the current biome
            // the width is a random number between the min and max width
            let biome = &self.biomes.borrow()[curr_biome as usize];
            let biome_width = rand::random::<i32>().abs() % (biome.max_width - biome.min_width) + biome.min_width;
            for _ in 0..biome_width {
                min_heights.push(biome.min_terrain_height as f64);
                max_heights.push(biome.max_terrain_height as f64);
                biome_ids.push(curr_biome);
            }
            width += biome_width;

            // determine the next biome
            // the next biome is chosen randomly based on the weights of the edges
            let mut total_weight = 0;
            for (weight, _) in &biome.adjacent_biomes {
                total_weight += weight;
            }
            let mut rand = rand::random::<i32>().abs() % total_weight;
            for (weight, next_biome) in &biome.adjacent_biomes {
                rand -= weight;
                if rand < 0 {
                    curr_biome = *next_biome;
                    break;
                }
            }
        }

        println!("Creating a world with size {}x{}", width, height);

        self.total_tasks = width * height;
        self.status_text = status_text.clone();

        let start_time = std::time::Instant::now();

        *status_text.borrow() = "Generating world".to_string();
        blocks.create(width, height);

        let mut terrain = vec![vec![BlockId::new(); height as usize]; width as usize];

        let mut min_cave_thresholds = vec![0.0; width as usize];
        let mut max_cave_thresholds = vec![0.15; width as usize];

        let mut ores_start_noises = HashMap::new();
        let mut ores_end_noises = HashMap::new();

        for block_id in blocks.get_all_block_ids() {
            ores_start_noises.insert(block_id, vec![-1.0; width as usize]);
            ores_end_noises.insert(block_id, vec![-1.0; width as usize]);
        }

        for x in 0..width {
            let biome = &self.biomes.borrow()[biome_ids[x as usize] as usize];
            for ore in &biome.ores {
                ores_start_noises.get_mut(&ore.block).unwrap()[x as usize] = ore.start_noise;
                ores_end_noises.get_mut(&ore.block).unwrap()[x as usize] = ore.end_noise;
            }
        }

        let convolution_size = 50;
        for _ in 0..5 {
            min_heights = convolve(&min_heights, convolution_size);
            max_heights = convolve(&max_heights, convolution_size);
            min_cave_thresholds = convolve(&min_cave_thresholds, convolution_size);
            max_cave_thresholds = convolve(&max_cave_thresholds, convolution_size);
        }

        for block_id in blocks.get_all_block_ids() {
            for _ in 0..5 {
                ores_start_noises.insert(block_id, convolve(&ores_start_noises[&block_id], convolution_size));
                ores_end_noises.insert(block_id, convolve(&ores_end_noises[&block_id], convolution_size));
            }
        }

        let cave_noise = Perlin::new(rng.next_u32());
        let terrain_noise = Perlin::new(rng.next_u32());
        let mut ore_noises = HashMap::new();
        for block_id in blocks.get_all_block_ids() {
            ore_noises.insert(block_id, Perlin::new(rng.next_u32()));
        }

        for x in 0..width {
            let terrain_noise_val = ((turbulence(&terrain_noise, x as f64 / 150.0, 0.0) + 1.0) * (max_heights[x as usize] - min_heights[x as usize])) as i32 + min_heights[x as usize] as i32 + height * 2 / 3;
            for y in 0..height {
                self.next_task();
                let terrain_height = height - y;

                let cave_noise_val = f64::abs(turbulence(&cave_noise, x as f64 / 80.0, y as f64 / 80.0));
                let cave_threshold = y as f64 / height as f64 * (max_cave_thresholds[x as usize] - min_cave_thresholds[x as usize]) + min_cave_thresholds[x as usize];

                let mut curr_block = self.biomes.borrow()[biome_ids[x as usize] as usize].base_block;

                for block in blocks.get_all_block_ids() {
                    let start_noise = ores_start_noises[&block][x as usize];
                    let end_noise = ores_end_noises[&block][x as usize];
                    if (start_noise, end_noise) != (-1.0, -1.0) {
                        let ore_noise = turbulence(&ore_noises.get(&block).unwrap(), x as f64 / 15.0, y as f64 / 15.0);
                        let ore_threshold = y as f64 / height as f64 * (end_noise - start_noise) + start_noise;
                        if ore_threshold > ore_noise {
                            curr_block = block;
                        }
                    }
                }

                if terrain_height > terrain_noise_val || cave_threshold > cave_noise_val {
                    curr_block = blocks.air;
                };

                terrain[x as usize][y as usize] = curr_block;
            }
        }

        blocks.create_from_block_ids(terrain);

        println!("World generated in {}ms", start_time.elapsed().as_millis());

        if self.current_task != self.total_tasks {
            panic!("Not all tasks were completed! {} != {}", self.current_task, self.total_tasks);
        }
    }
}

#[derive(Clone)]
struct Ore {
    pub block: BlockId,
    pub start_noise: f64,
    pub end_noise: f64,
}

#[derive(Clone)]
struct Biome {
    pub min_width: i32,
    pub max_width: i32,
    pub min_terrain_height: i32,
    pub max_terrain_height: i32,
    pub base_block: BlockId,
    // the first element is connection weight, the second is the biome id
    pub adjacent_biomes: Vec<(i32, i32)>,
    pub ores: Vec<Ore>,
}

impl Biome {
    fn new() -> Self {
        Self {
            min_width: 0,
            max_width: 0,
            min_terrain_height: 0,
            max_terrain_height: 0,
            base_block: BlockId::new(),
            adjacent_biomes: Vec::new(),
            ores: Vec::new(),
        }
    }
}

// make Biome compatible with Lua
impl LuaUserData for Biome {
    // implement index and new_index metamethods to allow reading and writing to fields
    fn add_methods<'lua, M: UserDataMethods<'lua, Self>>(methods: &mut M) {
        // add meta method to access fields
        methods.add_meta_method(rlua::MetaMethod::Index, |_lua_ctx, this, key: String| {
            match key.as_str() {
                "min_width" => Ok(this.min_width),
                "max_width" => Ok(this.max_width),
                "min_terrain_height" => Ok(this.min_terrain_height),
                "max_terrain_height" => Ok(this.max_terrain_height),
                _ => Err(rlua::Error::RuntimeError(format!("{} is not a valid field of Biome", key))),
            }
        });

        methods.add_meta_method(rlua::MetaMethod::Index, |_lua_ctx, this, key: String| {
            match key.as_str() {
                "base_block" => Ok(this.base_block),
                _ => Err(rlua::Error::RuntimeError(format!("{} is not a valid field of Biome", key))),
            }
        });

        // add meta method to set fields
        methods.add_meta_method_mut(rlua::MetaMethod::NewIndex, |_lua_ctx, this, (key, value): (String, rlua::Value)| {
            match key.as_str() {
                "min_width" => {
                    match value {
                        rlua::Value::Integer(b) => this.min_width = b as i32,
                        _ => return Err(rlua::Error::RuntimeError(format!("value is not a valid value for min_width")))
                    }
                    Ok(())
                },
                "max_width" => {
                    match value {
                        rlua::Value::Integer(b) => this.max_width = b as i32,
                        _ => return Err(rlua::Error::RuntimeError(format!("value is not a valid value for max_width")))
                    }
                    Ok(())
                },
                "min_terrain_height" => {
                    match value {
                        rlua::Value::Integer(b) => this.min_terrain_height = b as i32,
                        _ => return Err(rlua::Error::RuntimeError(format!("value is not a valid value for min_terrain_height")))
                    }
                    Ok(())
                },
                "max_terrain_height" => {
                    match value {
                        rlua::Value::Integer(b) => this.max_terrain_height = b as i32,
                        _ => return Err(rlua::Error::RuntimeError(format!("value is not a valid value for max_terrain_height")))
                    }
                    Ok(())
                },
                "base_block" => {
                    // base_block is a BlockId, so we need to convert the value to a BlockId
                    match value {
                        rlua::Value::UserData(b) => {
                            match b.borrow::<BlockId>() {
                                Ok(b) => this.base_block = *b,
                                Err(_) => return Err(rlua::Error::RuntimeError(format!("value is not a valid value for base_block")))
                            }
                        },
                        _ => return Err(rlua::Error::RuntimeError(format!("value is not a valid value for base_block")))
                    }
                    Ok(())
                },
                _ => Err(rlua::Error::RuntimeError(format!("{} is not a valid field of Biome", key))),
            }
        });

        // add method to add an ore
        methods.add_method_mut("add_ore", |lua_ctx, this, (block, start_noise, end_noise): (BlockId, f64, f64)| {
            this.ores.push(Ore {
                block,
                start_noise,
                end_noise,
            });
            Ok(())
        });
    }
}