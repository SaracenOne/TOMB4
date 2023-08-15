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
#include "../newinv.h"
#include "trng.h"
#include "trng_arithmetic.h"
#include "trng_condition.h"
#include "trng_extra_state.h"

bool NGCondition(short param, unsigned char extra, short timer) {
	switch (timer) {
	case INVENTORY_ITEM_IS_PRESENT: {
		// TODO: Does not yet cover all object types
		return have_i_got_object(param);
	}
	case INVENTORY_ITEM_HAS_AT_LEAST: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: INVENTORY_ITEM_HAS_AT_LEAST unimplemented!");
		return false;
		break;
	}
	case KEYBOARD_SCANCODE_IS_CURRENTLY: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: KEYBOARD_SCANCODE_IS_CURRENTLY unimplemented!");
		return false;
		break;
	}
	case CREATURE_IS_CURRENTLY_OF_STATE: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: CREATURE_IS_CURRENTLY_OF_STATE unimplemented!");
		return false;
		break;
	}
	case CREATURE_CURRENT_ANIMATION_0_31_IS: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: CREATURE_CURRENT_ANIMATION_IS unimplemented!");
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
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: Unimplemented NGCondition Lara Status %u!", param);
				break;
		}
		break;
	}
	case LARA_IS_TOUCHING_MOVEABLE: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: LARA_IS_TOUCHING_MOVEABLE unimplemented!");
		return false;
		break;
	}
	case LARA_IS_PERFORMING_ANIMATION: {
		if (lara_item->current_anim_state - objects[LARA].anim_index == param) {
			return true;
		} else {
			return false;
		}
		break;
	}
	case LARA_IS_TOUCHING_STATIC_ITEM: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: LARA_IS_TOUCHING_STATIC_ITEM unimplemented!");
		return false;
		break;
	}
	case LARA_IS_HOLDING_OR_DRIVING_ITEMS: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: LARA_IS_HOLDING_OR_DRIVING_ITEMS unimplemented!");
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
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: LARA_IS_LESS_OR_EVEN_CLICKS_DISTANT_TO_MOVEABLE unimplemented!");
		return false;
		break;
	}
	case LARA_IS_TOUCHING_MOVEABLE_WITH_MESH_NUMBER: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: LARA_IS_TOUCHING_MOVEABLE_WITH_MESH_NUMBER unimplemented!");
		return false;
		break;
	}
	default:
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: %u unimplemented!", timer);
		break;
	};

	return false;
};