# Tomb4Plus changelog

## [0.2.1.0-alpha](https://github.com/SaracenOne/Tomb4Plus/releases/tag/0.2.1.0-alpha) (April 17th 2024)

- Fixed major regression when climbing around an inner corner.
- Added WEATHER_SET_X_DISTANCE_FOG_VALUE flipeffect.
- Added WEATHER_SET_X_END_FOG_LIMIT_FOR_DISTANCE_FOG flipeffect.
- Added multiplier when adding shotgun shell equipment.
- Added music track playback when entering a boat.
- Improved accuracy of CREATURE_IS_CURRENTLY condition.
- Improved accuracy of OPEN_OR_CLOSE_ITEM action.
- Improved accuracy of KILL_OBJECT action.
- Improved accuracy of TRNG actions oneshot flag.
- Added log warnings when unsupported OCB codes are used for tightrope.
- Added CUST_KEEP_DEAD_ENEMIES.
- Added CUST_LOOK_TRANSPARENT.
- Fixed CUST_SET_STATIC_DAMAGE.
- Fixed CUST_HAIR_TYPE.
- Added FMOV_HEAVY_AT_END, FMOV_TRIGGERS_ALL, and FMOV_HEAVY_ALL flags.
- Fixed position of audio for when moving statics.
- Added support for alternative sample rates in XAudio2.
- Fixed fog rendering in title level.
- Added dart and falling block customisation.
- Fixed camera targets not reseting when calling triggergroups.

## [0.2.0.0-alpha](https://github.com/SaracenOne/Tomb4Plus/releases/tag/0.2.0.0-alpha) (April 9th 2024)

- Added support for native 64-bit build.
- Added early BGFX OpenGL renderer to replace legacy D3D renderer.
- Added MSAA support to BGFX renderer.
- Added support for volumetric and coloured distance fog in the same level.
- Made application Unicode aware.
- Added optional TR5-style inventory shading.
- Made special object point light ambient calculations more accurate to TR4.
- De-hardcoded static object shatter properties.
- Experimental Chronicles-style coloured fog.
- Replaced Tomb4 distance fog setting with option to speecify a minimum polygon clip range.
- Added movement with analog stick.
- Added freeze camera cheat.
- Improved compatibility with TRNG existing triggers, scripting, animation, MultiEnvConditions, and other features (thanks to Paolone for releasing the TRNG source code).
- Added TRNG standard string print trigger.
- Added Early WIP boat implementation.
- Fixed TRNG fog setting and add support for negative fog values.
- Added support for early SOUND_SET_X_VOLUME_FOR_AUDIO_TRACK_ON_CHANNEL trigger.
- Fixed ng_input_simulate_oneshot.
- Added support for TRNG variable comparison conditionals.
- Added CUST_CAMERA support.
- Added SCREEN_FLASH_SCREEN_WITH_LIGHT_COLOR_FOR_DURATION.
- Added ANIMCOMMAND_ACTIVATE_HEAVY_TRIGGERS_IN_SECTOR_WHERE_LARA_IS.
- Added TRIGGER_SET_X_MOVEABLE_AS_ACTIVE_ITEM.
- Added TRIGGER_REMOVE_X_MOVEABLE_AS_ACTIVE_ITEM.
- Added TRIGGERGROUP_ENABLE_NEWLY_THE_ONESHOT_X_TRIGGERGROUP_ALREADY_PERFORMED.
- Added support for swamp rooms.
- Fixed ItemGroup triggers.
- Improved buggy AssignSlot behaviour.
- Implemented more LARA_IS_PERFORMING_X_ACTION conditional checks.
- Partially implemented CUST_HAIR_TYPE.
- Added GT_LOADED_SAVEGAME.
- Added GT_USED_BIG_MEDIPACK.
- Added GT_USED_LITTLE_MEDIPACK.
- Changed detection method for presence of TRNG triggers.
- Partially implementated ENEMY_EFFECTS_APPLY_ON_X_MOVEABLE_THE_E_EFFECT.
- Added additional StaticFlags support.
- Fix selecting items activating irrelevant inverted global triggers.
- Support for TREP smash and kill rolling balls.
- Added CHANGE_ACCEL FURR opcode.
- Added CHANGE_VERT_ACCEL FURR opcode.
- Added LOAD_LEVEL FURR opcode.
- Added ACTIVATE_ITEM FURR opcode.
- Added DRAW_ITEM FURR opcode.
- Added SWAP_LARA_MESH FURR opcode.
- Added INC_HP FURR opcode.
- Added INC_AIR FURR opcode.
- Added SET_POISON FURR opcode.
- Added ADD_KEY FURR opcode.
- Added REMOVE_KEY FURR opcode.
- Added ADD_QUESTITEM FURR opcode.
- Added REMOVE_QUESTITEM FURR opcode.
- Added SET_WATERSKIN1 FURR opcode.
- Added SET_WATERSKIN2 FURR opcode.
- Added SET_PUZZLEITEM FURR opcode.
- Added ADD_PUZZLEITEM FURR opcode.
- Added ADD_PUZZLECOMBO FURR opcode.
- Added ADD_SUPPLY FURR opcode.
- Added ADD_WEAPON FURR opcode.
- Added REMOVE_WEAPON FURR opcode.
- Added SET_HP FURR opcode.
- Added RESET FURR opcode (incomplete).
- Added IF_LARA opcodes.
- Fixed ROTATE FURR opcode.
- Added disable_sentry_flame_attack toggle.
- Improved bar customisation flexibility.
- Added envmap sprite index customisation.
- Added fallback for invalid Lara meshswaps in savegames.
- Fixed game_data directory creation.
- Reverted default looped audio track range to original.
- Added Workaround for drawing objects with invalid mesh and animation data.
- Fixed Last Revelation cutscene skipper mesh swaps.
- Fixed Lara rotating when interacting with a small switch.
- Improved accuracy of CreatureAnimation function.
- Added optional 'fix_vertical_water_warp' toggle.
- Fixed fog in train levels.

## [0.1.0.1-alpha](https://github.com/SaracenOne/Tomb4Plus/releases/tag/0.1.0.1-alpha) (February 15th 2024)

- Fix typo in README.md file linking to tomb4_feature_extractor.

## [0.1.0.0-alpha](https://github.com/SaracenOne/Tomb4Plus/releases/tag/0.1.0.0-alpha) (February 14th 2024)

Initial release.