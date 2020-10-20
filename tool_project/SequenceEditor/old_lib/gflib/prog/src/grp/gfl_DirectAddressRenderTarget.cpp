//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DirectAddressRenderTarget.cpp
 *	@brief  アドレス直指定のレンダーターゲット
 *	@author	takahashi tomoya
 *	@date		2011.03.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include <grp/gfl_DirectAddressRenderTarget.h>

namespace gfl {
namespace grp {

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  直値アドレス指定　レンダーターゲット生成
 *
 *	@param	allocator         デバイスアロケータ
 *	@param  width         バッファ幅
 *	@param  height        バッファ高さ
 *	@param  color_format  カラーバッファフォーマット
 *	@param  depth_format  デプスバッファフォーマット
 *	@param  color_address カラーバッファアドレス
 *	@param  depth_address デプスバッファアドレス
 *	@param  fboID         フレームバッファID  （GL用　なければ０）
 *
 *	@return レンダーターゲット
 */
//-----------------------------------------------------------------------------
DirectAddressRenderTarget* DirectAddressRenderTarget::Create( gfl::heap::NwAllocator* allocator, s32 width, s32 height, gfl::grp::RenderColorFormat color_format, gfl::grp::RenderDepthFormat depth_format, u32 color_address, u32 depth_address, GLuint fboID )
{
  const int renderTargetAlignment = 32;
  void* memory = allocator->Alloc(sizeof(DirectAddressRenderTarget), renderTargetAlignment);
  GFL_ASSERT_STOP(memory);
  

  nw::gfx::IRenderTarget::Description description;
  
  description.width   = width;
  description.height  = height;
  description.colorFormat = static_cast<nw::gfx::RenderColorFormat>(color_format);
  description.depthFormat = static_cast<nw::gfx::RenderDepthFormat>(depth_format);

  return GFL_NEW_PLACEMENT(memory) DirectAddressRenderTarget( allocator, description, fboID , color_address , depth_address );
}


//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 *
 *	@param	pAllocator    アロケータ
 *	@param  description   初期化データ
 *	@param  fboID         GL用フレームバッファID
 */
//-----------------------------------------------------------------------------
DirectAddressRenderTarget::DirectAddressRenderTarget( nw::os::IAllocator* pAllocator, const nw::gfx::IRenderTarget::Description& description, GLuint fboID , u32 color_address, u32 depth_address ) :
  nw::gfx::IRenderTarget( pAllocator ),
  m_ActivateCommand(NULL),
  m_Description(description)
{

  m_BackBufferObject.SetHeight( m_Description.height );
  m_BackBufferObject.SetWidth( m_Description.width );
  m_BackBufferObject.SetColorFormat( m_Description.colorFormat );
  m_BackBufferObject.SetColorAddress( color_address );
  m_BackBufferObject.SetDepthFormat( m_Description.depthFormat );
  m_BackBufferObject.SetDepthAddress( depth_address );
  
  // GL用フレームバッファIDを指定（大本のバッファIDなので、破棄しないように注意！
  m_BackBufferObject.SetFboID(fboID);
}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
DirectAddressRenderTarget::~DirectAddressRenderTarget()
{
  // このレンダーターゲットが確保したフレームバッファオブジェクトでないので、クリア
  m_BackBufferObject.SetFboID(0);
}



}  // namespace grp
}  // namespace gfl
