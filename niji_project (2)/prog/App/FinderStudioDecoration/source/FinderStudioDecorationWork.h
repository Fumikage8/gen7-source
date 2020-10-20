#if !defined( __FINDER_STUDIO_DECORATION_WORK_H_INCLUDED__ )
#define __FINDER_STUDIO_DECORATION_WORK_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecorationWork.h
 * @date    2017/02/10 15:24:27
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：共有データ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class DecoItemManager;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
/// ---[ 定数定義 begin ]---

/// 保存タイプ
enum SaveType
{
  SAVE_TYPE_OVER_WRITE,
  SAVE_TYPE_NEW_WRITE,
  SAVE_TYPE_NUM,
};

/// ---[ 定数定義 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    共有データ
///
//////////////////////////////////////////////////////////////////////////
struct DecoWork
{
  /// 保存タイプ
  SaveType    save_type;
  /// 写真データ
  void*       photo_data;
  /// ヒープ
  app::util::Heap*    app_heap;
  /// レンダリングマネージャ
  app::util::AppRenderingManager*   render_man;
  /// デコアイテムマネージャ
  DecoItemManager*    deco_man;

  //-------------------------------------------------------
  // @brief   コンストラクタ
  //-------------------------------------------------------
  DecoWork( void )
    : save_type( SAVE_TYPE_NUM )
    , photo_data( 0 )
    , app_heap(0)
    , render_man( 0 )
    , deco_man( 0 )
  {
    ;
  }
};

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATION_WORK_H_INCLUDED__
