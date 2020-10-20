#ifndef __GFL_VIEWPORT_H__
#define __GFL_VIEWPORT_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Viewport.h
 *	@brief  ビューポート
 *	@author	Koji Kawada
 *	@date		2011.03.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>


namespace gfl {
namespace grp {

  
// 前方宣言
//class FrameBuffer;  // @note ここにGetNwViewportを利用するクラスを書いて下さい。
class RenderTarget;
namespace g3d {
  class RenderSystem;
}  // namespace g3d


//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	ビューポートクラス
//=====================================
class Viewport
{
  GFL_FORBID_COPY_AND_ASSIGN(Viewport);


  // 下記クラスにだけprivateメンバ関数へのアクセスを許す
  //friend class gfl::grp::FrameBuffer;  // @note ここにGetNwViewportを利用するクラスを書いて下さい。
  friend class gfl::grp::RenderTarget;
  friend class gfl::grp::g3d::RenderSystem;




public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ / デストラクタ
   */
  //-----------------------------------------------------------------------------
  Viewport(void);
  Viewport(
      f32 left,
      f32 top,
      f32 right,
      f32 bottom,
      f32 near = 0.0f,
      f32 far  = 1.0f);
  virtual ~Viewport();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         取得 / 設定
   */
  //-----------------------------------------------------------------------------
  void SetBound(
      f32 left,
      f32 top,
      f32 right,
      f32 bottom );
  void GetBound(
      f32* left,
      f32* top,
      f32* right,
      f32* bottom ) const;
  void SetNear(f32 near);
  f32 GetNear(void) const;
  void SetFar(f32 far);
  f32 GetFar(void) const;
  void SetDepthRange(f32 near, f32 far);

  
 
  
  // 以下のメンバ関数はthis及びfriendからしかアクセスしません
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         取得
   */
  //-----------------------------------------------------------------------------
  const nw::gfx::Viewport& GetNwViewport(void) const
  {
    return m_nw_viewport;
  }

  


  // 以下のメンバ変数はthisからしかアクセスしません
private:
  nw::gfx::Viewport  m_nw_viewport;
};

}  // namespace grp
}  // namespace gfl

#endif // __GFL_VIEWPORT_H__
