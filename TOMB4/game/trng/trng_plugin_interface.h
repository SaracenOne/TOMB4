#pragma once

struct TRNGPlugin {
	//void InitLevel(int newLevel, int oldLevel, unsigned int fil_flags);
	//void PreloadLevel(); // Called before beginning loading of new level

	//int PluginFlipEffect();
	//int PluginAction();
	//int PluginCondition();
	//int PluginCustomize();
	//int PluginParameters();
	//int PluginAssignSlot();

	//void CycleBegin();
};

void RegisterVirtualPlugin(const char *pluginName, TRNGPlugin *pluginStructure) {

};