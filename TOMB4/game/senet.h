#pragma once
#include "../global/types.h"

void InitialiseSenet(short item_number);
void MakeMove(long piece, long displacement);
void SenetControl(short item_number);
long CheckSenetWinner(long won);
void InitialiseGameStix(short item_number);
void ThrowSticks(ITEM_INFO* item);
void GameStixControl(short item_number);
void GameStixCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void ShockwaveExplosion(ITEM_INFO* item, ulong col, long speed);
void ControlGodHead(short item_number);
void DrawGodHead(ITEM_INFO* item);

#define SENET_ITEM_MID 3
#define SENET_ITEM_COUNT 6
#define SENET_BOARD_COUNT 17

extern long SenetTargetX;
extern long SenetTargetZ;
extern short senet_item[SENET_ITEM_COUNT];
extern char piece_moving;
extern char last_throw;
extern char senet_board[SENET_BOARD_COUNT];
extern char senet_piece[SENET_ITEM_COUNT];
