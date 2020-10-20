#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_AudioDevice.h
 Description: Helper file that manages simple audio playback

 Copyright (C)2010 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/
#pragma once
#ifndef helper_AudioDevice_h
#define helper_AudioDevice_h

/*---------------------------------------------------------------------------*/

#include <nn.h>
#include <nn/os.h>
#include <nn/fnd.h>
#include <nn/snd.h>
#include "heap/include/gfl2_Heap.h"
#include "thread/include/gfl2_Thread.h"
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

namespace audiodevice
{
    typedef void(* SoundCallback)(uptr arg);

    bool            Initialize              (gfl2::heap::NwAllocator* pAllocator, u32 maxNbSound, bool useThread = true);
    void            Finalize                (void);
    SoundCallback   GetSoundCallback        (void);
    int             AllocSound              (s32 sampleRate, s32 channelCount, u32 nbWaveBuffer);
    void            FreeSound               (int soundId);
    void            PlaySound               (int soundId);
    void            StopSound               (int soundId);
    void            PauseSound              (int soundId);
    void            ResumeSound             (int soundId);
    bool            PushAudioPacket         (int soundId, void* pcmBuffer = NULL, u32 bufferSize = 0);
    u32             GetMaxAudioPackets      (int soundId);
    u32             GetPendingAudioPackets  (int soundId);
    s64             GetTotalSamplePushed    (int soundId);
    s64             GetTotalSamplePlayed    (int soundId);
    s32             GetSampleRate           (int soundId);
    s32             GetChannelCount         (int soundId);

} // end of namespace audiodevice

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif // ifndef helper_AudioDevice_h

/*---------------------------------------------------------------------------*/

#endif// defined(GF_PLATFORM_CTR)
