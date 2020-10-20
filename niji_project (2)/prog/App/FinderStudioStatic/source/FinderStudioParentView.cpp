//======================================================================
/**
 * @file    FinderStudioParentView.cpp
 * @date    2016/11/22 17:19:51
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：親ビュー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioParentView.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

ParentView::ParentView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
: Static::ViewBase( heap, renderMan )
{
  ;
}

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
