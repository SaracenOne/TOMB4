#include "../../tomb4/pch.h"

#include "../../specific/function_stubs.h"
#include "../control.h"
#include "../effects.h"
#include "../objects.h"
#include "../lara.h"
#include "../gameflow.h"
#include "trng.h"
#include "trng_extra_state.h"
#include "trng_script_parser.h"

#include "../../tomb4/mod_config.h"

void NGSetup() {
	NGSetupExtraState();
	NGLoadTables(gfCurrentLevel);
}

void NGFrameFinish() {
	NGFrameFinishExtraState();
}

bool NGUseNGFlipEffects() {
	MOD_GLOBAL_INFO global_info = get_game_mod_global_info();

	return global_info.trng_flipeffects_enabled;
}

bool NGUseNGActions() {
	MOD_GLOBAL_INFO global_info = get_game_mod_global_info();

	return global_info.trng_actions_enabled;
}

void NGInit() {
	NGInitLevelArray();
}

void NGCleanup() {
	NGScriptCleanup();
}