// ============================================================================
/*
 * @file JoinFestaPlayerListSelectWork.h
 * @brief プレイヤー単体選択アプリのワーククラスです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECT_WORK_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECT_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListManager.h"
#include "NetApp/JoinFestaPlayerListSelect/source/JoinFestaPlayerListSelectDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaPlayerListSelectWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectWork );

private:

  static const u32  APP_HEAP_SIZE             = NetApp::JoinFestaPlayerListSelect::JOINFESTAPLAYERLISTSELECT_APP_HEAP_SIZE;
  static const u32  APP_DEVICE_HEAP_SIZE      = NetApp::JoinFestaPlayerListSelect::JOINFESTAPLAYERLISTSELECT_APP_DEVICE_HEAP_SIZE;

public:
  JoinFestaPlayerListSelectWork();
  virtual~JoinFestaPlayerListSelectWork();

  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& GetJoinFestaPlayerListManager(){ return m_JoinFestaPlayerListManager; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerData* GetSelectPlayer() { return m_pSelectPlayer; }
  void SetSelectPlayer( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData ) { m_pSelectPlayer = pData; }

  void SetVipActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pVipActivePlayerData ){ m_pVipActivePlayerData = pVipActivePlayerData; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerData* GetVipActivePlayerData() { return m_pVipActivePlayerData; }

  void SetGuestActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pGuestActivePlayerData ){ m_pGuestActivePlayerData = pGuestActivePlayerData; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerData* GetGuestActivePlayerData() { return m_pGuestActivePlayerData; }

  void ResetActivePlayerData();

  void SetShowVip( bool bFlag ){ m_bShowVip = bFlag; }
  bool GetShowVip() { return m_bShowVip; }

private:

  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager    m_JoinFestaPlayerListManager;
  NetAppLib::JoinFestaUI::JoinFestaPlayerData*          m_pSelectPlayer;
  NetAppLib::JoinFestaUI::JoinFestaPlayerData*          m_pVipActivePlayerData;
  NetAppLib::JoinFestaUI::JoinFestaPlayerData*          m_pGuestActivePlayerData;
  bool                                                  m_bShowVip;

};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTWORK_H_INCLUDE
