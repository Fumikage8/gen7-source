// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiLowerView.h
 * @brief プレイヤー複数選択アプリの下画面を表示するクラスです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTMULTILOWERVIEW_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTMULTILOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListSelectLowerViewBase.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaPlayerListSelectMultiLowerView : public NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectMultiLowerView );
public:
  JoinFestaPlayerListSelectMultiLowerView( NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork* pJoinFestaPlayerListSelectMultiWork );
  virtual~JoinFestaPlayerListSelectMultiLowerView();

  // PaneListView::Listener
  virtual void PaneListItemSelectListener( u32 pos );

private:
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork*   m_pJoinFestaPlayerListSelectMultiWork;
};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTMULTILOWERVIEW_H_INCLUDE
