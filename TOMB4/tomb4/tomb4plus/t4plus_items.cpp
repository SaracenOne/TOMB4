#include "../../tomb4/pch.h"

#include "../../specific/function_stubs.h"
#include "t4plus_items.h"

#include "../../game/control.h"
#include "../../game/lot.h"
#include "../../game/items.h"
#include "../../game/objects.h"

void T4PlusActivateItem(int item_id, bool anti) {
	ITEM_INFO* item;

	item = &items[item_id];

	if (!item->active) {
		if (anti) {
			item->flags &= ~(IFL_CODEBITS | IFL_REVERSE);
			return;
		}

		item->flags |= IFL_CODEBITS;

		if (objects[item->object_number].intelligent) {
			if (item->status == ITEM_INACTIVE) {
				item->touch_bits = 0;
				item->status = ITEM_ACTIVE;
				AddActiveItem(item_id);
				EnableBaddieAI(item_id, 1);
			}
			else if (item->status == ITEM_INVISIBLE) {
				item->touch_bits = 0;

				if (EnableBaddieAI(item_id, 0))
					item->status = ITEM_ACTIVE;
				else
					item->status = ITEM_INVISIBLE;

				AddActiveItem(item_id);
			}
		}
		else {
			item->touch_bits = 0;
			AddActiveItem(item_id);
			item->status = ITEM_ACTIVE;
		}
	}
	else {
		if (!anti) {
			item->flags |= IFL_CODEBITS;
			return;
		}

		if (item->object_number == EARTHQUAKE) {
			item->item_flags[0] = 0;
			item->item_flags[1] = 100;
		}

		item->flags &= ~(IFL_CODEBITS | IFL_REVERSE);
	}
}