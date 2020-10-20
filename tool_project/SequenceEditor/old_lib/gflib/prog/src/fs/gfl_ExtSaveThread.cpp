//=============================================================================
/**
 * @brief  外部セーブデータファイルの読み書きを行うスレッド
 * @file   gfl_ExtSaveThread.cpp
 * @author obata_toshihiro
 * @author  tamada
 * @date    2012.09.15
 *
 * @note    スレッドの生成・削除を繰り返さないように、スリープさせるようにしたい。
 */
//=============================================================================
#include <nw.h>
#include <gfl_Heap.h>
#include <base/gfl_Thread.h>
#include <fs/gfl_ExtSaveSystem.h>
#include "gfl_ExtSaveThread.h" 

namespace gfl {
  namespace fs {



    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap           スレッドのスタック領域に使用するヒープ
     * @param stack_size     スレッドのスタック領域のサイズ
     * @param extsave_system  外部セーブデータシステム
     * @param mode           動作モード
     *
     * @note インスタンス生成後, 自動的にスレッドを起動する.
     */
    //-------------------------------------------------------------------------
    ExtSaveThread::ExtSaveThread(
        gfl::heap::HeapBase* heap,
        u32 stack_size,
        ExtSaveSystem* extsave_system ):
      Thread( heap, stack_size ),
      m_extsave_system( extsave_system )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    ExtSaveThread::~ExtSaveThread()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief スレッドメイン処理
     * @note 読み込み( 書き込み ) >> 自殺
     */
    //-------------------------------------------------------------------------
    void ExtSaveThread::Main( void )
    {
      m_extsave_system->ThreadCallBack();
      this->Kill();
    }



  } // namespace fs
} // namespace gfl
