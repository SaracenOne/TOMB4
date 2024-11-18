#pragma once

enum TriggerGroupExecutionType {
	TRIGGER_GROUP_EXECUTION_MULTIPLE = 0,
	TRIGGER_GROUP_EXECUTION_SINGLE,
	TRIGGER_GROUP_EXECUTION_CONTINUOUS
};

extern bool NGTriggerGroupFunction(uint32_t trigger_group_id, uint8_t execution_type);
extern void NGProcessTriggerGroups();