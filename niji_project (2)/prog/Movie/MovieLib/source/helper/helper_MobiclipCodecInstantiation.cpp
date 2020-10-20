#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_MobiclipCodecInstantiation_h
 Description: Helper file that manages audio and video codecs instantiation.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#include <nn.h>
#include <nn/fnd.h>

/*---------------------------------------------------------------------------*/

#include "mobiclip.h"
#include "helper_MobiclipCodecInstantiation.h"

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*
 Instantiates a new video codec, based on the mw::mo::moflex::VideoStreamInfo
 structure information. If the codec type is unknown no instance is created.
 *---------------------------------------------------------------------------*/

bool MobiclipCodecInstantiation::CreateVideoCodec (gfl2::heap::NwAllocator* pAllocator, 
                                                   mw::mo::moflex::VideoStreamInfo& videoStreamInfo, 
                                                   mw::mo::GenericVideoDecoder*& pOutVideoDecoder)
{
    if (videoStreamInfo.m_VideoCodec != mw::mo::moflex::VIDEO_CODEC_MOBICLIP)
        return false;

    pOutVideoDecoder = new (pAllocator->Alloc(sizeof(mw::mo::mobiclip::Decoder), 4)) mw::mo::mobiclip::Decoder;

    if ((videoStreamInfo.m_Layout == mw::mo::moflex::LAYOUT_INTERLEAVED_LEFT_EYE_FIRST ||
        videoStreamInfo.m_Layout == mw::mo::moflex::LAYOUT_INTERLEAVED_RIGHT_EYE_FIRST) && 
        m_MinimalUseOfMemory == false)
        pOutVideoDecoder->Create(videoStreamInfo.m_Width, videoStreamInfo.m_Height, 12);
    else
        pOutVideoDecoder->Create(videoStreamInfo.m_Width, videoStreamInfo.m_Height, 6);

    return true;
}

/*---------------------------------------------------------------------------*
 Deletes the video codec instance, if not NULL
 *---------------------------------------------------------------------------*/

void MobiclipCodecInstantiation::DeleteVideoCodec (gfl2::heap::NwAllocator* pAllocator,
                                                   mw::mo::GenericVideoDecoder* pVideoDecoder)
{
    if (pVideoDecoder != NULL)
    {
        pVideoDecoder->~GenericVideoDecoder();
        pAllocator->Free(pVideoDecoder);
    }
}

/*---------------------------------------------------------------------------*
 Instantiates a new audio codec, based on the mw::mo::moflex::AudioStreamInfo
 structure information. If the codec type is unknown no instance is created.
 *---------------------------------------------------------------------------*/

bool MobiclipCodecInstantiation::CreateAudioCodec (gfl2::heap::NwAllocator* pAllocator,
                                                   mw::mo::moflex::AudioStreamInfo& audioStreamInfo, 
                                                   mw::mo::GenericAudioDecoder*& pOutAudioDecoder)
{
    switch (audioStreamInfo.m_AudioCodec)
    {
        case mw::mo::moflex::AUDIO_CODEC_FASTAUDIO:
            pOutAudioDecoder = new (pAllocator->Alloc(sizeof(mw::mo::fastaudio::Decoder), 4)) mw::mo::fastaudio::Decoder;
            break;

        case mw::mo::moflex::AUDIO_CODEC_IMAADPCM:
            pOutAudioDecoder = new (pAllocator->Alloc(sizeof(mw::mo::imaadpcm::Decoder), 4)) mw::mo::imaadpcm::Decoder;
            break;

        case mw::mo::moflex::AUDIO_CODEC_PCM:
            pOutAudioDecoder = NULL;
            break;

        default:
            return false;
    }

    if (pOutAudioDecoder != NULL)
        pOutAudioDecoder->Create(audioStreamInfo.m_Frequency, audioStreamInfo.m_Channel);

    return true;
}

/*---------------------------------------------------------------------------*
 Deletes the audio codec instance, if not NULL
 *---------------------------------------------------------------------------*/

void MobiclipCodecInstantiation::DeleteAudioCodec (gfl2::heap::NwAllocator* pAllocator,
                                                   mw::mo::GenericAudioDecoder* pAudioDecoder)
{
    if (pAudioDecoder != NULL)
    {
        pAudioDecoder->~GenericAudioDecoder();
        pAllocator->Free(pAudioDecoder);
    }
}

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif// defined(GF_PLATFORM_CTR)
