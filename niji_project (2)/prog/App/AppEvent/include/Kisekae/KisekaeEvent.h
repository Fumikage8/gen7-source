// ============================================================================
/*
 * @file KisekaeEvent.h
 * @brief きせかえアプリのイベント
 * @date 2015.10.24
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( APP_KISEKASE_EVENT_H_INCLUDE )
#define APP_KISEKASE_EVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

//  nijiのインクルード
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"

#include "App/Kisekae/include/KisekaeAppParam.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)

  class Proc;

GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

class KisekaeEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( KisekaeEvent );

public:
  static void StartEvent( GameSys::GameManager* pGameManager, App::Kisekae::APP_PARAM* pParam );

  KisekaeEvent( gfl2::heap::HeapBase* pHeap );
  virtual~KisekaeEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

private:

  void _Clear()
  {
    m_pProc       = NULL;
    m_pAppParam   = NULL;
  }

  App::Kisekae::Proc*                             m_pProc;
  App::Kisekae::APP_PARAM*                        m_pAppParam;
};

GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif // APP_KISEKASE_EVENT_H_INCLUDE
