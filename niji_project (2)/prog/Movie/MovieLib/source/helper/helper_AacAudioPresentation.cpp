#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_AacAudioPresentation.cpp
 Description: Helper file that that manages audio presentation (ie playback)
              in AAC ADTS LC profile format using mw::mo::helper::audiodevice.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#include <nn.h>
#include <nn/fnd.h>
#include <nn/aacdec.h>
#include <string.h>

/*---------------------------------------------------------------------------*/

#include "helper_AacAudioPresentation.h"
#include "helper_AudioDevice.h"

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*
 Audio presentation object constructor
 *---------------------------------------------------------------------------*/

AacAudioPresentation::AacAudioPresentation()
{
    m_PcmBufferL = NULL;
    m_PcmBufferR = NULL;

    for (int i=0; i<MAX_PENDING_ADTS_FRAME; i++)
        m_AdtsFrameArray[i] = NULL;

    m_EndAccThread = true;
}

/*---------------------------------------------------------------------------*
 Calls Finalize and returns false to indicate en error.
 *---------------------------------------------------------------------------*/

bool AacAudioPresentation::FinalizeOnError(void)
{
    Finalize();

    return false;
}

/*---------------------------------------------------------------------------*
 Initialization of the audio presentation object
 *---------------------------------------------------------------------------*/

bool AacAudioPresentation::Initialize(gfl2::heap::NwAllocator* pAllocator, const mw::mo::moflex::AudioStreamInfo* audioStreamInfo, int nbSeconds, int maxWaveBuffer)
{
    /* We call the parent Initialize method */
    if (PcmAudioPresentation::Initialize(pAllocator, audioStreamInfo, nbSeconds, maxWaveBuffer) == false)
        return false;

    /* Allocates all necessary buffers if we have to play AAC audio data */
    if (m_AudioStreamInfo->m_AudioCodec == 3)
    {
        m_PcmBufferL = (s16*)pAllocator->Alloc(1024*2, 32);
        if (m_PcmBufferL == NULL)
            return FinalizeOnError();

        m_PcmBufferR = (s16*)pAllocator->Alloc(1024*2, 32);
        if (m_PcmBufferR == NULL)
            return FinalizeOnError();

        m_AdtsPushIndex = 0;
        m_AdtsPopIndex = 0;
        m_AdtsFramesPushed = 0;

        for (int i=0; i<MAX_PENDING_ADTS_FRAME; i++)
        {
            m_AdtsFrameArray[i] = pAllocator->Alloc(NN_DSP_CODEC_AAC_BS_SIZE, 32);
            if (m_AdtsFrameArray[i] == NULL)
                return FinalizeOnError();
        }

        if (nn::aacdec::Initialize(nn::aacdec::FORM_ADTS) == false)
            return FinalizeOnError();

        m_EndAccThread = false;
        m_AacCriticalSection.Initialize();
        m_AacThread.StartUsingAutoStack(AacThreadFunc, this, AAC_THREAD_STACK_SIZE, AAC_THREAD_PRIORITY);
    }

    return true;
}

/*---------------------------------------------------------------------------*
 Finalization of the audio presentation object
 *---------------------------------------------------------------------------*/

void AacAudioPresentation::Finalize(void)
{
    /* Deallocates all necessary buffers if we have to play AAC audio data */
    if (m_AudioStreamInfo != NULL && m_AudioStreamInfo->m_AudioCodec == 3)
    {
        if (m_EndAccThread == false)
        {
            m_EndAccThread = true;

            m_AacThread.Join();
            m_AacThread.Finalize();
            m_AacCriticalSection.Finalize();
            m_EndAccThread = false;
        }

        nn::aacdec::Finalize();

        if (m_PcmBufferL != NULL)
        {
            m_pAllocator->Free(m_PcmBufferL);
            m_PcmBufferL = NULL;
        }

        if (m_PcmBufferR != NULL)
        {
            m_pAllocator->Free(m_PcmBufferR);
            m_PcmBufferR = NULL;
        }

        for (int i=0; i<MAX_PENDING_ADTS_FRAME; i++)
        {
            if (m_AdtsFrameArray[i] != NULL)
            {
                m_pAllocator->Free(m_AdtsFrameArray[i]);
                m_AdtsFrameArray[i] = NULL;
            }
        }
    }

    /* We call the parent Finalize method */
    PcmAudioPresentation::Finalize();
}

/*---------------------------------------------------------------------------*
  Thread that is in charge of decoding AAC frames and sending the decoded PCM
  data for playback.
 *---------------------------------------------------------------------------*/

void AacAudioPresentation::AacThreadFunc (AacAudioPresentation* aacAudioPresentation)
{
    for(;;)
    {
        /* Gives CPU time to other running thread. */
        nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMicroSeconds(1000));

        /* Requested to end the thread, we exit it */
        if (aacAudioPresentation->m_EndAccThread == true)
            return;

        /* Decodes ADTS frames and add them for playback */
        aacAudioPresentation->DecodeAndPlayAdtsFrames();
    }    
}

/*---------------------------------------------------------------------------*
  Decodes ADTS frames and add them for playback 
 *---------------------------------------------------------------------------*/

void AacAudioPresentation::DecodeAndPlayAdtsFrames (void)
{
    /* We decode ADTS frames and add them for playback */
    if (m_AdtsFramesPushed > 0)
    {
        nn::aacdec::Decode((u8*)m_AdtsFrameArray[m_AdtsPopIndex], m_PcmBufferL, m_PcmBufferR);
        nn::aacdec::Wait();

        /* We search and wait for  a free place inside pcm audio buffer used for playback to 
           store newly decoded frame */
        u8* nextAudioDataPtr;
        do
        {
            nextAudioDataPtr = GetNextAudioDataPtr(1024 * m_AudioStreamInfo->m_Channel * 2);
        }
        while(nextAudioDataPtr == NULL);

        /* We copy newly decoded audio frame into pcm audio buffer used for playback.
           If audio data is stereo we interleave left and right channel pcm data into 
           destination buffer. */
        s16* dstPtr = (s16*)nextAudioDataPtr;
        if (m_AudioStreamInfo->m_Channel == 2)
        {
            for (int i=0; i<1024; i++)
            {
                *(dstPtr++) = m_PcmBufferL[i];
                *(dstPtr++) = m_PcmBufferR[i];
            }
        }
        else
        {
            for (int i=0; i<1024; i++)
                *(dstPtr++) = m_PcmBufferL[i];
        }

        /* Newly copied audio data is sent for playback */
        while(!CommitAudioData(nextAudioDataPtr, 1024 * m_AudioStreamInfo->m_Channel * 2)){}
        
        m_AacCriticalSection.Enter();

        /* Pops the decoded ADTS frame */
        m_AdtsPopIndex++;
        if (m_AdtsPopIndex == MAX_PENDING_ADTS_FRAME)
            m_AdtsPopIndex = 0;
        m_AdtsFramesPushed--;

        m_AacCriticalSection.Leave();
    }
}

/*---------------------------------------------------------------------------*
  Decode AAC frames and append new audio data to the currently playing audio 
  object.
  This function is called by the mw::mo::helper::Player class. 
 *---------------------------------------------------------------------------*/

bool AacAudioPresentation::AddAudioData (void* pAudioFrameData, u32 audioFrameSize)
{
    /* We call parent method if case we don't have any AAC data to play */
    if (m_AudioStreamInfo->m_AudioCodec != 3)
        return PcmAudioPresentation::AddAudioData(pAudioFrameData, audioFrameSize);

    u32 offset = 0;
    u8* p = (u8*)pAudioFrameData;

    /* We retrieve the number of ADTS frames contained in the input buffer */
    int nbAdtsFrames = 0;
    while (offset < audioFrameSize)
    {
        nbAdtsFrames++;
        offset += nn::aacdec::GetAdtsFrameSize(p + offset);
    }

    /* We return false if we don't have enough place to store all ADTS frames */
    if (nbAdtsFrames > MAX_PENDING_ADTS_FRAME - m_AdtsFramesPushed)
        return false;

    m_AacCriticalSection.Enter();

    /* We copy all input ADTS frames to the internal adts audio buffers */
    offset = 0;
    while (offset < audioFrameSize)
    {
        u32 adtsFrameSize = nn::aacdec::GetAdtsFrameSize(p + offset);
        memcpy (m_AdtsFrameArray[m_AdtsPushIndex], p + offset, adtsFrameSize);
        offset += adtsFrameSize;

        nn::dsp::FlushDataCache((uptr)(m_AdtsFrameArray[m_AdtsPushIndex]), adtsFrameSize);

        m_AdtsPushIndex++;
        if (m_AdtsPushIndex == MAX_PENDING_ADTS_FRAME)
            m_AdtsPushIndex = 0;

        m_AdtsFramesPushed++;
    }
    
    m_AacCriticalSection.Leave();

    return true;
}

/*---------------------------------------------------------------------------*
 Stop the audio playback, if not done.
 *---------------------------------------------------------------------------*/

void AacAudioPresentation::StopPlayback (void)
{
    if (m_AudioStreamInfo != NULL && m_AudioStreamInfo->m_AudioCodec == 3 && 
        m_AudioPlaying == true)
    {
        do{}while(m_AdtsFramesPushed > 0);
    }

    PcmAudioPresentation::StopPlayback();
}

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/
#endif// defined(GF_PLATFORM_CTR)
