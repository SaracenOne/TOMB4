#pragma once


#ifdef USE_BGFX

#include <bgfx/bgfx.h>

#define SORT_BUFFER_VERT_COUNT 8192
#define MAX_VERTS_PER_SORT_BUFFER 21 * 3
#define MAX_SORT_VERTEX_BUFFERS 1024

extern size_t current_sort_vertex_buffer_idx;
extern size_t current_sort_vertex_buffer_offset;

extern bgfx::ProgramHandle m_outputVTLProgram;
extern bgfx::ProgramHandle m_outputVLProgram;
extern bgfx::UniformHandle s_texColor;
extern bgfx::VertexLayout ms_outputBucketVertexLayout;

extern GFXTLBUMPVERTEX* sort_buffer_vertex_buffers_frontbuffer;
extern GFXTLBUMPVERTEX* sort_buffer_vertex_buffers_backbuffer;
extern bgfx::DynamicVertexBufferHandle sort_buffer_vertex_handle[MAX_SORT_VERTEX_BUFFERS];

struct BGFXSortDrawCommand {
	size_t blend_type = 0;
	size_t count = 0;
	bgfx::TextureHandle texture;
};

extern BGFXSortDrawCommand sort_buffer_commands[MAX_SORT_VERTEX_BUFFERS];

extern void SetupOutputBucketVertexLayout();

extern void InitializeBGFX();
extern void ShutdownBGFX();

extern void SetupBGFXOutputPolyList();
extern void RenderBGFXDrawLists();

extern void StartBGFXFrame();
extern void EndBGFXFrame();



#endif