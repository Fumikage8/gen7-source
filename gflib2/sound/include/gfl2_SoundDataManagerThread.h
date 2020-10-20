//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl2_SoundDataManagerThread.h
 *	@brief  サウンドデータマネージャ　データ読み込み　スレッド
 *	@author	Miyuki Iwasawa
 *	@date		2015.03.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __GFL_SOUNDDATAMANAGERTHREAD_H__
#define __GFL_SOUNDDATAMANAGERTHREAD_H__
#pragma once

#include <nw/snd.h>


#include "macro/include/gfl2_macros.h"
#include "types/include/gfl2_Typedef.h"
#include "heap/include/gfl2_Heap.h"
#include "thread/include/gfl2_Thread.h"


namespace gfl2 {
  namespace sound {

    class SoundSystem;
    class SoundHeap;

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

    class SoundDataManagerThread : public gfl2::thread::ThreadInterface
    {
      GFL_FORBID_COPY_AND_ASSIGN( SoundDataManagerThread );

      public:

      struct Description 
      {
        SoundSystem * m_pSoundSystem; // [in] SoundSystem
        SoundHeap * m_pDest;    // [out]    メモリ読み込み格納先
        bool *      m_pResult;  // [out]    結果格納先
        u32         m_itemID;   // [in]     アイテムID
        SoundSystem::LoadFlags   m_loadFlag; // [in]     ロードフラグ
        Description( void ) : 
          m_pDest(NULL),
          m_itemID(0),
          m_loadFlag(SoundSystem::LOAD_ALL)
        { }
      };

      /**
       * @brief コンストラクタ
       * @param heap_for_thread スレッドの動作に使用するヒープ
       * @param desc            アーカイブ読み込みの詳細情報
       */
      SoundDataManagerThread( 
          gfl2::heap::HeapBase* heap_for_thread,
          const Description& desc );

      /**
       * @brief デストラクタ
       */
      virtual ~SoundDataManagerThread();

      //実行関数
      virtual void Function(void);

      //スタート関数
      void Start(s32 priority);

      //生存チェック関数
      bool IsAlive(void);

    private:

      Description m_desc;

      gfl2::thread::Thread* m_pThread;
      static const u32 STACK_SIZE = 4096;
      bool m_isFinished;
    };


  } //namespace sound
} //namespace gfl2

#endif	// __GFL_SOUNDDATAMANAGERTHREAD_H__

