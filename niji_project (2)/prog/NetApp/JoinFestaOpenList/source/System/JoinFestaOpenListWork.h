// ============================================================================
/*
 * @file JoinFestaOpenListWork.h
 * @brief 開催選択アプリのワーククラスです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAOPENLIST_WORK_H_INCLUDE )
#define JOINFESTAOPENLIST_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetApp/JoinFestaOpenList/source/JoinFestaOpenListDefine.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionListManager.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaOpenListWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaOpenListWork );

private:

  static const u32  APP_HEAP_SIZE             = NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_APP_HEAP_SIZE;
  static const u32  APP_DEVICE_HEAP_SIZE      = NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_APP_DEVICE_HEAP_SIZE;

public:
  JoinFestaOpenListWork();
  virtual~JoinFestaOpenListWork();

  NetAppLib::JoinFestaUI::JoinFestaAttractionListManager& GetJoinFestaAttractionListManager() { return m_JoinFestaAttractionListManager; }

  NetAppLib::JoinFestaUI::JoinFestaAttractionData* GetSelectAttraction() { return m_pSelectAttraction; }
  void SetSelectAttraction( NetAppLib::JoinFestaUI::JoinFestaAttractionData* pData ) { m_pSelectAttraction = pData; }

  NetAppLib::JoinFestaUI::JoinFestaAttractionData* GetDescriptionAttraction() { return m_pDescriptionAttraction; }
  void SetDescriptionAttraction( NetAppLib::JoinFestaUI::JoinFestaAttractionData* pData ) { m_pDescriptionAttraction = pData; }

private:

  NetAppLib::JoinFestaUI::JoinFestaAttractionListManager              m_JoinFestaAttractionListManager;
  NetAppLib::JoinFestaUI::JoinFestaAttractionData*                    m_pSelectAttraction;
  NetAppLib::JoinFestaUI::JoinFestaAttractionData*                    m_pDescriptionAttraction;

};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAOPENLISTWORK_H_INCLUDE
