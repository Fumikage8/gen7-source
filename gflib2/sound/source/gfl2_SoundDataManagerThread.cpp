//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl2_SoundDataManagerThread.cpp
 *	@brief  サウンドデータマネージャ　データ読み込み　スレッド
 *	@author Miyuki Iwasawa
 *	@date		2015.03.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <nw/snd.h>

#include "sound/include/gfl2_SoundSystem.h"
#include "sound/include/gfl2_SoundDataManagerThread.h"

namespace gfl2 {
  namespace sound {

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap_for_thread スレッドの動作に使用するヒープ
     * @param desc            アーカイブ読み込みの詳細情報
     */
    //-------------------------------------------------------------------------
    SoundDataManagerThread::SoundDataManagerThread( 
        gfl2::heap::HeapBase* heap_for_thread,
        const Description& desc ) :
      m_desc( desc ),
      m_pThread(NULL)
    {
      m_pThread = GFL_NEW(heap_for_thread) gfl2::thread::Thread(this, heap_for_thread, false, STACK_SIZE);
//      m_pThread->SetPriority(gfl2::thread::Thread::THREAD_PRI_HI);
      m_isFinished = false;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    SoundDataManagerThread::~SoundDataManagerThread()
    {
      GFL_SAFE_DELETE(m_pThread);
    }

    /**
     * @brief メイン処理
     * @note gfl::base::Thread をオーバーライド
     */
    void SoundDataManagerThread::Function( void )
    {
      *m_desc.m_pResult = m_desc.m_pSoundSystem->LoadSoundData( m_desc.m_pDest, m_desc.m_itemID, m_desc.m_loadFlag);
      // 自身のスレッドをKILL
      m_pThread->Kill();
      m_isFinished = true;
    }

    /**
     * @brief スレッドスタートリクエスト
     */
    void SoundDataManagerThread::Start( s32 priority )
    {
      m_pThread->Start(priority);
    }

    /**
     * @brief スレッドが生きているか調べる
     */
    bool SoundDataManagerThread::IsAlive( void )
    {
      if( m_pThread == NULL || m_isFinished ) return false;
      return true;  //m_pThread->IsAlive(); Killが呼ばれてもIsAlive=trueのままなので、フラグで管理に切り替え　151216
    }
  } //namespace snd
} //namespace gfl2



