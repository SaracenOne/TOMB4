#include "../tomb4/pch.h"
#include "camera.h"
#include "../specific/3dmath.h"
#include "../specific/function_stubs.h"
#include "control.h"
#include "delstuff.h"
#include "larafire.h"
#include "sound.h"
#include "deltapak.h"
#include "draw.h"
#include "../specific/input.h"
#include "effect2.h"
#include "lara.h"
#include "savegame.h"
#include "../tomb4/tomb4.h"
#include "../tomb4/tomb4plus/t4plus_environment.h"
#include "../tomb4/mod_config.h"
#include "gameflow.h"
#include "sphere.h"

// T4Plus
bool freeze_camera_button_pressed = false;
bool camera_frozen = false;

CAMERA_INFO camera;

GAME_VECTOR ForcedFixedCamera;
char UseForcedFixedCamera;

PHD_VECTOR LaraTorchStart;
PHD_VECTOR LaraTorchEnd;
long bLaraTorch;
long LaraTorchIntensity;
long LaraTorchYRot;

camera_type BinocularOldCamera = CHASE_CAMERA;
long BinocularOn = 0;
long BinocularRange = 0;
long ExittingBinos = 0;
long LaserSight = 0;

SHATTER_ITEM ShatterItem;

static OLD_CAMERA old_cam;
static GAME_VECTOR last_target;
static GAME_VECTOR last_ideal;
static char TargetSnaps = 0;
static char CameraSnaps = 0;

static GAME_VECTOR static_lookcamp;
static GAME_VECTOR static_lookcamt;

bool tr5_camera_behaviour = false;

void InitialiseCamera()
{
	MOD_LEVEL_CAMERA_INFO *mod_camera_info = get_game_mod_level_camera_info(gfCurrentLevel);

	camera_frozen = false;

	last_target.x = lara_item->pos.x_pos;
	camera.target.x = last_target.x;
	camera.shift = lara_item->pos.y_pos - BLOCK_SIZE;
	last_target.y = camera.shift;
	camera.target.y = camera.shift;
	last_target.z = lara_item->pos.z_pos;
	camera.target.z = last_target.z;
	camera.pos.y = camera.shift;
	last_target.room_number = lara_item->room_number;
	camera.target.room_number = lara_item->room_number;
	camera.pos.x = last_target.x;
	camera.pos.z = last_target.z - 100;
	camera.pos.room_number = lara_item->room_number;
	camera.target_distance = mod_camera_info->chase_camera_distance;
	camera.item = nullptr;
	camera.number_frames = 1;
	camera.type = CHASE_CAMERA;
	camera.speed = 1;
	camera.flags = 0;
	camera.bounce = 0;
	camera.number = -1;
	camera.fixed_camera = 0;
	AlterFOV(DEGREES_TO_ROTATION(DEFAULT_FOV));
	UseForcedFixedCamera = 0;
	CalculateCamera();
}

void MoveCamera(GAME_VECTOR* ideal, long speed)
{
	FLOOR_INFO* floor;
	GAME_VECTOR temp1;
	GAME_VECTOR temp2;
	long height, ceiling, shake, rndval, wx, wy, wz, dx, dy, dz;
	short room_number;

	if (BinocularOn < 0)
	{
		speed = 1;
		BinocularOn++;
	}

	// T4Plus
	bool force_camera_update = tr5_camera_behaviour;

	if (!force_camera_update &&
		old_cam.pos.x_rot == lara_item->pos.x_rot && old_cam.pos.y_rot == lara_item->pos.y_rot && old_cam.pos.z_rot == lara_item->pos.z_rot &&
		old_cam.pos2.x_rot == lara.head_x_rot && old_cam.pos2.y_rot == lara.head_y_rot && old_cam.pos2.x_pos == lara.torso_x_rot &&
		old_cam.pos2.y_pos == lara.torso_y_rot && old_cam.pos.x_pos == lara_item->pos.x_pos && old_cam.pos.y_pos == lara_item->pos.y_pos &&
		old_cam.pos.z_pos == lara_item->pos.z_pos && old_cam.current_anim_state == lara_item->current_anim_state &&
		old_cam.goal_anim_state == lara_item->goal_anim_state && old_cam.target_distance == camera.target_distance &&
		old_cam.target_elevation == camera.target_elevation && old_cam.target_angle == camera.target_angle &&
		camera.old_type == camera.type && BinocularOn >= 0)
	{
		ideal->x = last_ideal.x;
		ideal->y = last_ideal.y;
		ideal->z = last_ideal.z;
		ideal->room_number = last_ideal.room_number;
	}
	else
	{
		old_cam.pos.x_rot = lara_item->pos.x_rot;
		old_cam.pos.y_rot = lara_item->pos.y_rot;
		old_cam.pos.z_rot = lara_item->pos.z_rot;
		old_cam.pos2.x_rot = lara.head_x_rot;
		old_cam.pos2.y_rot = lara.head_y_rot;
		old_cam.pos2.x_pos = lara.torso_x_rot;
		old_cam.pos2.y_pos = lara.torso_y_rot;
		old_cam.pos.x_pos = lara_item->pos.x_pos;
		old_cam.pos.y_pos = lara_item->pos.y_pos;
		old_cam.pos.z_pos = lara_item->pos.z_pos;
		old_cam.current_anim_state = lara_item->current_anim_state;
		old_cam.goal_anim_state = lara_item->goal_anim_state;
		old_cam.target_distance = camera.target_distance;
		old_cam.target_elevation = camera.target_elevation;
		old_cam.target_angle = camera.target_angle;
		last_ideal.x = ideal->x;
		last_ideal.y = ideal->y;
		last_ideal.z = ideal->z;
		last_ideal.room_number = ideal->room_number;
	}

	camera.pos.x += (ideal->x - camera.pos.x) / speed;
	camera.pos.y += (ideal->y - camera.pos.y) / speed;
	camera.pos.z += (ideal->z - camera.pos.z) / speed;
	camera.pos.room_number = ideal->room_number;

	if (camera.bounce)
	{
		if (camera.bounce <= 0)
		{
			rndval = -camera.bounce;
			shake = rndval >> 1;
			camera.target.x += GetRandomControl() % rndval - shake;
			camera.target.y += GetRandomControl() % rndval - shake;
			camera.target.z += GetRandomControl() % rndval - shake;
			camera.bounce += 5;
		}
		else
		{
			camera.pos.y += camera.bounce;
			camera.target.y += camera.bounce;
			camera.bounce = 0;
		}
	}

	wz = camera.pos.z;
	wy = camera.pos.y;
	wx = camera.pos.x;
	room_number = camera.pos.room_number;

	// T4Plus - swamps
	floor = GetFloor(wx, wy + CLICK_SIZE, wz, &room_number);
	if (T4PlusIsRoomSwamp(&room[room_number]))
	{
		wy = room[room_number].maxceiling - CLICK_SIZE;
		floor = GetFloor(wx, wy, wz, &camera.pos.room_number);
	}
	
	floor = GetFloor(wx, wy, wz, &room_number);
	height = GetHeight(floor, wx, wy, wz);
	ceiling = GetCeiling(floor, wx, wy, wz);

	if (wy < ceiling || wy > height)
	{
		mgLOS(&camera.target, &camera.pos, 0);
		dx = abs(camera.pos.x - ideal->x);
		dy = abs(camera.pos.y - ideal->y);
		dz = abs(camera.pos.z - ideal->z);

		if (dx < 768 && dy < 768 && dz < 768)
		{
			temp1.x = camera.pos.x;
			temp1.y = camera.pos.y;
			temp1.z = camera.pos.z;
			temp1.room_number = camera.pos.room_number;
			temp2.x = ideal->x;
			temp2.y = ideal->y;
			temp2.z = ideal->z;
			temp2.room_number = ideal->room_number;

			if (!(mgLOS(&temp2, &temp1, 0)))
			{
				CameraSnaps++;

				if (CameraSnaps >= 8)
				{
					camera.pos.x = ideal->x;
					camera.pos.y = ideal->y;
					camera.pos.z = ideal->z;
					camera.pos.room_number = ideal->room_number;
					CameraSnaps = 0;
				}
			}
		}
	}

	wz = camera.pos.z;
	wy = camera.pos.y;
	wx = camera.pos.x;
	room_number = camera.pos.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	height = GetHeight(floor, wx, wy, wz);
	ceiling = GetCeiling(floor, wx, wy, wz);

	if (wy - (CLICK_SIZE - 1) < ceiling && height < wy + (CLICK_SIZE - 1) && ceiling < height && ceiling != NO_HEIGHT && height != NO_HEIGHT)
		camera.pos.y = (height + ceiling) >> 1;
	else if (height < wy + (CLICK_SIZE - 1) && ceiling < height && ceiling != NO_HEIGHT && height != NO_HEIGHT)
		camera.pos.y = height - (CLICK_SIZE - 1);
	else if (wy - (CLICK_SIZE - 1) < ceiling && ceiling < height && ceiling != NO_HEIGHT && height != NO_HEIGHT)
		camera.pos.y = ceiling + (CLICK_SIZE - 1);
	else if (ceiling >= height || height == NO_HEIGHT || ceiling == NO_HEIGHT)
	{
		camera.pos.x = ideal->x;
		camera.pos.y = ideal->y;
		camera.pos.z = ideal->z;
		camera.pos.room_number = ideal->room_number;
	}

	GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);
	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);

	if (camera.mike_at_lara)
	{
		if (!tr5_camera_behaviour) {
			if (camera.actual_elevation != lara.head_y_rot + lara.torso_y_rot + lara_item->pos.y_rot) {
				camera.actual_elevation = lara.head_y_rot + lara.torso_y_rot + lara_item->pos.y_rot;
			}
		}
		camera.mike_pos.x = lara_item->pos.x_pos;
		camera.mike_pos.y = lara_item->pos.y_pos;
		camera.mike_pos.z = lara_item->pos.z_pos;
	}
	else
	{
		dx = camera.target.x - camera.pos.x;
		dz = camera.target.z - camera.pos.z;
		dx = phd_atan(dz, dx);
		if (!tr5_camera_behaviour) {
			if (camera.actual_elevation != (short)dx) {
				camera.actual_elevation = (short)dx;
			}
		}
		camera.mike_pos.x = camera.pos.x + (phd_sin(dx) * phd_persp >> W2V_SHIFT);
		camera.mike_pos.y = camera.pos.y;
		camera.mike_pos.z = camera.pos.z + (phd_cos(dx) * phd_persp >> W2V_SHIFT);
	}

	camera.old_type = camera.type;
}

long mgLOS(GAME_VECTOR* start, GAME_VECTOR* target, long push)
{
	FLOOR_INFO* floor;
	long x, y, z, h, c, cdiff, hdiff, dx, dy, dz, clipped, nc, i;
	short room_number, room_number2;

	dx = (target->x - start->x) >> 3;
	dy = (target->y - start->y) >> 3;
	dz = (target->z - start->z) >> 3;
	x = start->x;
	y = start->y;
	z = start->z;
	room_number = start->room_number;
	room_number2 = room_number;
	nc = 0;
	clipped = 0;

	for (i = 0; i < 8; i++)
	{
		room_number = room_number2;
		floor = GetFloor(x, y, z, &room_number2);

		// T4Plus - swamps
		if (T4PlusIsRoomSwamp(&room[room_number2]))
		{
			clipped = 1;
			break;
		}

		h = GetHeight(floor, x, y, z);
		c = GetCeiling(floor, x, y, z);

		if (h == NO_HEIGHT || c == NO_HEIGHT || c >= h)
		{
			if (!nc)
			{
				x += dx;
				y += dy;
				z += dz;
				continue;
			}

			clipped = 1;
			break;
		}

		if (y > h)
		{
			hdiff = y - h;

			if (hdiff < push)
				y = h;
			else
			{
				clipped = 1;
				break;
			}
		}

		if (y < c)
		{
			cdiff = c - y;

			if (cdiff < push)
				y = c;
			else
			{
				clipped = 1;
				break;
			}
		}

		nc = 1;
		x += dx;
		y += dy;
		z += dz;
	}

	if (i)
	{
		x -= dx;
		y -= dy;
		z -= dz;
	}

	target->x = x;
	target->y = y;
	target->z = z;
	GetFloor(x, y, z, &room_number);
	target->room_number = room_number;
	return !clipped;
}

long CameraCollisionBounds(GAME_VECTOR* ideal, long push, long yfirst)
{
	FLOOR_INFO* floor;
	long wx, wy, wz, h, c;
	short room_number;

	wx = ideal->x;
	wy = ideal->y;
	wz = ideal->z;

	if (yfirst)
	{
		room_number = ideal->room_number;
		floor = GetFloor(wx, wy, wz, &room_number);
		h = GetHeight(floor, wx, wy, wz);
		c = GetCeiling(floor, wx, wy, wz);

		if (c > wy - (CLICK_SIZE - 1) && h < wy + (CLICK_SIZE - 1) && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = (h + c) >> 1;
		else if (h < wy + (CLICK_SIZE - 1) && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = h - (CLICK_SIZE - 1);
		else if (c > wy - (CLICK_SIZE - 1) && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = c + (CLICK_SIZE - 1);
	}

	room_number = ideal->room_number;
	floor = GetFloor(wx - push, wy, wz, &room_number);
	h = GetHeight(floor, wx - push, wy, wz);
	c = GetCeiling(floor, wx - push, wy, wz);

	if (h < wy || h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy < c)
		wx = push + (wx & ~0x3FF);

	room_number = ideal->room_number;
	floor = GetFloor(wx, wy, wz - push, &room_number);
	h = GetHeight(floor, wx, wy, wz - push);
	c = GetCeiling(floor, wx, wy, wz - push);

	if (h < wy || h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy < c)
		wz = push + (wz & ~0x3FF);

	room_number = ideal->room_number;
	floor = GetFloor(wx + push, wy, wz, &room_number);
	h = GetHeight(floor, wx + push, wy, wz);
	c = GetCeiling(floor, wx + push, wy, wz);

	if (h < wy || h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy < c)
		wx = (wx | 0x3FF) - push;

	room_number = ideal->room_number;
	floor = GetFloor(wx, wy, wz + push, &room_number);
	h = GetHeight(floor, wx, wy, wz + push);
	c = GetCeiling(floor, wx, wy, wz + push);

	if (h < wy || h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy < c)
		wz = (wz | 0x3FF) - push;

	if (!yfirst)
	{
		room_number = ideal->room_number;
		floor = GetFloor(wx, wy, wz, &room_number);
		h = GetHeight(floor, wx, wy, wz);
		c = GetCeiling(floor, wx, wy, wz);

		if (c > wy - (CLICK_SIZE - 1) && h < wy + (CLICK_SIZE - 1) && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = (h + c) >> 1;
		else if (h < wy + (CLICK_SIZE - 1) && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = h - (CLICK_SIZE - 1);
		else if (c > wy - (CLICK_SIZE - 1) && c < h && c != NO_HEIGHT && h != NO_HEIGHT)
			wy = c + (CLICK_SIZE - 1);
	}

	room_number = ideal->room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	if (h < wy || wy < c || h == NO_HEIGHT || c == NO_HEIGHT || c >= h)
		return 1;

	GetFloor(wx, wy, wz, &ideal->room_number);
	ideal->x = wx;
	ideal->y = wy;
	ideal->z = wz;
	return 0;
}

void LaraTorch(PHD_VECTOR* Soffset, PHD_VECTOR* Eoffset, short yrot, long brightness)
{
	FLOOR_INFO* floor;
	long x, y, z, sx, sy, sz, dx, dy, dz, falloff, counter, h, c, j;
	long offs[5];
	short room_number;

	counter = 0;
	falloff = 15;
	sx = Soffset->x;
	sy = Soffset->y;
	sz = Soffset->z;
	dx = (Eoffset->x - sx) >> 5;
	dy = (Eoffset->y - sy) >> 5;
	dz = (Eoffset->z - sz) >> 5;
	offs[0] = 0;
	offs[1] = -0x4000;
	offs[2] = -0x4001;
	offs[3] = 0x4000;
	offs[4] = 0x4001;

	for (int i = 0; i < MAX_DYNAMICS; i++)
	{
		if (counter)
		{
			counter--;

			brightness -= 7;

			if (brightness < 8)
				break;

			if (falloff < 31)
				falloff += 2;

			sx += dx;
			sy += dy;
			sz += dz;
			continue;
		}

		for (j = 0; j < 5; j++)
		{
			if (offs[j])
			{
				x = sx + (falloff * (phd_sin(offs[j] + yrot) / 4) >> 5);

				if (offs[j] & 1)
					y = sy - (falloff << 7);
				else
					y = sy + (falloff << 7);

				z = sz + (falloff * (phd_cos(offs[j] + yrot) / 4) >> 5);
			}
			else
			{
				x = sx;
				y = sy;
				z = sz;
			}

			room_number = lara_item->room_number;
			floor = GetFloor(x, y, z, &room_number);
			h = GetHeight(floor, x, y, z);
			c = GetCeiling(floor, x, y, z);

			if (h == NO_HEIGHT || c == NO_HEIGHT || c >= h || y < c || h < y)
				break;
		}

		if (j < 5)
		{
			TriggerDynamic(sx, sy, sz, falloff, brightness, brightness, brightness >> 1);
			counter = 5;
		}

		if (counter)
			counter--;

		brightness -= 7;

		if (brightness < 8)
			break;

		if (falloff < 31)
			falloff += 2;

		sx += dx;
		sy += dy;
		sz += dz;
	}
}

void UpdateCameraElevation() {
	PHD_VECTOR pos;
	PHD_VECTOR pos1;
	if (camera.lara_node != -1) {
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		if (camera.item != NULL) {
			GetJointAbsPosition(camera.item, &pos, camera.lara_node);
		} else {
			GetLaraJointPos(&pos, camera.lara_node);
		}
		pos1.x = 0;
		pos1.y = -CLICK_SIZE;
		pos1.z = (BLOCK_SIZE * 2);
		if (camera.item != NULL) {
			GetJointAbsPosition(camera.item, &pos1, camera.lara_node);
		} else {
			GetLaraJointPos(&pos1, camera.lara_node);
		}
		pos.z = pos1.z - pos.z;
		pos.x = pos1.x - pos.x;
		camera.actual_angle = (short)(camera.target_angle + phd_atan(pos.z, pos.x));
	} else {
		camera.actual_angle = lara_item->pos.y_rot + camera.target_angle;
	}
	camera.actual_elevation += (camera.target_elevation - camera.actual_elevation) >> 3;
}


void ChaseCamera(ITEM_INFO* item)
{
	FLOOR_INFO* floor;
	GAME_VECTOR ideal;
	GAME_VECTOR ideals[MAX_CAMERA_CHASE_IDEALS];
	GAME_VECTOR temp[2];
	long distance, dx, dz, farthest, farthestnum, h, c, wx, wy, wz;
	short angle, room_number;

	MOD_LEVEL_CAMERA_INFO* mod_camera_info = get_game_mod_level_camera_info(gfCurrentLevel);


	if (!camera.target_elevation)
		camera.target_elevation = mod_camera_info->chase_camera_vertical_orientation;

	camera.target_elevation += item->pos.x_rot;

	if (tr5_camera_behaviour) {
		UpdateCameraElevation();
	}

	if (tr5_camera_behaviour) {
		if (camera.actual_elevation > DEGREES_TO_ROTATION(85))
			camera.actual_elevation = DEGREES_TO_ROTATION(85);
		else if (camera.actual_elevation < -DEGREES_TO_ROTATION(85))
			camera.actual_elevation = -DEGREES_TO_ROTATION(85);

		distance = camera.target_distance * phd_cos(camera.actual_elevation) >> W2V_SHIFT;
	} else {
		if (camera.target_elevation > DEGREES_TO_ROTATION(85))
			camera.target_elevation = DEGREES_TO_ROTATION(85);
		else if (camera.target_elevation < -DEGREES_TO_ROTATION(85))
			camera.target_elevation = -DEGREES_TO_ROTATION(85);

		distance = camera.target_distance * phd_cos(camera.target_elevation) >> W2V_SHIFT;
	}

	room_number = camera.target.room_number;
	floor = GetFloor(camera.target.x, camera.target.y + CLICK_SIZE, camera.target.z, &room_number);

	if (T4PlusIsRoomSwamp(&room[room_number]))
	{
		camera.target.y = room[room_number].maxceiling - CLICK_SIZE;
	}

	wx = camera.target.x;
	wy = camera.target.y;
	wz = camera.target.z;
	floor = GetFloor(wx, wy, wz, &camera.target.room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	if (!tr5_camera_behaviour) {
		if (c + 16 > h - 16 && h != NO_HEIGHT && c != NO_HEIGHT)
		{
			camera.target.y = (h + c) >> 1;
			camera.target_elevation = 0;
		}
		else if (wy > h - 16 && h != NO_HEIGHT)
		{
			camera.target.y = h - 16;
			camera.target_elevation = 0;
		}
		else if (wy < c + 16 && c != NO_HEIGHT)
		{
			camera.target.y = c + 16;
			camera.target_elevation = 0;
		}

		wx = camera.target.x;
		wy = camera.target.y;
		wz = camera.target.z;
	}

	GetFloor(wx, wy, wz, &camera.target.room_number);
	room_number = camera.target.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	if (wy < c || wy > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT)
	{
		if (tr5_camera_behaviour) {
			TargetSnaps++;
		}
		camera.target.x = last_target.x;
		camera.target.y = last_target.y;
		camera.target.z = last_target.z;
		camera.target.room_number = last_target.room_number;
	} else {
		TargetSnaps = 0;
	}

	if (tr5_camera_behaviour) {
		for (int i = 0; i < MAX_CAMERA_CHASE_IDEALS; i++)
			ideals[i].y = camera.target.y + (camera.target_distance * phd_sin(camera.actual_elevation) >> W2V_SHIFT);
	} else {
		for (int i = 0; i < MAX_CAMERA_CHASE_IDEALS; i++)
			ideals[i].y = (camera.target_distance * phd_sin(camera.target_elevation) >> W2V_SHIFT) + camera.target.y;
	}

	farthest = 0x7FFFFFFF;
	farthestnum = 0;

	for (int i = 0; i < MAX_CAMERA_CHASE_IDEALS; i++)
	{
		if (i) {
			angle = (i - 1) << W2V_SHIFT;
		} else {
			if (tr5_camera_behaviour) {
				angle = camera.actual_angle;
			} else {
				angle = camera.target_angle + item->pos.y_rot;
			}
		}

		ideals[i].x = camera.target.x - ((distance * phd_sin(angle)) >> W2V_SHIFT);
		ideals[i].z = camera.target.z - ((distance * phd_cos(angle)) >> W2V_SHIFT);
		ideals[i].room_number = camera.target.room_number;

		if (mgLOS(&camera.target, &ideals[i], 200))
		{
			temp[0].x = ideals[i].x;
			temp[0].y = ideals[i].y;
			temp[0].z = ideals[i].z;
			temp[0].room_number = ideals[i].room_number;
			temp[1].x = camera.pos.x;
			temp[1].y = camera.pos.y;
			temp[1].z = camera.pos.z;
			temp[1].room_number = camera.pos.room_number;

			if (!tr5_camera_behaviour) {
				if (!i)
				{
					farthestnum = 0;
					break;
				}
			}

			if (mgLOS(&temp[0], &temp[1], 0))
			{
				if (tr5_camera_behaviour) {
					if (!i)
					{
						farthestnum = 0;
						break;
					}
				}

				dx = SQUARE(camera.pos.x - ideals[i].x);
				dz = SQUARE(camera.pos.z - ideals[i].z);
				dz += dx;

				if (dz < farthest)
				{
					farthest = dz;
					farthestnum = i;
				}
			}
		}
		else if (!i)
		{
			temp[0].x = ideals[i].x;
			temp[0].y = ideals[i].y;
			temp[0].z = ideals[i].z;
			temp[0].room_number = ideals[i].room_number;
			temp[1].x = camera.pos.x;
			temp[1].y = camera.pos.y;
			temp[1].z = camera.pos.z;
			temp[1].room_number = camera.pos.room_number;

			if (!tr5_camera_behaviour || mgLOS(&temp[0], &temp[1], 0)) {
				dx = SQUARE(camera.target.x - ideals[i].x);
				dz = SQUARE(camera.target.z - ideals[i].z);
				dz += dx;

				if (dz > 0x90000)
				{
					farthestnum = 0;
					break;
				}
			}
		}
	}

	ideal.x = ideals[farthestnum].x;
	ideal.y = ideals[farthestnum].y;
	ideal.z = ideals[farthestnum].z;
	ideal.room_number = ideals[farthestnum].room_number;
	CameraCollisionBounds(&ideal, (CLICK_SIZE + HALF_CLICK_SIZE), 1);

	if (camera.old_type == FIXED_CAMERA)
		camera.speed = 1;

	MoveCamera(&ideal, camera.speed);
}

void CombatCamera(ITEM_INFO* item)
{
	FLOOR_INFO* floor;
	GAME_VECTOR ideal;
	GAME_VECTOR ideals[MAX_CAMERA_CHASE_IDEALS];
	GAME_VECTOR temp[2];
	long distance, dx, dz, farthest, farthestnum, h, c, wx, wy, wz;
	short angle, room_number;

	MOD_LEVEL_CAMERA_INFO *mod_camera_info = get_game_mod_level_camera_info(gfCurrentLevel);

	camera.target.x = item->pos.x_pos;
	camera.target.z = item->pos.z_pos;

	if (lara.target)
	{
		if (tr5_camera_behaviour) {
			camera.target_angle = lara.target_angles[0];
		} else {
			camera.target_angle = lara.target_angles[0] + item->pos.y_rot;
		}
		camera.target_elevation = lara.target_angles[1] + item->pos.x_rot;
	}
	else
	{
		if (tr5_camera_behaviour) {
			camera.target_angle = lara.head_y_rot + lara.torso_y_rot;
			if (!tomb4.combat_cam_tilt) {
				camera.target_elevation = lara.head_x_rot + lara.torso_x_rot + item->pos.x_rot + mod_camera_info->chase_camera_vertical_orientation;
			} else {
				camera.target_elevation = lara.head_x_rot + lara.torso_x_rot + item->pos.x_rot + mod_camera_info->combat_camera_vertical_orientation;
			}
			camera.target_elevation = lara.head_x_rot + lara.torso_x_rot + item->pos.x_rot - DEGREES_TO_ROTATION(15);
		}
		else {
			camera.target_angle = lara.head_y_rot + lara.torso_y_rot + item->pos.y_rot;
			if (!tomb4.combat_cam_tilt) {
				camera.target_elevation = lara.torso_x_rot + item->pos.x_rot + lara.head_x_rot + mod_camera_info->chase_camera_vertical_orientation;
			} else {
				camera.target_elevation = lara.torso_x_rot + item->pos.x_rot + lara.head_x_rot + mod_camera_info->combat_camera_vertical_orientation;
			}
		}
	}


	room_number = camera.target.room_number;
	floor = GetFloor(camera.target.x, camera.target.y + CLICK_SIZE, camera.target.z, &room_number);

	if (T4PlusIsRoomSwamp(&room[room_number])) {
		camera.target.y = room[room_number].maxceiling - CLICK_SIZE;
	}

	wx = camera.target.x;
	wy = camera.target.y;
	wz = camera.target.z;
	floor = GetFloor(wx, wy, wz, &camera.target.room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	if (c + QUARTER_CLICK_SIZE > h - QUARTER_CLICK_SIZE && h != NO_HEIGHT && c != NO_HEIGHT)
	{
		camera.target.y = (c + h) >> 1;
		camera.target_elevation = 0;
	}
	else if (camera.target.y > h - QUARTER_CLICK_SIZE && h != NO_HEIGHT)
	{
		camera.target.y = h - QUARTER_CLICK_SIZE;
		camera.target_elevation = 0;
	}
	else if (camera.target.y < c + QUARTER_CLICK_SIZE && c != NO_HEIGHT)
	{
		camera.target.y = c + QUARTER_CLICK_SIZE;
		camera.target_elevation = 0;
	}

	wx = camera.target.x;
	wy = camera.target.y;
	wz = camera.target.z;
	GetFloor(wx, wy, wz, &camera.target.room_number);
	room_number = camera.target.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	if (wy < c || wy > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT)
	{
		if (tr5_camera_behaviour) {
			TargetSnaps++;
		}
		camera.target.x = last_target.x;
		camera.target.y = last_target.y;
		camera.target.z = last_target.z;
		camera.target.room_number = last_target.room_number;
	} else {
		TargetSnaps = 0;
	}

	if (tr5_camera_behaviour) {
		UpdateCameraElevation();
	}
	camera.target_distance = mod_camera_info->combat_camera_distance;
	if (tr5_camera_behaviour) {\
		distance = camera.target_distance * phd_cos(camera.actual_elevation) >> W2V_SHIFT;
	} else {
		distance = camera.target_distance * phd_cos(camera.target_elevation) >> W2V_SHIFT;
	}

	if (tr5_camera_behaviour) {
		for (int i = 0; i < MAX_CAMERA_CHASE_IDEALS; i++)
			ideals[i].y = camera.target.y + (camera.target_distance * phd_sin(camera.actual_elevation) >> W2V_SHIFT);
	} else {
		for (int i = 0; i < MAX_CAMERA_CHASE_IDEALS; i++)
			ideals[i].y = (camera.target_distance * phd_sin(camera.target_elevation) >> W2V_SHIFT) + camera.target.y;
	}

	farthest = 0x7FFFFFFF;
	farthestnum = 0;

	for (int i = 0; i < MAX_CAMERA_CHASE_IDEALS; i++)
	{
		if (i) {
			angle = (i - 1) << W2V_SHIFT;
		} else {
			if (tr5_camera_behaviour) {
				angle = camera.actual_angle;
			} else {
				angle = camera.target_angle;
			}
		}

		ideals[i].x = camera.target.x - ((distance * phd_sin(angle)) >> W2V_SHIFT);
		ideals[i].z = camera.target.z - ((distance * phd_cos(angle)) >> W2V_SHIFT);
		ideals[i].room_number = camera.target.room_number;

		if (mgLOS(&camera.target, &ideals[i], 200))
		{
			temp[0].x = ideals[i].x;
			temp[0].y = ideals[i].y;
			temp[0].z = ideals[i].z;
			temp[0].room_number = ideals[i].room_number;
			temp[1].x = camera.pos.x;
			temp[1].y = camera.pos.y;
			temp[1].z = camera.pos.z;
			temp[1].room_number = camera.pos.room_number;

			if (!tr5_camera_behaviour) {
				if (!i)
				{
					farthestnum = 0;
					break;
				}
			}

			if (mgLOS(&temp[0], &temp[1], 0) || !i)
			{
				if (tr5_camera_behaviour) {
					if (!i)
					{
						farthestnum = 0;
						break;
					}
				}

				dx = SQUARE(camera.pos.x - ideals[i].x);
				dz = SQUARE(camera.pos.z - ideals[i].z);
				dz += dx;

				if (dz < farthest)
				{
					farthest = dz;
					farthestnum = i;
				}
			}
		}
		else if (!i)
		{
			temp[0].x = ideals[i].x;
			temp[0].y = ideals[i].y;
			temp[0].z = ideals[i].z;
			temp[0].room_number = ideals[i].room_number;
			temp[1].x = camera.pos.x;
			temp[1].y = camera.pos.y;
			temp[1].z = camera.pos.z;
			temp[1].room_number = camera.pos.room_number;

			if (!tr5_camera_behaviour || mgLOS(&temp[0], &temp[1], 0)) {
				dx = SQUARE(camera.target.x - ideals[i].x);
				dz = SQUARE(camera.target.z - ideals[i].z);
				dz += dx;

				if (dz > 0x90000)
				{
					farthestnum = 0;
					break;
				}
			}
		}
	}

	ideal.x = ideals[farthestnum].x;
	ideal.y = ideals[farthestnum].y;
	ideal.z = ideals[farthestnum].z;
	ideal.room_number = ideals[farthestnum].room_number;
	CameraCollisionBounds(&ideal, (CLICK_SIZE + HALF_CLICK_SIZE), 1);

	if (camera.old_type == FIXED_CAMERA)
		camera.speed = 1;

	MoveCamera(&ideal, camera.speed);
}

void LookCamera(ITEM_INFO* item)
{
	GAME_VECTOR ideal;
	PHD_VECTOR pos1;
	PHD_VECTOR pos2;
	PHD_VECTOR pos3;
	FLOOR_INFO* floor;
	long shake, dx, dy, dz, wx, wy, wz, h, c, rndval, lp;
	short room_number, room_number2, hxrot, txrot, hyrot, tyrot;

	MOD_LEVEL_CAMERA_INFO *mod_camera_info = get_game_mod_level_camera_info(gfCurrentLevel);

	hxrot = lara.head_x_rot;
	hyrot = lara.head_y_rot;
	txrot = lara.torso_x_rot;
	tyrot = lara.torso_y_rot;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	lara.head_x_rot <<= 1;
	lara.head_y_rot <<= 1;

	if (lara.head_x_rot > DEGREES_TO_ROTATION(55))
		lara.head_x_rot = DEGREES_TO_ROTATION(55);
	else if (lara.head_x_rot < -DEGREES_TO_ROTATION(75))
		lara.head_x_rot = -DEGREES_TO_ROTATION(75);

	if (lara.head_y_rot < -DEGREES_TO_ROTATION(80))
		lara.head_y_rot = -DEGREES_TO_ROTATION(80);
	else if (lara.head_y_rot > DEGREES_TO_ROTATION(80))
		lara.head_y_rot = DEGREES_TO_ROTATION(80);

	pos1.x = 0;
	pos1.y = mod_camera_info->look_camera_height;
	pos1.z = QUARTER_CLICK_SIZE;
	GetLaraJointPos(&pos1, LMX_HEAD);
	room_number = lara_item->room_number;
	floor = GetFloor(pos1.x, pos1.y, pos1.z, &room_number);
	h = GetHeight(floor, pos1.x, pos1.y, pos1.z);
	c = GetCeiling(floor, pos1.x, pos1.y, pos1.z);

	if (h == NO_HEIGHT || c == NO_HEIGHT || c >= h || pos1.y > h || pos1.y < c)
	{
		pos1.x = 0;
		pos1.y = mod_camera_info->look_camera_height;
		pos1.z = 0;
		GetLaraJointPos(&pos1, LMX_HEAD);

		// T4Plus - swamp
		room_number = lara_item->room_number;
		floor = GetFloor(pos1.x, pos1.y + CLICK_SIZE, pos1.z, &room_number);
		if (T4PlusIsRoomSwamp(&room[room_number]))
			pos1.y = room[room_number].maxceiling - CLICK_SIZE;

		floor = GetFloor(pos1.x, pos1.y, pos1.z, &room_number);
		h = GetHeight(floor, pos1.x, pos1.y, pos1.z);
		c = GetCeiling(floor, pos1.x, pos1.y, pos1.z);

		if (h == NO_HEIGHT || c == NO_HEIGHT || c >= h || pos1.y > h || pos1.y < c)
		{
			pos1.x = 0;
			pos1.y = mod_camera_info->look_camera_height;
			pos1.z = -QUARTER_CLICK_SIZE;
			GetLaraJointPos(&pos1, LMX_HEAD);

			room_number = lara_item->room_number;
			floor = GetFloor(pos1.x, pos1.y + CLICK_SIZE, pos1.z, &room_number);

			if (T4PlusIsRoomSwamp(&room[room_number]))
				pos1.y = room[room_number].maxceiling - CLICK_SIZE;

			floor = GetFloor(pos1.x, pos1.y, pos1.z, &room_number);
			h = GetHeight(floor, pos1.x, pos1.y, pos1.z);
			c = GetCeiling(floor, pos1.x, pos1.y, pos1.z);

			if (h == NO_HEIGHT || c == NO_HEIGHT || c >= h || pos1.y > h || pos1.y < c)
			{
				pos1.x = 0;
				pos1.y = mod_camera_info->look_camera_height;
				pos1.z = -QUARTER_CLICK_SIZE;
				GetLaraJointPos(&pos1, LMX_HEAD);
			}
		}
	}

	pos2.x = 0;
	pos2.y = 0;
	pos2.z = mod_camera_info->look_camera_distance;
	GetLaraJointPos(&pos2, LMX_HEAD);
	pos3.x = 0;
	pos3.y = 0;
	pos3.z = (BLOCK_SIZE * 2);
	GetLaraJointPos(&pos3, LMX_HEAD);
	wy = pos1.y;
	wx = pos1.x;
	wz = pos1.z;
	dx = (pos2.x - pos1.x) >> 3;
	dy = (pos2.y - pos1.y) >> 3;
	dz = (pos2.z - pos1.z) >> 3;
	room_number2 = lara_item->room_number;

	for (lp = 0; lp < 8; lp++)
	{
		room_number = room_number2;

		// T4Plus - swamp
		floor = GetFloor(wx, wy + CLICK_SIZE, wz, &room_number2);
		if (T4PlusIsRoomSwamp(&room[room_number]))
		{
			wy = room[room_number2].maxceiling - CLICK_SIZE;
			break;
		}

		floor = GetFloor(wx, wy, wz, &room_number2);
		h = GetHeight(floor, wx, wy, wz);
		c = GetCeiling(floor, wx, wy, wz);

		if (h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy > h || wy < c)
			break;

		wz += dz;
		wx += dx;
		wy += dy;
	}

	if (lp)
	{
		wx -= dx;
		wy -= dy;
		wz -= dz;
	}

	ideal.x = wx;
	ideal.y = wy;
	ideal.z = wz;
	ideal.room_number = room_number;

	if (old_cam.pos.x_rot == lara.head_x_rot && old_cam.pos.y_rot == lara.head_y_rot && old_cam.pos.x_pos == lara_item->pos.x_pos &&
		old_cam.pos.y_pos == lara_item->pos.y_pos && old_cam.pos.z_pos == lara_item->pos.z_pos &&
		old_cam.current_anim_state == lara_item->current_anim_state && old_cam.goal_anim_state == lara_item->goal_anim_state &&
		camera.old_type == LOOK_CAMERA)
	{
		ideal.x = static_lookcamp.x;
		ideal.y = static_lookcamp.y;
		ideal.z = static_lookcamp.z;
		ideal.room_number = static_lookcamp.room_number;
		pos3.x = static_lookcamt.x;
		pos3.y = static_lookcamt.y;
		pos3.z = static_lookcamt.z;
	}
	else
	{
		old_cam.pos.x_rot = lara.head_x_rot;
		old_cam.pos.y_rot = lara.head_y_rot;
		old_cam.pos.x_pos = lara_item->pos.x_pos;
		old_cam.pos.y_pos = lara_item->pos.y_pos;
		old_cam.pos.z_pos = lara_item->pos.z_pos;
		old_cam.current_anim_state = lara_item->current_anim_state;
		old_cam.goal_anim_state = lara_item->goal_anim_state;
		static_lookcamp.room_number = room_number;
		static_lookcamp.x = wx;
		static_lookcamp.y = wy;
		static_lookcamp.z = wz;
		static_lookcamt.x = pos3.x;
		static_lookcamt.y = pos3.y;
		static_lookcamt.z = pos3.z;
	}

	CameraCollisionBounds(&ideal, 224, 1);

	if (camera.old_type == FIXED_CAMERA)
	{
		camera.pos.x = ideal.x;
		camera.pos.y = ideal.y;
		camera.pos.z = ideal.z;
		camera.target.x = pos3.x;
		camera.target.y = pos3.y;
		camera.target.z = pos3.z;
	}
	else
	{
		camera.pos.x += (ideal.x - camera.pos.x) >> 2;
		camera.pos.y += (ideal.y - camera.pos.y) >> 2;
		camera.pos.z += (ideal.z - camera.pos.z) >> 2;
		camera.target.x += (pos3.x - camera.target.x) >> 2;
		camera.target.y += (pos3.y - camera.target.y) >> 2;
		camera.target.z += (pos3.z - camera.target.z) >> 2;
	}

	camera.target.room_number = lara_item->room_number;

	if (camera.bounce && camera.type == camera.old_type)
	{
		if (camera.bounce <= 0)
		{
			rndval = -camera.bounce;
			shake = -camera.bounce >> 1;
			camera.target.x += GetRandomControl() % rndval - shake;
			camera.target.y += GetRandomControl() % rndval - shake;
			camera.target.z += GetRandomControl() % rndval - shake;
			camera.bounce += 5;
		}
		else
		{
			camera.pos.y += camera.bounce;
			camera.target.y += camera.bounce;
			camera.bounce = 0;
		}
	}

	GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);
	wx = camera.pos.x;
	wy = camera.pos.y;
	wz = camera.pos.z;
	room_number = camera.pos.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	if (c > wy - (CLICK_SIZE - 1) && h<wy + (CLICK_SIZE - 1) && h > c && c != NO_HEIGHT && h != NO_HEIGHT)
		camera.pos.y = (h + c) >> 1;
	else if (h < wy + (CLICK_SIZE - 1) && h > c && c != NO_HEIGHT && h != NO_HEIGHT)
		camera.pos.y = h - (CLICK_SIZE - 1);
	else if (c > wy - (CLICK_SIZE - 1) && h > c && c != NO_HEIGHT && h != NO_HEIGHT)
		camera.pos.y = c + (CLICK_SIZE - 1);

	wx = camera.pos.x;
	wy = camera.pos.y;
	wz = camera.pos.z;
	room_number = camera.pos.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	// T4Plus - swamp
	if (T4PlusIsRoomSwamp(&room[room_number]))
	{
		camera.pos.y = room[room_number].y - CLICK_SIZE;
	}
	else if (wy < c || wy > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT)
		mgLOS(&camera.target, &camera.pos, 0);

	wx = camera.pos.x;
	wy = camera.pos.y;
	wz = camera.pos.z;
	room_number = camera.pos.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);
	
	if (wy < c || wy > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT || T4PlusIsRoomSwamp(&room[room_number]))
	{
		camera.pos.x = pos1.x;
		camera.pos.y = pos1.y;
		camera.pos.z = pos1.z;
		camera.pos.room_number = lara_item->room_number;
	}

	GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);
	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);

	if (camera.mike_at_lara)
	{
		if (!tr5_camera_behaviour) {
			if (camera.actual_elevation != lara.head_y_rot + lara.torso_y_rot + lara_item->pos.y_rot) {
				camera.actual_elevation = lara.head_y_rot + lara.torso_y_rot + lara_item->pos.y_rot;
			}
		}
		camera.mike_pos.x = lara_item->pos.x_pos;
		camera.mike_pos.y = lara_item->pos.y_pos;
		camera.mike_pos.z = lara_item->pos.z_pos;
	}else {
		dx = camera.target.x - camera.pos.x;
		dz = camera.target.z - camera.pos.z;
		dx = phd_atan(dz, dx);
		if (!tr5_camera_behaviour) {
			if (camera.actual_elevation != (short)dx) {
				camera.actual_elevation = (short)dx;
			}
		}
		camera.mike_pos.x = camera.pos.x + (phd_sin(dx) * phd_persp >> W2V_SHIFT);
		camera.mike_pos.y = camera.pos.y;
		camera.mike_pos.z = camera.pos.z + (phd_cos(dx) * phd_persp >> W2V_SHIFT);
	}

	camera.old_type = camera.type;
	lara.head_x_rot = hxrot;
	lara.head_y_rot = hyrot;
	lara.torso_x_rot = txrot;
	lara.torso_y_rot = tyrot;
}

void FixedCamera()
{
	OBJECT_VECTOR* fixed;
	GAME_VECTOR ideal;

	if (UseForcedFixedCamera)
	{
		ideal.x = ForcedFixedCamera.x;
		ideal.y = ForcedFixedCamera.y;
		ideal.z = ForcedFixedCamera.z;
		ideal.room_number = ForcedFixedCamera.room_number;
	}
	else
	{
		fixed = &camera.fixed[camera.number];
		ideal.x = fixed->x;
		ideal.y = fixed->y;
		ideal.z = fixed->z;
		ideal.room_number = fixed->data;
	}

	camera.fixed_camera = 1;
	MoveCamera(&ideal, 1);

	if (camera.timer)
	{
		camera.timer--;

		if (!camera.timer)
			camera.timer = -1;
	}
}

void BinocularCamera(ITEM_INFO* item)
{
	FLOOR_INFO* floor;
	PHD_VECTOR pos1;
	PHD_VECTOR pos3;
	PHD_VECTOR Soffset;
	PHD_VECTOR Eoffset;
	short* ammo;
	long shake, speed, c, BinocStep, rndval, dx, dz;
	short room_number, hxrot, hyrot;
	char Fire;

	if (!LaserSight)
	{
		if (inputBusy & IN_DRAW)
			ExittingBinos = 1;
		else if (ExittingBinos)
		{
			ExittingBinos = 0;
			BinocularRange = 0;
			AlterFOV(DEGREES_TO_ROTATION(DEFAULT_FOV));
			lara_item->mesh_bits = -1;
			lara.Busy = 0;
			lara.head_y_rot = 0;
			lara.head_x_rot = 0;
			lara.torso_y_rot = 0;
			lara.torso_x_rot = 0;
			bLaraTorch = 0;
			camera.type = BinocularOldCamera;
			return;
		}
	}

	lara_item->mesh_bits = 0;
	AlterFOV(short(7 * (((BLOCK_SIZE * 2) + (QUARTER_CLICK_SIZE / 2)) - BinocularRange)));
	hxrot = lara.head_x_rot << 1;
	hyrot = lara.head_y_rot;

	if (hxrot > DEGREES_TO_ROTATION(75))
		hxrot = DEGREES_TO_ROTATION(75);
	else if (hxrot < -DEGREES_TO_ROTATION(75))
		hxrot = -DEGREES_TO_ROTATION(75);

	if (hyrot > DEGREES_TO_ROTATION(80))
		hyrot = DEGREES_TO_ROTATION(80);
	else if (hyrot < -DEGREES_TO_ROTATION(80))
		hyrot = -DEGREES_TO_ROTATION(80);

	hyrot += lara_item->pos.y_rot;
	pos1.x = lara_item->pos.x_pos;
	pos1.y = lara_item->pos.y_pos;
	pos1.z = lara_item->pos.z_pos;
	room_number = lara_item->room_number;
	floor = GetFloor(pos1.x, pos1.y, pos1.z, &room_number);
	c = GetCeiling(floor, pos1.x, pos1.y, pos1.z);

	if (c <= pos1.y - (HALF_CLICK_SIZE + CLICK_SIZE))
		pos1.y -= (HALF_CLICK_SIZE + CLICK_SIZE);
	else
		pos1.y += QUARTER_CLICK_SIZE;

	speed = (20736 * phd_cos(hxrot)) >> W2V_SHIFT;
	pos3.x = pos1.x + (phd_sin(hyrot) * speed >> W2V_SHIFT);
	pos3.y = pos1.y - (phd_sin(hxrot) * 20736 >> W2V_SHIFT);
	pos3.z = pos1.z + (phd_cos(hyrot) * speed >> W2V_SHIFT);
	camera.pos.x = pos1.x;
	camera.pos.y = pos1.y;
	camera.pos.z = pos1.z;
	camera.pos.room_number = room_number;

	if (camera.old_type == FIXED_CAMERA)
	{
		camera.target.x = pos3.x;
		camera.target.y = pos3.y;
		camera.target.z = pos3.z;
	}
	else
	{
		camera.target.x += (pos3.x - camera.target.x) >> 2;
		camera.target.y += (pos3.y - camera.target.y) >> 2;
		camera.target.z += (pos3.z - camera.target.z) >> 2;
	}

	camera.target.room_number = lara_item->room_number;

	if (camera.bounce && camera.type == camera.old_type)
	{
		if (camera.bounce <= 0)
		{
			rndval = -camera.bounce;
			shake = rndval >> 1;
			camera.target.x += (GetRandomControl() % rndval - shake) << 4;
			camera.target.y += (GetRandomControl() % rndval - shake) << 4;
			camera.target.z += (GetRandomControl() % rndval - shake) << 4;
			camera.bounce += 5;
		}
		else
		{
			camera.bounce = 0;
			camera.target.y += camera.bounce;
		}
	}

	GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);
	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);

	if (camera.mike_at_lara)
	{
		if (!tr5_camera_behaviour) {
			if (camera.actual_elevation != lara.head_y_rot + lara.torso_y_rot + lara_item->pos.y_rot) {
				camera.actual_elevation = lara.head_y_rot + lara.torso_y_rot + lara_item->pos.y_rot;
			}
		}
		camera.mike_pos.x = lara_item->pos.x_pos;
		camera.mike_pos.y = lara_item->pos.y_pos;
		camera.mike_pos.z = lara_item->pos.z_pos;
	} else {
		dx = camera.target.x - camera.pos.x;
		dz = camera.target.z - camera.pos.z;
		dx = phd_atan(dz, dx);
		if (!tr5_camera_behaviour) {
			if (camera.actual_elevation != (short)dx) {
				camera.actual_elevation = (short)dx;
			}
		}
		camera.mike_pos.x = camera.pos.x + (phd_sin(dx) * phd_persp >> W2V_SHIFT);
		camera.mike_pos.y = camera.pos.y;
		camera.mike_pos.z = camera.pos.z + (phd_cos(dx) * phd_persp >> W2V_SHIFT);
	}

	BinocStep = inputBusy & IN_WALK ? (QUARTER_CLICK_SIZE / 2) : QUARTER_CLICK_SIZE;

	if (inputBusy & IN_SPRINT)
		BinocularRange -= BinocStep;
	else if (inputBusy & IN_DUCK)
		BinocularRange += BinocStep;

	if (BinocularRange < 128)
		BinocularRange = 128;
	else if (BinocularRange > 1536)
		BinocularRange = 1536;

	Soffset.x = camera.pos.x;
	Soffset.y = camera.pos.y;
	Soffset.z = camera.pos.z;
	Eoffset.x = camera.target.x;
	Eoffset.y = camera.target.y;
	Eoffset.z = camera.target.z;

	if (LaserSight)
	{
		ammo = get_current_ammo_pointer(lara.gun_type);

		if (inputBusy & IN_ACTION && !WeaponDelay && ammo[0])
		{
			Fire = 1;

			if (lara.gun_type == WEAPON_REVOLVER)
			{
				WeaponDelay = 16;
				savegame.Game.AmmoUsed++;
			}
			else
				WeaponDelay = 32;

			if (ammo[0] != -1)
				ammo[0]--;
		}
		else
			Fire = 0;

		GetTargetOnLOS(&camera.pos, &camera.target, 1, Fire);
	}
	else
	{
		GetTargetOnLOS(&camera.pos, &camera.target, 0, 0);

		if (inputBusy & IN_ACTION)
			LaraTorch(&Soffset, &Eoffset, lara.head_y_rot, 192);
		else
			bLaraTorch = 0;
	}
}

void ConfirmCameraTargetPos() {
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	long wx, wy, wz, c, h;
	short room_number;

	pos.z = 0;
	pos.y = 0;
	pos.x = 0;
	GetJointAbsPosition(lara_item, &pos, LM_HEAD);

	if (camera.lara_node != -1) {
		camera.target.x = pos.x;
		camera.target.y = pos.y;
		camera.target.z = pos.z;
	} else {
		camera.target.x = lara_item->pos.x_pos;
		camera.target.y = (camera.target.y + pos.y) >> 1;
		camera.target.z = lara_item->pos.z_pos;
	}

	wx = camera.target.x;
	wy = camera.target.y;
	wz = camera.target.z;
	room_number = camera.target.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz);
	c = GetCeiling(floor, wx, wy, wz);

	if (wy < c || h < wy || h <= c || h == NO_HEIGHT || c == NO_HEIGHT) {
		camera.target.x = pos.x;
		camera.target.y = pos.y;
		camera.target.z = pos.z;
	}
}

void CalculateCamera()
{
	ITEM_INFO* item;
	OBJECT_VECTOR* fixed;
	PHD_VECTOR v;
	short* bounds;
	long shift, fixed_camera, y, dx, dz;
	short gotit, change, tilt;

	if (camera_frozen)
		return;

	MOD_LEVEL_CAMERA_INFO *mod_camera_info = get_game_mod_level_camera_info(gfCurrentLevel);

	if (BinocularRange)
	{
		BinocularOn = 1;
		BinocularCamera(lara_item);

		if (BinocularRange)
			return;
	}

	if (BinocularOn == 1)
		BinocularOn = -8;

	old_cam.t.z = camera.target.z;
	old_cam.t.x = camera.target.x;
	old_cam.t.y = camera.target.y;

	if (UseForcedFixedCamera)
	{
		camera.type = FIXED_CAMERA;

		if (camera.old_type == FIXED_CAMERA)
			camera.speed = 1;
	}

	if (room[camera.pos.room_number].flags & ROOM_UNDERWATER)
	{
		SoundEffect(SFX_UNDERWATER, 0, SFX_ALWAYS);

		if (!camera.underwater)
			//empty func call here
			camera.underwater = 1;
	}
	else if (camera.underwater)
		//if unused var -> empty func call here
		camera.underwater = 0;

	if (camera.type == CINEMATIC_CAMERA)
	{
		do_new_cutscene_camera();
		return;
	}

	item = camera.item;

	if (camera.item && (camera.type == FIXED_CAMERA || camera.type == HEAVY_CAMERA))
		fixed_camera = 1;
	else
	{
		item = lara_item;
		fixed_camera = 0;
	}

	bounds = GetBoundsAccurate(item);

	if (tr5_camera_behaviour) {
		y = ((bounds[2] + bounds[3]) >> 1) + item->pos.y_pos - CLICK_SIZE;
	} else {
		if (fixed_camera)
			y = item->pos.y_pos + (bounds[2] + bounds[3]) / 2;
		else
			y = item->pos.y_pos + bounds[3] + ((3 * (bounds[2] - bounds[3])) >> 2);
	}

	if (camera.item && !fixed_camera)
	{
		dx = camera.item->pos.x_pos - item->pos.x_pos;
		dz = camera.item->pos.z_pos - item->pos.z_pos;
		shift = phd_sqrt(SQUARE(dx) + SQUARE(dz));
		gotit = short(phd_atan(dz, dx) - item->pos.y_rot);
		gotit >>= 1;
		bounds = GetBoundsAccurate(camera.item);
		tilt = (short)phd_atan(shift, y - (bounds[2] + bounds[3]) / 2 - camera.item->pos.y_pos) >> 1;

		if (gotit > -DEGREES_TO_ROTATION(50) && gotit < DEGREES_TO_ROTATION(50) && tilt > -DEGREES_TO_ROTATION(85) && tilt < DEGREES_TO_ROTATION(85))
		{
			change = gotit - lara.head_y_rot;

			if (change > DEGREES_TO_ROTATION(4))
				lara.head_y_rot += DEGREES_TO_ROTATION(4);
			else if (change < -DEGREES_TO_ROTATION(4))
				lara.head_y_rot -= DEGREES_TO_ROTATION(4);
			else
				lara.head_y_rot = gotit;

			lara.torso_y_rot = lara.head_y_rot;
			change = tilt - lara.head_x_rot;

			if (change > DEGREES_TO_ROTATION(4))
				lara.head_x_rot += DEGREES_TO_ROTATION(4);
			else if (change < -DEGREES_TO_ROTATION(4))
				lara.head_x_rot -= DEGREES_TO_ROTATION(4);
			else
				lara.head_x_rot = tilt;

			lara.torso_x_rot = lara.head_x_rot;
			camera.type = LOOK_CAMERA;
			camera.item->looked_at = 1;
		}
	}

	if (camera.type != LOOK_CAMERA && camera.type != COMBAT_CAMERA)
	{
		last_target.x = camera.target.x;
		last_target.y = camera.target.y;
		last_target.z = camera.target.z;
		last_target.room_number = camera.target.room_number;

		if (!tr5_camera_behaviour) {
			camera.target.x = item->pos.x_pos;
			camera.target.z = item->pos.z_pos;

			if (camera.flags == 1 || UseForcedFixedCamera)
			{
				shift = (bounds[4] + bounds[5]) / 2;
				camera.target.x += (phd_sin(item->pos.y_rot) * shift >> W2V_SHIFT);
				camera.target.z += (phd_cos(item->pos.y_rot) * shift >> W2V_SHIFT);
			}
		}

		camera.target.room_number = item->room_number;
		camera.target.y = y;

		if (tr5_camera_behaviour) {
			gotit = 0;
			if (camera.type != CHASE_CAMERA && camera.flags != 3) {
				fixed = &camera.fixed[camera.number];
				if (fixed->flags & 2) {
					v.x = 0;
					v.y = 0;
					v.z = 0;
					GetLaraJointPos(&v, LMX_TORSO);
					camera.target.x = v.x;
					camera.target.y = v.y;
					camera.target.z = v.z;
					gotit = 1;
				}
			}
			if (!gotit) {
				shift = (bounds[0] + bounds[1] + bounds[4] + bounds[5]) >> 2;
				camera.target.x = item->pos.x_pos + (shift * phd_sin(item->pos.y_rot) >> W2V_SHIFT);
				camera.target.z = item->pos.z_pos + (shift * phd_cos(item->pos.y_rot) >> W2V_SHIFT);
				
				if (item->object_number == LARA) {
					ConfirmCameraTargetPos();
				}
			}
		}

		if (fixed_camera != camera.fixed_camera)
		{
			camera.fixed_camera = 1;
			camera.speed = 1;
		}
		else
		{
			camera.fixed_camera = 0;

			if (camera.speed != 1 && camera.old_type != LOOK_CAMERA && BinocularOn >= 0)
			{
				if (TargetSnaps <= 8) {
					camera.target.x = old_cam.t.x + ((camera.target.x - old_cam.t.x) >> 2);
					camera.target.y = old_cam.t.y + ((camera.target.y - old_cam.t.y) >> 2);
					camera.target.z = old_cam.t.z + ((camera.target.z - old_cam.t.z) >> 2);
				} else {
					TargetSnaps = 0;
				}
			}
		}

		GetFloor(camera.target.x, camera.target.y, camera.target.z, &camera.target.room_number);

		if (tr5_camera_behaviour) {
			if (abs(last_target.x - camera.target.x) < 4 && abs(last_target.y - camera.target.y) < 4 && abs(last_target.z - camera.target.z) < 4)
			{
				camera.target.x = last_target.x;
				camera.target.y = last_target.y;
				camera.target.z = last_target.z;
			}
		}

		if (camera.type == CHASE_CAMERA || camera.flags == 3)
			ChaseCamera(item);
		else
			FixedCamera();
	}
	else
	{
		if (!tr5_camera_behaviour) {
			if (camera.type != COMBAT_CAMERA || tomb4.combat_cam_tilt)
				y -= mod_camera_info->add_on_battle_camera_top;
		}

		if (camera.type == COMBAT_CAMERA)
		{
			last_target.x = camera.target.x;
			last_target.y = camera.target.y;
			last_target.z = camera.target.z;
			last_target.room_number = camera.target.room_number;
		}

		camera.target.room_number = item->room_number;

		if (camera.fixed_camera || BinocularOn < 0)
		{
			camera.target.y = y;
			camera.speed = 1;
		}
		else
		{
			camera.target.y += (y - camera.target.y) >> 2;
			camera.speed = camera.type != LOOK_CAMERA ? 8 : 4;
		}

		camera.fixed_camera = 0;

		if (camera.type == LOOK_CAMERA)
			LookCamera(item);
		else
			CombatCamera(item);
	}

	camera.fixed_camera = fixed_camera;
	camera.last = camera.number;

	if (camera.type != HEAVY_CAMERA || camera.timer == -1)
	{
		camera.type = CHASE_CAMERA;
		camera.speed = mod_camera_info->camera_speed;
		camera.number = -1;
		camera.last_item = camera.item;
		camera.item = 0;
		camera.target_elevation = 0;
		camera.target_angle = 0;
		camera.target_distance = mod_camera_info->chase_camera_distance;
		camera.flags = 0;
		camera.lara_node = -1;
	}
}
