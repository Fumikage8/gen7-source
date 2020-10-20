#if defined(GF_PLATFORM_CTR)
#if !defined( __HELPER_VIDEOPRESENTATION_H__ )
#define __HELPER_VIDEOPRESENTATION_H__
/*---------------------------------------------------------------------------*
 File:        helper_VideoPresentation.h
 Description: Helper file that exposes a generic interface for video 
              image presentation to screen.

              All pure virtual methods must be implemented by derived classes.

              See helper_GlVideoPresentation.h, helper_FbVideoPresentation.h 
              and their .cpp files for an implementation examples.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_VideoPresentation_h
#define helper_VideoPresentation_h

/*---------------------------------------------------------------------------*/

#include <nn/types.h>
#include <nn/fnd.h>

#include "heap/include/gfl2_Heap.h"
#include "heap/include/gfl2_NwAllocator.h"
//#include <gflib.h>

/*---------------------------------------------------------------------------*/

#include "moflex/moflex_Demuxer.h"

/*---------------------------------------------------------------------------*
  C++ API interface
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*/

class VideoPresentation
{
protected:

    gfl2::heap::NwAllocator*                   m_pAllocator;
    const mw::mo::moflex::VideoStreamInfo* m_TopScreenVideoStreamInfo;
    const mw::mo::moflex::VideoStreamInfo* m_BottomScreenVideoStreamInfo;

public:

    VideoPresentation() : m_pAllocator(NULL), m_TopScreenVideoStreamInfo(NULL), m_BottomScreenVideoStreamInfo(NULL){}
    ~VideoPresentation(){}

    virtual bool Initialize (gfl2::heap::NwAllocator* pAllocator,
                             const mw::mo::moflex::VideoStreamInfo* topScreenVideoStreamInfo,
                             const mw::mo::moflex::VideoStreamInfo* bottomScreenVideoStreamInfo = NULL);

    virtual void Finalize   (void);

    virtual bool  ScreenBufferLinear (void) = 0;
    virtual void* GetScreenBufferPtr (int eyeIndex) = 0;
    virtual u32   GetScreenStride    (int eyeIndex) = 0;
    virtual u32   GetDisplayId       (int eyeIndex) = 0;

    const mw::mo::moflex::VideoStreamInfo* GetTopVideoStreamInfo    (void){return m_TopScreenVideoStreamInfo;}
    const mw::mo::moflex::VideoStreamInfo* GetBottomVideoStreamInfo (void){return m_BottomScreenVideoStreamInfo;}
};

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif  // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif  // ifndef helper_VideoPresentation.h

/*---------------------------------------------------------------------------*/
#endif // __HELPER_VIDEOPRESENTATION_H__
#endif// defined(GF_PLATFORM_CTR)
