//===================================================================
/**
 * @file    GameEvent.cpp
 * @brief   イベントの基底クラス
 * @author  k.ohno
 * @date    2011.3.6   Syachiから移植
*/
//===================================================================

#include "GameSys/include/GameEvent.h"

namespace GameSys {

//--------------------------------------------------------------
/**
 * @brief   コンストラクタ
 * @param   heap       workをとるためのメモリクラス
 */
//--------------------------------------------------------------
#ifdef RO_VALID
  GameEvent::GameEvent(gfl2::heap::HeapBase* heap) :
    mParent(NULL),mSeq(0),mWorkHeap(heap),mState(STATE_BOOT_CHECK),mpModule(NULL)
  {
  }
#else
  GameEvent::GameEvent(gfl2::heap::HeapBase* heap) :
    mParent(NULL),mSeq(0),mWorkHeap(heap),mState(STATE_BOOT_CHECK)
  {
  }
#endif
  //デストラクタ
  GameEvent::~GameEvent()
  {
  }

//----------------------------------------------------------------------------
/**
 *	@brief  イベント更新
 */
//-----------------------------------------------------------------------------
  GMEVENT_RESULT GameEvent::UpdateFunc( GameManager* gmgr )
{
  switch( mState ){
  case STATE_BOOT_CHECK: 
    // BootCheck trueで起動。
    if( this->BootChk(gmgr ) == false ){  
      break;
    }
    
    this->InitFunc( gmgr );
    mState = STATE_MAIN;

    // break through ↓

  case STATE_MAIN: 
    {
      GMEVENT_RESULT result;

      result = this->MainFunc( gmgr );

      // 終了
      if( result == GMEVENT_RES_FINISH ){
        this->EndFunc( gmgr );
        mState = STATE_END;
        return GMEVENT_RES_FINISH;
      }
      
      return result;
    }

  case STATE_END: 
    return GMEVENT_RES_FINISH;

  default:
    GFL_ASSERT(0);
    break;
  }

  return GMEVENT_RES_CONTINUE;
  }

//----------------------------------------------------------------------------
/**
 *	@brief  イベント破棄をコール
 */
//-----------------------------------------------------------------------------
  void GameEvent::CallEnd( GameManager* gmgr )
  {
    if( mState == STATE_MAIN ){
      this->EndFunc( gmgr );
      mState = STATE_END;
    }
  }

} //namespace GameSys
