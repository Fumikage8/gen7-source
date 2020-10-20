
//=================================================================================================
/**
 * @file SoundUpdateThread.h
 * @brief サウンドシステムのアップデートを行うスレッド
 * @author kousaka_shin
 * @date 2015.07.05
 */
//=================================================================================================
#ifndef __SOUNDUPDATETHREAD_H__
#define __SOUNDUPDATETHREAD_H__
#pragma once
#include "Sound/include/Sound.h"

#if SOUND_UPDATE_THREAD_ENABLE

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_heap.h>
#include <thread/include/gfl2_Thread.h>

namespace Sound {

  class PMSoundSystem;


  class SoundUpdateThread : public gfl2::thread::ThreadInterface
  {
    GFL_FORBID_COPY_AND_ASSIGN( SoundUpdateThread );

  public:

    SoundUpdateThread( gfl2::heap::HeapBase* heap );

    ~SoundUpdateThread();

  public:

    void Signal(void);

  private:
    enum
    {
      SLEEP_MSEC = 1000 / 60,//60fps
    };

  private:

    static const u32 STACK_SIZE;
    gfl2::thread::Thread* m_pThread;
    gfl2::thread::Event* m_pEvent;

    //実行関数
    virtual void Function(void);
  };


} // namespace Sound

#endif // __SOUNDUPDATETHREAD_H__
#endif