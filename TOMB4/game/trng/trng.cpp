#include "../../tomb4/pch.h"

#include "../../specific/function_stubs.h"
#include "../control.h"
#include "../effects.h"
#include "../objects.h"
#include "../lara.h"
#include "trng.h"
#include "trng_extra_state.h"

void NGSetup() {
	NGSetupExtraState();
}

void NGFrameFinish() {
	NGFrameFinishExtraState();
}

bool NGUseNGFlipEffects() {
	return true;
}