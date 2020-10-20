#if defined(GF_PLATFORM_CTR)
#if !defined( __HELPER_FBVIDEOPRESENTATION_H__ )
#define __HELPER_FBVIDEOPRESENTATION_H__
/*---------------------------------------------------------------------------*
 File:        helper_FbVideoPresentation.h
 Description: Helper file that implements video image presentation to screen
              by directly accessing the CTR device screen frame buffers.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_FbVideoPresentation_h
#define helper_FbVideoPresentation_h

/*---------------------------------------------------------------------------*/

#include "helper_VideoPresentation.h"

/*---------------------------------------------------------------------------*
  C++ API interface
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*/

class FbVideoPresentation : public VideoPresentation
{
private:

    u32 m_DisplayId[3];

public:

    FbVideoPresentation(){}

    virtual bool  Initialize (gfl2::heap::NwAllocator* pAllocator, 
                              const mw::mo::moflex::VideoStreamInfo* topScreenVideoStreamInfo,
                              const mw::mo::moflex::VideoStreamInfo* bottomScreenVideoStreamInfo = NULL);

    virtual bool  ScreenBufferLinear (void);
    virtual void* GetScreenBufferPtr (int eyeIndex);
    virtual u32   GetScreenStride    (int eyeIndex);
    virtual u32   GetDisplayId       (int eyeIndex);
    
    void          Draw               (void);
    void          Flip               (void);
};

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif  // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif  // ifndef helper_FbVideoPresentation.h

/*---------------------------------------------------------------------------*/
#endif // __HELPER_FBVIDEOPRESENTATION_H__
#endif// defined(GF_PLATFORM_CTR)
