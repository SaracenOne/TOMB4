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

extern unsigned char NGNumericOperationByte(NGNumericOperationType number_operation, unsigned char variable, unsigned int value);
extern unsigned short NGNumericOperationShort(NGNumericOperationType number_operation, unsigned short variable, unsigned int value);
extern unsigned long NGNumericOperationLong(NGNumericOperationType number_operation, unsigned long variable, unsigned int value);
extern void NGNumericOperation(NGNumericOperationType number_operation, unsigned int variable, unsigned int value);
extern unsigned int NGNumericGetVariable(unsigned int variable);