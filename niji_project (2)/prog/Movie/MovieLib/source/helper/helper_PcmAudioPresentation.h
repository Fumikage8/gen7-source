#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_PcmAudioPresentation.h
 Description: Helper file that that manages audio presentation (ie playback)
              in PCM format using mw::mo::helper::audiodevice.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_PcmAudioPresentation_h
#define helper_PcmAudioPresentation_h

/*---------------------------------------------------------------------------*/

#include "helper_AudioPresentationBuffer.h"

/*---------------------------------------------------------------------------*
  C++ API interface
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*/

class PcmAudioPresentation : public AudioPresentationBuffer
{
private:

    int     m_SoundId;
    u8*     m_AudioBuffer;
    u32     m_AudioBufferSize;
    s64     m_AudioBytesPushed;
    u32     m_AudioPushIndex;
    
protected:

    bool    m_AudioPlaying;
	bool    m_AudioPaused;
    int:    16; // padding added to avoid compiler warning

    u8*        GetNextAudioDataPtr (u32 audioDataSize);
    bool    CommitAudioData        (u8* audiodataPtr, u32 audioDataSize);

public:

    PcmAudioPresentation() : m_SoundId(-1), m_AudioBuffer(NULL), m_AudioPlaying(false), m_AudioPaused(false){}

    virtual bool Initialize (gfl2::heap::NwAllocator* pAllocator, const mw::mo::moflex::AudioStreamInfo* audioStreamInfo, int nbSeconds = 1, int maxWaveBuffer = 16);
    virtual void Finalize   (void);

    virtual bool AddAudioData (void* pAudioFrameData, u32 audioFrameSize);

    void StartPlayback  (void);
    void StopPlayback   (void);
    void PausePlayback  (void);
    void ResumePlayback (void);
    bool AudioUnderflow (void);
};

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif  // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif  // ifndef helper_PcmAudioPresentation_h

/*---------------------------------------------------------------------------*/

#endif// defined(GF_PLATFORM_CTR)
