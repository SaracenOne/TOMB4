#include "../../tomb4/pch.h"

#include "trng.h"
#include "trng_action.h"
#include "trng_extra_state.h"

#include "../../specific/function_stubs.h"
#include "../../specific/file.h"
#include "../../tomb4/mod_config.h"

#include "../../tomb4/tomb4plus/t4plus_items.h"

#include "../control.h"
#include "../effects.h"
#include "../objects.h"
#include "../door.h"
#include "../items.h"
#include "../lot.h"
#include "../effect2.h"
#include "../lara1gun.h"
#include "../sound.h"
#include "../box.h"
#include "../camera.h"
#include "../spotcam.h"
#include "../lara.h"

void NGHurtEnemy(unsigned short item_id, unsigned short damage) {
	if (items[item_id].hit_points > 0) {
		items[item_id].hit_points -= damage;
	}
}

NGActionRepeatType NGActionTrigger(unsigned short param, unsigned short extra, short timer, bool is_heavy_triggered) {
	unsigned char action_type = (unsigned char)extra & 0xff;
	unsigned char action_data = (unsigned char)(extra >> 8) & 0xff;
	
	NGStoreBackupTriggerRoomAndIndex();

	bool already_triggered = NGIsActionOneShotTriggeredForTile() || NGCheckActionFloorStatePressedThisFrameOrLastFrame(is_heavy_triggered);

	NGActionRepeatType repeat_type = NG_ACTION_REPEAT_TYPE_ON_REENTRY;

	if (!already_triggered) {
		int flags = 0;
		if (is_heavy_triggered) {
			flags |= NG_TRIGGER_FLAG_HEAVY;
		}
		repeat_type = NGAction(param, extra, flags);
		NGRegisterTriggeredItemForTimerfield(param);
	}

	// Replicates a weird bug in the original (?)
	if (action_type == TRIGGER_MOVEABLE_ACTIVATE_WITH_TIMER || action_type == UNTRIGGER_MOVEABLE_ACTIVATE_WITH_TIMER) {
		if (!NGCheckActionFloorStatePressedThisFrameOrLastFrame(is_heavy_triggered)) {
			ITEM_INFO* item;

			item = &items[param];
			if (item->active) {
				item->timer = timer * 30;
			}
		}
	}

	NGRestoreBackupTriggerRoomAndIndex();

	return repeat_type;
}

void NGTurnItemBy45DegreeHorizontalIncrements(int item_id,  int increments, int speed) {
	NGSetItemHorizontalRotationSpeed(item_id, NG_DEGREE(speed));

	switch (increments) {
		// Clockwise slowly (one degree per frame)
	case 0x00:
		if (speed > 0)
			NGSetItemHorizontalRotationRemaining(item_id, NG_DEGREE(45));
		else
			NGSetItemHorizontalRotationRemaining(item_id, NG_DEGREE(-45));
		break;
	case 0x01:
		if (speed > 0)
			NGSetItemHorizontalRotationRemaining(item_id, NG_DEGREE(90));
		else
			NGSetItemHorizontalRotationRemaining(item_id, NG_DEGREE(-90));
		break;
	case 0x02:
		if (speed > 0)
			NGSetItemHorizontalRotationRemaining(item_id, NG_DEGREE(135));
		else
			NGSetItemHorizontalRotationRemaining(item_id, NG_DEGREE(-135));
		break;
	case 0x03:
		if (speed > 0)
			NGSetItemHorizontalRotationRemaining(item_id, NG_DEGREE(180));
		else
			NGSetItemHorizontalRotationRemaining(item_id, NG_DEGREE(-180));
		break;
	case 0x04:
		if (speed > 0)
			NGSetItemHorizontalRotationRemaining(item_id, NG_DEGREE(225));
		else
			NGSetItemHorizontalRotationRemaining(item_id, NG_DEGREE(-225));
		break;
	case 0x05:
		if (speed > 0)
			NGSetItemHorizontalRotationRemaining(item_id, NG_DEGREE(270));
		else
			NGSetItemHorizontalRotationRemaining(item_id, NG_DEGREE(-270));
		break;
	case 0x06:
		if (speed > 0)
			NGSetItemHorizontalRotationRemaining(item_id, NG_DEGREE(315));
		else
			NGSetItemHorizontalRotationRemaining(item_id, NG_DEGREE(-315));
		break;
	case 0x07:
		if (speed > 0)
			NGSetItemHorizontalRotationRemaining(item_id, NG_DEGREE(360));
		else
			NGSetItemHorizontalRotationRemaining(item_id, NG_DEGREE(-360));
		break;
		break;
	default:
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGTurnItemBy45DegreeHorizontalIncrements: increment count %u unimplemented!", increments);
		break;
	}
}

void NGTurnItemBy45DegreeVerticalIncrements(int item_id, int increments, int speed) {
	NGSetItemVerticalRotationSpeed(item_id, NG_DEGREE(speed));

	switch (increments) {
		// Clockwise slowly (one degree per frame)
	case 0x00:
		if (speed > 0)
			NGSetItemVerticalRotationRemaining(item_id, NG_DEGREE(45));
		else
			NGSetItemVerticalRotationRemaining(item_id, NG_DEGREE(-45));
		break;
	case 0x01:
		if (speed > 0)
			NGSetItemVerticalRotationRemaining(item_id, NG_DEGREE(90));
		else
			NGSetItemVerticalRotationRemaining(item_id, NG_DEGREE(-90));
		break;
	case 0x02:
		if (speed > 0)
			NGSetItemVerticalRotationRemaining(item_id, NG_DEGREE(135));
		else
			NGSetItemVerticalRotationRemaining(item_id, NG_DEGREE(-135));
		break;
	case 0x03:
		if (speed > 0)
			NGSetItemVerticalRotationRemaining(item_id, NG_DEGREE(180));
		else
			NGSetItemVerticalRotationRemaining(item_id, NG_DEGREE(-180));
		break;
	case 0x04:
		if (speed > 0)
			NGSetItemVerticalRotationRemaining(item_id, NG_DEGREE(225));
		else
			NGSetItemVerticalRotationRemaining(item_id, NG_DEGREE(-225));
		break;
	case 0x05:
		if (speed > 0)
			NGSetItemVerticalRotationRemaining(item_id, NG_DEGREE(270));
		else
			NGSetItemVerticalRotationRemaining(item_id, NG_DEGREE(-270));
		break;
	case 0x06:
		if (speed > 0)
			NGSetItemVerticalRotationRemaining(item_id, NG_DEGREE(315));
		else
			NGSetItemVerticalRotationRemaining(item_id, NG_DEGREE(-315));
		break;
	case 0x07:
		if (speed > 0)
			NGSetItemVerticalRotationRemaining(item_id, NG_DEGREE(360));
		else
			NGSetItemVerticalRotationRemaining(item_id, NG_DEGREE(-360));
		break;
		break;
	default:
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGTurnItemBy45DegreeVerticalIncrements: increment count %u unimplemented!", increments);
		break;
	}
}

NGActionRepeatType NGAction(unsigned short item_id, unsigned short extra, int flags) {
	unsigned char action_type = (unsigned char)extra & 0xff;
	unsigned char action_data = (unsigned char)(extra >> 8) & 0xff;

	if (item_id < 0) {
		NGLog(NG_LOG_TYPE_ERROR, "ActionNG: Negative item ID!");
		return NG_ACTION_REPEAT_TYPE_ALWAYS;
	}

	if (item_id >= ITEM_COUNT) {
		NGLog(NG_LOG_TYPE_ERROR, "Invalid item number overflow.");
		return NG_ACTION_REPEAT_TYPE_ALWAYS;
	}

	NGActionRepeatType repeat_type = NG_ACTION_REPEAT_TYPE_ON_REENTRY;

	switch (action_type) {
		// TODO: values are estimated and may not be accurate.
		// Also need to check the behaviour when an action is already 
		case TURN_X_ANIMATING_MOVING_SLOWLY_IN_CLOCKWISE_OF_DEGREES: {
			if (!NGGetItemHorizontalRotationRemaining(item_id)) {
				NGTurnItemBy45DegreeHorizontalIncrements(item_id, action_data, 1);
			}
			break;
		}
		case TURN_X_ANIMATING_MOVING_SLOWLY_IN_INVERSE_CLOCKWISE_OF_DEGREES: {
			if (!NGGetItemHorizontalRotationRemaining(item_id)) {
				NGTurnItemBy45DegreeHorizontalIncrements(item_id, action_data, -1);
			}
			break;
		}
		case TURN_X_ANIMATING_MOVING_FASTLY_IN_CLOCKWISE_OF_DEGREES: {
			if (!NGGetItemHorizontalRotationRemaining(item_id)) {
				NGTurnItemBy45DegreeHorizontalIncrements(item_id, action_data, 2);
			}
			break;
		}
		case TURN_X_ANIMATING_MOVING_FASTLY_IN_INVERSE_CLOCKWISE_OF_DEGREES: {
			if (!NGGetItemHorizontalRotationRemaining(item_id)) {
				NGTurnItemBy45DegreeHorizontalIncrements(item_id, action_data, -2);
			}
			break;
		}
		case TURN_X_ANIMATING_MOVING_ENDLESSLY_IN_WAY: {
			switch (action_data) {
				// Clockwise slowly (one degree per frame)
			case 0x00:
				NGSetItemHorizontalRotationSpeed(item_id, NG_DEGREE(1));
				NGSetItemHorizontalRotationRemaining(item_id, 0);
				break;
				// Clockwise fastly (two degrees per frame)
			case 0x01:
				NGSetItemHorizontalRotationSpeed(item_id, NG_DEGREE(2));
				NGSetItemHorizontalRotationRemaining(item_id, 0);
				break;
				// Inverse Clockwise slowly (one degree per frame)
			case 0x02:
				NGSetItemHorizontalRotationSpeed(item_id, NG_DEGREE(-1));
				NGSetItemHorizontalRotationRemaining(item_id, 0);
				break;
				// Inverse Clockwise fastly (two degrees per frame)
			case 0x03:
				NGSetItemHorizontalRotationSpeed(item_id, NG_DEGREE(-2));
				NGSetItemHorizontalRotationRemaining(item_id, 0);
				break;
			default:
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TURN_X_ANIMATING_MOVING_ENDLESSLY_IN_WAY: action data %u unimplemented!", action_data);
				break;
			}
			break;
		}
		case TURN_VERTICALLY_X_ANIMATING_MOVING_SLOWLY_IN_CLOCKWISE_OF_DEGREES: {
			if (!NGGetItemVerticalRotationRemaining(item_id)) {
				NGTurnItemBy45DegreeVerticalIncrements(item_id, action_data, 1);
			}
			break;
		}
		case TURN_VERTICALLY_X_ANIMATING_MOVING_SLOWLY_IN_INVERSE_CLOCKWISE_OF_DEGREES: {
			if (!NGGetItemVerticalRotationRemaining(item_id)) {
				NGTurnItemBy45DegreeVerticalIncrements(item_id, action_data, -1);
			}
			break;
		}
		case TURN_VERTICALLY_X_ANIMATING_MOVING_FASTLY_IN_CLOCKWISE_OF_DEGREES: {
			if (!NGGetItemVerticalRotationRemaining(item_id)) {
				NGTurnItemBy45DegreeVerticalIncrements(item_id, action_data, 2);
			}
			break;
		}
		case TURN_VERTICALLY_X_ANIMATING_MOVING_FASTLY_IN_INVERSE_CLOCKWISE_OF_DEGREES: {
			if (!NGGetItemVerticalRotationRemaining(item_id)) {
				NGTurnItemBy45DegreeVerticalIncrements(item_id, action_data, -2);
			}
			break;
		}
		case TURN_VERTICALLY_X_ANIMATING_MOVING_ENDLESS_IN_WAY: {
			switch (action_data) {
				// Clockwise slowly (one degree per frame)
			case 0x00:
				NGSetItemVerticalRotationSpeed(item_id, NG_DEGREE(1));
				NGSetItemVerticalRotationRemaining(item_id, 0);
				break;
				// Clockwise fastly (two degrees per frame)
			case 0x01:
				NGSetItemVerticalRotationSpeed(item_id, NG_DEGREE(2));
				NGSetItemVerticalRotationRemaining(item_id, 0);
				break;
				// Inverse Clockwise slowly (one degree per frame)
			case 0x02:
				NGSetItemVerticalRotationSpeed(item_id, NG_DEGREE(-1));
				NGSetItemVerticalRotationRemaining(item_id, 0);
				break;
				// Inverse Clockwise fastly (two degrees per frame)
			case 0x03:
				NGSetItemVerticalRotationSpeed(item_id, NG_DEGREE(-2));
				NGSetItemVerticalRotationRemaining(item_id, 0);
				break;
			default:
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TURN_VERTICALLY_X_ANIMATING_MOVING_ENDLESS_IN_WAY: action data %u unimplemented!", action_data);
				break;
			}
			break;
		}
		case PERFORM_FLIPEFFECT_ON_ITEM: {
			effect_routines[action_data](&items[item_id]);
			break;
		}
		// TODO: the kill behaviour needs to be re-evaluated
		case KILL_OBJECT: {
			ITEM_INFO* item = &items[item_id];
			// The behaviour for what determines what items we can't kill is very weird.
			// You can explode creatures over and over again even after death, but only after they've been active once.
			// This should recreate behaviour I've observed, but it's still probably not accurate.
			if ((objects[item->object_number].intelligent && !(item->flags & IFL_TRIGGERED)))
				return NG_ACTION_REPEAT_TYPE_ALWAYS;

			switch (action_data) {
				// 0 vitality
				case 0x00: {
					ITEM_INFO* item = &items[item_id];
					item->hit_points = 0;
					break;
				}
				// Remove immediately
				case 0x01: {
					ITEM_INFO *item = &items[item_id];
					item->status = ITEM_INVISIBLE;
					RemoveActiveItem(item_id);
					if (objects[item->object_number].intelligent)
						DisableBaddieAI(item_id);
					item->hit_points = -16384;
					item->collidable = 0;
					item->flags |= IFL_INVISIBLE | IFL_CLEARBODY;
					break;
				}
				// Explosion
				case 0x02: {
					ITEM_INFO* item = &items[item_id];

					TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -2, 0, item->room_number);
					for (int i = 0; i < 3; i++)
						TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -1, 0, item->room_number);

					SoundEffect(SFX_EXPLOSION1, &item->pos, 0x1800000 | SFX_SETPITCH);
					SoundEffect(SFX_EXPLOSION2, &item->pos, 0);

					item->status = ITEM_INVISIBLE;
					RemoveActiveItem(item_id);
					if (objects[item->object_number].intelligent)
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

					break;
				}
				// Explode Creature
				case 0x04: {
					ITEM_INFO* item = &items[item_id];

					SoundEffect(SFX_EXPLOSION1, &item->pos, 0x1800004);
					SoundEffect(SFX_EXPLOSION2, &item->pos, 0);

					item->after_death = 1;
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
					// TODO: This one is weird, not quite sure how to properly recreate it behaviour
					ITEM_INFO *item = &items[item_id];
					item->status = ITEM_INVISIBLE;
					RemoveActiveItem(item_id);
					if (objects[item->object_number].intelligent)
						DisableBaddieAI(item_id);
					break;
				}
				// Antitrigger
				case 0x07: {
					ITEM_INFO* item = &items[item_id];
					if (item->status != ITEM_INVISIBLE)
						item->status = ITEM_INACTIVE;
					item->flags = item->flags & ~(IFL_CODEBITS | IFL_REVERSE);
					RemoveActiveItem(item_id);
					if (objects[item->object_number].intelligent)
						DisableBaddieAI(item_id);
					break;
				}
				// Smoke emitter
				case 0x08: {
					ITEM_INFO* item = &items[item_id];
					item->flags = item->flags & ~(IFL_CODEBITS | IFL_REVERSE);
					break;
				}
				default: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "KILL_OBJECT: action data %u unimplemented!", action_data);
					break;
				}
			}
			break;
		}
		case FREEZE_ENEMY_FOR_SECONDS: {
			// TODO: need to re-examine this function. Right now, the implementation of 'freezing' an enemy simply disable their control
			// update, but it's possible it might affect their status instead.
			if (!NGIsItemFrozen(item_id)) {
				if (action_data == 0) {
					NGSetItemFreezeTimer(item_id, -1);
				}
				else {
					NGSetItemFreezeTimer(item_id, action_data * 30);
				}
			}
			break;
		}
		case UNFREEZE_ENEMY_WITH_EFFECT: {
			if (action_data != 0x00) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "UNFREEZE_ENEMY_WITH_EFFECT: action data %u unimplemented!", action_data);
			}

			if (NGIsItemFrozen(item_id)) {
				// TODO: action_data signifies a special effect when unfreezeing
				NGSetItemFreezeTimer(item_id, 0);
			}
			break;
		}
		case ENEMY_SAVE_THE_COORDINATES_AND_FACING_OF_X_MOVEABLE_IN_SAVEGAME: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ENEMY_SAVE_THE_COORDINATES_AND_FACING_OF_X_MOVEABLE_IN_SAVEGAME unimplemented!");
			break;
		}
		case HURT_ENEMY: {
			NGHurtEnemy(item_id, action_data & 0x7f);
			break;
		}
		case MOVE_ITEM_IMMEDIATELY_TO_LARA_START_POS_WITH_MATCHING_OCB_SETTINGS: {
			int lara_start_pos_id = NGFindIndexForLaraStartPosWithMatchingOCB(action_data & 0x7f);
			if (lara_start_pos_id >= 0) {
				AIOBJECT *ai = &AIObjects[lara_start_pos_id];
				if (ai) {
					items[item_id].pos.x_pos = ai->x;
					items[item_id].pos.y_pos = ai->y;
					items[item_id].pos.z_pos = ai->z;
					items[item_id].pos.y_rot = ai->y_rot;
					items[item_id].room_number = ai->room_number;
				}
			}
			break;
		}
		case ACTIVATE_CAMERA_WITH_TIMER: {
			if (item_id >= number_cameras) {
				NGLog(NG_LOG_TYPE_ERROR, "Invalid camera number.");
				break;
			}

			if (camera.fixed[item_id].flags & 0x100)
				break;

			camera.number = item_id;

			if (camera.type == LOOK_CAMERA || camera.type == COMBAT_CAMERA) {
				if (!(camera.fixed[item_id].flags & 0x01)) {
					break;
				}
			}

			camera.timer = action_data * 30;
			camera.speed = 1;
			if (ng_camera_target_id == NO_ITEM) {
				ng_camera_target_id = lara.item_number;
			}
			
			camera.item = &items[ng_camera_target_id];

			if (flags & NG_TRIGGER_FLAG_BUTTON_ONESHOT) {
				camera.flags |= 0x100;
			}
			if (flags & NG_TRIGGER_FLAG_HEAVY) {
				camera.type = HEAVY_CAMERA;
			}
			else {
				camera.type = FIXED_CAMERA;
			}

			break;
		}
		case SET_MOVEABLE_AS_TARGET_FOR_CAMERA: {
			if (item_id >= ITEM_COUNT) {
				NGLog(NG_LOG_TYPE_ERROR, "Invalid camera target.");
				break;
			}

			ng_camera_target_id = item_id;
			break;
		}
		case TRIGGER_MOVEABLE_ACTIVATE_WITH_TIMER: {
			items[item_id].timer = ((short)(action_data & 0x7f)) * 30;
			T4PlusActivateItem(item_id, false);
			break;
		}
		case UNTRIGGER_MOVEABLE_ACTIVATE_WITH_TIMER: {
			items[item_id].timer = ((short)(action_data & 0x7f)) * 30;
			T4PlusActivateItem(item_id, true);
			break;
		}
		case ACTIVATE_OR_UNTRIGGER_FLYBY_SEQUENCE: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ACTIVATE_OR_UNTRIGGER_FLYBY_SEQUENCE may not be implemented correctly!");

			if (item_id >= MAXIMUM_SPOTCAMS) {
				NGLog(NG_LOG_TYPE_ERROR, "Invalid spotcam!");
			}

			if (action_data == 0) {
				if (!(SpotCam[item_id].flags & SP_FLYBYONESHOT)) {
					bUseSpotCam = 1;

					if (flags & NG_TRIGGER_FLAG_BUTTON_ONESHOT) {
						SpotCam[item_id].flags |= SP_FLYBYONESHOT;
					}

					InitialiseSpotCam(item_id);
				}
			} else if (action_data == 1){
				if (bUseSpotCam) {
					SpotcamResetFOV();
					bUseSpotCam = 0;
				}
			} else {
				NGLog(NG_LOG_TYPE_ERROR, "ACTIVATE_OR_UNTRIGGER_FLYBY_SEQUENCE invalid action data!");
			}
			break;
		}
		case EFFECT_ADD_TO_ENEMY: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "EFFECT_ADD_TO_ENEMY unimplemented!");
			break;
		}
		case EFFECT_REMOVE_TO_ENEMY: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "EFFECT_REMOVE_TO_ENEMY unimplemented!");
			break;
		}
		case ENEMY_SET_MESH_AS_INVISIBLE: {
			NGToggleItemMeshVisibilityMaskBit(item_id, action_data, false);
			break;
		}
		case ENEMY_SET_MESH_AS_VISIBLE: {
			NGToggleItemMeshVisibilityMaskBit(item_id, action_data, true);
			break;
		}
		case SHOW_TRIGGER_COUNTDOWN_TIMER_FOR_ENEMY: {
			NGSetDisplayTimerForMoveableWithType(item_id, (NGTimerTrackerType)(action_data & 0x7f));
			break;
		}
		case SET_ENEMY_TRANSPARENCY_LEVEL: {
			NGSetAfterDeathOverride(item_id, action_data);
			items[item_id].after_death = action_data;
			break;
		}
		case FORCE_ANIMATION_0_TO_31_ON_ITEM: {
			NGSetItemAnimation(item_id, action_data & 0x1f, true, false, false, true);
			break;
		}
		case FORCE_ANIMATION_32_TO_63_ON_ITEM: {
			NGSetItemAnimation(item_id, (action_data & 0x1f) + 32, true, false, false, true);
			break;
		}
		case FORCE_ANIMATION_64_TO_95_ON_ITEM: {
			NGSetItemAnimation(item_id, (action_data & 0x1f) + 64, true, false, false, true);
			break;
		}
		case TURN_X_ANIMATION_MOVING_SLOWLY_IN_CLOCKWISE_UNTIL_FACING: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TURN_X_ANIMATION_MOVING_SLOWLY_IN_CLOCKWISE_UNTIL_FACING unimplemented!");
			break;
		}
		case TURN_X_ANIMATION_MOVING_SLOWLY_IN_INVERSE_CLOCKWISE_UNTIL_FACING: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TURN_X_ANIMATION_MOVING_SLOWLY_IN_INVERSE_CLOCKWISE_UNTIL_FACING unimplemented!");
			break;
		}
		case TURN_X_ANIMATION_MOVING_FASTLY_IN_CLOCKWISE_UNTIL_FACING: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TURN_X_ANIMATION_MOVING_FASTLY_IN_CLOCKWISE_UNTIL_FACING unimplemented!");
			break;
		}
		case TURN_X_ANIMATION_MOVING_FASTLY_IN_INVERSE_CLOCKWISE_UNTIL_FACING: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TURN_X_ANIMATION_MOVING_FASTLY_IN_INVERSE_CLOCKWISE_UNTIL_FACING unimplemented!");
			break;
		}
		case TURN_VERTICALLY_X_ANIMATION_MOVING_SLOWLY_IN_CLOCKWISE_UNTIL_FACING: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TURN_VERTICALLY_X_ANIMATION_MOVING_SLOWLY_IN_CLOCKWISE_UNTIL_FACING unimplemented!");
			break;
		}
		case TURN_VERTICALLY_X_ANIMATION_MOVING_SLOWLY_IN_INVERSE_CLOCKWISE_UNTIL_FACING: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TURN_VERTICALLY_X_ANIMATION_MOVING_SLOWLY_IN_INVERSE_CLOCKWISE_UNTIL_FACING unimplemented!");
			break;
		}
		case TURN_VERTICALLY_X_ANIMATION_MOVING_FASTLY_IN_CLOCKWISE_UNTIL_FACING: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TURN_VERTICALLY_X_ANIMATION_MOVING_FASTLY_IN_CLOCKWISE_UNTIL_FACING unimplemented!");
			break;
		}
		case TURN_VERTICALLY_X_ANIMATION_MOVING_FASTLY_IN_INVERSE_CLOCKWISE_UNTIL_FACING: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TURN_VERTICALLY_X_ANIMATION_MOVING_FASTLY_IN_INVERSE_CLOCKWISE_UNTIL_FACING unimplemented!");
			break;
		}
		case OPEN_OR_CLOSE_DOOR_ITEM: {
			repeat_type = NG_ACTION_REPEAT_TYPE_ALWAYS;

			T4PlusActivateItem(item_id, false);
			items[item_id].timer = 0;

			bool reverse = (items[item_id].flags & IFL_REVERSE) ? true : false;

			if (action_data) {
				if (reverse)
					items[item_id].flags &= ~(IFL_CODEBITS);
				else
					items[item_id].flags |= IFL_CODEBITS;
			} else {
				if (reverse)
					items[item_id].flags |= IFL_CODEBITS;
				else
					items[item_id].flags &= ~(IFL_CODEBITS);
			}
			break;
		}
		case MOVE_CONTINUOUSLY_FORWARD_BACKWARD_X_ANIMATING_FOR_CLICKS:
			NGLog(NG_LOG_TYPE_POSSIBLE_INACCURACY, "NGAction: MOVE_CONTINUOUSLY_FORWARD_BACKWARD_X_ANIMATING_FOR_CLICKS may not be accurate.");
			if (!NGGetItemHorizontalMovementRemainingUnits(item_id)) {
				NGSetItemHorizontalMovementAngle(item_id, items[item_id].pos.y_rot);
				NGSetItemHorizontalMovementRemainingUnits(item_id, (action_data + 1) * 256);
				NGSetItemHorizontalMovementRepeatUnits(item_id, (action_data + 1) * 256);
				NGSetItemHorizontalMovementSpeed(item_id, 32);
				NGSetItemMovementInProgressSound(item_id, -1);
				NGSetItemMovementFinishedSound(item_id, -1);
			}
			break;
		case MOVE_ITEM_UP_FOR_CLICKS:
			if (!is_mod_trng_version_equal_or_greater_than_target(1, 3, 0, 0)) {
				repeat_type = NG_ACTION_REPEAT_TYPE_NEVER;
			}

			if (!NGGetItemVerticalMovementRemainingUnits(item_id)) {
				NGSetItemVerticalMovementRemainingUnits(item_id, (action_data+1) * -256);
				NGSetItemVerticalMovementRepeatUnits(item_id, 0);
				NGSetItemVerticalMovementSpeed(item_id, -32);
				NGSetItemMovementInProgressSound(item_id, -1);
				NGSetItemMovementFinishedSound(item_id, -1);
			}
			break;
		case MOVE_ITEM_DOWN_FOR_CLICKS:
			if (!is_mod_trng_version_equal_or_greater_than_target(1, 3, 0, 0)) {
				repeat_type = NG_ACTION_REPEAT_TYPE_NEVER;
			}

			if (!NGGetItemVerticalMovementRemainingUnits(item_id)) {
				NGSetItemVerticalMovementRemainingUnits(item_id, (action_data+1) * 256);
				NGSetItemVerticalMovementRepeatUnits(item_id, 0);
				NGSetItemVerticalMovementSpeed(item_id, 32);
				NGSetItemMovementInProgressSound(item_id, -1);
				NGSetItemMovementFinishedSound(item_id, -1);
			}
			break;
		case MOVE_ITEM_WEST_FOR_CLICKS:
			if (!is_mod_trng_version_equal_or_greater_than_target(1, 3, 0, 0)) {
				repeat_type = NG_ACTION_REPEAT_TYPE_NEVER;
			}

			if (!NGGetItemHorizontalMovementRemainingUnits(item_id)) {
				NGSetItemHorizontalMovementAngle(item_id, (short)0xC000);
				NGSetItemHorizontalMovementRemainingUnits(item_id, (action_data+1) * 256);
				NGSetItemHorizontalMovementRepeatUnits(item_id, 0);
				NGSetItemHorizontalMovementSpeed(item_id, 32);
				NGSetItemMovementInProgressSound(item_id, -1);
				NGSetItemMovementFinishedSound(item_id, -1);
			}
			break;
		case MOVE_ITEM_NORTH_FOR_CLICKS:
			if (!is_mod_trng_version_equal_or_greater_than_target(1, 3, 0, 0)) {
				repeat_type = NG_ACTION_REPEAT_TYPE_NEVER;
			}

			if (!NGGetItemHorizontalMovementRemainingUnits(item_id)) {
				NGSetItemHorizontalMovementAngle(item_id, (short)0x0000);
				NGSetItemHorizontalMovementRemainingUnits(item_id, (action_data + 1) * 256);
				NGSetItemHorizontalMovementRepeatUnits(item_id, 0);
				NGSetItemHorizontalMovementSpeed(item_id, 32);
				NGSetItemMovementInProgressSound(item_id, -1);
				NGSetItemMovementFinishedSound(item_id, -1);
			}
			break;
		case MOVE_ITEM_EAST_FOR_CLICKS:
			if (!is_mod_trng_version_equal_or_greater_than_target(1, 3, 0, 0)) {
				repeat_type = NG_ACTION_REPEAT_TYPE_NEVER;
			}

			if (!NGGetItemHorizontalMovementRemainingUnits(item_id)) {
				NGSetItemHorizontalMovementAngle(item_id, (short)0x4000);
				NGSetItemHorizontalMovementRemainingUnits(item_id, (action_data + 1) * 256);
				NGSetItemHorizontalMovementRepeatUnits(item_id, 0);
				NGSetItemHorizontalMovementSpeed(item_id, 32);
				NGSetItemMovementInProgressSound(item_id, -1);
				NGSetItemMovementFinishedSound(item_id, -1);
			}
			break;
		case MOVE_ITEM_SOUTH_FOR_CLICKS:
			if (!is_mod_trng_version_equal_or_greater_than_target(1, 3, 0, 0)) {
				repeat_type = NG_ACTION_REPEAT_TYPE_NEVER;
			}

			if (!NGGetItemHorizontalMovementRemainingUnits(item_id)) {
				NGSetItemHorizontalMovementAngle(item_id, (short)0x8000);
				NGSetItemHorizontalMovementRemainingUnits(item_id, (action_data + 1) * 256);
				NGSetItemHorizontalMovementRepeatUnits(item_id, 0);
				NGSetItemHorizontalMovementSpeed(item_id, 32);
				NGSetItemMovementInProgressSound(item_id, -1);
				NGSetItemMovementFinishedSound(item_id, -1);
			}
			break;
		case MOVE_CONTINUOUSLY_UPSTAIRS_DOWNSTAIRS_X_ANIMATING_FOR_CLICKS:
			if (!NGGetItemVerticalMovementRemainingUnits(item_id)) {
				NGSetItemVerticalMovementRemainingUnits(item_id, (action_data + 1) * -256);
				NGSetItemVerticalMovementRepeatUnits(item_id, -((action_data + 1) * -256));
				NGSetItemVerticalMovementSpeed(item_id, -32);
				NGSetItemMovementInProgressSound(item_id, -1);
				NGSetItemMovementFinishedSound(item_id, -1);
			}
			break;
		case DISABLE_ITEM_COLLISION:
			NGDisableItemCollision(item_id);
			break;
		case ENABLE_ITEM_COLLISION:
			NGEnableItemCollision(item_id);
			break;
		case MOVE_ITEM_UP_BY_UNITS_X8: {
			NGMoveItemByUnits(item_id, NG_UP, 8 * ((action_data)+1));
			break;
		}
		case MOVE_ITEM_DOWN_BY_UNITS_X8: {
			NGMoveItemByUnits(item_id, NG_DOWN, 8 * ((action_data)+1));
			break;
		}
		case MOVE_ITEM_WEST_BY_UNITS_X8: {
			NGMoveItemByUnits(item_id, NG_WEST, 8 * ((action_data)+1));
			break;
		}
		case MOVE_ITEM_NORTH_BY_UNITS_X8: {
			NGMoveItemByUnits(item_id, NG_NORTH, 8 * ((action_data)+1));
			break;
		}
		case MOVE_ITEM_EAST_BY_UNITS_X8: {
			NGMoveItemByUnits(item_id, NG_EAST, 8 * ((action_data)+1));
			break;
		}
		case MOVE_ITEM_SOUTH_BY_UNITS_X8: {
			NGMoveItemByUnits(item_id, NG_SOUTH, 8 * ((action_data)+1));
			break;
		}
		case TURN_STOP_CIRCULAR_TURNING_FOR_X_ANIMATION_ITEM_IN_WAY: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TURN_STOP_CIRCULAR_TURNING_FOR_X_ANIMATION_ITEM_IN_WAY unimplemented!");
			break;
		}
		case CREATURE_FORCE_FRAME_OF_CURRENT_ANIMATION: {
			ITEM_INFO* item = &items[item_id];
			if (item) {
				item->frame_number = anims[item->anim_number].frame_base + action_data;
			}
			break;
		}
		case CREATURE_FORCE_STATE_ID: {
			ITEM_INFO* item = &items[item_id];
			if (item) {
				item->current_anim_state = action_data;
				item->goal_anim_state = action_data; // Do we need to change the goal state too or not?
			}
			break;
		}
		case TRIGGER_SET_X_MOVEABLE_AS_ACTIVE_ITEM: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TRIGGER_SET_X_MOVEABLE_AS_ACTIVE_ITEM unimplemented!");
			break;
		}
		default:
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Unimplemented NGAction %u", action_type);
			break;
		};
	return repeat_type;
};