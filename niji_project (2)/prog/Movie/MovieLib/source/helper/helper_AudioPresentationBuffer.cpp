#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_AudioPresentationBuffer.cpp
 Description: Helper file that exposes a generic interface useful for audio
              presentation (ie playback).
              All pure virtual methods must be implemented by derived classes.
              See helper_PcmAudioPresentation.h and .cpp files for an 
              implementation examples.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#include <nn.h>
#include <nn/fnd.h>

/*---------------------------------------------------------------------------*/

#include "helper_AudioPresentationBuffer.h"

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*
 Initialization of the audio presentation buffer object
 *---------------------------------------------------------------------------*/

bool AudioPresentationBuffer::Initialize (gfl2::heap::NwAllocator* pAllocator, const mw::mo::moflex::AudioStreamInfo* audioStreamInfo, int nbSeconds, int maxWaveBuffer)
{
	(void)nbSeconds, (void)maxWaveBuffer;
	
    if (pAllocator == NULL || audioStreamInfo == NULL)
        return false;

    m_pAllocator = pAllocator;
    m_AudioStreamInfo = audioStreamInfo;

    return true;
}

/*---------------------------------------------------------------------------*
 Finalization of the audio presentation buffer object
 *---------------------------------------------------------------------------*/

void AudioPresentationBuffer::Finalize(void)
{
    m_pAllocator = NULL;
    m_AudioStreamInfo = NULL;
}

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/
#endif// defined(GF_PLATFORM_CTR)
