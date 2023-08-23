#pragma once

enum NGNumericOperationType {
	NG_ADD = 0,
	NG_SUBTRACT,
	NG_MULTIPLY,
	NG_DIVIDE,
};

extern unsigned char NGNumericOperationByte(NGNumericOperationType number_operation, unsigned char variable, unsigned int value);
extern unsigned short NGNumericOperationShort(NGNumericOperationType number_operation, unsigned short variable, unsigned int value);
extern unsigned long NGNumericOperationLong(NGNumericOperationType number_operation, unsigned long variable, unsigned int value);
extern void NGNumericOperation(NGNumericOperationType number_operation, unsigned int variable, unsigned int value);
extern unsigned int NGNumericGetVariable(unsigned int variable);