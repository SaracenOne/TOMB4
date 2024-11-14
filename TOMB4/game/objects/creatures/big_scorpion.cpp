#include "../../../tomb4/pch.h"
#include "big_scorpion.h"
#include "../../box.h"
#include "../../objects.h"
#include "../../../specific/3dmath.h"
#include "../../control.h"
#include "../../lot.h"
#include "../../items.h"
#include "../../../specific/function_stubs.h"
#include "../../effects.h"
#include "../../lara.h"
#include "../../deltapak.h"
#include "../../gameflow.h"
#include "../../../tomb4/mod_config.h"
#include "../../../tomb4/tomb4plus/t4plus_objects.h"

static BITE_INFO stinger{ 0, 0, 0, 8 };
static BITE_INFO pincer{ 0, 0, 0, 23 };

void InitialiseBigScorpion(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);

	if (item->trigger_flags == 1)
	{
		item->anim_number = objects[SCORPION].anim_index + 7;
		item->current_anim_state = 8;
		item->goal_anim_state = 8;
	}
	else
	{
		item->anim_number = objects[SCORPION].anim_index + 2;
		item->current_anim_state = 1;
		item->goal_anim_state = 1;
	}

	item->frame_number = anims[item->anim_number].frame_base;
}

void BigScorpionControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	FLOOR_INFO* floor;
	CREATURE_INFO* scorpion;
	AI_INFO info;
	long s, c, x, z, h, h2, dist, bestdist;
	short angle, room_number, xrot, zrot, target_num, hp;

	if (!CreatureActive(item_number))
		return;

	MOD_LEVEL_OBJECT_CUSTOMIZATION *mod_object_customization = get_game_mod_level_object_customization_for_slot(gfCurrentLevel, SCORPION);

	angle = 0;
	item = &items[item_number];
	scorpion = (CREATURE_INFO*)item->data;
	s = (682 * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	c = (682 * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;

	x = item->pos.x_pos + s;
	z = item->pos.z_pos + c;
	room_number = item->room_number;
	floor = GetFloor(x, item->pos.y_pos, z, &room_number);
	h = GetHeight(floor, x, item->pos.y_pos, z);

	if (abs(item->pos.y_pos - h) > HALF_BLOCK_SIZE)
		h = item->pos.y_pos;

	x = item->pos.x_pos - s;
	z = item->pos.z_pos - c;
	room_number = item->room_number;
	floor = GetFloor(x, item->pos.y_pos, z, &room_number);
	h2 = GetHeight(floor, x, item->pos.y_pos, z);

	if (abs(item->pos.y_pos - h2) > HALF_BLOCK_SIZE)
		h2 = item->pos.y_pos;

	xrot = (short)phd_atan(1364, h2 - h);

	x = item->pos.x_pos - c;
	z = item->pos.z_pos + s;
	room_number = item->room_number;
	floor = GetFloor(x, item->pos.y_pos, z, &room_number);
	h = GetHeight(floor, x, item->pos.y_pos, z);

	if (abs(item->pos.y_pos - h) > HALF_BLOCK_SIZE)
		h = item->pos.y_pos;

	x = item->pos.x_pos + c;
	z = item->pos.z_pos - s;
	room_number = item->room_number;
	floor = GetFloor(x, item->pos.y_pos, z, &room_number);
	h2 = GetHeight(floor, x, item->pos.y_pos, z);

	if (abs(item->pos.y_pos - h2) > HALF_BLOCK_SIZE)
		h2 = item->pos.y_pos;

	zrot = (short)phd_atan(1364, h2 - h);

	if (item->hit_points <= 0)
	{
		item->hit_points = 0;
		s = 1;

		if (item->current_anim_state != 6)
		{
			if (item->trigger_flags > 0 && item->trigger_flags < 7)
			{
				cutseq_num = 4;
				item->anim_number = objects[SCORPION].anim_index + 5;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 6;
				item->status = ITEM_INVISIBLE;
				scorpion->maximum_turn = 0;

				for (target_num = room[item->room_number].item_number; target_num != NO_ITEM; target_num = enemy->next_item)
				{
					enemy = &items[target_num];

					if (enemy->object_number == TROOPS && enemy->trigger_flags == 1)
					{
						DisableBaddieAI(target_num);
						KillItem(target_num);
						enemy->flags |= IFL_CLEARBODY;
						break;
					}
				}

				s = 0;
			}
			else if (item->current_anim_state != 7)
			{
				item->anim_number = objects[SCORPION].anim_index + 5;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 6;
				s = 0;
			}
		}

		if (s)
		{
			if (cutseq_num == 4)
			{
				item->frame_number = anims[item->anim_number].frame_end - 1;
				item->status = ITEM_INVISIBLE;
			}
			else if (item->current_anim_state == 6)
			{
				if (item->status == ITEM_INVISIBLE)
					item->status = ITEM_ACTIVE;
			}
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(scorpion);
		else
		{
			if (scorpion->hurt_by_lara && item->current_anim_state != 8)
				scorpion->enemy = lara_item;
			else
			{
				scorpion->enemy = 0;
				bestdist = 0x7FFFFFFF;

				for (int i = 0; i < MAXIMUM_BADDIES; i++)
				{
					target_num = baddie_slots[i].item_num;

					if (target_num != NO_ITEM && target_num != item_number)
					{
						enemy = &items[target_num];

						if (enemy->object_number != T4PlusGetLaraSlotID() && enemy->object_number != SCORPION && (enemy != lara_item || scorpion->hurt_by_lara))
						{
							s = enemy->pos.x_pos - item->pos.x_pos;
							c = enemy->pos.z_pos - item->pos.z_pos;
							dist = SQUARE(s) + SQUARE(c);

							if (dist < bestdist)
							{
								scorpion->enemy = enemy;
								bestdist = dist;
							}
						}
					}
				}
			}
		}

		CreatureAIInfo(item, &info);
		enemy = scorpion->enemy;

		if (enemy != lara_item)
			phd_atan(lara_item->pos.z_pos - item->pos.z_pos, lara_item->pos.x_pos - item->pos.x_pos);

		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, scorpion->maximum_turn);

		switch (item->current_anim_state)
		{
		case 1:
			scorpion->maximum_turn = 0;
			scorpion->flags = 0;

			if (info.distance > 0x1C6E39)
				item->goal_anim_state = 2;
			else if (info.bite)
			{
				scorpion->maximum_turn = DEGREES_TO_ROTATION(2);

				if (GetRandomControl() & 1 || enemy->object_number == TROOPS && enemy->hit_points <= 15)
					item->goal_anim_state = 4;
				else
					item->goal_anim_state = 5;
			}
			else if (!info.ahead)
				item->goal_anim_state = 2;

			break;

		case 2:
			scorpion->maximum_turn = DEGREES_TO_ROTATION(2);

			if (info.distance < 0x1C6E39)
				item->goal_anim_state = 1;
			else
				item->goal_anim_state = 3;

			break;

		case 3:
			scorpion->maximum_turn = DEGREES_TO_ROTATION(3);

			if (info.distance < 0x1C6E39)
				item->goal_anim_state = 1;

			break;

		case 4:
		case 5:
			hp = lara_item->hit_points;

			if (abs(info.angle) < DEGREES_TO_ROTATION(2))
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= DEGREES_TO_ROTATION(2);
			else
				item->pos.y_rot += DEGREES_TO_ROTATION(2);

			if (!scorpion->flags && enemy && enemy != lara_item && info.distance < 0x1C6E39)
			{
				enemy->hit_points -= 15;

				if (enemy->hit_points <= 0)
				{
					item->goal_anim_state = 7;
					scorpion->maximum_turn = 0;
				}

				enemy->hit_status = 1;
				scorpion->flags = 1;
				CreatureEffectT(item, &stinger, 10, item->pos.y_rot + 0x8000, DoBloodSplat);
			}
			else if (!scorpion->flags && item->touch_bits & 0x1B00100)
			{
				lara_item->hit_points -= mod_object_customization->damage_1;
				lara_item->hit_status = 1;

				if (item->current_anim_state == 5)
				{
					lara.dpoisoned += 2048;
					CreatureEffectT(item, &stinger, 10, item->pos.y_rot + 0x8000, DoBloodSplat);
				}
				else
					CreatureEffectT(item, &pincer, 10, item->pos.y_rot + 0x8000, DoBloodSplat);

				scorpion->flags = 1;

				if (hp && lara_item->hit_points <= 0)
				{
					CreatureKill(item, 6, 7, 442);
					scorpion->maximum_turn = 0;
					return;
				}
			}

			break;

		case 8:
			scorpion->maximum_turn = 0;

			if (item->frame_number == anims[item->anim_number].frame_end)
				item->trigger_flags++;

			if (enemy && enemy->hit_points <= 0 || item->trigger_flags > 6)
			{
				item->goal_anim_state = 7;
				enemy->hit_points = 0;
			}

			break;
		}
	}

	if (abs(xrot - item->pos.x_rot) < CLICK_SIZE)
		item->pos.x_rot = xrot;
	else if (xrot > item->pos.x_rot)
		item->pos.x_rot += CLICK_SIZE;
	else if (xrot < item->pos.x_rot)
		item->pos.x_rot -= CLICK_SIZE;

	if (abs(zrot - item->pos.z_rot) < CLICK_SIZE)
		item->pos.z_rot = zrot;
	else if (zrot > item->pos.z_rot)
		item->pos.z_rot += CLICK_SIZE;
	else if (zrot < item->pos.z_rot)
		item->pos.z_rot -= CLICK_SIZE;

	if (!cutseq_num)
		CreatureAnimation(item_number, angle, 0);
}