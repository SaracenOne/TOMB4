#pragma once
#include "../../../global/types.h"

#define BADDY_STATE_IDLE 0
#define BADDY_STATE_WALK 1
#define BADDY_STATE_RUN 2
#define BADDY_STATE_DODGE_START 4
#define BADDY_STATE_DODGE 8
#define BADDY_STATE_DODGE_END 9
#define BADDY_STATE_DRAW_GUN 10
#define BADDY_STATE_HOLSTER_GUN 11
#define BADDY_STATE_DRAW_SWORD 12
#define BADDY_STATE_HOLSTER_SWORD 13
#define BADDY_STATE_FIRE 14
#define BADDY_STATE_SWORD_HIT_FRONT 15
#define BADDY_STATE_SWORD_HIT_RIGHT 16
#define BADDY_STATE_SWORD_HIT_LEFT 17
#define BADDY_STATE_MONKEY_GRAB 18
#define BADDY_STATE_MONKEY_IDLE 19
#define BADDY_STATE_MONKEY_FORWARD 20
#define BADDY_STATE_MONKEY_PUSH_OFF 21
#define BADDY_STATE_MONKEY_FALL_LAND 22
#define BADDY_STATE_ROLL_LEFT 23
#define BADDY_STATE_JUMP_RIGHT 24
#define BADDY_STATE_STAND_TO_CROUCH 25
#define BADDY_STATE_CROUCH 26
#define BADDY_STATE_CROUCH_PICKUP 27
#define BADDY_STATE_CROUCH_TO_STAND 28
#define BADDY_STATE_WALK_SWORD_HIT_RIGHT 29
#define BADDY_STATE_SOMERSAULT 30
#define BADDY_STATE_AIM 31
#define BADDY_STATE_DEATH 32
#define BADDY_STATE_JUMP_FORWARD_1_BLOCK 33
#define BADDY_STATE_JUMP_FORWARD_FALL 34
#define BADDY_STATE_MONKEY_TO_FREEFALL 35
#define BADDY_STATE_FREEFALL 36
#define BADDY_STATE_FREEFALL_LAND_DEATH 37
#define BADDY_STATE_JUMP_FORWARD_2_BLOCKS 38
#define BADDY_STATE_CLIMB_4_STEPS 39
#define BADDY_STATE_CLIMB_3_STEPS 40
#define BADDY_STATE_CLIMB_2_STEPS 41
#define BADDY_STATE_JUMP_OFF_4_STEPS 42
#define BADDY_STATE_JUMP_OFF_3_STEPS 43
#define BADDY_STATE_BLIND 44

#define BADDY_SOMERSAULT_END_ANIMATION 4
#define BADDY_STAND_IDLE_ANIMATION 18
#define BADDY_STAND_TO_ROLL_LEFT_ANIMATION 24
#define BADDY_CROUCH_ANIMATION 29
#define BADDY_STAND_DEATH_ANIMATION 45
#define BADDY_STAND_TO_JUMP_RIGHT_ANIMATION 47
#define BADDY_STAND_TO_JUMP_FORWARD_ANIMATION 55
#define BADDY_MONKEY_TO_FREEFALL_ANIMATION 59
#define BADDY_CLIMB_4_CLICKS_ANIMATION 62
#define BADDY_CLIMB_3_CLICKS_ANIMATION 63
#define BADDY_CLIMB_2_CLICKS_ANIMATION 64
#define BADDY_JUMP_OFF_4_CLICKS_ANIMATION 65
#define BADDY_JUMP_OFF_3_CLICKS_ANIMATION 66
#define BADDY_BLIND_ANIMATION 68

void InitialiseBaddy(short item_number);
void BaddyControl(short item_number);
