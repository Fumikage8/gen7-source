#if PM_DEBUG

#if !defined(__FINDER_STUDIO_DECO_TEST_DRAW_LISTENER_H_INCLUDED__)
#define __FINDER_STUDIO_DECO_TEST_DRAW_LISTENER_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FStudioDecoTestDrawListener.h
 * @date    2016/10/26 20:13:03
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FStudioDecoTestDefine.h"

#include <math/include/gfl2_math.h>
#include <Layout/include/gfl2_Layout.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

class DrawListener
{
public:
  //-------------------------------------------------------
  // @brief   終了通知
  //-------------------------------------------------------
  virtual void OnEndDecoTest( void ) { ; }

  //-------------------------------------------------------
  // @brief   リクエスト通知
  //-------------------------------------------------------
  virtual void OnRequestShootAndSave( void ) { ; }
  virtual void OnOpenDecoWindow( DecoItemKind type ){ ; }
  virtual void OnUpdateTouchPoint( gfl2::math::VEC3 vec ) { ; }

  //-------------------------------------------------------
  // @brief   デコアイテムの追加と削除の通知
  //-------------------------------------------------------
  virtual void OnAddDecoItem( void ) { ; }
  virtual void OnRemoveDecoItem( void ) { ; }
  virtual void OnSetDecoItem( u32 index ) { ; }
};

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // __FINDER_STUDIO_DECO_TEST_DRAW_LISTENER_H_INCLUDED__

#endif // #if PM_DEBUG
