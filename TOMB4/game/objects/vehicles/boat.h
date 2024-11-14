#pragma once
#include "../../../global/types.h"

#define RUBBER_BOAT_TOP_SPEED	110
#define RUBBER_BOAT_SLOW_SPEED	(RUBBER_BOAT_TOP_SPEED / 3)
#define RUBBER_BOAT_FAST_SPEED	(RUBBER_BOAT_TOP_SPEED + 75)

#define MOTOR_BOAT_TOP_SPEED	90
#define MOTOR_BOAT_SLOW_SPEED	(MOTOR_BOAT_TOP_SPEED / 3)
#define MOTOR_BOAT_FAST_SPEED	(MOTOR_BOAT_TOP_SPEED + 75)

#define BOAT_OCB_HEADLIGHT (1 << 0)
#define BOAT_OCB_SKIP_HEAVY_TRIGGERS (1 << 1)
#define BOAT_OCB_SKIP_REGULAR_TRIGGERS (1 << 2)
#define BOAT_OCB_LOOK_AROUND (1 << 3)
#define BOAT_OCB_NO_FUEL (1 << 4)
#define BOAT_OCB_FUEL_MANAGEMENT (1 << 5)
#define BOAT_OCB_ANCHORED (1 << 6)
#define BOAT_OCB_SHOW_FUEL_BAR (1 << 7)

struct BOAT_INFO
{
	long boat_turn;
	long left_fallspeed;
	long right_fallspeed;
	short tilt_angle;
	short extra_rotation;
	long water;
	long pitch;
	short prop_rot;
	short light_intensity;
};

extern void InitialiseBoat(short item_num);
extern void BoatCollision(short item_num, ITEM_INFO *l, COLL_INFO *coll);
extern void DrawBoat(ITEM_INFO *item);

extern void RubberBoatControl(short item_num);
extern void MotorBoatControl(short item_num);
