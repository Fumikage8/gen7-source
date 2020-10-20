// ============================================================================
/*
 * @file JoinFestaPlayerListShowWork.h
 * @brief 閲覧アプリのワーククラスです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSHOW_WORK_H_INCLUDE )
#define JOINFESTAPLAYERLISTSHOW_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListManager.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordListManager.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaSurveyListManager.h"
#include "NetApp/JoinFestaPlayerListShow/source/JoinFestaPlayerListShowDefine.h"

#include "../../include/JoinFestaPlayerListShowAppParam.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaPlayerListShowWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListShowWork );

private:

  static const u32  APP_HEAP_SIZE             = NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_APP_HEAP_SIZE;
  static const u32  APP_DEVICE_HEAP_SIZE      = NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_APP_DEVICE_HEAP_SIZE;

public:
  JoinFestaPlayerListShowWork();
  virtual~JoinFestaPlayerListShowWork();

  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& GetJoinFestaPlayerListManager(){ return m_JoinFestaPlayerListManager; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerData& GetSelectPlayer() { return m_SelectPlayer; }

  NetAppLib::JoinFestaUI::JoinFestaRecordListManager& GetJoinFestaRecordListManager() { return m_JoinFestaRecordListManager; }
  NetAppLib::JoinFestaUI::JoinFestaRecordData& GetSelectRecord() { return m_SelectRecord; }

  NetAppLib::JoinFestaUI::JoinFestaSurveyListManager& GetJoinFestaSurveyListManager() { return m_JoinFestaSurveyListManager; }


  void SetVipActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pVipActivePlayerData ){ m_pVipActivePlayerData = pVipActivePlayerData; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerData* GetVipActivePlayerData() { return m_pVipActivePlayerData; }

  void SetGuestActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pGuestActivePlayerData ){ m_pGuestActivePlayerData = pGuestActivePlayerData; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerData* GetGuestActivePlayerData() { return m_pGuestActivePlayerData; }

  void SetBlackActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pBlackActivePlayerData ){ m_pBlackActivePlayerData = pBlackActivePlayerData; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerData* GetBlackActivePlayerData() { return m_pBlackActivePlayerData; }

  void ResetActivePlayerData();

  void SetShowType( NetApp::JoinFestaPlayerListShow::ShowType eShowType ){ m_eShowType = eShowType; }
  NetApp::JoinFestaPlayerListShow::ShowType GetShowType(){ return m_eShowType; }

  void SetDescriptionOpenAnimeFlag( bool bFlag ) { m_bDescriptionOpenAnimeFlag = bFlag; }
  bool GetDescriptionOpenAnimeFlag() { return m_bDescriptionOpenAnimeFlag; }

  void SetUpdateNotificationFlag( bool bFlag ) { m_bUpdateNotificationFlag = bFlag; }
  bool GetUpdateNotificationFlag() { return m_bUpdateNotificationFlag; }

  void SetAppParam(APP_PARAM* pAppParam)    {m_pAppParam = pAppParam;}
  APP_PARAM* GetAppParam(void) const        {return m_pAppParam;}

private:

  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager    m_JoinFestaPlayerListManager;
  NetAppLib::JoinFestaUI::JoinFestaPlayerData           m_SelectPlayer;

  NetAppLib::JoinFestaUI::JoinFestaRecordListManager    m_JoinFestaRecordListManager;
  NetAppLib::JoinFestaUI::JoinFestaRecordData           m_SelectRecord;

  NetAppLib::JoinFestaUI::JoinFestaSurveyListManager    m_JoinFestaSurveyListManager;


  NetAppLib::JoinFestaUI::JoinFestaPlayerData*          m_pVipActivePlayerData;
  NetAppLib::JoinFestaUI::JoinFestaPlayerData*          m_pGuestActivePlayerData;
  NetAppLib::JoinFestaUI::JoinFestaPlayerData*          m_pBlackActivePlayerData;

  NetApp::JoinFestaPlayerListShow::ShowType             m_eShowType;
  bool                                                  m_bDescriptionOpenAnimeFlag;

  bool                                                  m_bUpdateNotificationFlag;

  APP_PARAM*                                            m_pAppParam;
};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSHOWWORK_H_INCLUDE
