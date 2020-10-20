#if defined(GF_PLATFORM_CTR)
#if !defined( __HELPER_GLVIDEOPRESENTATION_H__ )
#define __HELPER_GLVIDEOPRESENTATION_H__
/*---------------------------------------------------------------------------*
 File:        helper_GlVideoPresentation.h
 Description: Helper file that implements video image presentation to screen
              using OpenGL ES API.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_GlVideoPresentation_h
#define helper_GlVideoPresentation_h

/*---------------------------------------------------------------------------*/

#include "helper_VideoPresentationBuffer.h"
#include "helper_RgbImage.h"


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

class GlVideoPresentation : public VideoPresentationBuffer
{
private:

    u8*                      m_RgbVideoImageData[3];
    u32                      m_RgbVideoImageStride[3];
    u32                      m_DisplayId[3];
    mw::mo::helper::RgbImage m_RgbVideoImage[3];
        
    bool AllocateImage (
						const mw::mo::moflex::VideoStreamInfo* inVideoStreamInfo, 
                        u8** outImageData, mw::mo::helper::RgbImage* outImageClass, u32* outStride);

public:

    GlVideoPresentation();

    virtual bool Initialize (
							 gfl2::heap::NwAllocator* pAllocator,
               gfl2::util::GLHeapAllocator* pGLDeviceAllocator,
               movie::renderer::MobiclipRenderingPipeLine* renderingPipeLine,
                             const mw::mo::moflex::VideoStreamInfo* topScreenVideoStreamInfo,
                             const mw::mo::moflex::VideoStreamInfo* bottomScreenVideoStreamInfo = NULL);
    virtual void Finalize   (movie::renderer::MobiclipRenderingPipeLine* renderingPipeLine);

    virtual bool  ScreenBufferLinear (void);
    virtual void* GetScreenBufferPtr (int eyeIndex);
    virtual u32   GetScreenStride    (int eyeIndex);
    virtual u32   GetDisplayId       (int eyeIndex);

    void          Draw               (void);
	void UpdateTexture();
    void          Flip               (void);
};

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif  // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif  // ifndef helper_GlVideoPresentation.h

/*---------------------------------------------------------------------------*/
#endif // __HELPER_GLVIDEOPRESENTATION_H__
#endif// defined(GF_PLATFORM_CTR)
