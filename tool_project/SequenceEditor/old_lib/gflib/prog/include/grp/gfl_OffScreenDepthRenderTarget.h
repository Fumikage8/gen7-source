#ifndef __GFL_OFFSCREENDEPTHRENDERTARGET_H__
#define __GFL_OFFSCREENDEPTHRENDERTARGET_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_OffScreenDepthRenderTarget.h
 *	@brief  オフスクリーンデプスありレンダーターゲット
 *	@author	tomoya takahashi
 *	@date		2010.12.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <nn.h>

// NW4C
#include <nw.h>
#include <nw/gfx.h>

//  gflib grp
#include <grp/gfl_GraphicsSystem.h>

// gflib
#include <gfl_Heap.h>

namespace gfl {
namespace grp {

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	オフスクリーンレンダラーターゲット　デプスバッファあり。
//=====================================
class OffScreenDepthRenderTarget : public nw::gfx::IRenderTarget
{
  GFL_FORBID_COPY_AND_ASSIGN(OffScreenDepthRenderTarget);

public:

  // 設定の取得
  virtual const nw::gfx::IRenderTarget::Description& GetDescription() const { return m_Description; }

  // フレームバッファオブジェクトの取得
  virtual const nw::gfx::FrameBufferObject& GetBufferObject() const { return m_BackBufferObject; }

  //----------------------------------------------------------------------------
  /**
   *	@brief  生成
   *  
   *	@param	allocator     アロケータ
   *	@param  resTexture    テクスチャリソース
   *	@param  depth_format  デプスバッファフォーマット
   *	@param  depth_area    デプスバッファを確保するメモリタイプ
   *	@param  depthAddress  確保済みアドレス指定。０のときアロケートします。（デフォルト０）
   */
  //-----------------------------------------------------------------------------
  static OffScreenDepthRenderTarget* Create( gfl::heap::NwAllocator* allocator, nw::gfx::res::ResTexture resTexture, gfl::grp::RenderDepthFormat depth_format, gfl::grp::MemoryArea depth_area, u32 depthAddress = 0 );

private:

  //コンストラクタ
  OffScreenDepthRenderTarget( nw::os::IAllocator* pAllocator, const nw::gfx::IRenderTarget::Description& description, nw::gfx::res::ResPixelBasedTexture resTexture , u32 depthAddress );

  // デストラクタ
  virtual ~OffScreenDepthRenderTarget();


  nw::gfx::FrameBufferObject m_BackBufferObject;
  void* m_ActivateCommand;
  
  nw::gfx::IRenderTarget::Description m_Description;

  nw::gfx::res::ResTexture m_Texture;

  u32 m_DepthBufferID;
  u32 m_DepthAddr;
  
  friend class nw::gfx::IRenderTarget;
};


}  // namespace grp
}  // namespace gfl


#endif // __GFL_OFFSCREENDEPTHRENDERTARGET_H__
