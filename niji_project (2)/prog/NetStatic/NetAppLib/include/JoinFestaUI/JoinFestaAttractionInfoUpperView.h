// ============================================================================
/*
 * @file JoinFestaAttractionInfoUpperView.h
 * @brief アトラクション情報上画面
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONINFOUPPERVIEW_H_INCLUDE )
#define JOINFESTAATTRACTIONINFOUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionListManager.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaAttractionInfoUpperView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionInfoUpperView );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_UPPER,
  };

public:

  JoinFestaAttractionInfoUpperView(
    NetAppLib::System::ApplicationWorkBase*       pWorkBase,
    NetAppLib::System::ResourceID                 layoutResourceID );
  virtual~JoinFestaAttractionInfoUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  void SetJoinFestaAttractionData( NetAppLib::JoinFestaUI::JoinFestaAttractionData* pData );

private:
  void updateTimeLimitPaneText( NetAppLib::JoinFestaUI::JoinFestaAttractionData* pData );

private:

  NetAppLib::System::ApplicationWorkBase*               m_pWorkBase;
  NetAppLib::JoinFestaUI::JoinFestaAttractionData*      m_pJoinFestaAttractionData;
  bool                                                  m_bDrawFlag;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAATTRACTIONINFOUPPERVIEW_H_INCLUDE
