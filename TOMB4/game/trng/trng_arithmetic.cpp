#include "../../tomb4/pch.h"

#include "trng.h"
#include "trng_arithmetic.h"
#include "trng_extra_state.h"

unsigned char NGNumericOperationByte(NGNumericOperationType number_operation_type, unsigned char variable, unsigned int value) {
	switch (number_operation_type) {
		case NG_SET: {
			return value;
		}
		case NG_ADD: {
			return variable + value;
		}
		case NG_SUBTRACT: {
			return variable - value;
		}
		case NG_MULTIPLY: {
			return variable * value;
		}
		case NG_DIVIDE: {
			return variable / value;
		}
		case NG_BIT_SET: {
			return variable | (1 << value);
		}
		case NG_BIT_CLEAR: {
			return variable & ~(1 << value);
		}
	}

	return 0;
}

unsigned short NGNumericOperationShort(NGNumericOperationType number_operation_type, unsigned short variable, unsigned int value) {
	switch (number_operation_type) {
		case NG_SET: {
			return value;
			break;
		}
		case NG_ADD: {
			return variable + value;
			break;
		}
		case NG_SUBTRACT: {
			return variable - value;
			break;
		}
		case NG_MULTIPLY: {
			return variable * value;
			break;
		}
		case NG_DIVIDE: {
			return variable / value;
			break;
		}
		case NG_BIT_SET: {
			return variable | (1 << value);
		}
		case NG_BIT_CLEAR: {
			return variable & ~(1 << value);
		}
	}
	
	return 0;
}

unsigned long NGNumericOperationLong(NGNumericOperationType number_operation_type, unsigned long variable, unsigned int value) {
	switch (number_operation_type) {
		case NG_SET: {
			return value;
			break;
		}
		case NG_ADD: {
			return variable + value;
			break;
		}
		case NG_SUBTRACT: {
			return variable - value;
			break;
		}
		case NG_MULTIPLY: {
			return variable * value;
			break;
		}
		case NG_DIVIDE: {
			return variable / value;
			break;
		}
		case NG_BIT_SET: {
			return variable | (1 << value);
		}
		case NG_BIT_CLEAR: {
			return variable & ~(1 << value);
		}
	}

	return 0;
}

void NGNumericOperation(NGNumericOperationType number_operation, unsigned int variable, unsigned int value) {
	switch (variable) {
		case 0xffff: {
			ng_current_value = NGNumericOperationByte(number_operation, ng_current_value, value);
			break;
		}

		/* Globals */

		// Global Alfa Byte
		case 0x00: {
			unsigned char ng_global_alfa_1 = (ng_global_alfa & 0xff);
			ng_global_alfa_1 = NGNumericOperationByte(number_operation, ng_global_alfa_1, value);
			ng_global_alfa = (ng_global_alfa & ~0xff) | (((int)ng_global_alfa_1)) & 0xff;
			break;
		}
		case 0x01: {
			unsigned char ng_global_alfa_2 = (ng_global_alfa >> 8) & 0xff;
			ng_global_alfa_2 = NGNumericOperationByte(number_operation, ng_global_alfa_2, value);
			ng_global_alfa = (ng_global_alfa & ~0xff00) | (((int)ng_global_alfa_2) << 8) & 0xff00;
			break;
		}
		case 0x02: {
			unsigned char ng_global_alfa_3 = (ng_global_alfa >> 16) & 0xff;
			ng_global_alfa_3 = NGNumericOperationByte(number_operation, ng_global_alfa_3, value);
			ng_global_alfa = (ng_global_alfa & ~0xff0000) | (((int)ng_global_alfa_3) << 16) & 0xff0000;
			break;
		}
		case 0x03: {
			unsigned char ng_global_alfa_4 = (ng_global_alfa >> 24) & 0xff;
			ng_global_alfa_4 = NGNumericOperationByte(number_operation, ng_global_alfa_4, value);
			ng_global_alfa = (ng_global_alfa & ~0xff000000) | (((int)ng_global_alfa_4) << 24) & 0xff000000;
			break;
		}
		// Global Beta Byte
		case 0x04: {
			unsigned char ng_global_beta_1 = (ng_global_beta & 0xff);
			ng_global_beta_1 = NGNumericOperationByte(number_operation, ng_global_beta_1, value);
			ng_global_beta = (ng_global_beta & ~0xff) | (((int)ng_global_beta_1)) & 0xff;
			break;
		}
		case 0x05: {
			unsigned char ng_global_beta_2 = (ng_global_beta >> 8) & 0xff;
			ng_global_beta_2 = NGNumericOperationByte(number_operation, ng_global_beta_2, value);
			ng_global_beta = (ng_global_beta & ~0xff00) | (((int)ng_global_beta_2) << 8) & 0xff00;
			break;
		}
		case 0x06: {
			unsigned char ng_global_beta_3 = (ng_global_beta >> 16) & 0xff;
			ng_global_beta_3 = NGNumericOperationByte(number_operation, ng_global_beta_3, value);
			ng_global_beta = (ng_global_beta & ~0xff0000) | (((int)ng_global_beta_3) << 16) & 0xff0000;
			break;
		}
		case 0x07: {
			unsigned char ng_global_beta_4 = (ng_global_beta >> 24) & 0xff;
			ng_global_beta_4 = NGNumericOperationByte(number_operation, ng_global_beta_4, value);
			ng_global_beta = (ng_global_beta & ~0xff000000) | (((int)ng_global_beta_4) << 24) & 0xff000000;
			break;
		}
		// Global Delta Byte
		case 0x08: {
			unsigned char ng_global_delta_1 = (ng_global_delta & 0xff);
			ng_global_delta_1 = NGNumericOperationByte(number_operation, ng_global_delta_1, value);
			ng_global_delta = (ng_global_delta & ~0xff) | (((int)ng_global_delta_1)) & 0xff;
			break;
		}
		case 0x09: {
			unsigned char ng_global_delta_2 = (ng_global_delta >> 8) & 0xff;
			ng_global_delta_2 = NGNumericOperationByte(number_operation, ng_global_delta_2, value);
			ng_global_delta = (ng_global_delta & ~0xff00) | (((int)ng_global_delta_2) << 8) & 0xff00;
			break;
		}
		case 0x0a: {
			unsigned char ng_global_delta_3 = (ng_global_delta >> 16) & 0xff;
			ng_global_delta_3 = NGNumericOperationByte(number_operation, ng_global_delta_3, value);
			ng_global_delta = (ng_global_delta & ~0xff0000) | (((int)ng_global_delta_3) << 16) & 0xff0000;
			break;
		}
		case 0x0b: {
			unsigned char ng_global_delta_4 = (ng_global_delta >> 24) & 0xff;
			ng_global_delta_4 = NGNumericOperationByte(number_operation, ng_global_delta_4, value);
			ng_global_delta = (ng_global_delta & ~0xff000000) | (((int)ng_global_delta_4) << 24) & 0xff000000;
			break;
		}
		// Global Short Alfa
		case 0x10: {
			unsigned short ng_global_short_alfa_1 = (ng_global_alfa & 0xffff);
			ng_global_short_alfa_1 = NGNumericOperationShort(number_operation, ng_global_short_alfa_1, value);
			ng_global_alfa = (ng_global_alfa & ~0xffff) | (((int)ng_global_short_alfa_1)) & 0xffff;
			break;
		}
		case 0x11: {
			unsigned short ng_global_short_alfa_2 = (ng_global_alfa & 0xffff0000);
			ng_global_short_alfa_2 = NGNumericOperationShort(number_operation, ng_global_short_alfa_2, value);
			ng_global_alfa = (ng_global_alfa & ~0xffff0000) | (((int)ng_global_short_alfa_2)) & 0xffff0000;
			break;
		}
		// Global Short Beta
		case 0x12: {
			unsigned short ng_global_short_beta_1 = (ng_global_beta & 0xffff);
			ng_global_short_beta_1 = NGNumericOperationShort(number_operation, ng_global_short_beta_1, value);
			ng_global_beta = (ng_global_beta & ~0xffff) | (((int)ng_global_short_beta_1)) & 0xffff;
			break;
		}
		case 0x13: {
			unsigned short ng_global_short_beta_2 = (ng_global_beta & 0xffff0000);
			ng_global_short_beta_2 = NGNumericOperationShort(number_operation, ng_global_short_beta_2, value);
			ng_global_beta = (ng_global_beta & ~0xffff0000) | (((int)ng_global_short_beta_2)) & 0xffff0000;
			break;
		}
		// Global Short Delta
		case 0x14: {
			unsigned short ng_global_short_delta_1 = (ng_global_delta & 0xffff);
			ng_global_short_delta_1 = NGNumericOperationShort(number_operation, ng_global_short_delta_1, value);
			ng_global_delta = (ng_global_delta & ~0xffff) | (((int)ng_global_short_delta_1)) & 0xffff;
			break;
		}
		case 0x15: {
			unsigned short ng_global_short_delta_2 = (ng_global_delta & 0xffff0000);
			ng_global_short_delta_2 = NGNumericOperationShort(number_operation, ng_global_short_delta_2, value);
			ng_global_delta = (ng_global_delta & ~0xffff0000) | (((int)ng_global_short_delta_2)) & 0xffff0000;
			break;
		}

		// Global Long Alfa
		case 0x30: {
			ng_global_alfa = NGNumericOperationLong(number_operation, ng_global_alfa, value);
			break;
		}
		// Global Long Beta
		case 0x31: {
			ng_global_beta = NGNumericOperationLong(number_operation, ng_global_beta, value);
			break;
		}
		// Global Long Delta
		case 0x32: {
			ng_global_delta = NGNumericOperationLong(number_operation, ng_global_delta, value);
			break;
		}
		// Global Long Timer
		case 0x33: {
			ng_global_timer = NGNumericOperationLong(number_operation, ng_global_timer, value);
			break;
		}
		// Global Last Input Number
		case 0x35: {
			ng_last_input_number = NGNumericOperationLong(number_operation, ng_global_timer, value);
			break;
		}

		/* Locals */

		// Local Alfa Byte
		case 0x40: {
			unsigned char ng_local_alfa_1 = (ng_local_alfa & 0xff);
			ng_local_alfa_1 = NGNumericOperationByte(number_operation, ng_local_alfa_1, value);
			ng_local_alfa = (ng_local_alfa & ~0xff) | (((int)ng_local_alfa_1)) & 0xff;
			break;
		}
		case 0x41: {
			unsigned char ng_local_alfa_2 = (ng_local_alfa >> 8) & 0xff;
			ng_local_alfa_2 = NGNumericOperationByte(number_operation, ng_local_alfa_2, value);
			ng_local_alfa = (ng_local_alfa & ~0xff00) | (((int)ng_local_alfa_2) << 8) & 0xff00;
			break;
		}
		case 0x42: {
			unsigned char ng_local_alfa_3 = (ng_local_alfa >> 16) & 0xff;
			ng_local_alfa_3 = NGNumericOperationByte(number_operation, ng_local_alfa_3, value);
			ng_local_alfa = (ng_local_alfa & ~0xff0000) | (((int)ng_local_alfa_3) << 16) & 0xff0000;
			break;
		}
		case 0x43: {
			unsigned char ng_local_alfa_4 = (ng_local_alfa >> 24) & 0xff;
			ng_local_alfa_4 = NGNumericOperationByte(number_operation, ng_local_alfa_4, value);
			ng_local_alfa = (ng_local_alfa & ~0xff000000) | (((int)ng_local_alfa_4) << 24) & 0xff000000;
			break;
		}
		// Local Beta Byte
		case 0x44: {
			unsigned char ng_local_beta_1 = (ng_local_beta & 0xff);
			ng_local_beta_1 = NGNumericOperationByte(number_operation, ng_local_beta_1, value);
			ng_local_beta = (ng_local_beta & ~0xff) | (((int)ng_local_beta_1)) & 0xff;
			break;
		}
		case 0x45: {
			unsigned char ng_local_beta_2 = (ng_local_beta >> 8) & 0xff;
			ng_local_beta_2 = NGNumericOperationByte(number_operation, ng_local_beta_2, value);
			ng_local_beta = (ng_local_beta & ~0xff00) | (((int)ng_local_beta_2) << 8) & 0xff00;
			break;
		}
		case 0x46: {
			unsigned char ng_local_beta_3 = (ng_local_beta >> 16) & 0xff;
			ng_local_beta_3 = NGNumericOperationByte(number_operation, ng_local_beta_3, value);
			ng_local_beta = (ng_local_beta & ~0xff0000) | (((int)ng_local_beta_3) << 16) & 0xff0000;
			break;
		}
		case 0x47: {
			unsigned char ng_local_beta_4 = (ng_local_beta >> 24) & 0xff;
			ng_local_beta_4 = NGNumericOperationByte(number_operation, ng_local_beta_4, value);
			ng_local_beta = (ng_local_beta & ~0xff000000) | (((int)ng_local_beta_4) << 24) & 0xff000000;
			break;
		}
		// Local Delta Byte
		case 0x48: {
			unsigned char ng_local_delta_1 = (ng_local_delta & 0xff);
			ng_local_delta_1 = NGNumericOperationByte(number_operation, ng_local_delta_1, value);
			ng_local_delta = (ng_local_delta & ~0xff) | (((int)ng_local_delta_1)) & 0xff;
			break;
		}
		case 0x49: {
			unsigned char ng_local_delta_2 = (ng_local_delta >> 8) & 0xff;
			ng_local_delta_2 = NGNumericOperationByte(number_operation, ng_local_delta_2, value);
			ng_local_delta = (ng_local_delta & ~0xff00) | (((int)ng_local_delta_2) << 8) & 0xff00;
			break;
		}
		case 0x4a: {
			unsigned char ng_local_delta_3 = (ng_local_delta >> 16) & 0xff;
			ng_local_delta_3 = NGNumericOperationByte(number_operation, ng_local_delta_3, value);
			ng_local_delta = (ng_local_delta & ~0xff0000) | (((int)ng_local_delta_3) << 16) & 0xff0000;
			break;
		}
		case 0x4b: {
			unsigned char ng_local_delta_4 = (ng_local_delta >> 24) & 0xff;
			ng_local_delta_4 = NGNumericOperationByte(number_operation, ng_local_delta_4, value);
			ng_local_delta = (ng_local_delta & ~0xff000000) | (((int)ng_local_delta_4) << 24) & 0xff000000;
			break;
		}
		// Local Short Alfa
		case 0x50: {
			unsigned short ng_local_short_alfa_1 = (ng_local_alfa & 0xffff);
			ng_local_short_alfa_1 = NGNumericOperationShort(number_operation, ng_local_short_alfa_1, value);
			ng_local_alfa = (ng_local_alfa & ~0xffff) | (((int)ng_local_short_alfa_1)) & 0xffff;
			break;
		}
		case 0x51: {
			unsigned short ng_local_short_alfa_2 = (ng_local_alfa & 0xffff0000);
			ng_local_short_alfa_2 = NGNumericOperationShort(number_operation, ng_local_short_alfa_2, value);
			ng_local_alfa = (ng_local_alfa & ~0xffff0000) | (((int)ng_local_short_alfa_2)) & 0xffff0000;
			break;
		}
		// Local Short Beta
		case 0x52: {
			unsigned short ng_local_short_beta_1 = (ng_local_beta & 0xffff);
			ng_local_short_beta_1 = NGNumericOperationShort(number_operation, ng_local_short_beta_1, value);
			ng_local_beta = (ng_local_beta & ~0xffff) | (((int)ng_local_short_beta_1)) & 0xffff;
			break;
		}
		case 0x53: {
			unsigned short ng_local_short_beta_2 = (ng_local_beta & 0xffff0000);
			ng_local_short_beta_2 = NGNumericOperationShort(number_operation, ng_local_short_beta_2, value);
			ng_local_beta = (ng_local_beta & ~0xffff0000) | (((int)ng_local_short_beta_2)) & 0xffff0000;
			break;
		}
		// Local Short Delta
		case 0x54: {
			unsigned short ng_local_short_delta_1 = (ng_local_delta & 0xffff);
			ng_local_short_delta_1 = NGNumericOperationShort(number_operation, ng_local_short_delta_1, value);
			ng_local_delta = (ng_local_delta & ~0xffff) | (((int)ng_local_short_delta_1)) & 0xffff;
			break;
		}
		case 0x55: {
			unsigned short ng_local_short_delta_2 = (ng_local_delta & 0xffff0000);
			ng_local_short_delta_2 = NGNumericOperationShort(number_operation, ng_local_short_delta_2, value);
			ng_local_delta = (ng_local_delta & ~0xffff0000) | (((int)ng_local_short_delta_2)) & 0xffff0000;
			break;
		}

		// Local Long Alfa
		case 0x70: {
			ng_local_alfa = NGNumericOperationLong(number_operation, ng_local_alfa, value);
			break;
		}
		// Local Long Beta
		case 0x71: {
			ng_local_beta = NGNumericOperationLong(number_operation, ng_local_beta, value);
			break;
		}
		// Local Long Delta
		case 0x72: {
			ng_local_delta = NGNumericOperationLong(number_operation, ng_local_delta, value);
			break;
		}
		// Local Long Timer
		case 0x73: {
			ng_local_timer = NGNumericOperationLong(number_operation, ng_local_timer, value);
			break;
		}
	}
}

int NGNumericGetVariable(unsigned int variable) {
	switch (variable) {
		case 0xffff: {
			return ng_current_value;
		}

		/* Globals */

		// Global Alfa Byte
		case 0x00: {
			unsigned char ng_global_alfa_1 = (ng_global_alfa & 0xff);
			return ng_global_alfa_1;
		}
		case 0x01: {
			unsigned char ng_global_alfa_2 = (ng_global_alfa >> 8) & 0xff;
			return ng_global_alfa_2;
		}
		case 0x02: {
			unsigned char ng_global_alfa_3 = (ng_global_alfa >> 16) & 0xff;
			return ng_global_alfa_3;
		}
		case 0x03: {
			unsigned char ng_global_alfa_4 = (ng_global_alfa >> 24) & 0xff;
			return ng_global_alfa_4;
		}
		// Global Beta Byte
		case 0x04: {
			unsigned char ng_global_beta_1 = (ng_global_beta & 0xff);
			return ng_global_beta_1;
		}
		case 0x05: {
			unsigned char ng_global_beta_2 = (ng_global_beta >> 8) & 0xff;
			return ng_global_beta_2;
		}
		case 0x06: {
			unsigned char ng_global_beta_3 = (ng_global_beta >> 16) & 0xff;
			return ng_global_beta_3;
		}
		case 0x07: {
			unsigned char ng_global_beta_4 = (ng_global_beta >> 24) & 0xff;
			return ng_global_beta_4;
		}
		// Global Delta Byte
		case 0x08: {
			unsigned char ng_global_delta_1 = (ng_global_delta & 0xff);
			return ng_global_delta_1;
		}
		case 0x09: {
			unsigned char ng_global_delta_2 = (ng_global_delta >> 8) & 0xff;
			return ng_global_delta_2;
		}
		case 0x0a: {
			unsigned char ng_global_delta_3 = (ng_global_delta >> 16) & 0xff;
			return ng_global_delta_3;
		}
		case 0x0b: {
			unsigned char ng_global_delta_4 = (ng_global_delta >> 24) & 0xff;
			return ng_global_delta_4;
		}
		// Global Short Alfa
		case 0x10: {
			unsigned short ng_global_short_alfa_1 = (ng_global_alfa & 0xffff);
			return ng_global_short_alfa_1;
		}
		case 0x11: {
			unsigned short ng_global_short_alfa_2 = (ng_global_alfa & 0xffff0000);
			return ng_global_short_alfa_2;
		}
		// Global Short Beta
		case 0x12: {
			unsigned short ng_global_short_beta_1 = (ng_global_beta & 0xffff);
			return ng_global_short_beta_1;
		}
		case 0x13: {
			unsigned short ng_global_short_beta_2 = (ng_global_beta & 0xffff0000);
			return ng_global_short_beta_2;
		}
		// Global Short Delta
		case 0x14: {
			unsigned short ng_global_short_delta_1 = (ng_global_delta & 0xffff);
			return ng_global_short_delta_1;
		}
		case 0x15: {
			unsigned short ng_global_short_delta_2 = (ng_global_delta & 0xffff0000);
			return ng_global_short_delta_2;
		}

		// Global Long Alfa
		case 0x30: {
			return ng_global_alfa;
		}
		// Global Long Beta
		case 0x31: {
			return ng_global_beta;
		}
		// Global Long Delta
		case 0x32: {
			return ng_global_delta;
		}
		// Global Long Timer
		case 0x33: {
			return ng_global_timer;
		}
		// Global Last Input Number
		case 0x35: {
			return ng_last_input_number;
		}

		/* Locals */

		// Local Alfa Byte
		case 0x40: {
			unsigned char ng_local_alfa_1 = (ng_local_alfa & 0xff);
			return ng_local_alfa_1;
		}
		case 0x41: {
			unsigned char ng_local_alfa_2 = (ng_local_alfa >> 8) & 0xff;
			return ng_local_alfa_2;
		}
		case 0x42: {
			unsigned char ng_local_alfa_3 = (ng_local_alfa >> 16) & 0xff;
			return ng_local_alfa_3;
		}
		case 0x43: {
			unsigned char ng_local_alfa_4 = (ng_local_alfa >> 24) & 0xff;
			return ng_local_alfa_4;
		}
		// Local Beta Byte
		case 0x44: {
			unsigned char ng_local_beta_1 = (ng_local_beta & 0xff);
			return ng_local_beta_1;
		}
		case 0x45: {
			unsigned char ng_local_beta_2 = (ng_local_beta >> 8) & 0xff;
			return ng_local_beta_2;
		}
		case 0x46: {
			unsigned char ng_local_beta_3 = (ng_local_beta >> 16) & 0xff;
			return ng_local_beta_3;
		}
		case 0x47: {
			unsigned char ng_local_beta_4 = (ng_local_beta >> 24) & 0xff;
			return ng_local_beta_4;
		}
		// Local Delta Byte
		case 0x48: {
			unsigned char ng_local_delta_1 = (ng_local_delta & 0xff);
			return ng_local_delta_1;
		}
		case 0x49: {
			unsigned char ng_local_delta_2 = (ng_local_delta >> 8) & 0xff;
			return ng_local_delta_2;
		}
		case 0x4a: {
			unsigned char ng_local_delta_3 = (ng_local_delta >> 16) & 0xff;
			return ng_local_delta_3;
		}
		case 0x4b: {
			unsigned char ng_local_delta_4 = (ng_local_delta >> 24) & 0xff;
			return ng_local_delta_4;
		}
		// Local Short Alfa
		case 0x50: {
			unsigned short ng_local_short_alfa_1 = (ng_local_alfa & 0xffff);
			return ng_local_short_alfa_1;
		}
		case 0x51: {
			unsigned short ng_local_short_alfa_2 = (ng_local_alfa & 0xffff0000);
			return ng_local_short_alfa_2;
		}
		// Local Short Beta
		case 0x52: {
			unsigned short ng_local_short_beta_1 = (ng_local_beta & 0xffff);
			return ng_local_short_beta_1;
		}
		case 0x53: {
			unsigned short ng_local_short_beta_2 = (ng_local_beta & 0xffff0000);
			return ng_local_short_beta_2;
		}
		// Local Short Delta
		case 0x54: {
			unsigned short ng_local_short_delta_1 = (ng_local_delta & 0xffff);
			return ng_local_short_delta_1;
		}
		case 0x55: {
			unsigned short ng_local_short_delta_2 = (ng_local_delta & 0xffff0000);
			return ng_local_short_delta_2;
		}

		// Local Long Alfa
		case 0x70: {
			return ng_local_alfa;
		}
		// Local Long Beta
		case 0x71: {
			return ng_local_beta;
		}
		// Local Long Delta
		case 0x72: {
			return ng_local_delta;
		}
		// Local Long Timer
		case 0x73: {
			return ng_local_timer;
		}
		default: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGNumericGetVariable: variable type %u unimplemented!", variable);
			return 0;
		}
	}
}

int NGNumericGetSavegameValue(unsigned int variable) {
	switch (variable) {
		default: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGNumericGetSavegameValue: Unimplemented savegame value: %u", variable);
			break;
		}
	}

	return -1;
}