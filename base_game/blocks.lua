blocks = {}

function register_blocks()
    blocks.air = terralistic_get_block_id_by_name("air")

    -- DIRT
    block_type = terralistic_new_block_type()
    block_type["name"] = "dirt"
    block_type["can_update_states"] = true
    block_type["break_time"] = 1000
    blocks.dirt = terralistic_register_block_type(block_type)

    -- STONE
    block_type = terralistic_new_block_type()
    block_type["name"] = "stone_block"
    block_type["can_update_states"] = true
    block_type["break_time"] = 1000
    blocks.stone_block = terralistic_register_block_type(block_type)

    -- COPPER ORE
    block_type = terralistic_new_block_type()
    block_type["name"] = "copper_ore"
    block_type["can_update_states"] = true
    block_type["break_time"] = 1000
    blocks.copper_ore = terralistic_register_block_type(block_type)

    -- GRASS BLOCK
    block_type = terralistic_new_block_type()
    block_type["name"] = "grass_block"
    block_type["can_update_states"] = true
    block_type["break_time"] = 1000
    blocks.grass_block = terralistic_register_block_type(block_type)

    terralistic_connect_blocks(blocks.dirt, blocks.grass_block)

    -- WOOD
    block_type = terralistic_new_block_type()
    block_type["name"] = "wood"
    block_type["can_update_states"] = true
    block_type["break_time"] = 1000
    block_type["ghost"] = true
    block_type["transparent"] = true
    block_type["effective_tool"] = tools.axe
    block_type["required_tool_power"] = 10
    blocks.wood = terralistic_register_block_type(block_type)

    -- BRANCH
    block_type = terralistic_new_block_type()
    block_type["name"] = "branch"
    block_type["break_time"] = 1000
    block_type["ghost"] = true
    block_type["transparent"] = true
    blocks.branch = terralistic_register_block_type(block_type)

    -- LEAVES
    block_type = terralistic_new_block_type()
    block_type["name"] = "leaves"
    block_type["ghost"] = true
    block_type["transparent"] = true
    blocks.leaves = terralistic_register_block_type(block_type)

    -- CANOPY
    block_type = terralistic_new_block_type()
    block_type["name"] = "canopy"
    block_type["ghost"] = true
    block_type["transparent"] = true
    block_type["width"] = 5
    block_type["height"] = 5
    blocks.canopy = terralistic_register_block_type(block_type)

    terralistic_connect_blocks(blocks.wood, blocks.canopy)

    -- GRASS
    block_type = terralistic_new_block_type()
    block_type["name"] = "grass"
    block_type["ghost"] = true
    block_type["transparent"] = true
    block_type["break_time"] = 0
    blocks.grass = terralistic_register_block_type(block_type)

    -- STONE
    block_type = terralistic_new_block_type()
    block_type["name"] = "stone"
    block_type["ghost"] = true
    block_type["transparent"] = true
    block_type["break_time"] = 1000
    blocks.stone = terralistic_register_block_type(block_type)
end

function on_block_break(x, y, block_id)
    if block_id == wood_block then
        -- if the wood block is broken, the wood block above it is also broken
        if terralistic_get_block(x, y - 1) == blocks.wood then
            terralistic_break_block(x, y - 1)
        end
        -- also break left and right wood blocks
        if terralistic_get_block(x - 1, y) == blocks.wood then
            terralistic_break_block(x - 1, y)
        end
        if terralistic_get_block(x + 1, y) == blocks.wood then
            terralistic_break_block(x + 1, y)
        end

        -- also break branch blocks on the left and right
        if terralistic_get_block(x - 1, y) == blocks.branch then
            terralistic_break_block(x - 1, y)
        end
        if terralistic_get_block(x + 1, y) == blocks.branch then
            terralistic_break_block(x + 1, y)
        end

        -- also break canopy_block 1 block above
        if terralistic_get_block(x, y - 1) == blocks.canopy then
            terralistic_break_block(x, y - 1)
        end
    end

    -- if branch_block is broken, break the leaves_block on the left and right
    if block_id == branch_block then
        if terralistic_get_block(x - 1, y) == blocks.leaves then
            terralistic_break_block(x - 1, y)
        end

        if terralistic_get_block(x + 1, y) == blocks.leaves then
            terralistic_break_block(x + 1, y)
        end
    end
end
