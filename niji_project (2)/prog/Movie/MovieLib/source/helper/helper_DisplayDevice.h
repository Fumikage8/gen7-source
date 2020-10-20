#if defined(GF_PLATFORM_CTR)
#if !defined( __HELPER_DISPLAYDEVICE_H__ )
#define __HELPER_DISPLAYDEVICE_H__
/*---------------------------------------------------------------------------*
 File:        helper_DisplayDevice.h
 Description: Helper file that manages CTR display presentation functions in 
              mono(2D) and stereo(3D) mode.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_DisplayDevice_h
#define helper_DisplayDevice_h

/*---------------------------------------------------------------------------*/

//#include <gflib.h>
#include "heap/include/gfl2_Heap.h"
#include "heap/include/gfl2_NwAllocator.h"

/*---------------------------------------------------------------------------*
  C++ API interface
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*
  ENABLE_WAIT_VSYNC option of Initialize function tells that we want to wait 
  for the vertical retrace after a Flip function call before accessing display 
  buffers with either GetBackBuffer or GetFrontBuffer function. 
  By default it is selected and GetBackBuffer or GetFrontBuffer 
  function will wait for the VSync to be performed before returning display
  buffer pointer.
  If not selected then GetBackBuffer or GetFrontBuffer will return 
  immediately.
  We recommend keeping this option activated(default) to avoid potential 
  display tearing issues.
 *---------------------------------------------------------------------------*/

namespace displaydevice
{
    const u32 ENABLE_3D                  = 1;
    const u32 ENABLE_WAIT_VSYNC          = 2;
    const u32 ENABLE_TRIPLE_BUFFERING    = 4;
    const u32 ENABLE_VRAM_DISPLAY_BUFFER = 8;

    bool  Initialize     (gfl2::heap::NwAllocator* pAllocator, u32 displayOptions = (ENABLE_3D | ENABLE_WAIT_VSYNC));
    bool  Finalize       (void);
    bool  Enable3d       (bool enable3D);
    bool  Is3dEnabled    (void);
    void* GetBackBuffer  (s32 display);
    void* GetFrontBuffer (s32 display);
    void  Flip           (s32 display);

    void BeginDraw  (s32 display);
    void EndDraw    (void);
    void Clear      (const f32 red, const f32 green, const f32 blue, const f32 alpha);
    void CommitDraw (void);
} // end of namespace displaydevice

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif // ifndef helper_DisplayDevice_h

/*---------------------------------------------------------------------------*/

#endif // __HELPER_DISPLAYDEVICE_H__
#endif// defined(GF_PLATFORM_CTR)
