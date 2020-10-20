#if !defined(__FINDER_STUDIO_DECORATION_VIEW_LISTENER_H_INCLUDED__)
#define __FINDER_STUDIO_DECORATION_VIEW_LISTENER_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecorationViewListener.h
 * @date    2017/02/02 22:44:10
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：Viewリスナー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_Math.h>

#include <niji_conv_header/app/data/finder_studio/finder_studio_deco_type_def.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    Viewリスナー
///
//////////////////////////////////////////////////////////////////////////
class ViewListener
{
public:
  /**
   * @brief   新スタンプの設定
   *
   * @param   type        デコの種類
   * @param   decoIndex   デコのインデックス
   * @param   pos         デコの表示位置
   */
  virtual void MoveStartNewStamp( DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos )
  {
    GFL_UNUSED( type );
    GFL_UNUSED( decoIndex );
    GFL_UNUSED( pos );
  }

  /**
   * @brief   新スタンプの配置決定
   */
  virtual void MoveEndNewStamp( DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos )
  {
    GFL_UNUSED( type );
    GFL_UNUSED( decoIndex );
    GFL_UNUSED( pos );
  }

  /**
   * @brief   配置済みスタンプの移動開始
   *
   * @param   index   スタンプのインデックス(0～9)
   */
  virtual void MoveStartOldStamp( u8 index )
  {
    GFL_UNUSED( index );
  }

  /**
   * @brief   配置済みスタンプの移動終了
   *
   * @param   index   スタンプのインデックス(0～9)
   * @param   pos     スタンプの位置座標
   */
  virtual void MoveEndOldStamp( u8 index, gfl2::math::VEC3 pos )
  {
    GFL_UNUSED( index );
    GFL_UNUSED( pos );
  }

  /**
   * @brief   配置済みスタンプの破棄
   *
   * @param   index   スタンプのインデックス(0～9)
   */
  virtual void DeleteOldStamp( u8 index )
  {
    GFL_UNUSED( index );
  }

  /**
   * @brief   フレームの設定
   *
   * @param   frameIndex    フレームの種類
   */
  virtual void SetFrame( u32 frameIndex )
  {
    GFL_UNUSED( frameIndex );
  }

  /**
   * @brief   スタンプの配置が可能か？
   *
   * @retval  "true"なら配置可能
   * @retval  "false"なら配置不可
   */
  virtual bool IsCanPutStamp( void )
  {
    return true;
  }
};

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATION_VIEW_LISTENER_H_INCLUDED__
