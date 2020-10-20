#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_AudioPresentationBuffer.h
 Description: Helper file that exposes a generic interface useful for audio
              presentation (ie playback).
              All pure virtual methods must be implemented by derived classes.
              See helper_PcmAudioPresentation.h and .cpp files for an 
              implementation example.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_AudioPresentationBuffer_h
#define helper_AudioPresentationBuffer_h

/*---------------------------------------------------------------------------*/

#include <nn/types.h>
#include <nn/fnd.h>

/*---------------------------------------------------------------------------*/

#include "moflex/moflex_Demuxer.h"
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

/*---------------------------------------------------------------------------*/

class AudioPresentationBuffer
{
protected:

    gfl2::heap::NwAllocator*                m_pAllocator;
    const mw::mo::moflex::AudioStreamInfo* m_AudioStreamInfo;

public:

    AudioPresentationBuffer() : m_pAllocator(NULL), m_AudioStreamInfo(NULL){}
    ~AudioPresentationBuffer(){}

    virtual bool Initialize   (gfl2::heap::NwAllocator* pAllocator, 
                               const mw::mo::moflex::AudioStreamInfo* audioStreamInfo,
							   int nbSeconds = 1, 
							   int maxWaveBuffer = 16);
    virtual void Finalize     (void);

    virtual bool AddAudioData (void* pAudioFrameData, u32 audioFrameSize) = 0;
};

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif  // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif  // ifndef helper_AudioPresentationBuffer.h

/*---------------------------------------------------------------------------*/
#endif// defined(GF_PLATFORM_CTR)
