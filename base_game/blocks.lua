blocks = {}

function register_blocks()
    terralistic_print("registering blocks...")

    blocks.air = terralistic_get_block_id_by_name("air")

    -- DIRT
    blocks.dirt = terralistic_register_block_type(
            -- effective_tool
            tools.shovel,
            -- required_tool_power
            10,
            -- ghost
            false,
            -- transparent
            false,
            -- name
            "dirt",
            -- connects_to
            {},
            -- break_time
            700,
            -- light_emission
            0, 0, 0,
            -- size
            1, 1,
            -- can_update_states
            true,
            -- feet_collidable
            false,
            -- clickable
            false,
            -- inventory_slots
            {}
    )

    -- STONE
    blocks.stone_block = terralistic_register_block_type(
            -- effective_tool
            tools.pickaxe,
            -- required_tool_power
            10,
            -- ghost
            false,
            -- transparent
            false,
            -- name
            "stone_block",
            -- connects_to
            {},
            -- break_time
            1000,
            -- light_emission
            0, 0, 0,
            -- size
            1, 1,
            -- can_update_states
            true,
            -- feet_collidable
            false,
            -- clickable
            false,
            -- inventory_slots
            {}
    )

    -- COPPER ORE
    blocks.copper_ore = terralistic_register_block_type(
            -- effective_tool
            tools.pickaxe,
            -- required_tool_power
            10,
            -- ghost
            false,
            -- transparent
            false,
            -- name
            "copper_ore",
            -- connects_to
            {},
            -- break_time
            2000,
            -- light_emission
            0, 0, 0,
            -- size
            1, 1,
            -- can_update_states
            true,
            -- feet_collidable
            false,
            -- clickable
            false,
            -- inventory_slots
            {}
    )
    
    -- IRON ORE
    blocks.iron_ore = terralistic_register_block_type(
            -- effective_tool
            tools.pickaxe,
            -- required_tool_power
            10,
            -- ghost
            false,
            -- transparent
            false,
            -- name
            "iron_ore",
            -- connects_to
            {},
            -- break_time
            2000,
            -- light_emission
            0, 0, 0,
            -- size
            1, 1,
            -- can_update_states
            true,
            -- feet_collidable
            false,
            -- clickable
            false,
            -- inventory_slots
            {}
    )
    
    -- TIN ORE
    blocks.tin_ore = terralistic_register_block_type(
            -- effective_tool
            tools.pickaxe,
            -- required_tool_power
            10,
            -- ghost
            false,
            -- transparent
            false,
            -- name
            "tin_ore",
            -- connects_to
            {},
            -- break_time
            2000,
            -- light_emission
            0, 0, 0,
            -- size
            1, 1,
            -- can_update_states
            true,
            -- feet_collidable
            false,
            -- clickable
            false,
            -- inventory_slots
            {}
    )

    -- GRASS BLOCK
    blocks.grass_block = terralistic_register_block_type(
            -- effective_tool
            nil,
            -- required_tool_power
            0,
            -- ghost
            false,
            -- transparent
            false,
            -- name
            "grass_block",
            -- connects_to
            {},
            -- break_time
            0,
            -- light_emission
            0, 0, 0,
            -- size
            1, 1,
            -- can_update_states
            true,
            -- feet_collidable
            false,
            -- clickable
            false,
            -- inventory_slots
            {}
    )

    terralistic_connect_blocks(blocks.dirt, blocks.grass_block)

    -- WOOD
    blocks.wood = terralistic_register_block_type(
            -- effective_tool
            tools.axe,
            -- required_tool_power
            10,
            -- ghost
            true,
            -- transparent
            true,
            -- name
            "wood",
            -- connects_to
            {},
            -- break_time
            2000,
            -- light_emission
            0, 0, 0,
            -- size
            1, 1,
            -- can_update_states
            true,
            -- feet_collidable
            false,
            -- clickable
            false,
            -- inventory_slots
            {}
    )

    -- BRANCH
    blocks.branch = terralistic_register_block_type(
            -- effective_tool
            nil,
            -- required_tool_power
            0,
            -- ghost
            true,
            -- transparent
            true,
            -- name
            "branch",
            -- connects_to
            {},
            -- break_time
            1500,
            -- light_emission
            0, 0, 0,
            -- size
            1, 1,
            -- can_update_states
            false,
            -- feet_collidable
            false,
            -- clickable
            false,
            -- inventory_slots
            {}
    )

    -- LEAVES
    blocks.leaves = terralistic_register_block_type(
            -- effective_tool
            nil,
            -- required_tool_power
            0,
            -- ghost
            true,
            -- transparent
            true,
            -- name
            "leaves",
            -- connects_to
            {},
            -- break_time
            nil,
            -- light_emission
            0, 0, 0,
            -- size
            1, 1,
            -- can_update_states
            false,
            -- feet_collidable
            false,
            -- clickable
            false,
            -- inventory_slots
            {}
    )

    -- CANOPY
    blocks.canopy = terralistic_register_block_type(
            -- effective_tool
            nil,
            -- required_tool_power
            0,
            -- ghost
            true,
            -- transparent
            true,
            -- name
            "canopy",
            -- connects_to
            {},
            -- break_time
            nil,
            -- light_emission
            0, 0, 0,
            -- size
            5, 5,
            -- can_update_states
            false,
            -- feet_collidable
            false,
            -- clickable
            false,
            -- inventory_slots
            {}
    )

    terralistic_connect_blocks(blocks.wood, blocks.canopy)

    -- GRASS
    blocks.grass = terralistic_register_block_type(
            -- effective_tool
            nil,
            -- required_tool_power
            0,
            -- ghost
            true,
            -- transparent
            true,
            -- name
            "grass",
            -- connects_to
            {},
            -- break_time
            0,
            -- light_emission
            0, 0, 0,
            -- size
            1, 1,
            -- can_update_states
            false,
            -- feet_collidable
            false,
            -- clickable
            false,
            -- inventory_slots
            {}
    )

    -- STONE
    blocks.stone = terralistic_register_block_type(
            -- effective_tool
            nil,
            -- required_tool_power
            0,
            -- ghost
            true,
            -- transparent
            true,
            -- name
            "stone",
            -- connects_to
            {},
            -- break_time
            500,
            -- light_emission
            0, 0, 0,
            -- size
            1, 1,
            -- can_update_states
            false,
            -- feet_collidable
            false,
            -- clickable
            false,
            -- inventory_slots
            {}
    )

    -- WOOD_PLANKS
    blocks.wood_planks = terralistic_register_block_type(
            -- effective_tool
            nil,
            -- required_tool_power
            0,
            -- ghost
            false,
            -- transparent
            false,
            -- name
            "wood_planks",
            -- connects_to
            {},
            -- break_time
            1000,
            -- light_emission
            0, 0, 0,
            -- size
            1, 1,
            -- can_update_states
            true,
            -- feet_collidable
            false,
            -- clickable
            false,
            -- inventory_slots
            {}
    )

    -- TORCH
    blocks.torch = terralistic_register_block_type(
            -- effective_tool
            nil,
            -- required_tool_power
            0,
            -- ghost
            true,
            -- transparent
            true,
            -- name
            "torch",
            -- connects_to
            {},
            -- break_time
            300,
            -- light_emission
            200, 200, 150,
            -- size
            1, 1,
            -- can_update_states
            false,
            -- feet_collidable
            false,
            -- clickable
            false,
            -- inventory_slots
            {}
    )
    
    -- FURNACE
    blocks.furnace = terralistic_register_block_type(
            -- effective_tool
            tools.pickaxe,
            -- required_tool_power
            10,
            -- ghost
            true,
            -- transparent
            true,
            -- name
            "furnace",
            -- connects_to
            {},
            -- break_time
            3000,
            -- light_emission
            0, 0, 0,
            -- size
            2, 2,
            -- can_update_states
            false,
            -- feet_collidable
            false,
            -- clickable
            false,
            -- inventory_slots
            {{0, 150}, {0, 210}, {100, 180}}
    )
end

function on_block_break(x, y, block_id)
    if block_id == blocks.wood then
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

    elseif block_id == blocks.branch then
        -- if branch_block is broken, break the leaves_block on the left and right
        if terralistic_get_block(x - 1, y) == blocks.leaves then
            terralistic_break_block(x - 1, y)
        end

        if terralistic_get_block(x + 1, y) == blocks.leaves then
            terralistic_break_block(x + 1, y)
        end

    elseif block_id == blocks.grass_block then
        -- if grass_block is broken, replace it with dirt_block
        terralistic_set_block(x, y, blocks.dirt)
    end
end

function get_smelting_result(item_id)
    if item_id == items.copper_ore then
        return items.wood_planks
    end
    if item_id == items.wood_planks then
        return items.copper_ore
    end
    return nil
end

function on_block_update(x, y)
    block_id = terralistic_get_block(x, y)

    if block_id == blocks.furnace then
        -- check furnace inventory and update it accordingly
        local inventory_items = terralistic_get_block_inventory_items(x, y)
        local inventory_item_counts = terralistic_get_block_inventory_item_counts(x, y)

        -- check if there is an item in the first slot and if it can be smelted
        if inventory_items[1] ~= nil and get_smelting_result(inventory_items[1]) ~= nil then
            local success = false
            -- if there is an item in the third slot, check if it is the same as the result of the smelting
            if inventory_items[3] == get_smelting_result(inventory_items[1]) then
                -- if it is the same, increase the count of the item in the second slot
                inventory_item_counts[3] = inventory_item_counts[3] + 1
                success = true
            else
                -- if it is not the same, check if the second slot is empty
                if inventory_items[3] == nil then
                    -- if it is empty, put the result of the smelting in the second slot
                    inventory_items[3] = get_smelting_result(inventory_items[1])
                    inventory_item_counts[3] = 1
                    success = true
                end
            end

            if success then
                -- decrease the count of the item in the first slot
                inventory_item_counts[1] = inventory_item_counts[1] - 1

                -- if the count of the item in the first slot is 0, remove the item from the first slot
                if inventory_item_counts[1] == 0 then
                    inventory_items[1] = nil
                    inventory_item_counts[1] = nil
                end

                terralistic_set_block_inventory_item(x, y, 0, inventory_items[1], inventory_item_counts[1])
                terralistic_set_block_inventory_item(x, y, 2, inventory_items[3], inventory_item_counts[3])
            end
        end
    end
end