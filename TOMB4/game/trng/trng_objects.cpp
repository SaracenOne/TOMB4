#include "../../tomb4/pch.h"

#include "../control.h"
#include "../lara.h"
#include "../gameflow.h"
#include "../objects.h"

#include "objects/trng_tightrope.h"
#include "objects/trng_parallel_bars.h"

void NGSetupNGObjects() {
	OBJECT_INFO* obj;

	obj = &objects[TIGHT_ROPE];
	obj->initialise = InitialiseTightRope;
	obj->collision = TightRopeCollision;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[PARALLEL_BARS];
	obj->collision = ParallelBarsCollision;
}