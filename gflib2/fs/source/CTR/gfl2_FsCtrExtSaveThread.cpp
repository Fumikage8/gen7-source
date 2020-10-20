#if   defined(GF_PLATFORM_CTR)
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
#include <fs/include/CTR/gfl2_FsCtrExtSaveSystem.h>
#include <fs/source/CTR/gfl2_FsCtrExtSaveThread.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 {
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
        gfl2::heap::HeapBase* heap,
        u32 stack_size,
        ExtSaveSystem* extsave_system ):
      m_extsave_system( extsave_system ),
      m_pThread( NULL )
    {
      m_pThread = GFL_NEW(heap) gfl2::thread::Thread(this, heap, true , stack_size );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    ExtSaveThread::~ExtSaveThread()
    {
      GFL_SAFE_DELETE( m_pThread );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief スレッドメイン処理
     * @note 読み込み( 書き込み ) >> 自殺
     */
    //-------------------------------------------------------------------------
    void ExtSaveThread::Function( void )
    {
      m_extsave_system->ThreadCallBack();
      this->Kill();
    }



  } // namespace fs
} // namespace gfl
#endif