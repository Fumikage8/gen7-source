//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_SoundDataManagerThread.cpp
 *	@brief  サウンドデータマネージャ　データ読み込み　スレッド
 *	@author	tomoya takahashi
 *	@date		2012.12.11
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <nw.h>
#include <snd/gfl_SoundSystem.h>
#include "gfl_SoundDataManagerThread.h"

namespace gfl {
  namespace snd {

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

    // スレッドのスタックサイズ
    const u32 SoundDataManagerThread::STACK_SIZE = 4096;


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap_for_thread スレッドの動作に使用するヒープ
     * @param desc            アーカイブ読み込みの詳細情報
     */
    //-------------------------------------------------------------------------
    SoundDataManagerThread::SoundDataManagerThread( 
        gfl::heap::HeapBase* heap_for_thread,
        const Description& desc ) :
      base::Thread( heap_for_thread, STACK_SIZE ),
      m_desc( desc )
    {
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    SoundDataManagerThread::~SoundDataManagerThread()
    {
    }



    /**
     * @brief メイン処理
     * @note gfl::base::Thread をオーバーライド
     */
    void SoundDataManagerThread::Main( void )
    {
      *m_desc.m_pResult = m_desc.m_pSoundSystem->LoadSoundData( m_desc.m_pDest, m_desc.m_itemID, m_desc.m_loadFlag);
      // 自身のスレッドをKILL
      this->Kill();
    }


  } //namespace snd
} //namespace gfl


