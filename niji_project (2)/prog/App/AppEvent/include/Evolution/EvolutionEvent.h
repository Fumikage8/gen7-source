#if !defined(EVOLUTIONEVENT_H_INCLUDED)
#define EVOLUTIONEVENT_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionEvent.h
 * @date   2015/08/24 Mon. 14:42:53
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>

#include  <pml/include/pmlib.h>

#include  "GameSys/include/GameEvent.h"
#include  "GameSys/include/GameEventManager.h"
#include  "GameSys/include/GameManager.h"
#include  "App/Evolution/include/EvolutionProcParam.h"


// =============================================================================
/**/
//==============================================================================
namespace App       {
namespace Evolution {
  class EvolutionProc;
}
}



namespace App   {
namespace Event {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/08/24 Mon. 14:49:03
  */
//==============================================================================
class EvolutionEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( EvolutionEvent );

public:
  //------------------------------------------------------------------
  /**
    *  @func     StartEvent
    *  @brief    EvolutionEventの開始
    */
  //------------------------------------------------------------------
  static void StartEvent(GameSys::GameManager* pGameManager);

  //------------------------------------------------------------------
  /**
    *  @func     EvolutionEvent
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  EvolutionEvent( gfl2::heap::HeapBase* pHeap );

  //------------------------------------------------------------------
  /**
    *  @func     ~EvolutionEvent
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~EvolutionEvent();


  /*
      GameEvent I/F
  */
  //------------------------------------------------------------------
  /**
    *  @func    BootChk
    *  @brief   起動判定
    */
  //------------------------------------------------------------------
  virtual bool  BootChk( GameSys::GameManager* pGameManager );

  //------------------------------------------------------------------
  /**
    *  @func    InitFunc
    *  @brief   初期化
    */
  //------------------------------------------------------------------
  virtual void  InitFunc( GameSys::GameManager* pGameManager );

  //------------------------------------------------------------------
  /**
    *  @func    EndFunc
    *  @brief   終了
    */
  //------------------------------------------------------------------
  virtual void  EndFunc( GameSys::GameManager* pGameManager );

  //------------------------------------------------------------------
  /**
    *  @func    MainFunc
    *  @brief   更新
    */
  //------------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  /*
      End GameEvent I/F
  */

private:
  /*
      members
  */
  App::Evolution::EvolutionProcParam  m_Param;     /**<  procパラメータ    */
  App::Evolution::EvolutionProc*      m_pProc;
  u32                                 m_Sequence;

};  /*  class EvolutionEvent  */



} /*  namespace Event */
} /*  namespace App   */
#endif  /*  #if !defined(EVOLUTIONEVENT_H_INCLUDED)  */
