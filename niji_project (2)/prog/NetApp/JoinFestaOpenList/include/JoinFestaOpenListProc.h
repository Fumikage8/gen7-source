// ============================================================================
/*
 * @file JoinFestaOpenListProc.h
 * @brief 開催選択アプリのプロッククラスです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAOPENLIST_PROC_H_INCLUDE )
#define JOINFESTAOPENLIST_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/JoinFestaOpenList/include/JoinFestaOpenListAppParam.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaOpenListApplicationSystem;
class JoinFestaOpenListWork;
GFL_NAMESPACE_END(System)


class JoinFestaOpenListProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaOpenListProc );
public:
  JoinFestaOpenListProc( NetApp::JoinFestaOpenList::APP_PARAM* pAppParam );
  virtual~JoinFestaOpenListProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pProcManager );
  virtual void DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

  void SetAppResult( NetApp::JoinFestaOpenList::APP_RESULT* pAppResult ){ m_pAppResult = pAppResult; }

private:

  enum
  {
    INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM,
    INIT_SEQUENCE_RESOURCE_LOADING,
  };

  NetApp::JoinFestaOpenList::APP_PARAM                                       m_AppParam;
  NetApp::JoinFestaOpenList::APP_RESULT*                                     m_pAppResult;
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork*                  m_pJoinFestaOpenListWork;
  NetApp::JoinFestaOpenList::System::JoinFestaOpenListApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAOPENLIST_PROC_H_INCLUDE
