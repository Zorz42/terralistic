--[[

This is a Terralistic mod file.
It is used to define the mod's name,
description, version and the behavior
of the mod.

]]--


-- This function returns the mod's name.
function mod_name()
    return "BaseGame"
end

-- This function returns the mod's description.
function mod_description()
    return "The base game. It contains the basic"..
    "game mechanics and the classic Terralistic"..
    "experience."
end

-- This function returns the mod's version.
function mod_version()
    return "0.1"
end

-- global variables for block IDs
air = 0 -- air is built-in and the id is always 0
dirt = -1

-- global variables for biome IDs
plains = -1

-- This function is called when the mod is loaded.
function init()
    terralistic_print("BaseGame mod loaded.")
    block_type = terralistic_new_block_type()
    block_type["name"] = "dirt"
    dirt = terralistic_register_block_type(block_type)
end

-- This function is called when the mod is loaded on a server.
function init_server()
    terralistic_print("BaseGame mod loaded on server.")
    plains = terralistic_add_biome(1.0)
end

-- This function is called when the mod is unloaded.
function stop()
    terralistic_print("BaseGame mod stopped.")
end

-- This function is called when the mod is updated.
function update()

end

-- This function is called, when a world is being generated.
function generate_block(x, y)
    -- there is a 10% chance that the block is dirt else it is air
    if math.random(1, 10) == 1 then
        return dirt
    else
        return air
    end
end