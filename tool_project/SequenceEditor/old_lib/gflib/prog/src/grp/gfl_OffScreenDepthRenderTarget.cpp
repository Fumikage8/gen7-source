//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_OffScreenDepthRenderTarget.cpp
 *	@brief  オフスクリーンデプスありレンダーターゲット
 *	@author	tomoya takahashi
 *	@date		2010.12.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <grp/gfl_OffScreenDepthRenderTarget.h>

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
 *	@brief  生成
 *  
 *	@param	allocator     アロケータ
 *	@param  resTexture    テクスチャリソース
 *	@param  depth_format  デプスバッファフォーマット
 *	@param  depth_area    デプスバッファを確保するメモリタイプ
 */
//-----------------------------------------------------------------------------
OffScreenDepthRenderTarget* OffScreenDepthRenderTarget::Create(gfl::heap::NwAllocator* allocator, nw::gfx::res::ResTexture resTexture, gfl::grp::RenderDepthFormat depth_format, gfl::grp::MemoryArea depth_area, u32 depthAddress )
{
  nw::gfx::res::ResPixelBasedTexture texture = nw::ut::ResStaticCast<nw::gfx::res::ResPixelBasedTexture>(resTexture.Dereference());

  const int renderTargetAlignment = 32;
  void* memory = allocator->Alloc(sizeof(OffScreenDepthRenderTarget), renderTargetAlignment);
  GFL_ASSERT_STOP(memory);


  const nw::gfx::RenderColorFormat formatTable[] =
  {
    nw::gfx::RENDER_COLOR_FORMAT_RGBA8,
    nw::gfx::RENDER_COLOR_FORMAT_RGB8,
    nw::gfx::RENDER_COLOR_FORMAT_RGB5_A1,
    nw::gfx::RENDER_COLOR_FORMAT_RGB565,
    nw::gfx::RENDER_COLOR_FORMAT_RGBA4,
    nw::gfx::RENDER_COLOR_FORMAT_NONE,
    nw::gfx::RENDER_COLOR_FORMAT_NONE,
    nw::gfx::RENDER_COLOR_FORMAT_NONE,
    nw::gfx::RENDER_COLOR_FORMAT_NONE,
    nw::gfx::RENDER_COLOR_FORMAT_NONE,
    nw::gfx::RENDER_COLOR_FORMAT_NONE,
    nw::gfx::RENDER_COLOR_FORMAT_NONE,
    nw::gfx::RENDER_COLOR_FORMAT_NONE,
    nw::gfx::RENDER_COLOR_FORMAT_NONE
  };

  nw::gfx::IRenderTarget::Description description;
  description.width = texture.GetWidth();
  description.height = texture.GetHeight();
  description.colorFormat = formatTable[texture.GetFormatHW()];
  GFL_ASSERT(description.colorFormat != nw::gfx::RENDER_COLOR_FORMAT_NONE);
  description.colorArea = static_cast<nw::gfx::GraphicsMemoryArea>(texture.GetLocationFlag() & (MEMORY_AREA_FCRAM | MEMORY_AREA_VRAMA | MEMORY_AREA_VRAMB));

  description.depthFormat = static_cast<nw::gfx::RenderDepthFormat>(depth_format);
  description.depthArea = static_cast<nw::gfx::GraphicsMemoryArea>(depth_area);

  switch (texture.GetTypeInfo())
  {
  case nw::gfx::res::ResImageTexture::TYPE_INFO:
    {
      description.shadowKind = nw::gfx::SHADOW_KIND_NONE;
    }
    break;
  case nw::gfx::res::ResCubeTexture::TYPE_INFO:
    {
      description.shadowKind = nw::gfx::SHADOW_KIND_NONE;
    }
    break;
  case nw::gfx::res::ResShadowTexture::TYPE_INFO:
    {
      description.shadowKind = nw::gfx::SHADOW_KIND_TEXTURE;
    }
    break;
  default:
    {
      GFL_ASSERT_STOP(0);
    }
    break;
  }

  return GFL_NEW_PLACEMENT(memory) OffScreenDepthRenderTarget( allocator, description, texture , depthAddress );
}
    
//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 *  
 *	@param	allocator     アロケータ
 *	@param  resTexture    テクスチャリソース
 *	@param  depth_format  デプスバッファフォーマット
 *	@param  depth_area    デプスバッファを確保するメモリタイプ
 */
//-----------------------------------------------------------------------------
OffScreenDepthRenderTarget::OffScreenDepthRenderTarget( nw::os::IAllocator* pAllocator, const nw::gfx::IRenderTarget::Description& description, nw::gfx::res::ResPixelBasedTexture resTexture , u32 depthAddress ) :
  nw::gfx::IRenderTarget( pAllocator ),
  m_ActivateCommand(NULL),
  m_Description(description),
  m_Texture(resTexture),
  m_DepthBufferID(0)
{
  nw::gfx::res::ResPixelBasedTexture resPixelBasedTexture = nw::ut::ResStaticCast<nw::gfx::res::ResPixelBasedTexture>(resTexture);

  GFL_ASSERT_STOP(resPixelBasedTexture.GetTextureObject() != 0);
  

  m_BackBufferObject.SetHeight( resTexture.GetHeight() );
  m_BackBufferObject.SetWidth( resTexture.GetWidth() );
  
  GLuint format;
  
  switch ( resTexture.GetFormatHW() )
  {
  case nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA8:    format = GL_RGBA8_OES; break;
  case nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA5551: format = GL_RGB5_A1; break;
  case nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA4:    format = GL_RGBA4; break;
  case nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGB565:   format = GL_RGB565; break;
  default:
      GFL_ASSERT_STOP_MSG(0, "illegal texture format for OffScreenBuffer");
      break;
  }
  
  m_BackBufferObject.SetColorFormat( format );

  switch (resTexture.GetTypeInfo())
  {
  case nw::gfx::res::ResImageTexture::TYPE_INFO:
    {
      u32 address = nw::ut::ResStaticCast<nw::gfx::res::ResImageTexture>(resTexture).GetImage().GetImageAddress();
      m_BackBufferObject.SetColorAddress( address );
    }
    break;
  case nw::gfx::res::ResCubeTexture::TYPE_INFO:
    {
      nw::gfx::res::ResCubeTexture::CubeFace face = nw::gfx::res::ResCubeTexture::CUBE_FACE_POSITIVE_X;
      u32 address = nw::ut::ResStaticCast<nw::gfx::res::ResCubeTexture>(resTexture).GetImage( face ).GetImageAddress();
      
      m_BackBufferObject.SetColorAddress( address );
    }
    break;
  case nw::gfx::res::ResShadowTexture::TYPE_INFO:
    {
      u32 address = nw::ut::ResStaticCast<nw::gfx::res::ResShadowTexture>(resTexture).GetImage().GetImageAddress();
      m_BackBufferObject.SetColorAddress( address );
    }
    break;
  default:
    {
      GFL_ASSERT_STOP(0);
    }
    break;
  }
  if ( depthAddress )
  {
    m_DepthAddr = depthAddress;
  }
  else
  {
    // デプスバッファの確保
    glGenRenderbuffers(1, &m_DepthBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBufferID);
    glRenderbufferStorage(
        GL_RENDERBUFFER | description.depthArea,
        description.depthFormat,
        description.width,
        description.height);

    GFL_GRPGL_ERROR_CHECK();


    //確保したデプスバッファの情報を設定

    glBindRenderbuffer( GL_RENDERBUFFER, m_DepthBufferID );
    GFL_GRPGL_ERROR_CHECK();
    glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_DATA_ADDR_DMP, reinterpret_cast<GLint*>(&m_DepthAddr) );
    GFL_GRPGL_ERROR_CHECK();
    glBindRenderbuffer( GL_RENDERBUFFER, 0 );
    GFL_GRPGL_ERROR_CHECK();
  }
  
  m_BackBufferObject.SetDepthFormat( description.depthFormat );
  m_BackBufferObject.SetDepthAddress( m_DepthAddr );

  GFL_GRPGL_ERROR_CHECK();

}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
OffScreenDepthRenderTarget::~OffScreenDepthRenderTarget()
{
  if (m_DepthBufferID != 0)
  {
    glDeleteRenderbuffers(1, &m_DepthBufferID);
  }
}


}  // namespace grp
}  // namespace gfl
