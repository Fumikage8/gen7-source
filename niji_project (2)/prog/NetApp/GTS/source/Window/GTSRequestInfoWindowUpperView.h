// ============================================================================
/*
 * @file GTSRequestInfoWindowUpperView.h
 * @brief リクエスト情報を表示するクラスです。
 * @date 2015.04.30
 */
// ============================================================================
#if !defined( GTSREQUESTINFOWINDOWUPPERVIEW_H_INCLUDE )
#define GTSREQUESTINFOWINDOWUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <str/include/gfl2_StrBuf.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "AppLib/include/Tool/AppToolItemIcon.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(Window)


class GTSRequestInfoWindowUpperView : public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSRequestInfoWindowUpperView );

public:

  GTSRequestInfoWindowUpperView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSRequestInfoWindowUpperView();

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsDrawing(void) const;

  void SetGTSIndexData( NetApp::GTS::GtsIndexData& gtsIndexData, bool bIsDownloadPokemon );

private:

  // UIView
  virtual void OnRemovedFromParent();

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_GTS,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_UPPER,
  };

  void setupTextMessage();

private:

  NetApp::GTS::ApplicationSystem::GTSWork*                     m_pGTSWork;
  gfl2::str::StrBuf                                            m_PlayerName;
  gfl2::str::StrBuf                                            m_PokemonName;
  app::tool::ItemIconToolEx*                                   m_pItemIconTool;

};


GFL_NAMESPACE_END(Window)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSREQUESTINFOWINDOWUPPERVIEW_H_INCLUDE
