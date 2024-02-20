#pragma once

#ifdef USE_BGFX

#include <bgfx/bgfx.h>

extern bgfx::ProgramHandle m_outputProgram;

struct BGFXTransformLitVertex
{
    float		    x;
    float	    	y;
    float		    z;
    float           w;
    unsigned int    color;
    unsigned int    specular;
    float           tu;
    float           tv;

    static void init() {
        ms_layout
            .begin()
            .add(bgfx::Attrib::Position, 4, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .add(bgfx::Attrib::Color1, 4, bgfx::AttribType::Uint8, true)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
    };

    static bgfx::VertexLayout ms_layout;
};

extern void InitializeBGFX();
extern void SetupBGFXOutputPolyList();
extern void StartBGFXFrame();
extern void EndBGFXFrame();

#endif