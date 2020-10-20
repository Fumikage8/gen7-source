//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_SoundDataManagerThread.h
 *	@brief  サウンドデータマネージャ　データ読み込み　スレッド
 *	@author	tomoya takahashi
 *	@date		2012.12.11
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __GFL_SOUNDDATAMANAGERTHREAD_H__
#define __GFL_SOUNDDATAMANAGERTHREAD_H__
#pragma once

#include <nw.h>

namespace gfl {
  namespace snd {

    class SoundSystem;

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

    class SoundDataManagerThread : public base::Thread
    {
      GFL_FORBID_COPY_AND_ASSIGN( SoundDataManagerThread );

      public:

      struct Description 
      {
        SoundSystem * m_pSoundSystem; // [in] SoundSystem
        SoundHeap * m_pDest;    // [out]    メモリ読み込み格納先
        bool *      m_pResult;  // [out]    結果格納先
        u32         m_itemID;   // [in]     アイテムID
        u32         m_loadFlag; // [in]     ロードフラグ
        Description( void ) : 
          m_pDest(NULL),
          m_itemID(0),
          m_loadFlag(false)
        { }
      };

      /**
       * @brief コンストラクタ
       * @param heap_for_thread スレッドの動作に使用するヒープ
       * @param desc            アーカイブ読み込みの詳細情報
       */
      SoundDataManagerThread( 
          gfl::heap::HeapBase* heap_for_thread,
          const Description& desc );

      /**
       * @brief デストラクタ
       */
      virtual ~SoundDataManagerThread();


      private:

      static const u32 STACK_SIZE;
      Description m_desc;

      //virtual void Initialize( void );
      virtual void Main( void ); 
      //virtual void Finalize( void );
      //virtual void OnKill( void );

    };


  } //namespace snd
} //namespace gfl

#endif	// __GFL_SOUNDDATAMANAGERTHREAD_H__

