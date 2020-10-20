#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_AacAudioPresentation.h
 Description: Helper file that that manages audio presentation (ie playback)
              in AAC ADTS LC profile format using mw::mo::helper::audiodevice.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_AacAudioPresentation_h
#define helper_AacAudioPresentation_h

/*---------------------------------------------------------------------------*/

#include "helper_PcmAudioPresentation.h"

#ifdef __CC_ARM
	#pragma push
	#pragma diag_suppress 2530 // padding added to end of struct
#endif

/*---------------------------------------------------------------------------*
  C++ API interface
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*
 Contants used to configure some class properties
 *---------------------------------------------------------------------------*/

const int    MAX_PENDING_ADTS_FRAME = 16;
const int    AAC_THREAD_PRIORITY    = 3;
const int    AAC_THREAD_STACK_SIZE  = 4096;

/*---------------------------------------------------------------------------*/

class AacAudioPresentation : public PcmAudioPresentation
{
private:

    void*           m_AdtsFrameArray[MAX_PENDING_ADTS_FRAME];
    volatile u32    m_AdtsPushIndex;
    volatile u32    m_AdtsPopIndex;
    volatile u32    m_AdtsFramesPushed;
    s16*            m_PcmBufferL;
    s16*            m_PcmBufferR;

    volatile bool               m_EndAccThread;
    int:                        24;
    nn::os::CriticalSection     m_AacCriticalSection;
    nn::os::Thread              m_AacThread;


    static void    AacThreadFunc (AacAudioPresentation* aacAudioPresentation);

    void    DecodeAndPlayAdtsFrames (void);
    bool    FinalizeOnError            (void);

public:

    AacAudioPresentation();

    virtual bool Initialize (gfl2::heap::NwAllocator* pAllocator, const mw::mo::moflex::AudioStreamInfo* audioStreamInfo, int nbSeconds = 1, int maxWaveBuffer = 16);
    virtual void Finalize   (void);

    virtual bool AddAudioData (void* pAudioFrameData, u32 audioFrameSize);

    void StopPlayback   (void);
};

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif  // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#ifdef __CC_ARM
	#pragma pop
#endif

#endif  // ifndef helper_AacAudioPresentation_h

/*---------------------------------------------------------------------------*/

#endif// defined(GF_PLATFORM_CTR)
