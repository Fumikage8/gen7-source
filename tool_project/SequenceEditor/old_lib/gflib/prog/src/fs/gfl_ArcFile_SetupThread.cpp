//========================================================================================
/**
 * @brief  ArcFileインスタンスを高速モードへ移行させるスレッド
 * @file   gfl_ArcFile_SetupThreadForFastMode.cpp
 * @author obata_toshihiro
 * @date   2011.05.10
 */
//========================================================================================
#include <gfl_Heap.h>
#include <base/gfl_Thread.h>
#include <fs/gfl_ArcFile.h>
#include "gfl_ArcFile_SetupThread.h"

namespace gfl {
  namespace fs { 


    //------------------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param arcfile                 高速モードへ移行させるインスタンス
     * @param heap_for_arcfile_table  高速アクセステーブルを確保するヒープ
     * @param heap_for_stack_area     スレッドのスタック領域を確保するヒープ
     * @param stack_size              スレッドのスタック領域のサイズ
     */
    //------------------------------------------------------------------------------------
    SetupThread::SetupThread(
        ArcFile* arcfile,
        gfl::heap::HeapBase* heap_for_arcfile_table,
        gfl::heap::HeapBase* heap_for_stack_area,
        u32 stack_size ) :
      gfl::base::Thread( heap_for_stack_area, stack_size ),
      m_heapForFastAccessTable( heap_for_arcfile_table ),
      m_arcfile( arcfile )
    {
    } 

    //------------------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //------------------------------------------------------------------------------------
    SetupThread::~SetupThread()
    {
    }

    /**
     * @brief スレッドメイン処理
     * @note 高速モードへの移行が完了したら自殺する
     * @note gfl::base::Thread::Main()をオーバーライド
     *
     * SBTS[1038]:排他制御を追加実装 by takahashi & iwasawa
     *
     * ArcFile::StartSetupForFastMode()でこのスレッドを起動した後、
     * WaitSetupForFastMode()で明示的にスレッド破棄を待ってから
     * AddReadRequest()等のファイルアクセススレッドを起動しないと
     * このスレッドが排他制御を行っていなかったために
     * ファイルへの同時アクセスが発生し不具合を引き起こしていた。
     *
     * SBTS[1038]の事例ではアーカイブファイルのロード領域が破壊され
     * NULL参照やヘッダ領域破壊によるアサートを誘発していた。
     */
    void SetupThread::Main( void )
    {
      // ファイルを開く
      if( m_arcfile->IsOpen() == false ) {
        m_arcfile->Open();
      }
      // ファイルオープンを待つ
      nn::os::LightEvent* open_event = m_arcfile->GetOpenEvent();
      if( !open_event->IsSignaled() ) {
        open_event->Wait();
      }

      // 高速モードへ移行
      if( m_arcfile->IsFastMode() == false ) {
        m_arcfile->SetupForFastMode( m_heapForFastAccessTable );
      }

      // 高速モードへの移行を待つ
      nn::os::LightEvent* fast_event = m_arcfile->GetFastSetupEvent();
      if( !fast_event->IsSignaled() ) {
        fast_event->Wait();
      }

      this->Kill();
    }


  } // namespace fs
} // namespace gfl
