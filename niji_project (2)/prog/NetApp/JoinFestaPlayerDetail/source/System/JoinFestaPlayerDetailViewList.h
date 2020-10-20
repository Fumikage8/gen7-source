﻿// ============================================================================
/*
 * @file JoinFestaPlayerDetailViewList.h
 * @brief 詳細アプリのViewリストです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERDETAIL_VIEWLIST_H_INCLUDE )
#define JOINFESTAPLAYERDETAIL_VIEWLIST_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(ui)
class UIView;
GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)
class NetApplicationViewBase;
GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)
class NetAppEmptyView;
GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaPlayerDetailViewList
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerDetailViewList );

public:

  JoinFestaPlayerDetailViewList( app::util::Heap* pAppHeap );
  virtual~JoinFestaPlayerDetailViewList();

  void Update();
  void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  void AddUpperView( NetAppLib::System::NetApplicationViewBase* pView );
  void AddLowerView( NetAppLib::System::NetApplicationViewBase* pView );

  void AddUpperViewInputEnable( NetAppLib::System::NetApplicationViewBase* pView );
  void AddLowerViewInputEnable( NetAppLib::System::NetApplicationViewBase* pView );

  void AddUpperViewInputDisable( NetAppLib::System::NetApplicationViewBase* pView );
  void AddLowerViewInputDisable( NetAppLib::System::NetApplicationViewBase* pView );

  void RemoveUpperView();
  void RemoveLowerView();

  void RemoveAllView();

private:

  NetAppLib::UI::NetAppEmptyView*       m_pRootUpperView;
  NetAppLib::UI::NetAppEmptyView*       m_pRootLowerView;
  app::ui::UIView*                      m_pActiveUpperView;
  app::ui::UIView*                      m_pActiveLowerView;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)


#endif // JOINFESTAPLAYERDETAIL_VIEWLIST_H_INCLUDE
