--[[
    @description Spawn to Ramp - acts as a path from spawn to the ramp.
    @version 0.1.0
    @author thebest12lines
    @date 4/15/2026
]]
-- will be converted to zepton::MovementPlan at runtime

-- use define_path for paths, and define_pattern for patterns
--define_path( 
--    nil,              -- path_name (leave nil for filename)
--    "spawn_ramp"      -- identifier (leave nil for custom/handled)
--    "0.1.0"           -- version (required)
--    "spawn to ramp"   -- readable_path_name (leave nil for filename)
--
--)
-- newer walk function that uses studs than seconds
walk({KEY_FORWARD, KEY_RIGHT}, 85.5);
walk({KEY_RIGHT}, 43.5)

-- this is the older equivalent version
--press_keys({KEY_FORWARD, KEY_RIGHT}, 2.95)
-- press_keys({KEY_RIGHT, KEY_RIGHT}, 1.5)