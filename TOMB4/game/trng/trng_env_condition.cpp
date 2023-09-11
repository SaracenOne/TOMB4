#include "../../tomb4/pch.h"
#include "trng_script_parser.h"
#include "trng.h"

void TestEnvCondition(NG_MULTI_ENV_TRIPLET *triplet) {
	switch (triplet->env_condition) {
	default:
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Unimplemented environment condition: %u!", triplet->env_condition);
	}
}