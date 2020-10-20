// ============================================================================
/*
 * @file NetAppRegulationUpperView.h
 * @brief 上画面にレギュレーションの情報を表示するクラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( NETAPPREGULATIONUPPERVIEW_H_INCLUDE )
#define NETAPPREGULATIONUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppRegulationDrawInfo.h"
#include "NetApp/RegulationSelect/source/RegulationSelectDefine.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)


class NetAppRegulationUpperView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NetAppRegulationUpperView );
public:
  NetAppRegulationUpperView(
    NetAppLib::System::ApplicationWorkBase* pWork,
    NetAppLib::System::ResourceID resourceID,
    NetAppLib::Message::MessageUtility* pMessageUtility );

  NetAppRegulationUpperView(
    app::util::Heap* pAppHeap,
    app::util::AppRenderingManager* pAppRenderingManager,
    void* pLayoutBuffer );
  
  virtual~NetAppRegulationUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  void SetRegulationDrawInfo( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo, NetApp::RegulationSelect::BootMode eBootMode );
  void ResetRegulationDrawInfo();

private:

  gfl2::str::StrBuf* getMessage( u32 messageID );

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_REGULATION,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_UPPER,
  };

private:
  app::util::Heap*                                m_pAppHeap;
  app::util::AppRenderingManager*                 m_pAppRenderingManager;
  NetAppLib::Message::MessageUtility*             m_pMessageUtility;
  NetAppLib::UI::RegulationDrawInfo*              m_pRegulationDrawInfo;

};


GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)

#endif // NETAPPREGULATIONUPPERVIEW_H_INCLUDE
