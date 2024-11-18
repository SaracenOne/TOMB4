#include "../../tomb4/pch.h"

#include "../../global/types.h"
#include "../../game/control.h"
#include "../../game/lara.h"
#include "../../game/camera.h"
#include "../../game/items.h"

void ControlTeleporter(short item_number) {
	ITEM_INFO *item = &items[item_number];

	if (TriggerActive(item)) {
		item->flags &= ~(IFL_CODEBITS);

		lara_item->pos.x_pos = item->pos.x_pos;
		lara_item->pos.y_pos = item->pos.y_pos;
		lara_item->pos.z_pos = item->pos.z_pos;
		lara_item->pos.y_rot = item->pos.y_rot + 0x8000;

		if (lara_item->room_number != item->room_number)
			ItemNewRoom(lara.item_number, item->room_number);

		camera.fixed_camera = 1;
	}
}
