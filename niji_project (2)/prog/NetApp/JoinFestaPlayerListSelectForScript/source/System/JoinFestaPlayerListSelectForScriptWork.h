// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptWork.h
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のワーククラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTFORSCRIPT_WORK_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTFORSCRIPT_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListManager.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordListManager.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaSurveyListManager.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/JoinFestaPlayerListSelectForScriptDefine.h"

#include "../../include/JoinFestaPlayerListSelectForScriptProc.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaPlayerListSelectForScriptWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectForScriptWork );

private:

  static const u32  APP_HEAP_SIZE             = NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_APP_HEAP_SIZE;
  static const u32  APP_DEVICE_HEAP_SIZE      = NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_APP_DEVICE_HEAP_SIZE;

public:
  JoinFestaPlayerListSelectForScriptWork();
  virtual~JoinFestaPlayerListSelectForScriptWork();

  NetApp::JoinFestaPlayerListSelectForScript::BootMode GetBootMode() const { return m_eBootMode; }
  void SetBootMode( NetApp::JoinFestaPlayerListSelectForScript::BootMode eBootMode ) { m_eBootMode = eBootMode; }

  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& GetJoinFestaPlayerListManager(){ return m_JoinFestaPlayerListManager; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerData* GetSelectPlayer() { return m_pSelectPlayer; }
  void SetSelectPlayer( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData ) { m_pSelectPlayer = pData; }

  NetAppLib::JoinFestaUI::JoinFestaRecordListManager& GetJoinFestaRecordListManager() { return m_JoinFestaRecordListManager; }
  NetAppLib::JoinFestaUI::JoinFestaRecordData& GetSelectRecord() { return m_SelectRecord; }

  NetAppLib::JoinFestaUI::JoinFestaSurveyListManager& GetJoinFestaSurveyListManager() { return m_JoinFestaSurveyListManager; }

  u32 GetListSize() const { return m_ListSize; }
  void SetListSize( u32 listSize ){ m_ListSize = listSize; }

  void SetVipActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pVipActivePlayerData ){ m_pVipActivePlayerData = pVipActivePlayerData; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerData* GetVipActivePlayerData() { return m_pVipActivePlayerData; }

  void SetGuestActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pGuestActivePlayerData ){ m_pGuestActivePlayerData = pGuestActivePlayerData; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerData* GetGuestActivePlayerData() { return m_pGuestActivePlayerData; }

  void ResetActivePlayerData();

  void SelectCancel( bool bIsSelectCancel ) { m_bIsSelectCancel = bIsSelectCancel; }
  bool IsSelectCancel() { return m_bIsSelectCancel; }

  void SetShowVip( bool bFlag ){ m_bShowVip = bFlag; }
  bool GetShowVip() { return m_bShowVip; }


  /*  BFC  */
  void          SetBFCModeInfo(BFCModeInfo* pInfo)                                {m_pBFCModeInfo = pInfo;}
  BFCModeInfo*  GetBFCModeInfo(void)                                              {return m_pBFCModeInfo;}
  void          SetCommonGrpIconData(app::tool::AppCommonGrpIconData* pIconData)  {m_pCommonIconData = pIconData;}
  app::tool::AppCommonGrpIconData*  GetCommonGrpIconData(void)                    {return m_pCommonIconData;}

private:

  NetApp::JoinFestaPlayerListSelectForScript::BootMode  m_eBootMode;

  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager    m_JoinFestaPlayerListManager;
  NetAppLib::JoinFestaUI::JoinFestaPlayerData*          m_pSelectPlayer;

  NetAppLib::JoinFestaUI::JoinFestaPlayerData*          m_pVipActivePlayerData;
  NetAppLib::JoinFestaUI::JoinFestaPlayerData*          m_pGuestActivePlayerData;

  NetAppLib::JoinFestaUI::JoinFestaRecordListManager    m_JoinFestaRecordListManager;
  NetAppLib::JoinFestaUI::JoinFestaRecordData           m_SelectRecord;

  NetAppLib::JoinFestaUI::JoinFestaSurveyListManager    m_JoinFestaSurveyListManager;

  u32                                                   m_ListSize;

  bool                                                  m_bIsSelectCancel;
  bool                                                  m_bShowVip;


  BFCModeInfo*                                          m_pBFCModeInfo;
  app::tool::AppCommonGrpIconData*                      m_pCommonIconData;
};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTFORSCRIPTWORK_H_INCLUDE
