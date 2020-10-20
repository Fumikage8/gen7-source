// ============================================================================
/*
 * @file JoinFestaAttractionResultWork.h
 * @brief アトラクション結果アプリのワーククラスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONRESULT_WORK_H_INCLUDE )
#define JOINFESTAATTRACTIONRESULT_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetApp/JoinFestaAttractionResult/source/JoinFestaAttractionResultDefine.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionListManager.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRankingListManager.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaAttractionResultWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionResultWork );

private:

  static const u32  APP_HEAP_SIZE             = NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_APP_HEAP_SIZE;
  static const u32  APP_DEVICE_HEAP_SIZE      = NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_APP_DEVICE_HEAP_SIZE;

public:
  JoinFestaAttractionResultWork();
  virtual~JoinFestaAttractionResultWork();

  void SetAttractionData( const NetAppLib::JoinFestaUI::JoinFestaAttractionData& attractionData ){ m_AttractionData = attractionData; }
  NetAppLib::JoinFestaUI::JoinFestaAttractionData& GetAttractionData(){ return m_AttractionData; }

  NetAppLib::JoinFestaUI::JoinFestaRankingListManager& GetJoinFestaRankingListManager(){ return m_JoinFestaRankingListManager; }

private:

  NetAppLib::JoinFestaUI::JoinFestaAttractionData         m_AttractionData;
  NetAppLib::JoinFestaUI::JoinFestaRankingListManager     m_JoinFestaRankingListManager;

};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONRESULTWORK_H_INCLUDE
