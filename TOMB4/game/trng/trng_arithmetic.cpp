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
	}

	return 0;
}

void NGNumericOperation(NGNumericOperationType number_operation, unsigned int variable, unsigned int value) {
	switch (variable) {
		case 0xffff: {
			current_value = NGNumericOperationByte(number_operation, current_value, value);
			break;
		}

		/* Globals */

		// Global Alfa Byte
		case 0x00: {
			unsigned char global_alfa_1 = (global_alfa & 0xff);
			global_alfa_1 = NGNumericOperationByte(number_operation, global_alfa_1, value);
			global_alfa = (global_alfa & ~0xff) | (((int)global_alfa_1)) & 0xff;
			break;
		}
		case 0x01: {
			unsigned char global_alfa_2 = (global_alfa >> 8) & 0xff;
			global_alfa_2 = NGNumericOperationByte(number_operation, global_alfa_2, value);
			global_alfa = (global_alfa & ~0xff00) | (((int)global_alfa_2) << 8) & 0xff00;
			break;
		}
		case 0x02: {
			unsigned char global_alfa_3 = (global_alfa >> 16) & 0xff;
			global_alfa_3 = NGNumericOperationByte(number_operation, global_alfa_3, value);
			global_alfa = (global_alfa & ~0xff0000) | (((int)global_alfa_3) << 16) & 0xff0000;
			break;
		}
		case 0x03: {
			unsigned char global_alfa_4 = (global_alfa >> 24) & 0xff;
			global_alfa_4 = NGNumericOperationByte(number_operation, global_alfa_4, value);
			global_alfa = (global_alfa & ~0xff000000) | (((int)global_alfa_4) << 24) & 0xff000000;
			break;
		}
		// Global Beta Byte
		case 0x04: {
			unsigned char global_beta_1 = (global_beta & 0xff);
			global_beta_1 = NGNumericOperationByte(number_operation, global_beta_1, value);
			global_beta = (global_beta & ~0xff) | (((int)global_beta_1)) & 0xff;
			break;
		}
		case 0x05: {
			unsigned char global_beta_2 = (global_beta >> 8) & 0xff;
			global_beta_2 = NGNumericOperationByte(number_operation, global_beta_2, value);
			global_beta = (global_beta & ~0xff00) | (((int)global_beta_2) << 8) & 0xff00;
			break;
		}
		case 0x06: {
			unsigned char global_beta_3 = (global_beta >> 16) & 0xff;
			global_beta_3 = NGNumericOperationByte(number_operation, global_beta_3, value);
			global_beta = (global_beta & ~0xff0000) | (((int)global_beta_3) << 16) & 0xff0000;
			break;
		}
		case 0x07: {
			unsigned char global_beta_4 = (global_beta >> 24) & 0xff;
			global_beta_4 = NGNumericOperationByte(number_operation, global_beta_4, value);
			global_beta = (global_beta & ~0xff000000) | (((int)global_beta_4) << 24) & 0xff000000;
			break;
		}
		// Global Delta Byte
		case 0x08: {
			unsigned char global_delta_1 = (global_delta & 0xff);
			global_delta_1 = NGNumericOperationByte(number_operation, global_delta_1, value);
			global_delta = (global_delta & ~0xff) | (((int)global_delta_1)) & 0xff;
			break;
		}
		case 0x09: {
			unsigned char global_delta_2 = (global_delta >> 8) & 0xff;
			global_delta_2 = NGNumericOperationByte(number_operation, global_delta_2, value);
			global_delta = (global_delta & ~0xff00) | (((int)global_delta_2) << 8) & 0xff00;
			break;
		}
		case 0x0a: {
			unsigned char global_delta_3 = (global_delta >> 16) & 0xff;
			global_delta_3 = NGNumericOperationByte(number_operation, global_delta_3, value);
			global_delta = (global_beta & ~0xff0000) | (((int)global_delta_3) << 16) & 0xff0000;
			break;
		}
		case 0x0b: {
			unsigned char global_delta_4 = (global_delta >> 24) & 0xff;
			global_delta_4 = NGNumericOperationByte(number_operation, global_delta_4, value);
			global_delta = (global_delta & ~0xff000000) | (((int)global_delta_4) << 24) & 0xff000000;
			break;
		}
		// Global Short Alfa
		case 0x10: {
			unsigned short global_short_alfa_1 = (global_alfa & 0xffff);
			global_short_alfa_1 = NGNumericOperationShort(number_operation, global_short_alfa_1, value);
			global_alfa = (global_alfa & ~0xffff) | (((int)global_short_alfa_1)) & 0xffff;
			break;
		}
		case 0x11: {
			unsigned short global_short_alfa_2 = (global_alfa & 0xffff0000);
			global_short_alfa_2 = NGNumericOperationShort(number_operation, global_short_alfa_2, value);
			global_alfa = (global_alfa & ~0xffff0000) | (((int)global_short_alfa_2)) & 0xffff0000;
			break;
		}
		// Global Short Beta
		case 0x12: {
			unsigned short global_short_beta_1 = (global_beta & 0xffff);
			global_short_beta_1 = NGNumericOperationShort(number_operation, global_short_beta_1, value);
			global_beta = (global_beta & ~0xffff) | (((int)global_short_beta_1)) & 0xffff;
			break;
		}
		case 0x13: {
			unsigned short global_short_beta_2 = (global_beta & 0xffff0000);
			global_short_beta_2 = NGNumericOperationShort(number_operation, global_short_beta_2, value);
			global_beta = (global_beta & ~0xffff0000) | (((int)global_short_beta_2)) & 0xffff0000;
			break;
		}
		// Global Short Delta
		case 0x14: {
			unsigned short global_short_delta_1 = (global_delta & 0xffff);
			global_short_delta_1 = NGNumericOperationShort(number_operation, global_short_delta_1, value);
			global_delta = (global_delta & ~0xffff) | (((int)global_short_delta_1)) & 0xffff;
			break;
		}
		case 0x15: {
			unsigned short global_short_delta_2 = (global_delta & 0xffff0000);
			global_short_delta_2 = NGNumericOperationShort(number_operation, global_short_delta_2, value);
			global_delta = (global_delta & ~0xffff0000) | (((int)global_short_delta_2)) & 0xffff0000;
			break;
		}

		// Global Long Alfa
		case 0x30: {
			global_alfa = NGNumericOperationLong(number_operation, global_alfa, value);
			break;
		}
		// Global Long Beta
		case 0x31: {
			global_beta = NGNumericOperationLong(number_operation, global_beta, value);
			break;
		}
		// Global Long Delta
		case 0x32: {
			global_delta = NGNumericOperationLong(number_operation, global_delta, value);
			break;
		}
		// Global Long Timer
		case 0x33: {
			global_timer = NGNumericOperationLong(number_operation, global_timer, value);
			break;
		}
		// Global Last Input Number
		case 0x35: {
			last_input_number = NGNumericOperationLong(number_operation, global_timer, value);
			break;
		}

		/* Locals */

		// Local Alfa Byte
		case 0x40: {
			unsigned char local_alfa_1 = (local_alfa & 0xff);
			local_alfa_1 = NGNumericOperationByte(number_operation, local_alfa_1, value);
			local_alfa = (local_alfa & ~0xff) | (((int)local_alfa_1)) & 0xff;
			break;
		}
		case 0x41: {
			unsigned char local_alfa_2 = (local_alfa >> 8) & 0xff;
			local_alfa_2 = NGNumericOperationByte(number_operation, local_alfa_2, value);
			local_alfa = (local_alfa & ~0xff00) | (((int)local_alfa_2) << 8) & 0xff00;
			break;
		}
		case 0x42: {
			unsigned char local_alfa_3 = (local_alfa >> 16) & 0xff;
			local_alfa_3 = NGNumericOperationByte(number_operation, local_alfa_3, value);
			local_alfa = (local_alfa & ~0xff0000) | (((int)local_alfa_3) << 16) & 0xff0000;
			break;
		}
		case 0x43: {
			unsigned char local_alfa_4 = (local_alfa >> 24) & 0xff;
			local_alfa_4 = NGNumericOperationByte(number_operation, local_alfa_4, value);
			local_alfa = (local_alfa & ~0xff000000) | (((int)local_alfa_4) << 24) & 0xff000000;
			break;
		}
		// Local Beta Byte
		case 0x44: {
			unsigned char local_beta_1 = (local_beta & 0xff);
			local_beta_1 = NGNumericOperationByte(number_operation, local_beta_1, value);
			local_beta = (local_beta & ~0xff) | (((int)local_beta_1)) & 0xff;
			break;
		}
		case 0x45: {
			unsigned char local_beta_2 = (local_beta >> 8) & 0xff;
			local_beta_2 = NGNumericOperationByte(number_operation, local_beta_2, value);
			local_beta = (local_beta & ~0xff00) | (((int)local_beta_2) << 8) & 0xff00;
			break;
		}
		case 0x46: {
			unsigned char local_beta_3 = (local_beta >> 16) & 0xff;
			local_beta_3 = NGNumericOperationByte(number_operation, local_beta_3, value);
			local_beta = (local_beta & ~0xff0000) | (((int)local_beta_3) << 16) & 0xff0000;
			break;
		}
		case 0x47: {
			unsigned char local_beta_4 = (local_beta >> 24) & 0xff;
			local_beta_4 = NGNumericOperationByte(number_operation, local_beta_4, value);
			local_beta = (local_beta & ~0xff000000) | (((int)local_beta_4) << 24) & 0xff000000;
			break;
		}
		// Local Delta Byte
		case 0x48: {
			unsigned char local_delta_1 = (local_delta & 0xff);
			local_delta_1 = NGNumericOperationByte(number_operation, local_delta_1, value);
			local_delta |= (((int)local_delta_1)) & 0xff;
			break;
		}
		case 0x49: {
			unsigned char local_delta_2 = (local_delta >> 8) & 0xff;
			local_delta_2 = NGNumericOperationByte(number_operation, local_delta_2, value);
			local_delta = (local_delta & ~0xff00) | (((int)local_delta_2) << 8) & 0xff00;
			break;
		}
		case 0x4a: {
			unsigned char local_delta_3 = (local_delta >> 16) & 0xff;
			local_delta_3 = NGNumericOperationByte(number_operation, local_delta_3, value);
			local_delta = (local_delta & ~0xff0000) | (((int)local_delta_3) << 16) & 0xff0000;
			break;
		}
		case 0x4b: {
			unsigned char local_delta_4 = (local_delta >> 24) & 0xff;
			local_delta_4 = NGNumericOperationByte(number_operation, local_delta_4, value);
			local_delta = (local_delta & ~0xff000000) | (((int)local_delta_4) << 24) & 0xff000000;
			break;
		}
		// Local Short Alfa
		case 0x50: {
			unsigned short local_short_alfa_1 = (local_alfa & 0xffff);
			local_short_alfa_1 = NGNumericOperationShort(number_operation, local_short_alfa_1, value);
			local_alfa = (local_alfa & ~0xffff) | (((int)local_short_alfa_1)) & 0xffff;
			break;
		}
		case 0x51: {
			unsigned short local_short_alfa_2 = (local_alfa & 0xffff0000);
			local_short_alfa_2 = NGNumericOperationShort(number_operation, local_short_alfa_2, value);
			local_alfa = (local_alfa & ~0xffff0000) | (((int)local_short_alfa_2)) & 0xffff0000;
			break;
		}
		// Local Short Beta
		case 0x52: {
			unsigned short local_short_beta_1 = (local_beta & 0xffff);
			local_short_beta_1 = NGNumericOperationShort(number_operation, local_short_beta_1, value);
			local_beta = (local_beta & ~0xffff) | (((int)local_short_beta_1)) & 0xffff;
			break;
		}
		case 0x53: {
			unsigned short local_short_beta_2 = (local_beta & 0xffff0000);
			local_short_beta_2 = NGNumericOperationShort(number_operation, local_short_beta_2, value);
			local_beta = (local_beta & ~0xffff0000) | (((int)local_short_beta_2)) & 0xffff0000;
			break;
		}
		// Local Short Delta
		case 0x54: {
			unsigned short local_short_delta_1 = (local_delta & 0xffff);
			local_short_delta_1 = NGNumericOperationShort(number_operation, local_short_delta_1, value);
			local_delta = (local_delta & ~0xffff) | (((int)local_short_delta_1)) & 0xffff;
			break;
		}
		case 0x55: {
			unsigned short local_short_delta_2 = (local_delta & 0xffff0000);
			local_short_delta_2 = NGNumericOperationShort(number_operation, local_short_delta_2, value);
			local_delta = (local_delta & ~0xffff0000) | (((int)local_short_delta_2)) & 0xffff0000;
			break;
		}

		// Local Long Alfa
		case 0x70: {
			local_alfa = NGNumericOperationLong(number_operation, local_alfa, value);
			break;
		}
		// Local Long Beta
		case 0x71: {
			local_beta = NGNumericOperationLong(number_operation, local_beta, value);
			break;
		}
		// Local Long Delta
		case 0x72: {
			local_delta = NGNumericOperationLong(number_operation, local_delta, value);
			break;
		}
		// Local Long Timer
		case 0x73: {
			local_timer = NGNumericOperationLong(number_operation, local_timer, value);
			break;
		}
	}
}

unsigned int NGNumericGetVariable(unsigned int variable) {
	switch (variable) {
		case 0xffff: {
			return current_value;
		}

		/* Globals */

		// Global Alfa Byte
		case 0x00: {
			unsigned char global_alfa_1 = (global_alfa & 0xff);
			return global_alfa_1;
		}
		case 0x01: {
			unsigned char global_alfa_2 = (global_alfa >> 8) & 0xff;
			return global_alfa_2;
		}
		case 0x02: {
			unsigned char global_alfa_3 = (global_alfa >> 16) & 0xff;
			return global_alfa_3;
		}
		case 0x03: {
			unsigned char global_alfa_4 = (global_alfa >> 24) & 0xff;
			return global_alfa_4;
		}
		// Global Beta Byte
		case 0x04: {
			unsigned char global_beta_1 = (global_beta & 0xff);
			return global_beta_1;
		}
		case 0x05: {
			unsigned char global_beta_2 = (global_beta >> 8) & 0xff;
			return global_beta_2;
		}
		case 0x06: {
			unsigned char global_beta_3 = (global_beta >> 16) & 0xff;
			return global_beta_3;
		}
		case 0x07: {
			unsigned char global_beta_4 = (global_beta >> 24) & 0xff;
			return global_beta_4;
		}
		// Global Delta Byte
		case 0x08: {
			unsigned char global_delta_1 = (global_delta & 0xff);
			return global_delta_1;
		}
		case 0x09: {
			unsigned char global_delta_2 = (global_delta >> 8) & 0xff;
			return global_delta_2;
		}
		case 0x0a: {
			unsigned char global_delta_3 = (global_delta >> 16) & 0xff;
			return global_delta_3;
		}
		case 0x0b: {
			unsigned char global_delta_4 = (global_delta >> 24) & 0xff;
			return global_delta_4;
		}
		// Global Short Alfa
		case 0x10: {
			unsigned short global_short_alfa_1 = (global_alfa & 0xffff);
			return global_short_alfa_1;
		}
		case 0x11: {
			unsigned short global_short_alfa_2 = (global_alfa & 0xffff0000);
			return global_short_alfa_2;
		}
		// Global Short Beta
		case 0x12: {
			unsigned short global_short_beta_1 = (global_beta & 0xffff);
			return global_short_beta_1;
		}
		case 0x13: {
			unsigned short global_short_beta_2 = (global_beta & 0xffff0000);
			return global_short_beta_2;
		}
		// Global Short Delta
		case 0x14: {
			unsigned short global_short_delta_1 = (global_delta & 0xffff);
			return global_short_delta_1;
		}
		case 0x15: {
			unsigned short global_short_delta_2 = (global_delta & 0xffff0000);
			return global_short_delta_2;
		}

		// Global Long Alfa
		case 0x30: {
			return global_alfa;
		}
		// Global Long Beta
		case 0x31: {
			return global_beta;
		}
		// Global Long Delta
		case 0x32: {
			return global_delta;
		}
		// Global Long Timer
		case 0x33: {
			return global_timer;
		}
		// Global Last Input Number
		case 0x35: {
			return last_input_number;
		}

		/* Locals */

		// Local Alfa Byte
		case 0x40: {
			unsigned char local_alfa_1 = (local_alfa & 0xff);
			return local_alfa_1;
		}
		case 0x41: {
			unsigned char local_alfa_2 = (local_alfa >> 8) & 0xff;
			return local_alfa_2;
		}
		case 0x42: {
			unsigned char local_alfa_3 = (local_alfa >> 16) & 0xff;
			return local_alfa_3;
		}
		case 0x43: {
			unsigned char local_alfa_4 = (local_alfa >> 24) & 0xff;
			return local_alfa_4;
		}
		// Local Beta Byte
		case 0x44: {
			unsigned char local_beta_1 = (local_beta & 0xff);
			return local_beta_1;
		}
		case 0x45: {
			unsigned char local_beta_2 = (local_beta >> 8) & 0xff;
			return local_beta_2;
		}
		case 0x46: {
			unsigned char local_beta_3 = (local_beta >> 16) & 0xff;
			return local_beta_3;
		}
		case 0x47: {
			unsigned char local_beta_4 = (local_beta >> 24) & 0xff;
			return local_beta_4;
		}
		// Local Delta Byte
		case 0x48: {
			unsigned char local_delta_1 = (local_delta & 0xff);
			return local_delta_1;
		}
		case 0x49: {
			unsigned char local_delta_2 = (local_delta >> 8) & 0xff;
			return local_delta_2;
		}
		case 0x4a: {
			unsigned char local_delta_3 = (local_delta >> 16) & 0xff;
			return local_delta_3;
		}
		case 0x4b: {
			unsigned char local_delta_4 = (local_delta >> 24) & 0xff;
			return local_delta_4;
		}
		// Local Short Alfa
		case 0x50: {
			unsigned short local_short_alfa_1 = (local_alfa & 0xffff);
			return local_short_alfa_1;
		}
		case 0x51: {
			unsigned short local_short_alfa_2 = (local_alfa & 0xffff0000);
			return local_short_alfa_2;
		}
		// Local Short Beta
		case 0x52: {
			unsigned short local_short_beta_1 = (local_beta & 0xffff);
			return local_short_beta_1;
		}
		case 0x53: {
			unsigned short local_short_beta_2 = (local_beta & 0xffff0000);
			return local_short_beta_2;
		}
		// Local Short Delta
		case 0x54: {
			unsigned short local_short_delta_1 = (local_delta & 0xffff);
			return local_short_delta_1;
		}
		case 0x55: {
			unsigned short local_short_delta_2 = (local_delta & 0xffff0000);
			return local_short_delta_2;
		}

		// Local Long Alfa
		case 0x70: {
			return local_alfa;
		}
		// Local Long Beta
		case 0x71: {
			return local_beta;
		}
		// Local Long Delta
		case 0x72: {
			return local_delta;
		}
		// Local Long Timer
		case 0x73: {
			return local_timer;
		}
		default: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGNumericGetVariable: variable type %u unimplemented!", variable);
			return 0;
		}
	}
}