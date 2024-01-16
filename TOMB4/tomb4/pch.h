// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#define DIRECTINPUT_VERSION 0x0800
#define _USE_MATH_DEFINES
#define NOMINMAX 
#include <windows.h>
#include <windowsx.h>
#ifndef USE_BGFX
#include <d3d.h>
#include <d3dtypes.h>
#endif
#include <cmath>
#include <stdio.h>
#include <process.h>
#if !defined(MA_AUDIO_SAMPLES) || !defined(MA_AUDIO_ENGINE)
#define DIRECTSOUND_VERSION	0x0800
#define XAUDIO2_HELPER_FUNCTIONS
#include <dsound.h>
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <MSAcm.h>
#endif
#if !defined(USE_SDL)
#include <dinput.h>
#include <mmreg.h>
#endif

#ifdef USE_BGFX
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/allocator.h>
#include <bx/math.h>
#include <bx/platform.h>
#endif

#include "../../resource.h"

#endif //PCH_H
