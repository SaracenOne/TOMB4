#pragma once

enum TriggerGroupExecutionType {
	TRIGGER_GROUP_EXECUTION_MULTIPLE = 0,
	TRIGGER_GROUP_EXECUTION_SINGLE,
	TRIGGER_GROUP_EXECUTION_CONTINUOUS
};

extern bool NGTriggerGroupFunction(unsigned int trigger_group_id, unsigned char execution_type);
extern void NGProcessTriggerGroups();