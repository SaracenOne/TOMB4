#pragma once

enum NGNumericOperationType {
	NG_SET = 0,
	NG_ADD,
	NG_SUBTRACT,
	NG_MULTIPLY,
	NG_DIVIDE,
	NG_BIT_SET,
	NG_BIT_CLEAR,
};

enum NGSavegameMemoryValue {
	NG_TRNG_ITEM_INDEX = 0x00, // (short, index of last item found with testposition or condition)
	NG_TRNG_ANIM_COMMAND_MOVEABLE_INDEX = 0x01, // (short, index of moveable performing last AnimCommand)
	NG_TRNG_SELECTED_ITEM_INDEX = 0x02, // (short, item index for selected item memory)
	NG_LARA_ITEM_INDEX = 0x03, // (short)
	NG_LARA_HANDS_ATTACHED_LARA_STATUS = 0x04, // (short)
	NG_LARA_HANDS_ITEM_IN_THE_HANDS_OF_LARA_CURRENT = 0x05, // (short)
	NG_LARA_HANDS_ITEM_IN_THE_HANDS_OF_LARA_FOLLOWING = 0x06, // (short)
	NG_LARA_CURRENT_WEAPON = 0x07, // (short, does not have to be hands)
	NG_LARA_ENVIRONMENT_WHERE_LARA_IS = 0x08, // (short ground, water, ect.)
	NG_LARA_TEST_CLIMB_SECTOR = 0x09, // (short)
	NG_LARA_AIR_FOR_LARA = 0x0A, // (short)
	NG_LARA_TIME_IN_DEATH = 0x0B, // (short)
	NG_LARA_HANDS_REMAINING_TIME_WITH_LIGHTED_FLARE_IN_THE_HANDS = 0x0C, // (short)
};

extern unsigned char NGNumericOperationByte(NGNumericOperationType number_operation, unsigned char variable, unsigned int value);
extern unsigned short NGNumericOperationShort(NGNumericOperationType number_operation, unsigned short variable, unsigned int value);
extern unsigned long NGNumericOperationLong(NGNumericOperationType number_operation, unsigned long variable, unsigned int value);
extern void NGNumericOperation(NGNumericOperationType number_operation, unsigned int variable, unsigned int value);
extern int NGNumericGetVariable(unsigned int variable);
extern int NGNumericGetSavegameValue(unsigned int variable);