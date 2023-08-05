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
#include "trng_arithmetic.h"
#include "trng_condition.h"
#include "trng_extra_state.h"

bool NGCondition(short param, unsigned char extra, short timer) {
	switch (timer) {
	case INVENTORY_ITEM_IS_PRESENT: {
		printf("Unimplemented NGCondition INVENTORY_ITEM_IS_PRESENT\n");
		return false;
		break;
	}
	case INVENTORY_ITEM_HAS_AT_LEAST: {
		printf("Unimplemented NGCondition INVENTORY_ITEM_HAS_AT_LEAST\n");
		return false;
		break;
	}
	case KEYBOARD_SCANCODE_IS_CURRENTLY: {
		printf("Unimplemented NGCondition KEYBOARD_SCANCODE_IS_CURRENTLY\n");
		return false;
		break;
	}
	case CREATURE_IS_CURRENTLY_OF_STATE: {
		printf("Unimplemented NGCondition CREATURE_IS_CURRENTLY_OF_STATE\n");
		return false;
		break;
	}
	case CREATURE_CURRENT_ANIMATION_0_31_IS: {
		printf("Unimplemented NGCondition CREATURE_CURRENT_ANIMATION_IS\n");
		return false;
		break;
	}
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
	case LARA_IS_TOUCHING_MOVEABLE: {
		printf("Unimplemented NGCondition LARA_IS_TOUCHING_MOVEABLE\n");
		return false;
		break;
	}
	case LARA_IS_PERFORMING_ANIMATION: {
		printf("Unimplemented NGCondition LARA_IS_PERFORMING_ANIMATION\n");
		return false;
		break;
	}
	case LARA_IS_TOUCHING_STATIC_ITEM: {
		printf("Unimplemented NGCondition LARA_IS_TOUCHING_STATIC_ITEM\n");
		return false;
		break;
	}
	case LARA_IS_HOLDING_OR_DRIVING_ITEMS: {
		printf("Unimplemented NGCondition LARA_IS_HOLDING_OR_DRIVING_ITEMS\n");
		return false;
		break;
	}
	case NUMERIC_VALUE_IS_LESS_THAN: {
		if (extra < NGNumericGetVariable(param))
			return true;
		else
			return false;
	}
	case NUMERIC_VALUE_IS_EQUAL_OR_GREATER_TO: {
		if (extra >= NGNumericGetVariable(param))
			return true;
		else
			return false;
	}
	case NUMERIC_VALUE_IS_EQUAL_TO: {
		if (extra == NGNumericGetVariable(param))
			return true;
		else
			return false;
	}
	case LARA_IS_LESS_OR_EVEN_CLICKS_DISTANT_TO_MOVEABLE: {
		printf("Unimplemented NGCondition LARA_IS_LESS_OR_EVEN_CLICKS_DISTANT_TO_MOVEABLE\n");
		return false;
		break;
	}
	case LARA_IS_TOUCHING_MOVEABLE_WITH_MESH_NUMBER: {
		printf("Unimplemented NGCondition LARA_IS_TOUCHING_MOVEABLE_WITH_MESH_NUMBER\n");
		return false;
		break;
	}
	default:
		printf("Unimplemented NGCondition %u\n", timer);
		break;
	};

	return false;
};