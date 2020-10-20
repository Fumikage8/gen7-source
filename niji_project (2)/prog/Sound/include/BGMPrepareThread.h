//=================================================================================================
/**
 * @file BGMPrepareThread.h
 * @brief BGMの再生準備を行うスレッド
 * @author obata_toshihiro
 * @date 2011.06.30
 */
//=================================================================================================
#ifndef __BGMPREPARETHREAD_H__
#define __BGMPREPARETHREAD_H__

#pragma once

#include <sound/include/gfl2_SoundSystem.h>
#include <sound/include/gfl2_SoundHeap.h>
#include <sound/include/gfl2_SoundDataManagerThread.h>

namespace Sound {

  class PMSoundSystem;


  class BGMPrepareThread : public gfl2::thread::ThreadInterface
  {
    GFL_FORBID_COPY_AND_ASSIGN( BGMPrepareThread );

    public:

    BGMPrepareThread( 
        gfl2::heap::HeapBase* heap_for_stack,
        Sound::PMSoundSystem* sound_system, u32 sound_item_id );

    ~BGMPrepareThread();


    private:

    static const u32 STACK_SIZE;

    Sound::PMSoundSystem* m_sound_system;
    u32 m_sound_item_id;
    u32 m_seq;
    gfl2::thread::Thread* m_pThread;

    //実行関数
    virtual void Function(void);
  };


} // namespace Sound

#endif // __BGMPREPARETHREAD_H__
