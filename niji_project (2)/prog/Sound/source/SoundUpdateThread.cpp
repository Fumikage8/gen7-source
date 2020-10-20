
//=================================================================================================
/**
 * @file SoundUpdateThread.cpp
 * @brief サウンドシステムのアップデートを行うスレッド
 * @author kousaka_shin
 * @date 2015.07.05
 */
//=================================================================================================

#include "Sound/include/SoundUpdateThread.h"

#if SOUND_UPDATE_THREAD_ENABLE
#include <thread/include/gfl2_ThreadStatic.h>
#include <macro/include/gfl2_Macros.h>


namespace Sound {


  const u32 SoundUpdateThread::STACK_SIZE = 4096;


  //-----------------------------------------------------------------------------------------------
  // コンストラクタ
  //-----------------------------------------------------------------------------------------------
  SoundUpdateThread::SoundUpdateThread( gfl2::heap::HeapBase* heap )
    : m_pThread(NULL)
    , m_pEvent(NULL)
  {
    m_pEvent = GFL_NEW(heap) gfl2::thread::Event(heap);
    m_pEvent->Initialize();
    m_pThread = GFL_NEW(heap) gfl2::thread::Thread(this, heap, true, STACK_SIZE);
    m_pThread->Start(gfl2::thread::Thread::THREAD_PRI_NORMAL-1);
    
  }


  //-----------------------------------------------------------------------------------------------
  // デストラクタ
  //-----------------------------------------------------------------------------------------------
  SoundUpdateThread::~SoundUpdateThread()
  {
    m_pThread->Kill();
    m_pEvent->Signal();
    m_pThread->Wait();
    GFL_SAFE_DELETE(m_pThread);
    m_pEvent->Finalize();
    GFL_SAFE_DELETE(m_pEvent);
  }



  //-----------------------------------------------------------------------------------------------
  // メインループとの同期
  //-----------------------------------------------------------------------------------------------
  void SoundUpdateThread::Signal(void)
  {
#if defined(GF_PLATFORM_CTR)
    //なぜかひっかかるようなので一旦コメントアウト　GFL_ASSERT( m_pEvent->TryWait() == false );//メインスレッドがサウンドスレッドより早く回ることは無いはず
#endif
    m_pEvent->Signal();
    
  }


  //-----------------------------------------------------------------------------------------------
  // メイン処理
  //-----------------------------------------------------------------------------------------------
  void SoundUpdateThread::Function( void )
  {
    m_pEvent->Wait();
    m_pEvent->Clear();

    Sound::Update();

    gfl2::thread::ThreadStatic::CurrentSleep( SLEEP_MSEC );

    Sound::Update();
  }



} // namespace Sound
#endif