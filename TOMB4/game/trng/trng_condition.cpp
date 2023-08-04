#include "../../tomb4/pch.h"

#include "../../specific/function_stubs.h"
#include "../../specific/file.h"
#include "../control.h"
#include "../effects.h"
#include "../objects.h"
#include "../door.h"
#include "../items.h"
#include "../lot.h"
#include "../lara.h"
#include "trng.h"
#include "trng_condition.h"
#include "trng_extra_state.h"

bool NGCondition(short param, unsigned char extra, short timer) {
	switch (timer) {
		// Lara status is enabled/disabled
		case LARA_STATUS_IS_ENABLED_OR_DISABLED: {
			switch (param) {
			// Poisoned
			case 2:
				if ((lara_item->poisoned) == (bool)extra)
					return true;
				break;
			// Lara touching floor
			case 4:
				if ((lara_item->pos.y_pos == lara_item->floor) == (bool)extra)
					return true;
				break;
			default:
				printf("Unimplemented NGCondition Lara Status %u\n", param);
				break;
			}
			break;
		}
		default:
			printf("Unimplemented NGCondition %u\n", timer);
			break;
		};
	return false;
};