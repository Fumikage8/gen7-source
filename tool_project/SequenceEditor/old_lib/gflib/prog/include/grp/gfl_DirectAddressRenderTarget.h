//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DirectAddressRenderTarget.h
 *	@brief  直接のaddress指定レンダーターゲット
 *	@author tomoya takahashi	
 *	@date		2011.03.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __GFL_DIRECTADDRESSRENDERTARGET_H__
#define __GFL_DIRECTADDRESSRENDERTARGET_H__
#pragma once


#include <nn.h>

#include <nw.h>
#include <nw/gfx.h>

#include <grp/gfl_GraphicsSystem.h>

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
///	ダイレクトアドレス　レンダーターゲット
//=====================================
class DirectAddressRenderTarget : public nw::gfx::IRenderTarget
{
  GFL_FORBID_COPY_AND_ASSIGN(DirectAddressRenderTarget);

public:
  // 設定の取得
  virtual const nw::gfx::IRenderTarget::Description& GetDescription() const { return m_Description; }

  // フレームバッファオブジェクトの取得
  virtual const nw::gfx::FrameBufferObject& GetBufferObject() const { return m_BackBufferObject; }

  
  //----------------------------------------------------------------------------
  /**
   *	@brief  生成
   *  
   *	@param	allocator     デバイスアロケータ
   *	@param  width         バッファ幅
   *	@param  height        バッファ高さ
   *	@param  color_format  カラーバッファフォーマット
   *	@param  depth_format  デプスバッファフォーマット
   *	@param  color_address カラーバッファアドレス
   *	@param  depth_address デプスバッファアドレス
   *	@param  fboID         フレームバッファID  （GL用　なければ０）
   */
  //-----------------------------------------------------------------------------
  static DirectAddressRenderTarget* Create( gfl::heap::NwAllocator* allocator, s32 width, s32 height, gfl::grp::RenderColorFormat color_format, gfl::grp::RenderDepthFormat depth_format, u32 color_address, u32 depth_address, GLuint fboID = 0 );

private:

  //コンストラクタ
  DirectAddressRenderTarget( nw::os::IAllocator* pAllocator, const nw::gfx::IRenderTarget::Description& description, GLuint fboID , u32 color_address, u32 depth_address );

  // デストラクタ
  virtual ~DirectAddressRenderTarget();


  nw::gfx::FrameBufferObject m_BackBufferObject;
  void* m_ActivateCommand;
  
  nw::gfx::IRenderTarget::Description m_Description;
  
  friend class nw::gfx::IRenderTarget;
  
};


}  // namespace grp
}  // namespace gfl

#endif	// __GFL_DIRECTADDRESSRENDERTARGET_H__

