#if !defined(EGGHATCHINGEVENT_H_INCLUDED)
#define EGGHATCHINGEVENT_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EggHatchingEvent.h
 * @date   2015/11/24 Tue. 17:29:10
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
#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameEvent.h"
#include  "NetApp/EggHatching/include/EggHatchingProcParam.h"

#include  "NetStatic/NetAppLib/include/Util/NetAppClearSettingUtility.h"

/*  save  */
#include  "Savedata/include/MyItemSave.h"

/*  zukan  */
#include  "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"

/*  wazaoshie  */
#include  "App/WazaOshie/include/WazaOshieAppParam.h"
#include  "App/WazaOshie/include/WazaOshieProc.h"

/*
  forward declare
*/
namespace NetAppLib  {
namespace System {
  class ApplicationSystemBase;
}
}
namespace NetApp {
namespace EggHatching {
  class EggHatchingProc;
}
}

namespace NetEvent  {
namespace EggHatching {
// =============================================================================
/**/
//==============================================================================
/**
  孵化Event起動パラメータ
  ポインタ参照先はEvent実行中は保持されること
*/
typedef struct  EggHatchingEventParam
{
  /*  進化関連  */
  pml::pokepara::PokemonParam*    m_pTargetEgg;     /**<  [in/out]  対象となる卵のパラメータ（直接Birth()される）  */
  bool                            m_inResort;       /**<  [in]  リゾートでの孵化ならtrue  */


  /*    */
  EggHatchingEventParam(void)
    : m_pTargetEgg(NULL),
      m_inResort(false)
  {}
}EggHatchingEventParam;



//==============================================================================
/**
  *  @class  EggHatchingEvent
  */
//==============================================================================
/*
  2016/01/05
  通信エラーハンドリングが不要のため、指示によりBaseClassをNetEventBaseからGameEventに変更
*/
class EggHatchingEvent
  : public    GameSys::GameEvent
  , protected NetAppLib::Util::NetAppClearSettingUtility
{
  GFL_FORBID_COPY_AND_ASSIGN( EggHatchingEvent );

public:
  /*  イベント起動  */
  static void StartEvent(GameSys::GameManager* pGameManager, EggHatchingEventParam& rEventParam);

  EggHatchingEvent( gfl2::heap::HeapBase* pHeap );
  virtual ~EggHatchingEvent();

  /*
      GameEvent I/F
  */
  virtual bool  BootChk( GameSys::GameManager* pGameManager );
  virtual void  InitFunc( GameSys::GameManager* pGameManager );
  virtual void  EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );
  /*
      End GameEvent I/F
  */

private:
  /*
    funcs
  */
  void  ResultHandler(GameSys::GameManager* pGameManager);


  /*
      members
  */
  EggHatchingEventParam                   m_EventParam;
  NetApp::EggHatching::APP_PARAM          m_ProcParam;
  NetApp::EggHatching::EggHatchingProc*   m_pProc;

  /*  図鑑  */
  app::event::ZukanRegisterEventParam     m_ZukanParam;
  PokeTool::PokeGetUtil::ReturnType::Tag  m_zukanCheckerReturnType;
};  /*  class EggHatchingEvent  */


} /*  namespace EggHatching  */
} /*  namespace NetApp       */
#endif  /*  #if !defined(EGGHATCHINGEVENT_H_INCLUDED)  */


