#define MAX_NG_LEVELS 64

struct NG_GLOBAL_TRIGGER {
	unsigned short flags = 0x0;
	unsigned short type = 0x00;
	unsigned int parameter = 0x00;
	unsigned short condition_trigger_group = 0x00;
	unsigned short perform_trigger_group = 0x00;
	unsigned short on_false_trigger_group = 0x00;
};

struct NG_GLOBAL_TRIGGER_RECORD {
	unsigned short record_id = 0;
	NG_GLOBAL_TRIGGER global_trigger;
};

struct NG_TRIGGER_GROUP_DATA {
	unsigned short first_field = 0x00;
	unsigned short second_field = 0x00;
	unsigned short third_field = 0x00;
};

#define NG_TRIGGER_GROUP_DATA_SIZE 0xff
struct NG_TRIGGER_GROUP {
	NG_TRIGGER_GROUP_DATA data[NG_TRIGGER_GROUP_DATA_SIZE];
};

struct NG_TRIGGER_GROUP_RECORD {
	unsigned short record_id = 0;
	NG_TRIGGER_GROUP trigger_group;
};

struct NG_ORGANIZER_APPOINTMENT {
	unsigned int time = 0;
	unsigned short trigger_group = 0;
};

#define NG_ORGANIZER_MAX_APPOINTMENTS 4096
struct NG_ORGANIZER {
	unsigned short flags = 0;
	unsigned short parameters = 0; // Unused
	NG_ORGANIZER_APPOINTMENT appointments[NG_ORGANIZER_MAX_APPOINTMENTS];
};

struct NG_ORGANIZER_RECORD {
	unsigned short record_id = 0;
	NG_ORGANIZER organizer;
};

#define NG_ITEM_GROUP_MAX_LIST 4096
struct NG_ITEM_GROUP {
	short item_list[NG_ITEM_GROUP_MAX_LIST];
};

struct NG_ITEM_GROUP_RECORD {
	unsigned short record_id = 0;
	NG_ITEM_GROUP item_group;
};

#define MAX_NG_GLOBAL_TRIGGERS 9999
#define MAX_NG_TRIGGER_GROUPS 9999
#define MAX_NG_ORGANIZERS 4999
#define MAX_NG_ITEM_GROUPS 999

struct NG_LEVEL_RECORD_DATA {
	int global_trigger_count = 0;
	NG_GLOBAL_TRIGGER_RECORD* global_triggers_table = NULL;
	int trigger_group_count = 0;
	NG_TRIGGER_GROUP_RECORD* trigger_group_table = NULL;
	int organizer_count = 0;
	NG_ORGANIZER_RECORD* organizer_table = NULL;
	int item_group_count = 0;
	NG_ITEM_GROUP_RECORD* item_group_table = NULL;
};

struct NG_LEVEL {
	NG_LEVEL_RECORD_DATA *records = NULL;
};

extern NG_LEVEL ng_levels[MAX_NG_LEVELS];

enum NG_ADD_EFFECT_TYPE {
	NG_ADD_NOTHING,
	NG_ADD_FLAME,
	NG_ADD_SMOKE,
	NG_ADD_BLOOD,
	NG_ADD_MIST,
	NG_ADD_LIGHT_FLAT,
	NG_ADD_LIGHT_BLINK,
	NG_ADD_LIGHT_SPOT,
	NG_ADD_LIGHT_GLOVE,
};

extern NG_GLOBAL_TRIGGER current_global_triggers[MAX_NG_GLOBAL_TRIGGERS];
extern NG_TRIGGER_GROUP current_trigger_groups[MAX_NG_TRIGGER_GROUPS];
extern NG_ORGANIZER current_organizers[MAX_NG_ORGANIZERS];
extern NG_ITEM_GROUP current_item_groups[MAX_NG_ITEM_GROUPS];

extern void NGInitLevelArray();
extern void NGLoadTablesForLevel(unsigned int level);
extern void NGScriptCleanup();
extern void NGReadNGGameflowInfo(char* gfScriptFile, unsigned int offset, unsigned int len);