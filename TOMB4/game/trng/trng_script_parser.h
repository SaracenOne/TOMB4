#define MAX_NG_LEVELS 64

#define MAX_NG_GLOBAL_TRIGGERS 9999
#define MAX_NG_TRIGGER_GROUPS 9999
#define MAX_NG_ORGANIZERS 4999
#define MAX_NG_ITEM_GROUPS 999

struct NG_LEVEL {
};

struct NG_GLOBAL_TRIGGER {
	unsigned short flags;
	unsigned short type;
	unsigned int parameter;
	unsigned short condition_trigger_group;
	unsigned short perform_trigger_group;
	unsigned short on_false_trigger_group;
};

#define NG_TRIGGER_GROUP_DATA_SIZE 0xff
struct NG_TRIGGER_GROUP {
	unsigned short data[NG_TRIGGER_GROUP_DATA_SIZE];
};

struct NG_ORGANIZER_APPOINTMENT {
	unsigned int time;
	unsigned short trigger_group;
};

#define NG_ORGANIZER_MAX_APPOINTMENTS 4096
struct NG_ORGANIZER {
	unsigned short flags;
	unsigned short parameters; // Unused
	NG_ORGANIZER_APPOINTMENT appointments[NG_ORGANIZER_MAX_APPOINTMENTS];
};

#define NG_ITEM_GROUP_MAX_RECORDS 4096
struct NG_ITEM_GROUP {
	unsigned short item_records[NG_ITEM_GROUP_MAX_RECORDS];
};

extern void NGLoaderHeader(char* gfScriptFile, unsigned int offset, unsigned int len);