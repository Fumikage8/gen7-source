#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_VideoPresentation.cpp
 Description: Helper file that exposes a generic interface for video 
              inmage presentation to screen.

              All pure virtual methods must be implemented by derived classes.
              Draw and Flip are only made to have a generic interface for 
              video presentation by all derived classes: the developper is
              free leave them empty and perform its own drawing routines.

              See helper_GlVideoPresentation.h, helper_FbVideoPresentation.h 
              and their .cpp files for an implementation examples.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#include <nn.h>
#include <nn/fnd.h>

/*---------------------------------------------------------------------------*/

#include "helper_VideoPresentation.h"

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*
 Initialization of the video presentation object
 *---------------------------------------------------------------------------*/

bool VideoPresentation::Initialize (gfl2::heap::NwAllocator* pAllocator,
                                    const mw::mo::moflex::VideoStreamInfo* topScreenVideoStreamInfo,
                                    const mw::mo::moflex::VideoStreamInfo* bottomScreenVideoStreamInfo)
{
    if (pAllocator== NULL)
        return false;

    m_pAllocator = pAllocator;
    m_TopScreenVideoStreamInfo = topScreenVideoStreamInfo;
    m_BottomScreenVideoStreamInfo = bottomScreenVideoStreamInfo;

    return true;
}

/*---------------------------------------------------------------------------*
 Initialization of the video presentation object
 *---------------------------------------------------------------------------*/

void VideoPresentation::Finalize (void)
{
    m_pAllocator = NULL;
    m_TopScreenVideoStreamInfo = NULL;
    m_BottomScreenVideoStreamInfo = NULL;
}

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/
#endif// defined(GF_PLATFORM_CTR)
