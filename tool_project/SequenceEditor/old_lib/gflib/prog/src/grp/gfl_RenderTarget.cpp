//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_RenderTarget.cpp
 *	@brief  レンダーターゲット
 *	@author	Koji Kawada
 *	@date		2011.03.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>
#include <grp/gfl_RenderTarget.h>
#include <grp/gfl_OffScreenDepthRenderTarget.h>
#include <grp/gfl_DirectAddressRenderTarget.h>


namespace gfl {
namespace grp {

//-----------------------------------------------------------------------------
/**
 *					クラス定義
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  OnScreenRenderTarget用Descriptionの初期化
 *
 *	通常のレンダーターゲットです。
 *
 *	カラーバッファあり。
 *	デプスバッファあり。
 *
 *  @param  p_desc                データ設定先
 *	@param	width                 幅
 *	@param	height                高さ
 *	@param	color_format          カラーフォーマット
 *	@param	depth_format          デプスフォーマット
 *	@param	color_area            カラーバッファのメモリ配置場所
 *	@param	depth_area            デプスバッファのメモリ配置場所
 *	@param	display_type          ディスプレイタイプ
 */
//-----------------------------------------------------------------------------
void RenderTarget::InitializeOnScreenDescription( Description* p_desc, s32 width, s32 height, gfl::grp::RenderColorFormat color_format, gfl::grp::RenderDepthFormat depth_format, gfl::grp::MemoryArea color_area, gfl::grp::MemoryArea depth_area, gfl::grp::DisplayType display_type )
{
  p_desc->type          = TYPE_ON_SCREEN_BUFFER;
  p_desc->width         = width;
  p_desc->height        = height;
  p_desc->color_format  = color_format;
  p_desc->depth_format  = depth_format;
  p_desc->color_area    = color_area;
  p_desc->depth_area    = depth_area;
  p_desc->display_type    = display_type;
  p_desc->color_address = 0;
  p_desc->depth_address = 0;
  p_desc->mipmap_size   = 1;
  p_desc->color_transfer= gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP;
  p_desc->executing_memory_fill = false;
  p_desc->is_dynamic_allocation = false;
  p_desc->fob_id          = 0;
  p_desc->shadow_kind   = gfl::grp::SHADOW_KIND_NONE;
  p_desc->is_perspective_shadow = false;
  p_desc->shadow_z_bias   = 0.0f;
  p_desc->shadow_z_scale  = 1.0f;
}

//----------------------------------------------------------------------------
/**
 *	@brief  OffScreenRenderTarget用のDescriptionを初期化
 *
 * カラーバッファをテクスチャとして使用することができるタイプのレンダーターゲットです。
 * デプスバッファはありません。
 *
 *	カラーバッファを通常テクスチャとして用意
 *	デプスバッファなし。
 *
 *  @param  p_desc                データ設定先
 *	@param	width                 幅
 *	@param	height                高さ
 *	@param	color_format          カラーフォーマット
 *	@param	color_area            カラーバッファメモリエリア
 *	@param	display_type          ディスプレイタイプ
 *  @param  mipmap_size           ミップマップサイズ
 *	@param	color_transfer        テクスチャのメモリ転送タイプ
 *	@param  executing_memory_fill 確保したピクセルを0でクリアするか？ する場合true
 *	@param  is_dynamic_allocation 動的にFCRAM上に領域を確保するか？ する場合true
 */
//-----------------------------------------------------------------------------
void RenderTarget::InitializeOffScreenDescription( Description* p_desc, s32 width, s32 height, gfl::grp::RenderColorFormat color_format, gfl::grp::MemoryArea color_area, gfl::grp::DisplayType display_type, s32 mipmap_size, gfl::grp::MemoryTransfer color_transfer, b32 executing_memory_fill, b32 is_dynamic_allocation )
{
  p_desc->type          = TYPE_OFF_SCREEN_BUFFER;
  p_desc->width         = width;
  p_desc->height        = height;
  p_desc->color_format  = color_format;
  p_desc->depth_format  = gfl::grp::RENDER_DEPTH_FORMAT_NONE;
  p_desc->color_area    = color_area;
  p_desc->depth_area    = gfl::grp::MEMORY_AREA_NONE;
  p_desc->color_address = 0;
  p_desc->depth_address = 0;
  p_desc->mipmap_size   = mipmap_size;
  p_desc->color_transfer= color_transfer;
  p_desc->executing_memory_fill = executing_memory_fill;
  p_desc->is_dynamic_allocation = is_dynamic_allocation;
  p_desc->display_type    = display_type;
  p_desc->fob_id          = 0;
  p_desc->shadow_kind   = gfl::grp::SHADOW_KIND_NONE;
  p_desc->is_perspective_shadow = false;
  p_desc->shadow_z_bias   = 0.0f;
  p_desc->shadow_z_scale  = 1.0f;
}



//----------------------------------------------------------------------------
/**
 *	@brief  OffScreenDepthRenderTarget用のDescriptionを初期化
 *
 * カラーバッファをテクスチャとして使用することができるタイプのレンダーターゲットです。
 * デプスバッファも用意します。
 *
 *	カラーバッファを通常テクスチャとして用意
 *	デプスバッファあり。
 *
 *  @param  p_desc                データ設定先
 *	@param	width                 幅
 *	@param	height                高さ
 *	@param	color_format          カラーフォーマット
 *	@param	depth_format          デプスフォーマット
 *	@param	color_area            カラーバッファメモリエリア
 *	@param	depth_area            デプスバッファメモリエリア
 *	@param	display_type          ディスプレイタイプ
 *  @param  mipmap_size           ミップマップサイズ
 *	@param	color_transfer        テクスチャのメモリ転送タイプ
 *	@param  executing_memory_fill 確保したピクセルを0でクリアするか？ する場合true
 *	@param  is_dynamic_allocation 動的にFCRAM上に領域を確保するか？ する場合true
 */
//-----------------------------------------------------------------------------
void RenderTarget::InitializeOffScreenDepthDescription( Description* p_desc, s32 width, s32 height, gfl::grp::RenderColorFormat color_format, gfl::grp::RenderDepthFormat depth_format, gfl::grp::MemoryArea color_area, gfl::grp::MemoryArea depth_area, gfl::grp::DisplayType display_type, s32 mipmap_size, gfl::grp::MemoryTransfer color_transfer, b32 executing_memory_fill, b32 is_dynamic_allocation )
{
  p_desc->type          = TYPE_OFF_SCREEN_BUFFER;
  p_desc->width         = width;
  p_desc->height        = height;
  p_desc->color_format  = color_format;
  p_desc->depth_format  = depth_format;
  p_desc->color_area    = color_area;
  p_desc->depth_area    = depth_area;
  p_desc->color_address = 0;
  p_desc->depth_address = 0;
  p_desc->mipmap_size   = mipmap_size;
  p_desc->color_transfer= color_transfer;
  p_desc->executing_memory_fill = executing_memory_fill;
  p_desc->is_dynamic_allocation = is_dynamic_allocation;
  p_desc->display_type    = display_type;
  p_desc->fob_id          = 0;
  p_desc->shadow_kind   = gfl::grp::SHADOW_KIND_NONE;
  p_desc->is_perspective_shadow = false;
  p_desc->shadow_z_bias   = 0.0f;
  p_desc->shadow_z_scale  = 1.0f;
}


//----------------------------------------------------------------------------
/**
 *	@brief  DirectAddressRenderTarget用のDescriptionを初期化
 *
 *  @param  p_desc                データ設定先
 *	@param	width                 幅
 *	@param	height                高さ
 *	@param	color_format          カラーフォーマット
 *	@param	depth_format          デプスフォーマット
 *	@param	color_address         カラーバッファアドレス
 *	@param	depth_address         デプスバッファアドレス
 *	@param	display_type          ディスプレイタイプ
 *	@param	fob_id                確保済みGL用フレームバッファオブジェクト（ない場合には0をいれてください）
 */
//-----------------------------------------------------------------------------
void RenderTarget::InitializeDirectAddressDescription( Description* p_desc, s32 width, s32 height, gfl::grp::RenderColorFormat color_format, gfl::grp::RenderDepthFormat depth_format, u32 color_address, u32 depth_address, gfl::grp::DisplayType display_type, GLuint fob_id )
{
  p_desc->type          = TYPE_ON_SCREEN_BUFFER;
  p_desc->width         = width;
  p_desc->height        = height;
  p_desc->color_format  = color_format;
  p_desc->depth_format  = depth_format;
  p_desc->color_area    = gfl::grp::MEMORY_AREA_NONE;
  p_desc->depth_area    = gfl::grp::MEMORY_AREA_NONE;
  p_desc->color_address = color_address;
  p_desc->depth_address = depth_address;
  p_desc->shadow_kind   = gfl::grp::SHADOW_KIND_NONE;
  p_desc->mipmap_size   = 1;
  p_desc->color_transfer= gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP;
  p_desc->executing_memory_fill = false;
  p_desc->is_dynamic_allocation = false;
  p_desc->is_perspective_shadow = false;
  p_desc->shadow_z_bias   = 0.0f;
  p_desc->shadow_z_scale  = 1.0f;
  p_desc->display_type    = display_type;
  p_desc->fob_id          = fob_id;
}


//----------------------------------------------------------------------------
/**
 *	@brief  Descriptionにシャドウテクスチャに関する部分の設定を行う。
 *	        ＊必ず、Initialzie～関数後によんでください。
 *
 *	@param	p_desc                データ設定先
 *	@param  shadow_kind           シャドウの種別
 *	@param  is_perspective_shadow 投影シャドウの場合はtrue
 *	@param  shadow_z_bias         シャドウテクスチャのZバイアス値
 *	@param  shadow_z_scale        オフセット値に乗算するスケール
 */
//-----------------------------------------------------------------------------
void RenderTarget::SetShadowTextureDescription( Description* p_desc, gfl::grp::ShadowType shadow_kind, b32 is_perspective_shadow, f32 shadow_z_bias, f32 shadow_z_scale )
{
  p_desc->shadow_kind   = shadow_kind;
  p_desc->is_perspective_shadow = is_perspective_shadow;
  p_desc->shadow_z_bias   = shadow_z_bias;
  p_desc->shadow_z_scale  = shadow_z_scale;
}


//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 *                 Createは呼ばれません。
 */
//-----------------------------------------------------------------------------
RenderTarget::RenderTarget(void)
  : m_buffer_type(TYPE_NONE),
    m_display_type(gfl::grp::DISPLAY_NONE),
    m_texture_owner(TEXTURE_OWNER_NONE),
    m_depth_format(RENDER_DEPTH_FORMAT_NONE),
    m_color_format(RENDER_COLOR_FORMAT_NONE),
    m_depth_area(MEMORY_AREA_NONE),
    m_nw_render_target(NULL),
    m_nw_res_texture(NULL),
    m_view_port()
{
  // 何もしない
}

//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 *                 Createを呼びます。
 */
//-----------------------------------------------------------------------------
RenderTarget::RenderTarget( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description )
  : m_buffer_type(TYPE_NONE),
    m_display_type(gfl::grp::DISPLAY_NONE),
    m_texture_owner(TEXTURE_OWNER_NONE),
    m_depth_format(RENDER_DEPTH_FORMAT_NONE),
    m_color_format(RENDER_COLOR_FORMAT_NONE),
    m_depth_area(MEMORY_AREA_NONE),
    m_nw_render_target(NULL),
    m_nw_res_texture(NULL),
    m_view_port()
{
  if( description ) m_depth_area = description->depth_area;
  this->Create( heap_allocator, device_allocator, description );
}
RenderTarget::RenderTarget( gfl::heap::NwAllocator* device_allocator, const Description* description )
  : m_buffer_type(TYPE_NONE),
    m_display_type(gfl::grp::DISPLAY_NONE),
    m_texture_owner(TEXTURE_OWNER_NONE),
    m_depth_format(RENDER_DEPTH_FORMAT_NONE),
    m_color_format(RENDER_COLOR_FORMAT_NONE),
    m_depth_area(MEMORY_AREA_NONE),
    m_nw_render_target(NULL),
    m_nw_res_texture(NULL),
    m_view_port()
{
  // OffScreenBufferだった場合の対処
  if( description ) m_depth_area = description->depth_area;
  if( description ){
    if( description->type != TYPE_ON_SCREEN_BUFFER){
      GFL_ASSERT(description->type == TYPE_ON_SCREEN_BUFFER); //@check
      // OnScreen出ない場合、デバイスメモリアロケータのみで生成
      this->Create( device_allocator, device_allocator, description );
      return ;
    }
  }
  
  this->CreateOnScreenBuffer( device_allocator, description );
}
// @note gfl::grp::g3d::Textureを渡す関数をつくったら廃止予定
RenderTarget::RenderTarget( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, nw::gfx::res::ResTexture nw_res_texture, const Description* description )
  : m_buffer_type(TYPE_NONE),
    m_display_type(gfl::grp::DISPLAY_NONE),
    m_texture_owner(TEXTURE_OWNER_NONE),
    m_depth_format(RENDER_DEPTH_FORMAT_NONE),
    m_color_format(RENDER_COLOR_FORMAT_NONE),
    m_depth_area(MEMORY_AREA_NONE),
    m_nw_render_target(NULL),
    m_nw_res_texture(NULL),
    m_view_port()
{
  m_texture_owner = TEXTURE_OWNER_USER;
  if( description ) m_depth_area = description->depth_area;
  this->CreateOffScreenBuffer( heap_allocator, device_allocator, nw_res_texture, description );
}

//-----------------------------------------------------------------------------
/*
 *  @brief         デストラクタ
 *                 Destroyを呼びます。
 */
//-----------------------------------------------------------------------------
RenderTarget::~RenderTarget()
{
  this->Destroy();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         生成
 */
//-----------------------------------------------------------------------------
void RenderTarget::Create( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description )
{
  Description l_description;
  if( description )
  {
    l_description = *description;
  }
  
  if( l_description.type == TYPE_OFF_SCREEN_BUFFER )
  {
    m_texture_owner = TEXTURE_OWNER_THIS;

    //m_nw_res_texture = reinterpret_cast<nw::gfx::res::ResTexture*>( GflHeapAllocMemory( heap_allocator->GetHeapBase(), sizeof(nw::gfx::res::ResTexture) ) ); 
    //CreateOffScreenBufferでGflHeapAllocMemoryしているので、ここでは不要。
    nw::gfx::res::ResTexture l_nw_res_texture;


    // 通常テクスチャの生成
    if( l_description.shadow_kind == gfl::grp::SHADOW_KIND_NONE )
    {
      nw::gfx::res::ResPixelBasedTexture::FormatHW format;
      switch(l_description.color_format)
      {
      case gfl::grp::RENDER_COLOR_FORMAT_RGBA8:   format = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA8;     break;
      case gfl::grp::RENDER_COLOR_FORMAT_RGB8:    format = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGB8;      break;
      case gfl::grp::RENDER_COLOR_FORMAT_RGBA4:   format = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA4;     break;
      case gfl::grp::RENDER_COLOR_FORMAT_RGB5_A1: format = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA5551;  break;
      case gfl::grp::RENDER_COLOR_FORMAT_RGB565:  format = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGB565;    break;
      default:                                    GFL_ASSERT_STOP(0);                                               break; 
      }

      nw::gfx::res::ResImageTexture res_texture;
      res_texture = nw::gfx::res::ResImageTexture::DynamicBuilder()
          .Height(l_description.width)  // レンダーターゲット用のテクスチャなので縦と横を入れ替える。
          .Width(l_description.height)
          .MipmapSize(l_description.mipmap_size)
          .LocationFlag(l_description.color_area | l_description.color_transfer)
          .LocationAddress(l_description.color_address)
          .Format(format)
          .ExecutingMemoryFill(l_description.executing_memory_fill)
          .DynamicAllocation(l_description.is_dynamic_allocation)
          .Create(device_allocator);
      res_texture.Setup(device_allocator, nw::gfx::ResGraphicsFile(NULL));
      l_nw_res_texture = res_texture;
     
    }
    // 影用テクスチャの生成
    else if( l_description.shadow_kind == gfl::grp::SHADOW_KIND_TEXTURE )
    {
      nw::gfx::res::ResShadowTexture res_texture;
      res_texture = nw::gfx::res::ResShadowTexture::DynamicBuilder()
          .Height(l_description.width)  // レンダーターゲット用のテクスチャなので縦と横を入れ替える。
          .Width(l_description.height)
          .LocationFlag(l_description.color_area | l_description.color_transfer)
          .LocationAddress(l_description.color_address)
          .ExecutingMemoryFill(l_description.executing_memory_fill)
          .PerspectiveShadow(l_description.is_perspective_shadow)  // Orthoカメラの場合はr値による除算を行わないようにします。
          .ShadowZBias(l_description.shadow_z_bias)                // (定数 * near / (far - near)) くらいがいいかも。
          .DynamicAllocation(l_description.is_dynamic_allocation)
          .Create(device_allocator);
      res_texture.Setup(device_allocator, nw::gfx::ResGraphicsFile(NULL));
      l_nw_res_texture = res_texture;
      
    }
    // キューブマップ用テクスチャの生成
    else if( l_description.shadow_kind == gfl::grp::SHADOW_KIND_CUBE )
    {
      // 未対応
      GFL_ASSERT_STOP(0);
    }


    // OFFスクリーンバッファの生成処理
    this->CreateOffScreenBuffer( heap_allocator, device_allocator, l_nw_res_texture, &l_description );

  }
  else  // if( l_description.type == TYPE_ON_SCREEN_BUFFER )
  {
    // ONスクリーンバッファの生成
    this->CreateOnScreenBuffer(device_allocator, description);
  }
}

void RenderTarget::CreateOnScreenBuffer( gfl::heap::NwAllocator* device_allocator, const Description* description)
{
  Description l_description;
  if( description )
  {
    l_description = *description;
  }

  // バッファのタイプ、フォーマットなどを保存
  m_buffer_type   = l_description.type;
  m_depth_format  = l_description.depth_format;
  m_display_type  = l_description.display_type;
  m_color_format  = l_description.color_format;

  // ビューポート設定
  SetDescriptionViewport(&l_description, &m_view_port);
  
  // アドレス指定と、メモリ確保で、生成するクラスを切り替える。
  if( (l_description.color_address == 0) && (l_description.depth_address == 0) ){

    // オンスクリーンバッファは縦と横が逆になっているため、
    // 幅と高さを逆に設定しています。
    m_nw_render_target = nw::gfx::IRenderTarget::Builder()
        .BufferSize( l_description.height, l_description.width )
        .ColorFormat( static_cast<nw::gfx::RenderColorFormat>(l_description.color_format) )
        .DepthFormat( static_cast<nw::gfx::RenderDepthFormat>(l_description.depth_format) )
        .ColorArea( static_cast<nw::gfx::GraphicsMemoryArea>(l_description.color_area) )
        .DepthArea( static_cast<nw::gfx::GraphicsMemoryArea>(l_description.depth_area) )
        .Create(device_allocator);

  }else{

    // オンスクリーンバッファは縦と横が逆になっているため、
    // 幅と高さを逆に設定しています。
    m_nw_render_target = gfl::grp::DirectAddressRenderTarget::Create( 
        device_allocator, 
        l_description.height, l_description.width, 
        l_description.color_format, l_description.depth_format, 
        l_description.color_address, l_description.depth_address,
        l_description.fob_id );

  }

  m_nw_res_texture = NULL;
}

// @note gfl::grp::g3d::Textureを渡す関数をつくったら廃止予定
void RenderTarget::CreateOffScreenBuffer( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, nw::gfx::res::ResTexture nw_res_texture, const Description* description )
{
  Description l_description;
  if( description )
  {
    l_description = *description;
  }

  // バッファタイプとデプスフォーマットの保存
  m_buffer_type   = l_description.type;
  m_depth_format  = l_description.depth_format;
  m_display_type  = l_description.display_type;
  m_color_format  = l_description.color_format;

  if( m_texture_owner == TEXTURE_OWNER_NONE )  // この関数が呼ばれた時点でTEXTURE_OWNER_NONEのときは、ユーザーがこの関数を呼び出している。
  {
    m_texture_owner = TEXTURE_OWNER_USER;
  }

  // ビューポート設定
  SetDescriptionViewport(&l_description, &m_view_port);
 
  if( l_description.depth_format == RENDER_DEPTH_FORMAT_NONE )
  {
    m_nw_render_target = nw::gfx::IRenderTarget::CreateOffScreenBuffer( device_allocator, nw_res_texture );
  }
  else
  {
    m_nw_render_target = gfl::grp::OffScreenDepthRenderTarget::Create( device_allocator, nw_res_texture, l_description.depth_format, l_description.depth_area, l_description.depth_address );
  }

  m_nw_res_texture = reinterpret_cast<nw::gfx::res::ResTexture*>( GflHeapAllocMemory( heap_allocator->GetHeapBase(), sizeof(nw::gfx::res::ResTexture) ) ); 
 
  GFL_ASSERT_STOP_MSG( m_nw_res_texture, "nw::gfx::res::ResTextureのメモリが確保できませんでした。\n" );
  if( m_nw_res_texture )
  {
    *m_nw_res_texture = nw_res_texture;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         破棄
 *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
 */
//-----------------------------------------------------------------------------
void RenderTarget::Destroy(void)
{
  if( m_nw_render_target )
  {
    nw::gfx::SafeDestroy(m_nw_render_target);
    m_nw_render_target = NULL;

    if( m_nw_res_texture )
    {
      if( m_texture_owner == TEXTURE_OWNER_THIS )
      {
        if(m_nw_res_texture->IsValid())
        {
          m_nw_res_texture->Cleanup();

          {
            nw::gfx::res::ResPixelBasedTexture texture = nw::ut::ResStaticCast<nw::gfx::res::ResPixelBasedTexture>(m_nw_res_texture->Dereference());
            switch(texture.GetTypeInfo())
            {
            case nw::gfx::res::ResImageTexture::TYPE_INFO:
              {
                nw::gfx::res::ResImageTexture res_texture = nw::ut::ResDynamicCast<nw::gfx::res::ResImageTexture>(*m_nw_res_texture);
                res_texture.DynamicDestroy();
              }
              break;
            case nw::gfx::res::ResShadowTexture::TYPE_INFO:
              {
                nw::gfx::res::ResShadowTexture res_texture = nw::ut::ResDynamicCast<nw::gfx::res::ResShadowTexture>(*m_nw_res_texture);
                res_texture.DynamicDestroy();
              }
              break;
            default:
              {
                GFL_ASSERT_STOP(0);
              }
              break;
            }
          }
        }
      }
      GflHeapFreeMemory( m_nw_res_texture );
    }
  }
  m_buffer_type   = TYPE_NONE;
  m_display_type  = gfl::grp::DISPLAY_NONE;
  m_texture_owner = TEXTURE_OWNER_NONE;
  m_color_format  = RENDER_COLOR_FORMAT_NONE;
  m_depth_format  = RENDER_DEPTH_FORMAT_NONE;
  m_nw_render_target = NULL;
  m_nw_res_texture = NULL;
}

// @note gfl::grp::g3d::Textureを返す関数をつくったら廃止予定
nw::gfx::res::ResTexture  RenderTarget::GetNwResTexture(void) const  // バッファをこのクラスにつくらせた場合は縦横が入れ替わった絵が得られます。
{
  GFL_ASSERT_STOP_MSG( m_nw_res_texture, "m_nw_res_textureが存在しません。\n" );
  return *m_nw_res_texture;
}

// nw::gfx::FrameBufferObject相当の機能
//-----------------------------------------------------------------------------
/*
 *  @brief         フレームバッファの設定用コマンドを設定する
 */
//-----------------------------------------------------------------------------
void RenderTarget::ActivateBuffer( void ) const
{
  if(m_nw_render_target){
    
    // フレームバッファのバインド
    m_nw_render_target->GetBufferObject().ActivateBuffer();
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         フレームバッファのクリアを実行する
 *                 クリアコマンドを積みたいコマンドリストをバインドしてから使用して下さい。
 *                 GraphicsSystemにあるクリア関数を使用することを推奨します。
 */
//-----------------------------------------------------------------------------
void RenderTarget::ClearBuffer(
    b32 b_color,
    const gfl::grp::ColorF32& clear_color,
    b32 b_depth,
    f32 clear_depth,
    b32 b_stencil,
    u8 clear_stencil )
{
  u32 mask = 0;
  mask |= (b_color)  ? (GL_COLOR_BUFFER_BIT)   : (0);
  mask |= (b_depth)  ? (GL_DEPTH_BUFFER_BIT)   : (0);
  mask |= (b_stencil)? (GL_STENCIL_BUFFER_BIT) : (0);
 
  //nngxFlush3DCommand();
  
  if ( mask )
  {//何かしらのビットが立っていたら実行
    m_nw_render_target->GetBufferObject().ClearBuffer(
        mask, 
        clear_color, 
        clear_depth,
        clear_stencil );
  }
  
  //nngxFlush3DCommand();
}

//----------------------------------------------------------------------------
/**
 *	@brief  フレームバッファの情報をメモリに出力
 *
 *	@param	type    バッファタイプ
 *	@param  memory  出力先アドレス( デバイスメモリかつ16バイトアライメント限定 )
 *
 *	*内部でカレントバッファを変更します。
 *  バッファをこのクラスにつくらせた場合は縦横が入れ替わった絵が得られます。
 */
//-----------------------------------------------------------------------------
void RenderTarget::OutputImage( RenderBufferType type, void* memory )
{
  u32 format_size;
  
#if 0
  //バインド処理
  //OpenGLの関数でキャプチャするので、
  //FrameBufferのバインドにもOpenGLを使用する。
  {
    GFL_ASSERT_STOP( this->GetFboID() != 0 ); // GL用バッファが確保されているかチェック
    glBindFramebuffer( GL_FRAMEBUFFER, this->GetFboID() );
  }
#endif

  nw::gfx::IRenderTarget::Description nw_description = m_nw_render_target->GetDescription();

  //メモリにコピー
  if(type == RENDERBUFFER_TYPE_COLOR){
//    GLenum colorFormat;
//    GLenum colorType;
    switch( nw_description.colorFormat ){
    case nw::gfx::RENDER_COLOR_FORMAT_RGBA8:
//      colorFormat = GL_RGBA;
//      colorType   = GL_UNSIGNED_BYTE;
      format_size = 4;
      break;
    case nw::gfx::RENDER_COLOR_FORMAT_RGB8:
//      colorFormat = GL_RGB;
//      colorType   = GL_UNSIGNED_BYTE;
      format_size = 3;
      break;
    case nw::gfx::RENDER_COLOR_FORMAT_RGBA4:
//      colorFormat = GL_RGBA;
//      colorType   = GL_UNSIGNED_SHORT_4_4_4_4;
      format_size = 2;
      break;
    case nw::gfx::RENDER_COLOR_FORMAT_RGB5_A1:
//      colorFormat = GL_RGBA;
//      colorType   = GL_UNSIGNED_SHORT_5_5_5_1;
      format_size = 2;
      break;
    case nw::gfx::RENDER_COLOR_FORMAT_RGB565:
//      colorFormat = GL_RGB;
//      colorType   = GL_UNSIGNED_SHORT_5_6_5;
      format_size = 2;
      break;
    default:
      //おかしい
      GFL_ASSERT(0);
//      colorFormat = GL_RGBA;
//      colorType   = GL_UNSIGNED_BYTE;
      format_size = 4;
      break;
    }
    
    u32 size = nw_description.width * nw_description.height * format_size;
    size = math::RoundUp<u32>( size, 16 ); // 16バイトアライメント
    
    nngxAddBlockImageCopyCommand( 
        reinterpret_cast<GLvoid*>( GetColorAddress() ),
        size, 0,
        memory,
        size, 0,
        size );

#if 0
    glReadPixels( 0, 0, nw_description.width, nw_description.height, 
        colorFormat, colorType, memory );
#endif
    GFL_GRPGL_ERROR_CHECK();
  }else{
//    GLenum depthFormat;
//    GLenum depthType;
    switch( nw_description.depthFormat ){
    case nw::gfx::RENDER_DEPTH_FORMAT_16:
//      depthFormat = GL_DEPTH_COMPONENT;
//      depthType   = GL_UNSIGNED_SHORT;
      format_size = 2;
      break;
    case nw::gfx::RENDER_DEPTH_FORMAT_24:
//      depthFormat = GL_DEPTH_COMPONENT;
//      depthType   = GL_UNSIGNED_INT24_DMP;
      format_size = 3;
      break;
    case nw::gfx::RENDER_DEPTH_FORMAT_24_STENCIL8:
//      depthFormat = GL_DEPTH24_STENCIL8_EXT;
//      depthType   = GL_UNSIGNED_INT;
      format_size = 4;
      break;
    default:
      //おかしい
      GFL_ASSERT(0);
//      depthFormat = GL_DEPTH_COMPONENT;
//      depthType   = GL_UNSIGNED_SHORT;
      format_size = 2;
      break;
    }
    GFL_ASSERT_STOP( nw_description.depthFormat != RENDER_DEPTH_FORMAT_NONE );
    
    u32 size = nw_description.width * nw_description.height * format_size;
    size = math::RoundUp<u32>( size, 16 ); // 16バイトアライメント
    
    nngxAddBlockImageCopyCommand( 
        reinterpret_cast<GLvoid*>( GetDepthAddress() ),
        size, 0,
        memory,
        size, 0,
        size );
  
#if 0
    glReadPixels( 0, 0, nw_description.width, nw_description.height, 
        depthFormat, depthType, memory );
#endif
    
    GFL_GRPGL_ERROR_CHECK();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  フレームバッファの情報をメモリにリニア出力
 *
 *	@param	type    バッファタイプ
 *	@param  memory  出力先アドレス( デバイスメモリかつ16バイトアライメント限定 )
 */
//-----------------------------------------------------------------------------
void RenderTarget::OutputB2LImage( RenderBufferType type, void* memory )
{
  u32 format_size;
  
  nw::gfx::IRenderTarget::Description nw_description = m_nw_render_target->GetDescription();

  //メモリにコピー
  if(type == RENDERBUFFER_TYPE_COLOR)
  {
    GLenum colorFormat;

    switch( nw_description.colorFormat ){
    case nw::gfx::RENDER_COLOR_FORMAT_RGBA8:
      colorFormat = GL_RGBA8_OES;
      format_size = 4;
      break;
    case nw::gfx::RENDER_COLOR_FORMAT_RGB8:
      colorFormat = GL_RGB8_OES;
      format_size = 3;
      break;
    case nw::gfx::RENDER_COLOR_FORMAT_RGBA4:
      colorFormat = GL_RGBA8_OES;
      format_size = 2;
      break;
    case nw::gfx::RENDER_COLOR_FORMAT_RGB5_A1:
      colorFormat = GL_RGBA8_OES;
      format_size = 2;
      break;
    case nw::gfx::RENDER_COLOR_FORMAT_RGB565:
      colorFormat = GL_RGB8_OES;
      format_size = 2;
      break;
    default:
      //おかしい
      GFL_ASSERT(0);
      colorFormat = GL_RGB8_OES;
      format_size = 4;
      break;
    }
    
    u32 size = nw_description.width * nw_description.height * format_size;
    size = math::RoundUp<u32>( size, 16 ); // 16バイトアライメント
    
    nngxAddB2LTransferCommand(
      reinterpret_cast<GLvoid*>( GetColorAddress() ),
                              nw_description.width,
                              nw_description.height,
                              colorFormat,
                              memory,
                              nw_description.width,
                              nw_description.height,
                              colorFormat,
                              NN_GX_ANTIALIASE_NOT_USED,
                              false,
                              DEFAULT_BLOCK_IMAGE_SIZE
      );
    GFL_GRPGL_ERROR_CHECK();
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  カラーバッファに情報をコピー（ブロックモード）
 *
 *	@param  memory  入力アドレス
 */
//-----------------------------------------------------------------------------
void RenderTarget::InputImage( const void* memory )
{
  GLvoid* dstadr;
  u32 format_size = 0;
  u32 size;
  
  dstadr = reinterpret_cast<GLvoid*>( this->GetColorAddress() );
  
  switch( this->GetRenderColorFormat() )
  {
  case RENDER_COLOR_FORMAT_RGBA8 : 
  case RENDER_COLOR_FORMAT_RGB8 :
    format_size = 4;
    break;
  case RENDER_COLOR_FORMAT_RGBA4 :
  case RENDER_COLOR_FORMAT_RGB5_A1 :
  case RENDER_COLOR_FORMAT_RGB565 :
    format_size = 2;
    break;
  case RENDER_COLOR_FORMAT_NONE :
  default : 
    GFL_ASSERT(0);
    break;
  }
  
  size = this->GetWidth() * this->GetHeight() * format_size;
  //nngxFlush3DCommand();
  nngxAddVramDmaCommand( memory, dstadr, size );
}

#if GFL_HIO_ENABLE
//----------------------------------------------------------------------------
/**
 *	@brief  フレームバッファの情報をファイルに出力
 *
 *	@param	file_path    出力パス
 *	@param	type		バッファタイプ
 *	@param  allocator  ワークの確保先
 *
 *	*内部でカレントバッファを変更します。
 *  バッファをこのクラスにつくらせた場合は縦横が入れ替わった絵が得られます。
 */
//-----------------------------------------------------------------------------
void RenderTarget::OutputImageFile( const char* file_path, RenderBufferType type, gfl::heap::NwAllocator* allocator )
{
	struct TgaFormat
	{
		u8	No_ID;				/* 0=イメージＩＤ無し*/
		u8	CMap_Type;		/* 0=カラーマップ無し */
		u8	Image_Type;		/* 2=無圧縮RGBタイプ 10=圧縮RGBタイプ */
		u8	CMap_Spec[5];	/* スキップされる*/

		s16	Ox;						/* 画面 左上 X 0固定 */
		s16	Oy;						/* 画面 左上 Y 0固定 */

		s16	Width;				/* 幅 (dot)*/
		s16	Hight;				/* 高さ (dot)*/

		u8	Pixel;				/* Pixel Size. 32=32bit */
		u8	IDesc_Type;		/* 8=TGA32 アトリビュート */
	};

	TgaFormat		header;
	int					headersize = sizeof(TgaFormat);

  nw::gfx::IRenderTarget::Description nw_description = m_nw_render_target->GetDescription();
	
  memset( &header, 0, sizeof(TgaFormat) );
	header.Width = nw_description.width;
	header.Hight = nw_description.height;
	header.Image_Type = 2;

	//メモリにコピー
  if(type == RENDERBUFFER_TYPE_COLOR)
	{
    switch( nw_description.colorFormat ){
    case nw::gfx::RENDER_COLOR_FORMAT_RGBA8:
		case nw::gfx::RENDER_COLOR_FORMAT_RGBA4:
		case nw::gfx::RENDER_COLOR_FORMAT_RGB5_A1:
      header.Pixel = 32;
      break;
    case nw::gfx::RENDER_COLOR_FORMAT_RGB8:
		case nw::gfx::RENDER_COLOR_FORMAT_RGB565:
      header.Pixel = 24;
      break;
    default:
      //おかしい
      GFL_ASSERT(0);
      header.Pixel = 32;
      break;
    }
  }
	else
	{
    switch( nw_description.depthFormat ){
    case nw::gfx::RENDER_DEPTH_FORMAT_16:
      header.Pixel = 16;
      break;
    case nw::gfx::RENDER_DEPTH_FORMAT_24:
      header.Pixel = 24;
      break;
    case nw::gfx::RENDER_DEPTH_FORMAT_24_STENCIL8:
      header.Pixel = 32;
      break;
    default:
      //おかしい
      GFL_ASSERT(0);
      header.Pixel = 32;
      break;
    }
	}

	s32			imageSize = nw_description.height * nw_description.width * (header.Pixel / 8);
	void* buf = allocator->Alloc( imageSize );

	this->OutputImage( type, buf );

	if ( header.Pixel == 32 )
	{
		ConvertPixelFormat( reinterpret_cast<char*>(buf), GL_RGBA, header.Width, header.Hight );
	}
	else
	{
		ConvertPixelFormat( reinterpret_cast<char*>(buf), GL_RGB, header.Width, header.Hight );
	}

	nn::hio::HostFile hostFile;

	nn::Result result = hostFile.Open( file_path, nn::hio::HostFile::ACCESS_MODE_READ_WRITE, nn::hio::HostFile::OPEN_DISP_CREATE_ALWAYS );
	hostFile.Write( &header, sizeof(TgaFormat) );
	hostFile.Write( buf, imageSize );
	hostFile.Close();

	allocator->Free( buf );
}
#endif

//-----------------------------------------------------------------------------
/*
 *  @brief         glのフレームバッファオブジェクトのIDを取得する
 */
//-----------------------------------------------------------------------------
GLuint RenderTarget::GetFboID(void) const
{
  return m_nw_render_target->GetBufferObject().GetFboID();
}

//----------------------------------------------------------------------------
/**
 *	@brief        カラーバッファのアドレスを取得
 *	@return       カラーバッファのアドレス
 */
//-----------------------------------------------------------------------------
u32 RenderTarget::GetColorAddress( void ) const
{
  return m_nw_render_target->GetBufferObject().GetColorAddress();
}

//----------------------------------------------------------------------------
/**
 *	@brief        デプスバッファのアドレスを取得
 *	@return       デプスバッファのアドレス
 */
//-----------------------------------------------------------------------------
u32 RenderTarget::GetDepthAddress( void ) const
{
  return m_nw_render_target->GetBufferObject().GetDepthAddress();
}


//----------------------------------------------------------------------------
/**
 *	@brief        幅の取得
 *	@return       幅
 */
//-----------------------------------------------------------------------------
s32 RenderTarget::GetWidth( void ) const
{
  if(    ( m_buffer_type == TYPE_ON_SCREEN_BUFFER )
      || ( m_buffer_type == TYPE_OFF_SCREEN_BUFFER && m_texture_owner == TEXTURE_OWNER_THIS ) )
  {
    // レンダーターゲットは縦横が入れ替わっている。
    return m_nw_render_target->GetBufferObject().GetHeight();
  }
  else
  {
    return m_nw_render_target->GetBufferObject().GetWidth();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief        高さの取得
 *	@return       高さ
 */
//-----------------------------------------------------------------------------
s32 RenderTarget::GetHeight( void ) const
{
  if(    ( m_buffer_type == TYPE_ON_SCREEN_BUFFER )
      || ( m_buffer_type == TYPE_OFF_SCREEN_BUFFER && m_texture_owner == TEXTURE_OWNER_THIS ) )
  {
    // レンダーターゲットは縦横が入れ替わっている。
    return m_nw_render_target->GetBufferObject().GetWidth();
  }
  else
  {
    return m_nw_render_target->GetBufferObject().GetHeight();
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief      ビューポート情報の取得
 *
 *  @param[out]  left      スクリーン　左座標
 *  @param[out]  top       スクリーン　上座標
 *  @param[out]  right     スクリーン　右座標
 *  @param[out]  bottom    スクリーン　下座標
 */
//-----------------------------------------------------------------------------
void RenderTarget::GetViewport( f32* left, f32* top, f32* right, f32* bottom ) const
{
  if(    ( m_buffer_type == TYPE_ON_SCREEN_BUFFER )
      || ( m_buffer_type == TYPE_OFF_SCREEN_BUFFER && m_texture_owner == TEXTURE_OWNER_THIS ) )
  {
    // レンダーターゲットは縦横が入れ替わっている。
    m_view_port.GetBound( top, left, bottom, right );
  }
  else
  {
    m_view_port.GetBound( left, top, right, bottom );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief      ビューポート情報の設定
 *
 *  @param  left      スクリーン　左座標
 *  @param  top       スクリーン　上座標
 *  @param  right     スクリーン　右座標
 *  @param  bottom    スクリーン　下座標
 */
//-----------------------------------------------------------------------------
void RenderTarget::SetViewport( f32 left, f32 top, f32 right, f32 bottom )
{
  if(    ( m_buffer_type == TYPE_ON_SCREEN_BUFFER )
      || ( m_buffer_type == TYPE_OFF_SCREEN_BUFFER && m_texture_owner == TEXTURE_OWNER_THIS ) )
  {
    // レンダーターゲットは縦横が入れ替わっている。
    m_view_port.SetBound( top, left, bottom, right );
  }
  else
  {
    m_view_port.SetBound( left, top, right, bottom );
  }
}


//-----------------------------------------------------------------------------
/*
 */
//-----------------------------------------------------------------------------
void RenderTarget::ConvertPixelFormat( char* p_data, GLenum format, int w, int h )
{
	switch( format ){
  case GL_RGBA8_OES:
    for( int y = 0; y < h; ++y )
		{
			for( int x = 0; x < w; ++x )
			{
				char	a = p_data[0];
        char	r = p_data[1];
        char	g = p_data[2];
        char	b = p_data[3];
				p_data[0] = r;
				p_data[1] = g;
        p_data[2] = b;
        p_data[3] = a;
				p_data += 4;
			}
		}
    break;
	case GL_RGB:
		for( int y = 0; y < h; ++y )
		{
			for( int x = 0; x < w; ++x )
			{
				char	r = p_data[0];
				p_data[0] = p_data[2];
				p_data[2] = r;
				p_data += 3;
			}
		}
		break;
	default:
		for( int y = 0; y < h; ++y )
		{
			for( int x = 0; x < w; ++x )
			{
				char	r = p_data[0];
				p_data[0] = p_data[2];
				p_data[2] = r;
				p_data += 4;
			}
		}
		break;
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief  DescriptionからViewport情報を生成
 *
 *	@param	description   
 *	@param	viewport      ビューポート格納先
 */
//-----------------------------------------------------------------------------
void RenderTarget::SetDescriptionViewport( const Description* description, Viewport* viewport )
{

  // Descriptionのwidthとheightを使用して、
  // Viewportを生成する。
  
  const f32 x = 0.0f;
  const f32 y = 0.0f;
  const f32 depth_range_near  = 0.0f;
  const f32 depth_range_far   = 1.0f;
  
  if(    ( m_buffer_type == TYPE_ON_SCREEN_BUFFER )
      || ( m_buffer_type == TYPE_OFF_SCREEN_BUFFER && m_texture_owner == TEXTURE_OWNER_THIS ) )
  {
    // レンダーターゲットは縦横が入れ替わっている。
    viewport->SetBound( y, x, description->height, description->width );
  }
  else
  {
    viewport->SetBound( x, y, description->width, description->height );
  }

  viewport->SetDepthRange( depth_range_near, depth_range_far );
}


}  // namespace grp
}  // namespace gfl

