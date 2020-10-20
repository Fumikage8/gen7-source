// ============================================================================
/*
 * @file KisekaeViewList.h
 * @brief きせかえアプリのViewリストです。
 * @date  2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( KISEKAE_VIEWLIST_H_INCLUDE )
#define KISEKAE_VIEWLIST_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "AppLib/include/Util/app_util_heap.h"

//  前方宣言
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


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(System)

class ViewList
{
  GFL_FORBID_COPY_AND_ASSIGN( ViewList );

public:

  ViewList( app::util::Heap* pAppHeap );
  virtual~ViewList();

  void Update();
  void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  void AddUpperView( app::ui::UIView* pView );
  void AddLowerView( app::ui::UIView* pView );

  void AddUpperViewInputEnable( app::ui::UIView* pView );
  void AddLowerViewInputEnable( app::ui::UIView* pView );

  void AddUpperViewInputDisable( app::ui::UIView* pView );
  void AddLowerViewInputDisable( app::ui::UIView* pView );

  void RemoveAllView();

private:

  NetAppLib::UI::NetAppEmptyView*       m_pRootUpperView;
  NetAppLib::UI::NetAppEmptyView*       m_pRootLowerView;
  app::ui::UIView*                      m_pActiveUpperView;
  app::ui::UIView*                      m_pActiveLowerView;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)


#endif // KISEKAE_VIEWLIST_H_INCLUDE
