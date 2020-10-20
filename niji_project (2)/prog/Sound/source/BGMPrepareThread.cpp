//=================================================================================================
/**
 * @file BGMPrepareThread.cpp
 * @brief BGMの再生準備を行うスレッド
 * @author obata_toshihiro
 * @date 2011.06.30
 */
//=================================================================================================

#include <sound/include/gfl2_SoundSystem.h>
#include <sound/include/gfl2_SoundHeap.h>
#include <sound/include/gfl2_SoundDataManagerThread.h>

#include "Sound/include/PMSoundSystem.h"
#include "Sound/include/BGMPrepareThread.h"

namespace Sound {


  const u32 BGMPrepareThread::STACK_SIZE = 4096;


  //-----------------------------------------------------------------------------------------------
  // コンストラクタ
  //-----------------------------------------------------------------------------------------------
  BGMPrepareThread::BGMPrepareThread(
      gfl2::heap::HeapBase* heap_for_stack,
      Sound::PMSoundSystem* sound_system, u32 sound_item_id ) : 
    m_sound_system( sound_system ),
    m_sound_item_id( sound_item_id ),
    m_seq( 0 )
  {
    m_pThread = GFL_NEW(heap_for_stack) gfl2::thread::Thread(this, heap_for_stack, false, STACK_SIZE);
    m_pThread->SetPriority(gfl2::thread::Thread::THREAD_PRI_HI);
  }


  //-----------------------------------------------------------------------------------------------
  // デストラクタ
  //-----------------------------------------------------------------------------------------------
  BGMPrepareThread::~BGMPrepareThread()
  {
  }




  //-----------------------------------------------------------------------------------------------
  // メイン処理
  //-----------------------------------------------------------------------------------------------
  void BGMPrepareThread::Function( void )
  {
    switch( m_seq ) {
    case 0:
      m_sound_system->PrepareBGM( m_sound_item_id );
      ++m_seq;
      break;

    case 1:
      if( m_sound_system->IsBGMPrepared() ) {
        m_pThread->Kill();  // 自殺
      }
      break;
    }
  }



} // namespace Sound
