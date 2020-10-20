// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiWork.h
 * @brief プレイヤー複数選択アプリのワーククラスです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTMULTI_WORK_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTMULTI_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListManager.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/JoinFestaPlayerListSelectMultiDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaPlayerListSelectMultiWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectMultiWork );

private:

  static const u32  APP_HEAP_SIZE             = NetApp::JoinFestaPlayerListSelectMulti::JOINFESTAPLAYERLISTSELECTMULTI_APP_HEAP_SIZE;
  static const u32  APP_DEVICE_HEAP_SIZE      = NetApp::JoinFestaPlayerListSelectMulti::JOINFESTAPLAYERLISTSELECTMULTI_APP_DEVICE_HEAP_SIZE;

public:
  JoinFestaPlayerListSelectMultiWork();
  virtual~JoinFestaPlayerListSelectMultiWork();

  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& GetJoinFestaPlayerListManager() { return m_JoinFestaPlayerListManager; }
  NetAppLib::JoinFestaUI::JoinFestPlayerDataList& GetSelectPlayerList() { return m_SelectPlayerList; }

  void SetVipActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pVipActivePlayerData ){ m_pVipActivePlayerData = pVipActivePlayerData; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerData* GetVipActivePlayerData() { return m_pVipActivePlayerData; }

  void SetGuestActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pGuestActivePlayerData ){ m_pGuestActivePlayerData = pGuestActivePlayerData; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerData* GetGuestActivePlayerData() { return m_pGuestActivePlayerData; }

  void ResetActivePlayerData();

  void SelectCancel( bool bIsSelectCancel ) { m_bIsSelectCancel = bIsSelectCancel; }
  bool IsSelectCancel() { return m_bIsSelectCancel; }

  void SetShowVip( bool bFlag ){ m_bShowVip = bFlag; }
  bool GetShowVip() { return m_bShowVip; }

private:

  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager    m_JoinFestaPlayerListManager;
  NetAppLib::JoinFestaUI::JoinFestPlayerDataList        m_SelectPlayerList;
  NetAppLib::JoinFestaUI::JoinFestaPlayerData*          m_pVipActivePlayerData;
  NetAppLib::JoinFestaUI::JoinFestaPlayerData*          m_pGuestActivePlayerData;
  bool                                                  m_bIsSelectCancel;
  bool                                                  m_bShowVip;

};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTMULTIWORK_H_INCLUDE
