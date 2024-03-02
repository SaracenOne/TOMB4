#pragma once


#ifdef USE_BGFX

#include <bgfx/bgfx.h>

#define SORT_BUFFER_VERT_COUNT 8192 * 3
#define MAX_SORT_DRAW_COMMANDS 2048

extern size_t current_sort_vertex_buffer_idx;
extern size_t current_sort_vertex_buffer_offset;

extern bgfx::ProgramHandle m_outputVTLTexProgram;
extern bgfx::ProgramHandle m_outputVTLTexAlphaClippedProgram;
extern bgfx::ProgramHandle m_outputVTLTexAlphaBlendedProgram;
extern bgfx::ProgramHandle m_outputVTLAlphaProgram;

extern bgfx::UniformHandle s_texColor;
extern bgfx::VertexLayout ms_outputBucketVertexLayout;

extern GFXTLBUMPVERTEX* sort_buffer_vertex_buffer;
extern bgfx::DynamicVertexBufferHandle sort_buffer_vertex_handle;

struct BGFXSortDrawCommand {
	size_t draw_type = 0;
	size_t offset = 0;
	size_t count = 0;
	bgfx::TextureHandle texture;
};

extern BGFXSortDrawCommand sort_buffer_commands[MAX_SORT_DRAW_COMMANDS];

extern void SetupOutputBucketVertexLayout();

extern void InitializeBGFX();
extern void ShutdownBGFX();

extern void SetupBGFXOutputPolyList();
extern void RenderBGFXDrawLists();

extern void StartBGFXFrame();
extern void EndBGFXFrame();



#endif