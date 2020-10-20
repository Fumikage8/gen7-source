//=================================================================================================
/**
 * @file  VsDemoEvent.h
 * @brief VSデモイベント
 * @date 2016/01/21
 * @author ichiraku_katsuhiko
 */
//=================================================================================================
#ifndef __VS_DEMO_EVENT_H__
#define __VS_DEMO_EVENT_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"

// VSデモパラメータ
#include "App/AppDemo/include/AppDemoProcParam.h"


namespace App {
namespace AppDemo {
class AppDemoProc_VS;
}
}

GFL_NAMESPACE_BEGIN(NetEvent)

/* -------------------------------------------------------------------------*/
/**
 *	@brief  VSデモイベント
 */
/* -------------------------------------------------------------------------*/
class VsDemoEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( VsDemoEvent ); // コピーを禁止

public:

  // このイベントを起動する関数 
  static void StartEvent( GameSys::GameManager* pGameManager, App::AppDemo::AppDemoProcParam_VS procParam );

  //! コンストラクタ
  VsDemoEvent( gfl2::heap::HeapBase* pHeap );
  //! デストラクタ
  virtual ~VsDemoEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  // デモパラメータ設定
  void SetAppDemoParamVs( App::AppDemo::AppDemoProcParam_VS procParam ){ m_procParam = procParam; }

private:

  App::AppDemo::AppDemoProcParam_VS   m_procParam;
  App::AppDemo::AppDemoProc_VS*       m_pProc;
  
};


GFL_NAMESPACE_END(NetEvent)


#endif // __VS_DEMO_EVENT_H__
