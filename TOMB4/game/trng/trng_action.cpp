#include "../../tomb4/pch.h"

#include "../../specific/function_stubs.h"
#include "../../specific/file.h"
#include "../control.h"
#include "../effects.h"
#include "../objects.h"
#include "../door.h"
#include "../items.h"
#include "../lot.h"
#include "trng.h"
#include "trng_action.h"
#include "trng_extra_state.h"
#include "../effect2.h"
#include "../lara1gun.h"
#include "../sound.h"
#include "../box.h"

void NGItemActivator(int item_id, bool anti) {
	ITEM_INFO* item;

	item = &items[item_id];

	if (!item->active)
	{
		if (anti)
			return;

		item->flags |= IFL_CODEBITS;

		if (objects[item->object_number].intelligent)
		{
			if (item->status == ITEM_INACTIVE)
			{
				item->touch_bits = 0;
				item->status = ITEM_ACTIVE;
				AddActiveItem(item_id);
				EnableBaddieAI(item_id, 1);
			}
			else if (item->status == ITEM_INVISIBLE)
			{
				item->touch_bits = 0;

				if (EnableBaddieAI(item_id, 0))
					item->status = ITEM_ACTIVE;
				else
					item->status = ITEM_INVISIBLE;

				AddActiveItem(item_id);
			}
		}
		else
		{
			item->touch_bits = 0;
			AddActiveItem(item_id);
			item->status = ITEM_ACTIVE;
		}
	} else {
		if (!anti)
			return;

		RemoveActiveItem(item_id);
		item->status = ITEM_INACTIVE;
		item->flags &= ~IFL_CODEBITS;
	}
}

int NGFindIndexForLaraStartPosWithMatchingOCB(unsigned int ocb) {
	for (int i = 0; i < nAIObjects; i++) {
		if (AIObjects[i].object_number == LARA_START_POS && ocb == AIObjects[i].trigger_flags) {
			return i;
		}
	}

	return -1;
}

void NGForceItemAnimation(unsigned short item_id, unsigned int animation) {
	items[item_id].anim_number = objects[items[item_id].object_number].anim_index + animation;
	items[item_id].frame_number = anims[items[item_id].anim_number].frame_base;
}

void NGHurtEnemy(unsigned short item_id, unsigned short damage) {
	if (items[item_id].hit_points > 0) {
		items[item_id].hit_points -= damage;
	}
}

int NGActionTrigger(unsigned short param, unsigned short extra, short timer, bool heavy) {
	unsigned char action_type = (unsigned char)extra & 0xff;
	unsigned char action_data = (unsigned char)(extra >> 8) & 0xff;
	
	if(!heavy)
		NGStoreBackupTriggerRoomAndIndex();

	int result = NGAction(param, extra, (heavy || !NGCheckActionFloorStatePressedThisFrameOrLastFrame(heavy)));

	// Replicates a weird bug in the original
	if (action_type == TRIGGER_MOVEABLE_ACTIVATE_WITH_TIMER || action_type == UNTRIGGER_MOVEABLE_ACTIVATE_WITH_TIMER || action_type == OPEN_OR_CLOSE_DOOR_ITEM) {
		if (!NGCheckActionFloorStatePressedThisFrameOrLastFrame(heavy)) {
			ITEM_INFO* item;

			item = &items[param];
			if (item->active) {
				item->timer = timer * 30;
			}
		}
	}

	if (!heavy)
		NGRestoreBackupTriggerRoomAndIndex();

	return result;
}

int NGAction(unsigned short param, unsigned short extra, bool first_frame) {
	unsigned char action_type = (unsigned char)extra & 0xff;
	unsigned char action_data = (unsigned char)(extra >> 8) & 0xff;

	short item_id = -1;
	if (first_frame) {
		item_id = param;
	}

	if (item_id < 0) {
		return -1;
	}

	switch (action_type) {
		case TURN_ANIMATING_MOVING_ENDLESSLY_IN_WAY: {
			// TODO: values are estimated and may not be accurate
			switch (action_data) {
				// Clockwise slowly (one degree per frame)
				case 0x00:
					NGSetAutoRotationPerFrame(item_id, 182);
					break;
				// Clockwise fastly (two degrees per frame)
				case 0x01:
					NGSetAutoRotationPerFrame(item_id, 182 * 2);
					break;
				// Inverse Clockwise slowly (one degree per frame)
				case 0x02:
					NGSetAutoRotationPerFrame(item_id, -182);
					break;
				// Inverse Clockwise fastly (two degrees per frame)
				case 0x03:
					NGSetAutoRotationPerFrame(item_id, -182 * 2);
					break;
			}
			break;
		}
		case PERFORM_FLIPEFFECT_ON_ITEM: {
			if (first_frame) {
				effect_routines[action_data](&items[item_id]);
			}
			break;
		}
		// TODO: the kill behaviour needs to be re-evaluated
		case KILL_OBJECT: {
			if (first_frame) {
				switch (action_data) {
					// 0 vitality
					case 0x00: {
						items[item_id].hit_points = 0;
						break;
					}
					// Remove immediately
					case 0x01: {
						ITEM_INFO *item = &items[item_id];
						item->status = ITEM_INVISIBLE;
						RemoveActiveItem(item_id);
						DisableBaddieAI(item_id);
						break;
					}
					// Explosion
					case 0x02: {
						ITEM_INFO* item = &items[item_id];

						TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -2, 0, item->room_number);
						for (int i = 0; i < 3; i++)
							TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -1, 0, item->room_number);

						SoundEffect(SFX_EXPLOSION1, &item->pos, 0x1800004);
						SoundEffect(SFX_EXPLOSION2, &item->pos, 0);

						item->status = ITEM_INVISIBLE;
						RemoveActiveItem(item_id);
						DisableBaddieAI(item_id);

						break;
					}
					// Underwater Explosion
					case 0x03: {
						ITEM_INFO* item = &items[item_id];

						TriggerUnderwaterExplosion(item, 0);

						SoundEffect(SFX_EXPLOSION1, &item->pos, 0x1800004);
						SoundEffect(SFX_EXPLOSION2, &item->pos, 0);

						item->status = ITEM_INVISIBLE;
						RemoveActiveItem(item_id);
						DisableBaddieAI(item_id);

						break;
					}
					// Explode Creature
					case 0x04: {
						ITEM_INFO* item = &items[item_id];

						SoundEffect(SFX_EXPLOSION1, &item->pos, 0x1800004);
						SoundEffect(SFX_EXPLOSION2, &item->pos, 0);

						item->hit_points = 0;
						CreatureDie(item_id, 1);

						break;
					}
					// Creature Die
					case 0x05: {
						ITEM_INFO* item = &items[item_id];

						bool is_invisible = item->flags & IFL_INVISIBLE;
						CreatureDie(item_id, 0);
						if (!is_invisible)
							item->flags = item->flags & ~IFL_INVISIBLE;

						item->after_death = 1;
						item->hit_points = 0;

						break;
					}
					// Hide
					case 0x06: {
						ITEM_INFO* item = &items[item_id];
						item->status = ITEM_INVISIBLE;
						RemoveActiveItem(item_id);
						DisableBaddieAI(item_id);
						break;
					}
					// Antitrigger
					case 0x07: {
						NGItemActivator(item_id, true);
						break;
					}
					// Smoke emitter
					case 0x08: {
						ITEM_INFO* item = &items[item_id];
						item->flags = item->flags & ~(IFL_CODEBITS | IFL_REVERSE);
						break;
					}
					default: {
						printf("Unimplemented KILL_OBJECT parameter\n");
						break;
					}
				}
			}
			break;
		}
		case FREEZE_ENEMY_FOR_SECONDS: {
			if (first_frame) {
				if (!NGIsItemFrozen(param)) {
					if (action_data == 0) {
						NGSetItemFreezeTimer(item_id, -1);
					}
					else {
						NGSetItemFreezeTimer(item_id, action_data * 30);
					}
				}
			}
			break;
		}
		case UNFREEZE_ENEMY_WITH_EFFECT: {
			if (!first_frame) {
				if (action_data != 0x00) {
					Log(0, "Unimplemented action data for UNFREEZE_ENEMY_WITH_EFFECT");
				}

				if (NGIsItemFrozen(item_id)) {
					// TODO: action_data signifies a special effect when unfreezeing
					NGSetItemFreezeTimer(item_id, 0);
				}
			}
			break;
		}
		case HURT_ENEMY: {
			if (first_frame) {
				NGHurtEnemy(item_id, action_data & 0x7f);
			}
			break;
		}
		case MOVE_ITEM_IMMEDIATELY_TO_LARA_START_POS_WITH_MATCHING_OCB_SETTINGS: {
			if (first_frame) {
				int lara_start_pos_id = NGFindIndexForLaraStartPosWithMatchingOCB(action_data & 0x7f);
				if (lara_start_pos_id >= 0) {
					AIOBJECT *ai = &AIObjects[lara_start_pos_id];
					if (ai) {
						items[item_id].pos.x_pos = ai->x;
						items[item_id].pos.y_pos = ai->y;
						items[item_id].pos.z_pos = ai->z;
						items[item_id].pos.y_rot = ai->y_rot;
					}
				}
			}
			break;
		}
		case TRIGGER_MOVEABLE_ACTIVATE_WITH_TIMER: {
			if (first_frame) {
				items[param].timer = ((short)(action_data & 0x7f)) * 30;
				NGItemActivator(item_id, false);
			}
			break;
		}
		case UNTRIGGER_MOVEABLE_ACTIVATE_WITH_TIMER: {
			if (first_frame) {
				items[param].timer = ((short)(action_data & 0x7f)) * 30;
				NGItemActivator(item_id, true);
			}
			break;
		}
		case EFFECT_ADD_TO_ENEMY: {
			if (first_frame) {
				printf("EFFECT_ADD_TO_ENEMY not yet implemented!\n");
			}
			break;
		}
		case EFFECT_REMOVE_TO_ENEMY: {
			if (first_frame) {
				printf("EFFECT_REMOVE_TO_ENEMY not yet implemented!\n");
			}
			break;
		}
		case SHOW_TRIGGER_COUNTDOWN_TIMER_FOR_ENEMY: {
			if (first_frame) {
				NGSetDisplayTimerForMoveableWithType(item_id, (NGTimerTrackerType)(action_data & 0x7f));
			}
			break;
		}
		case FORCE_ANIMATION_0_TO_31_ON_ITEM: {
			if (first_frame) {
				NGForceItemAnimation(item_id, action_data & 0x1f);
			}
			break;
		}
		case FORCE_ANIMATION_32_TO_63_ON_ITEM: {
			if (first_frame) {
				NGForceItemAnimation(item_id, (action_data & 0x1f) + 32);
			}
			break;
		}
		case FORCE_ANIMATION_64_TO_95_ON_ITEM: {
			if (first_frame) {
				NGForceItemAnimation(item_id, (action_data & 0x1f) + 64);
			}
			break;
		}
		case OPEN_OR_CLOSE_DOOR_ITEM: {
			if (first_frame) {
				items[item_id].timer = ((short)(action_data & 0x7f)) * 30;
				NGItemActivator(item_id, false);

				if (action_data) {
					items[item_id].flags |= IFL_CODEBITS;
				} else {
					items[item_id].flags &= ~IFL_CODEBITS;
				}
			}
			break;
		}
		case MOVE_ITEM_UP_FOR_CLICKS:
			if (first_frame) {
				if (NGGetItemUpDownTimer(item_id) == 0) {
					NGSetItemUpDownTimer(item_id, (action_data + 1) * 8);
				}
			}
			break;
		case MOVE_ITEM_DOWN_FOR_CLICKS:
			if (first_frame) {
				if (NGGetItemUpDownTimer(item_id) == 0) {
					NGSetItemUpDownTimer(item_id, (action_data + 1) * -8);
				}
			}
			break;
		case MOVE_ITEM_WEST_FOR_CLICKS:
			if (first_frame) {
				if (NGGetItemEastWestTimer(item_id) == 0) {
					NGSetItemEastWestTimer(item_id, (action_data + 1) * -8);
				}
			}
			break;
		case MOVE_ITEM_SOUTH_FOR_CLICKS:
			if (first_frame) {
				if (NGGetItemNorthSouthTimer(item_id) == 0) {
					NGSetItemNorthSouthTimer(item_id, (action_data + 1) * -8);
				}
			}
			break;
		case MOVE_ITEM_EAST_FOR_CLICKS:
			if (first_frame) {
				if (NGGetItemEastWestTimer(item_id) == 0) {
					NGSetItemEastWestTimer(item_id, (action_data + 1) * 8);
				}
			}
			break;
		case MOVE_ITEM_NORTH_FOR_CLICKS:
			if (first_frame) {
				if (NGGetItemNorthSouthTimer(item_id) == 0) {
					NGSetItemNorthSouthTimer(item_id, (action_data + 1) * 8);
				}
			}
			break;
		case DISABLE_ITEM_COLLISION:
			if (first_frame)
				NGDisableItemCollision(item_id);
			break;
		case ENABLE_ITEM_COLLISION:
			if (first_frame)
				NGEnableItemCollision(item_id);
			break;
		case MOVE_ITEM_UP_BY_UNITS_X8: {
			if (first_frame)
				NGMoveItemByUnits(item_id, NG_UP, 8 * ((action_data)+1));
			break;
		}
		case MOVE_ITEM_DOWN_BY_UNITS_X8: {
			if (first_frame)
				NGMoveItemByUnits(item_id, NG_DOWN, 8 * ((action_data)+1));
			break;
		}
		case MOVE_ITEM_WEST_BY_UNITS_X8: {
			if (first_frame)
				NGMoveItemByUnits(item_id, NG_WEST, 8 * ((action_data)+1));
			break;
		}
		case MOVE_ITEM_NORTH_BY_UNITS_X8: {
			if (first_frame)
				NGMoveItemByUnits(item_id, NG_NORTH, 8 * ((action_data)+1));
			break;
		}
		case MOVE_ITEM_EAST_BY_UNITS_X8: {
			if (first_frame)
				NGMoveItemByUnits(item_id, NG_EAST, 8 * ((action_data)+1));
			break;
		}
		case MOVE_ITEM_SOUTH_BY_UNITS_X8: {
			if (first_frame)
				NGMoveItemByUnits(item_id, NG_SOUTH, 8 * ((action_data)+1));
			break;
		}
		case CREATURE_FORCE_FRAME_OF_CURRENT_ANIMATION: {
			if (first_frame) {
				ITEM_INFO* item = &items[item_id];
				if (item) {
					item->frame_number = anims[item->anim_number].frame_base + action_data;
				}
			}
			break;
		}
		case CREATURE_FORCE_STATE_ID: {
			if (first_frame) {
				ITEM_INFO* item = &items[item_id];
				if (item) {
					item->current_anim_state = action_data; // Do we need to change the goal state too or not?
				}
			}
			break;
		}
		default:
			if (first_frame)
				printf("Unimplemented NGTrigger %u\n", action_type);
			break;
		};
	return item_id;
};