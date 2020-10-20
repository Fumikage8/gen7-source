#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_VideoPresentationBuffer.h
 Description: Helper file that exposes a generic interface useful for video 
              image presentation to screen.

              All pure virtual methods must be implemented by derived classes.

              See helper_GlVideoPresentation.h, helper_FbVideoPresentation.h 
              and their .cpp files for an implementation examples.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_VideoPresentationBuffer_h
#define helper_VideoPresentationBuffer_h

/*---------------------------------------------------------------------------*/

#include <nn/types.h>
#include <nn/fnd.h>
#include "heap/include/gfl2_Heap.h"
#include "heap/include/gfl2_NwAllocator.h"

/*---------------------------------------------------------------------------*/
#include "heap/include/gfl2_Heap.h"
#include "moflex/moflex_Demuxer.h"


namespace gfl2 {
namespace util {
class GLHeapAllocator;
}
}
namespace movie {
namespace renderer {
class MobiclipRenderingPipeLine;
}
}


/*---------------------------------------------------------------------------*
  C++ API interface
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*/

class VideoPresentationBuffer
{
protected:

    gfl2::heap::NwAllocator*                   m_pAllocator;
    const mw::mo::moflex::VideoStreamInfo* m_TopScreenVideoStreamInfo;
    const mw::mo::moflex::VideoStreamInfo* m_BottomScreenVideoStreamInfo;

public:

    VideoPresentationBuffer() : m_pAllocator(NULL), m_TopScreenVideoStreamInfo(NULL), m_BottomScreenVideoStreamInfo(NULL){}
    ~VideoPresentationBuffer(){}

    virtual bool Initialize (gfl2::heap::NwAllocator* pAllocator,
                             gfl2::util::GLHeapAllocator* pGLDeviceAllocator,
                             movie::renderer::MobiclipRenderingPipeLine* renderingPipeLine,
                             const mw::mo::moflex::VideoStreamInfo* topScreenVideoStreamInfo,
                             const mw::mo::moflex::VideoStreamInfo* bottomScreenVideoStreamInfo = NULL);

    virtual void Finalize   (movie::renderer::MobiclipRenderingPipeLine* renderingPipeLine);

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

#endif  // ifndef helper_VideoPresentationBuffer_h

/*---------------------------------------------------------------------------*/
#endif// defined(GF_PLATFORM_CTR)
