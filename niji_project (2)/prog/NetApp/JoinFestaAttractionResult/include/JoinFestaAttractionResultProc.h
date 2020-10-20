// ============================================================================
/*
 * @file JoinFestaAttractionResultProc.h
 * @brief アトラクション結果アプリのプロッククラスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONRESULT_PROC_H_INCLUDE )
#define JOINFESTAATTRACTIONRESULT_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/JoinFestaAttractionResult/include/JoinFestaAttractionResultAppParam.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaAttractionResultApplicationSystem;
class JoinFestaAttractionResultWork;
GFL_NAMESPACE_END(System)


class JoinFestaAttractionResultProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionResultProc );
public:
  JoinFestaAttractionResultProc( NetApp::JoinFestaAttractionResult::APP_PARAM* pAppParam );
  virtual~JoinFestaAttractionResultProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pProcManager );
  virtual void DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

private:

  enum
  {
    INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM,
    INIT_SEQUENCE_RESOURCE_LOADING,
  };

  NetApp::JoinFestaAttractionResult::APP_PARAM                                         m_AppParam;
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork*                  m_pJoinFestaAttractionResultWork;
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONRESULT_PROC_H_INCLUDE
