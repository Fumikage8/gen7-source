#if !defined __FINDER_STUDIO_PARENT_VIEW_H_INCLUDED__
#define __FINDER_STUDIO_PARENT_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioParentView.h
 * @date    2016/11/22 17:18:18
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：親ビュー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioViewBase.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

class ParentView
  : public Static::ViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ParentView );

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  ParentView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~ParentView( void ) { ; }
};

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_PARENT_VIEW_H_INCLUDED__
