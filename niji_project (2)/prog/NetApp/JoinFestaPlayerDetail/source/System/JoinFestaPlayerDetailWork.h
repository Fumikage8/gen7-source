// ============================================================================
/*
 * @file JoinFestaPlayerDetailWork.h
 * @brief 詳細アプリのワーククラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERDETAIL_WORK_H_INCLUDE )
#define JOINFESTAPLAYERDETAIL_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListManager.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordListManager.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaSurveyListManager.h"
#include "NetApp/JoinFestaPlayerDetail/source/JoinFestaPlayerDetailDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaPlayerDetailWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerDetailWork );

private:

  static const u32  APP_HEAP_SIZE             = NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_APP_HEAP_SIZE;
  static const u32  APP_DEVICE_HEAP_SIZE      = NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_APP_DEVICE_HEAP_SIZE;

public:
  JoinFestaPlayerDetailWork();
  virtual~JoinFestaPlayerDetailWork();

  NetAppLib::JoinFestaUI::JoinFestaPlayerData& GetSelectPlayer() { return m_SelectPlayer; }
  void SetSelectPlayer( const NetAppLib::JoinFestaUI::JoinFestaPlayerData& data ) { m_SelectPlayer = data; }

  NetAppLib::JoinFestaUI::JoinFestaRecordListManager& GetJoinFestaRecordListManager() { return m_JoinFestaRecordListManager; }
  NetAppLib::JoinFestaUI::JoinFestaRecordData& GetSelectRecord() { return m_SelectRecord; }

  NetAppLib::JoinFestaUI::JoinFestaSurveyListManager& GetJoinFestaSurveyListManager() { return m_JoinFestaSurveyListManager; }

private:

  NetAppLib::JoinFestaUI::JoinFestaPlayerData           m_SelectPlayer;


  NetAppLib::JoinFestaUI::JoinFestaRecordListManager    m_JoinFestaRecordListManager;
  NetAppLib::JoinFestaUI::JoinFestaRecordData           m_SelectRecord;

  NetAppLib::JoinFestaUI::JoinFestaSurveyListManager    m_JoinFestaSurveyListManager;


};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERDETAILWORK_H_INCLUDE
