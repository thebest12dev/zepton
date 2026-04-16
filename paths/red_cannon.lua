--[[
    @description Ramp to Red Cannon - acts as a path from ramp to the red cannon
    @version 0.1.0
    @author thebest12lines
    @date 4/16/2026
]]
-- will be converted to zepton::MovementPlan at runtime

-- use define_path for paths, and define_pattern for patterns
--define_path( 
--    nil,              -- path_name (leave nil for filename)
--    "red_cannon"      -- identifier (leave nil for custom/handled)
--    "0.1.0"           -- version (required)
--    "ramp to red cannon"   -- readable_path_name (leave nil for filename)
--
--)
-- newer walk function that uses studs than seconds
press_keys({KEY_SPACE}, 0.5)
walk({KEY_RIGHT, KEY_FORWARD}, 21.75)
press_keys({KEY_SPACE}, 0.5)
walk({KEY_RIGHT}, 21.75)
press_keys({KEY_ROTRIGHT}, 0.1)
press_keys({KEY_ROTRIGHT}, 0.1)
press_keys({KEY_ROTRIGHT}, 0.1)
press_keys({KEY_ROTRIGHT}, 0.1)
