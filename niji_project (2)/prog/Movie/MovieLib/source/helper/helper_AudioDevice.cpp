#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_AudioDevice.cpp
 Description: Helper file that manages simple audio playback

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#include <string.h>

/*---------------------------------------------------------------------------*/

#include "helper_AudioDevice.h"
#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
#include "helper_Player.h"

#ifdef __CC_ARM
	#pragma diag_suppress 1301 // padding inserted in struct
#endif

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {
namespace audiodevice {

/*---------------------------------------------------------------------------*
  Constants definition.
 *---------------------------------------------------------------------------*/

const int MAX_PENDING_AUDIO_PACKET   = 64;
const int SOUND_THREAD_PRIORITY      = 2;
const int SOUND_THREAD_STACK_SIZE    = 4096;

/*---------------------------------------------------------------------------*
  Structures definition.
 *---------------------------------------------------------------------------*/

struct Sound
{
    nn::snd::Voice*      m_pVoice;
    nn::snd::WaveBuffer* m_pWaveBufferArray;
    nn::snd::MixParam    m_MixParam;
    u32                  m_NbWaveBuffer;
    s32                  m_SampleRate;
    s32                  m_ChannelCount;
    volatile u32         m_WaveBufferPushIndex;
    volatile u32         m_WaveBufferPopIndex;
    volatile u32         m_NbWaveBufferPushed;
    volatile u32         m_NbPendingWaveBufferToPush;
    volatile s64         m_TotalSamplePushed;
    volatile s64         m_TotalSamplePlayed;
    volatile s64         m_StartPlaybackTime;
};

struct AudioPacket
{
    Sound* m_AssociatedSound;
    void*  m_AudioBufferPtr;
    u32    m_AudioBufferSize;
};
   
/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

static nn::os::Thread       s_ThreadSound;
static volatile bool        s_ThreadSoundFlag;
static volatile bool        s_ThreadSleepFlag;

static gfl2::heap::NwAllocator* s_pAllocator = NULL;
nn::os::CriticalSection     s_FsCriticalSection;
static u32                  s_maxNbSound;
static Sound*               s_SoundArray = NULL;
static AudioPacket          s_PendingAudioPacket[MAX_PENDING_AUDIO_PACKET];
static volatile u32         s_PendingAudioPacketPushIndex;
static volatile u32         s_PendingAudioPacketPopIndex;
static bool					s_useThread;

static gfl2::thread::SimpleStackArea*  mpStack;

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

static void SoundCallbackFunc(uptr param)
{
    (void)param;

    s_FsCriticalSection.Enter();

	if (s_useThread == true)
		nn::snd::WaitForDspSync();

    for (int i=0; i<s_maxNbSound; i++)
    {
        if (s_SoundArray[i].m_pVoice != NULL && s_SoundArray[i].m_NbWaveBufferPushed >0)
        {
            if (s_SoundArray[i].m_pWaveBufferArray[s_SoundArray[i].m_WaveBufferPopIndex].status == nn::snd::WaveBuffer::STATUS_DONE)
            {
                s_SoundArray[i].m_TotalSamplePlayed += (s64)s_SoundArray[i].m_pWaveBufferArray[s_SoundArray[i].m_WaveBufferPopIndex].sampleLength;
                s_SoundArray[i].m_NbWaveBufferPushed--;

                s_SoundArray[i].m_WaveBufferPopIndex++;
                if (s_SoundArray[i].m_WaveBufferPopIndex == s_SoundArray[i].m_NbWaveBuffer)
                    s_SoundArray[i].m_WaveBufferPopIndex = 0;
            }
        }
    }

    while(s_PendingAudioPacketPushIndex > s_PendingAudioPacketPopIndex)
    {
        Sound *sound = s_PendingAudioPacket[s_PendingAudioPacketPopIndex % MAX_PENDING_AUDIO_PACKET].m_AssociatedSound;
        if (sound != NULL)
        {
            if (sound->m_NbWaveBufferPushed < sound->m_NbWaveBuffer)
            {
                void* pcmBuffer = s_PendingAudioPacket[s_PendingAudioPacketPopIndex % MAX_PENDING_AUDIO_PACKET].m_AudioBufferPtr;
                u32 bufferSize = s_PendingAudioPacket[s_PendingAudioPacketPopIndex % MAX_PENDING_AUDIO_PACKET].m_AudioBufferSize;

                nn::snd::InitializeWaveBuffer(&sound->m_pWaveBufferArray[sound->m_WaveBufferPushIndex]);
                sound->m_pWaveBufferArray[sound->m_WaveBufferPushIndex].bufferAddress = pcmBuffer;
                u32 nbSampleToPlay = nn::snd::GetSampleLength(bufferSize, nn::snd::SAMPLE_FORMAT_PCM16, sound->m_ChannelCount);
                sound->m_TotalSamplePushed += (s64)nbSampleToPlay;
                sound->m_pWaveBufferArray[sound->m_WaveBufferPushIndex].sampleLength  = nbSampleToPlay;
                sound->m_pWaveBufferArray[sound->m_WaveBufferPushIndex].loopFlag  = false;
                sound->m_pVoice->AppendWaveBuffer(&sound->m_pWaveBufferArray[sound->m_WaveBufferPushIndex]);

                sound->m_NbWaveBufferPushed++;
                sound->m_WaveBufferPushIndex++;
                if (sound->m_WaveBufferPushIndex == sound->m_NbWaveBuffer)
                    sound->m_WaveBufferPushIndex = 0;

                sound->m_NbPendingWaveBufferToPush--;
            }
            else 
                break;
        }
        s_PendingAudioPacketPopIndex++; 
    }
  
	if (s_useThread == true)
		nn::snd::SendParameterToDsp();

    s_FsCriticalSection.Leave();
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

static void SoundThreadFunc(void* param)
{
	s_ThreadSoundFlag = true;
  s_ThreadSleepFlag = false;
	while (s_ThreadSoundFlag)
	{
    if( !s_ThreadSleepFlag ) {
		  SoundCallbackFunc((uptr)param);
		  nn::snd::SendParameterToDsp();
    }
		nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMicroSeconds(500));
	}
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

bool Initialize (gfl2::heap::NwAllocator* pAllocator, u32 maxNbSound, bool useThread)
{
	s_useThread = useThread;

    if (s_pAllocator != NULL)
        return false;

    s_pAllocator = pAllocator;
    s_maxNbSound = maxNbSound;
    
    s_SoundArray = reinterpret_cast<Sound*>(s_pAllocator->Alloc(s_maxNbSound * sizeof(Sound), 4));
    if (s_SoundArray == NULL)
    {
        s_pAllocator = NULL;
        s_maxNbSound = 0;
        return false;
    }

    memset(s_SoundArray,0,s_maxNbSound * sizeof(Sound));

    s_PendingAudioPacketPushIndex = 0;
    s_PendingAudioPacketPopIndex = 0;

    s_FsCriticalSection.Initialize();


  if (s_useThread == true){
    {
      gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_AUTO_STACK );
      mpStack = GFL_NEW(systemHeap) gfl2::thread::SimpleStackArea(systemHeap, SOUND_THREAD_STACK_SIZE);
    }
    s_ThreadSound.Start(SoundThreadFunc, (void*)NULL, *mpStack, SOUND_THREAD_PRIORITY);
  }

  
//	if (s_useThread == true)
//		s_ThreadSound.StartUsingAutoStack(SoundThreadFunc, (void*)NULL, SOUND_THREAD_STACK_SIZE, SOUND_THREAD_PRIORITY);

    return true;
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

void Finalize(void)
{
    if (s_pAllocator == NULL)
        return;

    s_FsCriticalSection.Enter();

    for (int i=0; i<s_maxNbSound; i++)
    {
        if (s_SoundArray[i].m_pVoice != NULL)
            s_SoundArray[i].m_pVoice->SetState( nn::snd::Voice::STATE_STOP );
    }

    s_FsCriticalSection.Leave();

    s_ThreadSoundFlag = false;

	if (s_useThread == true)
	{
		s_ThreadSound.Join();
		s_ThreadSound.Finalize();
	}

    for (int i=0; i<s_maxNbSound; i++)
        FreeSound(i);

    s_FsCriticalSection.Finalize();

    s_pAllocator->Free(s_SoundArray);
    s_pAllocator = NULL;

  GFL_SAFE_DELETE(mpStack);
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

SoundCallback GetSoundCallback (void)
{
	if (s_useThread == true)
		return NULL;
	else
		return SoundCallbackFunc;
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

int AllocSound (s32 sampleRate, s32 channelCount, u32 nbWaveBuffer)
{
    for (int i=0; i<s_maxNbSound; i++)
    {
        if (s_SoundArray[i].m_pVoice == NULL)
        {
            s_SoundArray[i].m_NbWaveBuffer = nbWaveBuffer;
            s_SoundArray[i].m_pWaveBufferArray = reinterpret_cast<nn::snd::WaveBuffer*>(s_pAllocator->Alloc(nbWaveBuffer * sizeof(nn::snd::WaveBuffer), 4));    
            if (s_SoundArray[i].m_pWaveBufferArray == NULL)
                return NULL;

            s_FsCriticalSection.Enter();
            
            s_SoundArray[i].m_pVoice = nn::snd::AllocVoice(128, NULL, NULL);

            s_FsCriticalSection.Leave();

            if (s_SoundArray[i].m_pVoice == NULL)
            {
                s_pAllocator->Free(s_SoundArray[i].m_pWaveBufferArray);
                return NULL;
            }

            s_SoundArray[i].m_WaveBufferPushIndex = 0;
            s_SoundArray[i].m_WaveBufferPopIndex = 0;
            s_SoundArray[i].m_NbWaveBufferPushed = 0;
            s_SoundArray[i].m_NbPendingWaveBufferToPush = 0;
            s_SoundArray[i].m_TotalSamplePushed = 0;
            s_SoundArray[i].m_TotalSamplePlayed = 0;
            s_SoundArray[i].m_MixParam.mainBus[0] = 1.0f;
            s_SoundArray[i].m_MixParam.mainBus[1] = 1.0f;
            s_SoundArray[i].m_SampleRate = sampleRate;
            s_SoundArray[i].m_ChannelCount = channelCount;

            return i;
        }
    }

    return -1;
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

void FreeSound (int soundId)
{
    if (soundId < 0 ||  soundId >= s_maxNbSound)
        return;

    s_FsCriticalSection.Enter();

    Sound* sound = &s_SoundArray[soundId];

    if (sound->m_pVoice != NULL)
    {
        nn::snd::FreeVoice(sound->m_pVoice);
        sound->m_pVoice = NULL;
    }
    if (sound->m_pWaveBufferArray != NULL)
    {
        s_pAllocator->Free(sound->m_pWaveBufferArray);
        sound->m_pWaveBufferArray = NULL;
    }

    s_FsCriticalSection.Leave();
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

void PlaySound (int soundId)
{
    if (soundId < 0 ||  soundId >= s_maxNbSound)
        return;
    
    s_FsCriticalSection.Enter();

    Sound* sound = &s_SoundArray[soundId];

    sound->m_StartPlaybackTime = (s64)nn::os::Tick::GetSystemCurrent();

    sound->m_pVoice->SetChannelCount(sound->m_ChannelCount);
    sound->m_pVoice->SetSampleFormat(nn::snd::SAMPLE_FORMAT_PCM16);
    sound->m_pVoice->SetMixParam(sound->m_MixParam);
    sound->m_pVoice->SetSampleRate(sound->m_SampleRate);
    sound->m_pVoice->SetPitch(1.0f);
    sound->m_pVoice->SetVolume(1.0f);
    sound->m_pVoice->SetState(nn::snd::Voice::STATE_PLAY);


    s_ThreadSleepFlag = false;

    s_FsCriticalSection.Leave();
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

void StopSound (int soundId)
{
    if (soundId < 0 ||  soundId >= s_maxNbSound)
        return;

    s_FsCriticalSection.Enter();

    Sound* sound = &s_SoundArray[soundId];

    sound->m_pVoice->SetState(nn::snd::Voice::STATE_STOP);

    u32 audioPacketPendingIndex = s_PendingAudioPacketPopIndex;
    while(s_PendingAudioPacketPushIndex > audioPacketPendingIndex)
    {
        if (s_PendingAudioPacket[audioPacketPendingIndex % MAX_PENDING_AUDIO_PACKET].m_AssociatedSound == sound)
            s_PendingAudioPacket[audioPacketPendingIndex % MAX_PENDING_AUDIO_PACKET].m_AssociatedSound = NULL;

        audioPacketPendingIndex++;
    }

    sound->m_WaveBufferPushIndex = 0;
    sound->m_WaveBufferPopIndex = 0;
    sound->m_NbWaveBufferPushed = 0;
    sound->m_NbPendingWaveBufferToPush = 0;
    sound->m_TotalSamplePushed = 0;
    sound->m_TotalSamplePlayed = 0;

    s_ThreadSleepFlag = true;

    s_FsCriticalSection.Leave();
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

void PauseSound (int soundId)
{
    if (soundId < 0 ||  soundId >= s_maxNbSound)
        return;

    s_FsCriticalSection.Enter();

    Sound* sound = &s_SoundArray[soundId];

    sound->m_pVoice->SetState(nn::snd::Voice::STATE_PAUSE);

    s_ThreadSleepFlag = true;

    s_FsCriticalSection.Leave();
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

void ResumeSound (int soundId)
{
    if (soundId < 0 ||  soundId >= s_maxNbSound)
        return;

    s_FsCriticalSection.Enter();

    Sound* sound = &s_SoundArray[soundId];

    sound->m_pVoice->SetState(nn::snd::Voice::STATE_PLAY);

    s_ThreadSleepFlag = false;

    s_FsCriticalSection.Leave();
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

bool PushAudioPacket (int soundId, void* pcmBuffer, u32 bufferSize)
{
    if (soundId < 0 ||  soundId >= s_maxNbSound)
	{
		NN_ASSERT(false);
        return false;
	}

    Sound* sound = &s_SoundArray[soundId];

    nn::os::CriticalSection::ScopedLock csLock(s_FsCriticalSection);

    if (sound->m_NbPendingWaveBufferToPush == sound->m_NbWaveBuffer)
	{
		NN_ASSERT(false);
		return false;
	}

    if ((s_PendingAudioPacketPushIndex - s_PendingAudioPacketPopIndex) == MAX_PENDING_AUDIO_PACKET)
	{
		NN_ASSERT(false);
		return false;
	}
    
    if (pcmBuffer != NULL && bufferSize > 0)
    {
        nn::snd::FlushDataCache(reinterpret_cast<uptr>(pcmBuffer), bufferSize);

        s_PendingAudioPacket[s_PendingAudioPacketPushIndex % MAX_PENDING_AUDIO_PACKET].m_AssociatedSound = sound;
        s_PendingAudioPacket[s_PendingAudioPacketPushIndex % MAX_PENDING_AUDIO_PACKET].m_AudioBufferPtr = pcmBuffer;
        s_PendingAudioPacket[s_PendingAudioPacketPushIndex % MAX_PENDING_AUDIO_PACKET].m_AudioBufferSize = bufferSize;

        s_PendingAudioPacketPushIndex++;

        sound->m_NbPendingWaveBufferToPush++;
    }

    return true;
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

s64 GetTotalSamplePushed (int soundId)
{
    if (soundId < 0 ||  soundId >= s_maxNbSound)
        return 0;

    Sound* sound = &s_SoundArray[soundId];

    return sound->m_TotalSamplePushed;
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

s64 GetTotalSamplePlayed (int soundId)
{
    if (soundId < 0 ||  soundId >= s_maxNbSound)
        return 0;

    Sound* sound = &s_SoundArray[soundId];

    return sound->m_TotalSamplePlayed;
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

s32 GetSampleRate (int soundId)
{
    if (soundId < 0 ||  soundId >= s_maxNbSound)
        return 0;

    Sound* sound = &s_SoundArray[soundId];

    return sound->m_SampleRate;
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

s32 GetChannelCount (int soundId)
{
    if (soundId < 0 ||  soundId >= s_maxNbSound)
        return 0;

    Sound* sound = &s_SoundArray[soundId];

    return sound->m_ChannelCount;
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

u32 GetMaxAudioPackets (int soundId)
{
    if (soundId < 0 ||  soundId >= s_maxNbSound)
        return 0;

    Sound* sound = &s_SoundArray[soundId];

    return sound->m_NbWaveBuffer;
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

u32 GetPendingAudioPackets (int soundId)
{
    if (soundId < 0 ||  soundId >= s_maxNbSound)
        return 0;

    Sound* sound = &s_SoundArray[soundId];

    return sound->m_NbPendingWaveBufferToPush;
}

/*---------------------------------------------------------------------------*
 
 *---------------------------------------------------------------------------*/

} // end of namespace audiodevice
} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/
#endif// defined(GF_PLATFORM_CTR)
