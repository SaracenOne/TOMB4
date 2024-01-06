#pragma once

#define MAX_MIRRORED_ANIMATING_ITEM_IDS 32

enum T4PlusMirrorDirection {
	T4_MIR_DIR_NORTH,
	T4_MIR_DIR_EAST,
	T4_MIR_DIR_SOUTH,
	T4_MIR_DIR_WEST,
	T4_MIR_DIR_UP,
	T4_MIR_DIR_DOWN,
};

struct T4PlusMirrorInfo {
	short mirror_room;
	int pivot_point;
	T4PlusMirrorDirection direction;
	short mirrored_animating_item_ids[MAX_MIRRORED_ANIMATING_ITEM_IDS];
	bool inverse;
};

struct T4PlusRoomMirrorInfo {
	int mirror_count = -1;
	T4PlusMirrorInfo *mirrors;
};

T4PlusRoomMirrorInfo *room_mirror_info;

bool T4PlusGetMirrorCountForRoom(int room_number) {
	return room_mirror_info[room_number].mirror_count;
}