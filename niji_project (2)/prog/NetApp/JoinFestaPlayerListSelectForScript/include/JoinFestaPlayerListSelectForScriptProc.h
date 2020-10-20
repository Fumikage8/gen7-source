// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptProc.h
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のプロッククラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTFORSCRIPT_PROC_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTFORSCRIPT_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/include/JoinFestaPlayerListSelectForScriptAppParam.h"
#include <NetStatic/NetAppLib/include/JoinFestaUI\JoinFestaPlayerData.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaPlayerListSelectForScriptApplicationSystem;
class JoinFestaPlayerListSelectForScriptWork;
GFL_NAMESPACE_END(System)


class JoinFestaPlayerListSelectForScriptProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectForScriptProc );
public:
  JoinFestaPlayerListSelectForScriptProc( NetApp::JoinFestaPlayerListSelectForScript::APP_PARAM* pAppParam);
  virtual~JoinFestaPlayerListSelectForScriptProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pProcManager );
  virtual void DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

  void SetAppResult( NetApp::JoinFestaPlayerListSelectForScript::APP_RESULT* pAppResult ){ m_pAppResult = pAppResult; }


  /*
    battleFesCircle
  */
  void  SetBFCInfo(BFCModeInfo* pBFCModeInfo) {m_pBFCModeInfo = pBFCModeInfo;}  /**<  BFCModeでは必須 */
  void  InsertPlayerData_To_BFCModeInfo(const NetAppLib::JoinFestaUI::JoinFestaPlayerData& rPlayerData)     {m_pBFCModeInfo->Insert(rPlayerData);}
  void  RemovePlayerData_From_BFCModeInfo(const NetAppLib::JoinFestaUI::JoinFestaPlayerData& rPlayerData)   {m_pBFCModeInfo->Remove(rPlayerData);}
  u32   GetPlayerDataNum_In_BFCModeInfo(void) const                                                         {return m_pBFCModeInfo->dataNum;}
  u32   GetPlayerDataCapacity_Of_BFCModeInfo(void) const                                                    {return BFCModeInfo::SELECT_MAX;}
  bool  IsBFCModeInfoFull(void) const                                                                       {return (GetPlayerDataCapacity_Of_BFCModeInfo() == GetPlayerDataNum_In_BFCModeInfo());}
  NetAppLib::JoinFestaUI::JoinFestaPlayerData*   GetPlayerData_From_BFCModeInfo(const u32 index)            {return m_pBFCModeInfo->GetPlayerData(index);}


private:

  enum
  {
    INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM,
    INIT_SEQUENCE_RESOURCE_LOADING,
  };

  NetApp::JoinFestaPlayerListSelectForScript::APP_PARAM                                                        m_AppParam;
  NetApp::JoinFestaPlayerListSelectForScript::APP_RESULT*                                                      m_pAppResult;
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork*                  m_pJoinFestaPlayerListSelectForScriptWork;
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptApplicationSystem*     m_pApplicationSystem;
  BFCModeInfo*                                                                                                 m_pBFCModeInfo;

};


GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTFORSCRIPT_PROC_H_INCLUDE
