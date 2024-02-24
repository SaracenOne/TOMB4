#pragma once

#ifdef USE_BGFX

#include <bgfx/bgfx.h>

extern bgfx::ProgramHandle m_outputProgram;

extern void InitializeBGFX();
extern void SetupBGFXOutputPolyList();
extern void StartBGFXFrame();
extern void EndBGFXFrame();

#endif