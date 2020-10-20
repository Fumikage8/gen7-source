#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_PcmAudioPresentation.cpp
 Description: Helper file that that manages audio presentation (ie playback)
              in PCM format using mw::mo::helper::audiodevice.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#include <nn.h>
#include <nn/fnd.h>
#include <string.h>

/*---------------------------------------------------------------------------*/

#include "helper_PcmAudioPresentation.h"
#include "helper_AudioDevice.h"

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*
 Initialization of the audio presentation object
 *---------------------------------------------------------------------------*/

bool PcmAudioPresentation::Initialize(gfl2::heap::NwAllocator* pAllocator, const mw::mo::moflex::AudioStreamInfo* audioStreamInfo, int nbSeconds, int maxWaveBuffer)
{
    /* We call the parent Initialize method */
    if (AudioPresentationBuffer::Initialize(pAllocator, audioStreamInfo) == false)
        return false;

    /* We allocate a buffer that can contain up to one second of audio data */
    m_AudioBufferSize = (audioStreamInfo->m_Frequency * audioStreamInfo->m_Channel * 2 * nbSeconds);

    m_AudioBuffer = (u8*)m_pAllocator->Alloc(m_AudioBufferSize, 32);
    if (m_AudioBuffer == NULL)
        return false;
    
    /* We allocate a mw::mo::helper::audiodevice sound object */
	m_SoundId = mw::mo::helper::audiodevice::AllocSound(audioStreamInfo->m_Frequency, audioStreamInfo->m_Channel, maxWaveBuffer);
    if (m_SoundId == -1)
    {
        m_pAllocator->Free(m_AudioBuffer);
        m_AudioBuffer = NULL;
        return false;
    }
    
    m_AudioPushIndex = 0;
    m_AudioBytesPushed = 0;

    return true;
}

/*---------------------------------------------------------------------------*
 Finalization of the audio presentation object
 *---------------------------------------------------------------------------*/

void PcmAudioPresentation::Finalize(void)
{
    /* We stop the playback */
    StopPlayback();

    if (m_SoundId != -1)
    {
        /* We de-allocate the mw::mo::helper::audiodevice sound object */
        mw::mo::helper::audiodevice::FreeSound(m_SoundId);
        m_SoundId = -1;

        /* We de-allocate the audio buffer */
        m_pAllocator->Free(m_AudioBuffer);
        m_AudioBuffer = NULL;
    }

    /* We call the parent Finalize method */
    AudioPresentationBuffer::Finalize();
}

/*---------------------------------------------------------------------------*
  Returns a pointer to a free continuous place in the audio buffer of the 
  requested size.
 *---------------------------------------------------------------------------*/

u8* PcmAudioPresentation::GetNextAudioDataPtr (u32 audioDataSize)
{
    /* We compute the free contiguous size available in the audio buffer. */
    s64 pushedBytes = m_AudioBytesPushed;

    s64 playedBytes = mw::mo::helper::audiodevice::GetTotalSamplePlayed(m_SoundId);
    playedBytes <<= m_AudioStreamInfo->m_Channel;

    u32 freeSize = m_AudioBufferSize - (u32)(pushedBytes - playedBytes);
        
    u32 pushIndex = m_AudioPushIndex;
    u32 sizeToEndOfBuffer = m_AudioBufferSize - pushIndex;
    if (sizeToEndOfBuffer < audioDataSize)
    {
        pushIndex = 0;
        freeSize -= sizeToEndOfBuffer; 
    }
        
    if (freeSize < audioDataSize)
        return NULL; /* There is not enough free place to store the new audio data. */

    return m_AudioBuffer + pushIndex;
}

/*---------------------------------------------------------------------------*
  Commits (that is send to the audio device for playback) the audio buffer
  given as input. Also updates audio buffer internal parameters.
 *---------------------------------------------------------------------------*/

bool PcmAudioPresentation::CommitAudioData (u8* audiodataPtr, u32 audioDataSize)
{
    if (mw::mo::helper::audiodevice::PushAudioPacket(m_SoundId, audiodataPtr, audioDataSize) == false)
        return false; /* mw::mo::helper::audiodevide audio object returned an error */

    m_AudioPushIndex = (audiodataPtr - m_AudioBuffer) + audioDataSize;
    m_AudioBytesPushed += (s64)audioDataSize;

    return true;
}

/*---------------------------------------------------------------------------*
  Append new audio data to the currently playing audio object.
  This function is called by the mw::mo::helper::Player class. 
  If there is not enough free place to store the new audio data or if the 
  mw::mo::helper::audiodevice audio object returns an error, false is returned;
  the mw::mo::helper::Player class will then try to push the same audio data
  later.
 *---------------------------------------------------------------------------*/

bool PcmAudioPresentation::AddAudioData (void* pAudioFrameData, u32 audioFrameSize)
{
    /* We retrieve a free contiguous place available in the audio buffer. */
    u8* dstPtr = GetNextAudioDataPtr(audioFrameSize);
    if (dstPtr == NULL) /* no place available, we return an error */
        return false;

    /* Audio data is copied. */
    memcpy(dstPtr, pAudioFrameData, audioFrameSize);

    /* We commit the newly copied audio data. */
    return CommitAudioData(dstPtr, audioFrameSize);
}

/*---------------------------------------------------------------------------*
  Start the audio playback, if not done.
 *---------------------------------------------------------------------------*/

void PcmAudioPresentation::StartPlayback (void)
{
    if (m_SoundId != -1 && m_AudioPlaying == false)
    {
        mw::mo::helper::audiodevice::PlaySound(m_SoundId);
        m_AudioPlaying = true;
		m_AudioPaused = false;
    }
}

/*---------------------------------------------------------------------------*
 Stop the audio playback, if not done.
 *---------------------------------------------------------------------------*/

void PcmAudioPresentation::StopPlayback (void)
{
    if (m_SoundId != -1)
    {
        mw::mo::helper::audiodevice::StopSound(m_SoundId);
        m_AudioPlaying = false;
		m_AudioPaused = false;
        m_AudioPushIndex = 0;
        m_AudioBytesPushed = 0;
    }
}

/*---------------------------------------------------------------------------*
 Pause the audio playback, if not done.
 *---------------------------------------------------------------------------*/

void PcmAudioPresentation::PausePlayback (void)
{
    if (m_SoundId != -1  && m_AudioPlaying == true)
    {
        mw::mo::helper::audiodevice::PauseSound(m_SoundId);
        m_AudioPlaying = false;
		m_AudioPaused = true;
    }
}

/*---------------------------------------------------------------------------*
  Resume the audio playback, if not done.
 *---------------------------------------------------------------------------*/

void PcmAudioPresentation::ResumePlayback (void)
{
    if (m_SoundId != -1 && m_AudioPlaying == false)
    {
		if (m_AudioPaused == true)
	        mw::mo::helper::audiodevice::ResumeSound(m_SoundId);
		else
			mw::mo::helper::audiodevice::PlaySound(m_SoundId);

        m_AudioPlaying = true;
		m_AudioPaused = false;
    }
}

/*---------------------------------------------------------------------------*
  We are in an audio underflow case if audio playback started and if 
  all pushed audio data have already been played.
 *---------------------------------------------------------------------------*/

bool PcmAudioPresentation::AudioUnderflow (void)
{
    if (m_AudioPlaying == true && 
        mw::mo::helper::audiodevice::GetTotalSamplePlayed(m_SoundId) > 0 && 
        mw::mo::helper::audiodevice::GetTotalSamplePlayed(m_SoundId) == mw::mo::helper::audiodevice::GetTotalSamplePushed(m_SoundId))
        return true;

    return false;
}

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/
#endif// defined(GF_PLATFORM_CTR)
