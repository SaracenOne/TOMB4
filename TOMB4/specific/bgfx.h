#pragma once


#ifdef USE_BGFX

#include <bgfx/bgfx.h>

#define SORT_BUFFER_VERT_COUNT 8192 * 3
#define MAX_SORT_DRAW_COMMANDS 2048

#define MAX_DRAW_COMMANDS 8

extern size_t last_sort_command_idx;
extern size_t last_sort_vertex_buffer_offset;

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

struct BGFXDrawCommand {
	bool is_sorted_command = false;
	size_t last_idx = 0;
};

extern BGFXSortDrawCommand sort_draw_commands[MAX_SORT_DRAW_COMMANDS];

extern void SetupOutputBucketVertexLayout();

extern void InitializeBGFX();
extern void ShutdownBGFX();

extern void SetupBGFXOutputPolyList();
extern void RenderBGFXDrawLists();

extern void StartBGFXFrame();
extern void EndBGFXFrame();

extern void AddBGFXDrawCommand(bool is_sorted_command);
extern void AddBGFXDrawSortCommand(GFXTLBUMPVERTEX* info, short num_verts, short texture, short type);
extern void ClearBGFXDrawCommand();

extern void FindBGFXBucket(long tpage, GFXTLBUMPVERTEX** Vpp, long** nVtxpp);

#endif