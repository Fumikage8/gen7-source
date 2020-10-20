//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Viewport.cpp
 *	@brief  ビューポート
 *	@author	Koji Kawada
 *	@date		2011.03.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>
#include <nw/ut.h>

// gflib
#include <gfl_Macros.h>

// gflib grp
#include <grp/gfl_Viewport.h>


namespace gfl {
namespace grp {

//-----------------------------------------------------------------------------
/**
 *					クラス定義
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	ビューポートクラス
//=====================================
Viewport::Viewport(void)
  : m_nw_viewport()
{
  // 何もしない
}

Viewport::Viewport(
    f32 left,
    f32 top,
    f32 right,
    f32 bottom,
    f32 near,
    f32 far)
  : m_nw_viewport(nw::ut::Rect(left,top,right,bottom),near,far)
{
  GFL_ASSERT(left>=0.0f&&top>=0.0f);  // x、yはOpenGLES2.0標準と異なり、0以上の値でなければなりません。  //@check 画面のどこに出るが分からないが続行可能。
}
  
Viewport::~Viewport()
{
  // 何もしない
}

void Viewport::SetBound(
    f32 left,
    f32 top,
    f32 right,
    f32 bottom )
{
  GFL_ASSERT(left>=0.0f&&top>=0.0f);  // x、yはOpenGLES2.0標準と異なり、0以上の値でなければなりません。  //@check 画面のどこに出るが分からないが続行可能。
  m_nw_viewport.SetBound( nw::ut::Rect(left,top,right,bottom) );
}

void Viewport::GetBound(
    f32* left,
    f32* top,
    f32* right,
    f32* bottom ) const
{
  const nw::ut::Rect& rect = m_nw_viewport.GetBound();
  *left   = rect.left;
  *top    = rect.top;
  *right  = rect.right;
  *bottom = rect.bottom;
}

void Viewport::SetNear(f32 near)
{
  f32 far = m_nw_viewport.GetDepthFar();
  m_nw_viewport.SetDepthRange( near, far );
}

f32 Viewport::GetNear(void) const
{
  return m_nw_viewport.GetDepthNear();
}

void Viewport::SetFar(f32 far)
{
  f32 near = m_nw_viewport.GetDepthNear();
  m_nw_viewport.SetDepthRange( near, far );
}

f32 Viewport::GetFar(void) const
{
  return m_nw_viewport.GetDepthFar();
}

void Viewport::SetDepthRange(f32 near, f32 far)
{
  m_nw_viewport.SetDepthRange( near, far );
}


}  // namespace grp
}  // namespace gfl
