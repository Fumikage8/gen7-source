// ============================================================================
/*
 * @file EndRollApplicationSystem.h
 * @brief エンドロールアプリのアプリケーションシステムです。
 * @date 2016.03.16
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( APPENDROLL_APPLICATIONSYSTEM_H_INCLUDE )
#define APPENDROLL_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

//  nijiのインクルード
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "GameSys/include/GameManager.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(System)

//  前方宣言
class EndRollWork;
class EndRollBuilder;
class EndRollViewList;
class EndRollViewObjects;
class EndRollViewSystemAccessor;

class EndRollApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( EndRollApplicationSystem );

public:
  EndRollApplicationSystem();
  virtual~EndRollApplicationSystem();

  void Build( App::EndRoll::System::EndRollWork* pEndRollWork, App::EndRoll::System::EndRollBuilder* pApplicationSystemBuilder );

  virtual bool End();

private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();
  virtual bool IsCreateViewComplete();

private:

  void _Clear()
  {
    m_pEndRollWork = NULL;
    m_pViewList = NULL;
    m_pViewObjects = NULL;
    m_pViewSystemAccessor = NULL;

    m_initSeq = m_endSeq  = 0;
  }

  App::EndRoll::System::EndRollWork*                   m_pEndRollWork;
  App::EndRoll::System::EndRollViewList*               m_pViewList;
  App::EndRoll::System::EndRollViewObjects*            m_pViewObjects;
  App::EndRoll::System::EndRollViewSystemAccessor*     m_pViewSystemAccessor;

  //  変更前の画面全体のクリア設定
  GameSys::GameManager::BUFFER_CLEAR_SETTING  m_aOldClearSetting[ gfl2::gfx::CtrDisplayNo::NumberOf ];


  u32 m_initSeq, m_endSeq;
};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)


#endif // APPENDROLL_APPLICATIONSYSTEM_H_INCLUDE
