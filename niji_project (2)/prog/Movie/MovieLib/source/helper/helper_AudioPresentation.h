#if defined(GF_PLATFORM_CTR)
#if !defined( __HELPER_AUDIOPRESENTATION_H__ )
#define __HELPER_AUDIOPRESENTATION_H__
/*---------------------------------------------------------------------------*
 File:        helper_AudioPresentation.h
 Description: Helper file that exposes a generic interface for audio
              presentation (ie playback).
              All pure virtual methods must be implemented by derived classes.
              See helper_PcmAudioPresentation.h and .cpp files for an 
              implementation example.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_AudioPresentation_h
#define helper_AudioPresentation_h

/*---------------------------------------------------------------------------*/

#include <nn/types.h>
#include <nn/fnd.h>

//#include <gflib.h>
#include "heap/include/gfl2_Heap.h"
/*---------------------------------------------------------------------------*/

#include "moflex/moflex_Demuxer.h"
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

class AudioPresentation
{
protected:

    gfl2::heap::NwAllocator*                   m_pAllocator;
    const mw::mo::moflex::AudioStreamInfo* m_AudioStreamInfo;

public:

    AudioPresentation() : m_pAllocator(NULL), m_AudioStreamInfo(NULL){}
    ~AudioPresentation(){}

    virtual bool Initialize   (gfl2::heap::NwAllocator* pAllocator, 
                               const mw::mo::moflex::AudioStreamInfo* audioStreamInfo);
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

#endif  // ifndef helper_AudioPresentation.h

/*---------------------------------------------------------------------------*/
#endif // __HELPER_AUDIOPRESENTATION_H__
#endif// defined(GF_PLATFORM_CTR)
