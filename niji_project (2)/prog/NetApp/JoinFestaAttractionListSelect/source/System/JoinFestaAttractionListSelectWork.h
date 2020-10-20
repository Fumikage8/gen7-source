// ============================================================================
/*
 * @file JoinFestaAttractionListSelectWork.h
 * @brief アトラクション開催参加アプリのワーククラスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONLISTSELECT_WORK_H_INCLUDE )
#define JOINFESTAATTRACTIONLISTSELECT_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionListManager.h"
#include "NetApp/JoinFestaAttractionListSelect/source/JoinFestaAttractionListSelectDefine.h"
#include "NetApp/JoinFestaAttractionListSelect/source/JoinFestaAttractionListSelectFrameID.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaAttractionListSelectWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionListSelectWork );

private:

  static const u32  APP_HEAP_SIZE             = NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_APP_HEAP_SIZE;
  static const u32  APP_DEVICE_HEAP_SIZE      = NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_APP_DEVICE_HEAP_SIZE;

public:
  JoinFestaAttractionListSelectWork();
  virtual~JoinFestaAttractionListSelectWork();

  NetAppLib::JoinFestaUI::JoinFestaAttractionListManager& GetJoinFestaAttractionListManager() { return m_JoinFestaAttractionListManager; }

  NetAppLib::JoinFestaUI::JoinFestaAttractionData* GetSelectAttraction() { return m_pSelectAttraction; }
  void SetSelectAttraction( NetAppLib::JoinFestaUI::JoinFestaAttractionData* pData ) { m_pSelectAttraction = pData; }

  NetAppLib::JoinFestaUI::JoinFestaAttractionData* GetDescriptionAttraction() { return m_pDescriptionAttraction; }
  void SetDescriptionAttraction( NetAppLib::JoinFestaUI::JoinFestaAttractionData* pData ) { m_pDescriptionAttraction = pData; }

  NetAppLib::JoinFestaUI::JoinFestaAttractionData* GetOpenActiveAttractionData() { return m_pOpenActiveAttractionData; }
  void SetOpenActiveAttractionData( NetAppLib::JoinFestaUI::JoinFestaAttractionData* pData ) { m_pOpenActiveAttractionData = pData; }

  NetAppLib::JoinFestaUI::JoinFestaAttractionData* GetEntryActiveAttractionData() { return m_pEntryActiveAttractionData; }
  void SetEntryActiveAttractionData( NetAppLib::JoinFestaUI::JoinFestaAttractionData* pData ) { m_pEntryActiveAttractionData = pData; }

  NetApp::JoinFestaAttractionListSelect::JoinFestaAttractionListSelectFrameID GetDescriptionBootFrameID() { return m_eDescriptionBootFrameID; }
  void SetDescriptionBootFrameID( NetApp::JoinFestaAttractionListSelect::JoinFestaAttractionListSelectFrameID eFrameID ) { m_eDescriptionBootFrameID = eFrameID; }

  void ResetActiveAttractionData();

private:

  NetAppLib::JoinFestaUI::JoinFestaAttractionListManager                               m_JoinFestaAttractionListManager;
  NetAppLib::JoinFestaUI::JoinFestaAttractionData*                                     m_pSelectAttraction;
  NetAppLib::JoinFestaUI::JoinFestaAttractionData*                                     m_pDescriptionAttraction;
  NetAppLib::JoinFestaUI::JoinFestaAttractionData*                                     m_pOpenActiveAttractionData;
  NetAppLib::JoinFestaUI::JoinFestaAttractionData*                                     m_pEntryActiveAttractionData;
  NetApp::JoinFestaAttractionListSelect::JoinFestaAttractionListSelectFrameID          m_eDescriptionBootFrameID;

};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONLISTSELECTWORK_H_INCLUDE
