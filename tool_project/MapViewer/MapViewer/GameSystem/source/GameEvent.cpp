//===================================================================
/**
 * @file    GameEvent.cpp
 * @brief   �C�x���g�̊��N���X
 * @author  k.ohno
 * @date    2011.3.6   Syachi����ڐA
*/
//===================================================================

#include "GameSystem/include/GameEvent.h"

namespace GameSystem {

//--------------------------------------------------------------
/**
 * @brief   �R���X�g���N�^
 * @param   heap       work���Ƃ邽�߂̃������N���X
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
  //�f�X�g���N�^
  GameEvent::~GameEvent()
  {
  }

//----------------------------------------------------------------------------
/**
 *	@brief  �C�x���g�X�V
 */
//-----------------------------------------------------------------------------
  GMEVENT_RESULT GameEvent::UpdateFunc( GameManager* gmgr )
  {
  switch( mState ){
  case STATE_BOOT_CHECK: 
    // BootCheck true�ŋN���B
    if( this->BootChk(gmgr ) == false ){  
      break;
    }
    
    this->InitFunc( gmgr );
    mState = STATE_MAIN;

    // break through ��

  case STATE_MAIN: 
    {
      GMEVENT_RESULT result;

      result = this->MainFunc( gmgr );

      // �I��
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
 *	@brief  �C�x���g�j�����R�[��
 */
//-----------------------------------------------------------------------------
  void GameEvent::CallEnd( GameManager* gmgr )
  {
    if( mState == STATE_MAIN ){
      this->EndFunc( gmgr );
      mState = STATE_END;
    }
  }

} //namespace GameSystem
