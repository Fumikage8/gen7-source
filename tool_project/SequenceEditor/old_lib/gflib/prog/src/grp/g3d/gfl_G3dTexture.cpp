//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dTexture.cpp
 *	@brief  3Dテクスチャ
 *	@author	Koji Kawada
 *	@date		2011.04.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Base.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSystem.h>

#include <grp/g3d/gfl_G3dTexture.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス定義
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3Dテクスチャクラス
//=====================================

//-----------------------------------------------------------------------------
/**
 *           staticメンバ関数
 */
//-----------------------------------------------------------------------------
// gfl::grp::g3d::Texture::Format -> gfl::grp::RenderColorFormat
gfl::grp::RenderColorFormat    Texture::ConvertTextureFormatToRenderColorFormat(gfl::grp::g3d::Texture::Format texture_format)
{
  gfl::grp::RenderColorFormat render_color_format = gfl::grp::RENDER_COLOR_FORMAT_NONE;
  switch(texture_format)
  {
  case gfl::grp::g3d::Texture::FORMAT_RGBA8    :
    {
      render_color_format = gfl::grp::RENDER_COLOR_FORMAT_RGBA8;
    }
    break;
  case gfl::grp::g3d::Texture::FORMAT_RGB8     :
    {
      render_color_format = gfl::grp::RENDER_COLOR_FORMAT_RGB8;
    }
    break;
  case gfl::grp::g3d::Texture::FORMAT_RGBA5551 :
    {
      render_color_format = gfl::grp::RENDER_COLOR_FORMAT_RGB5_A1;
    }
    break;
  case gfl::grp::g3d::Texture::FORMAT_RGB565   :
    {
      render_color_format = gfl::grp::RENDER_COLOR_FORMAT_RGB565;
    }
    break;
  case gfl::grp::g3d::Texture::FORMAT_RGBA4    : 
    {
      render_color_format = gfl::grp::RENDER_COLOR_FORMAT_RGBA4;
    }
    break;
  case gfl::grp::g3d::Texture::FORMAT_LA8      :
  case gfl::grp::g3d::Texture::FORMAT_HILO8    :
  case gfl::grp::g3d::Texture::FORMAT_L8       :
  case gfl::grp::g3d::Texture::FORMAT_A8       :
  case gfl::grp::g3d::Texture::FORMAT_LA4      :
  case gfl::grp::g3d::Texture::FORMAT_L4       :
  case gfl::grp::g3d::Texture::FORMAT_A4       :
  case gfl::grp::g3d::Texture::FORMAT_ETC1     :
  case gfl::grp::g3d::Texture::FORMAT_ETC1A4   :
    {
      GFL_ASSERT_MSG(0, "gfl::grp::g3d::Texture::Format %dに対応するgfl::grp::RenderColorFormatがありません。\n", texture_format);
      render_color_format = gfl::grp::RENDER_COLOR_FORMAT_NONE;
    }
    break;
  default:
    {
      GFL_ASSERT_MSG(0, "gfl::grp::g3d::Texture::Format %dが不明です。\n", texture_format);
      render_color_format = gfl::grp::RENDER_COLOR_FORMAT_NONE;
    }
    break;
  };
  return render_color_format;
}

// gfl::grp::RenderColorFormat    -> gfl::grp::g3d::Texture::Format
gfl::grp::g3d::Texture::Format Texture::ConvertRenderColorFormatToTextureFormat(gfl::grp::RenderColorFormat    render_color_format)
{
  gfl::grp::g3d::Texture::Format texture_format = gfl::grp::g3d::Texture::FORMAT_L4;  // NONEがないので1ピクセルあたりのサイズが最小となるフォーマットにしておく。
  switch(render_color_format)
  {
  case gfl::grp::RENDER_COLOR_FORMAT_NONE    :
    {
      GFL_ASSERT_MSG(0, "gfl::grp::RenderColorFormat %dに対応するgfl::grp::g3d::Texture::Formatがありません。\n", render_color_format);
      texture_format = gfl::grp::g3d::Texture::FORMAT_L4;  // NONEがないので1ピクセルあたりのサイズが最小となるフォーマットにしておく。
    }
    break;
  case gfl::grp::RENDER_COLOR_FORMAT_RGBA8   :
    {
      texture_format = gfl::grp::g3d::Texture::FORMAT_RGBA8;
    }
    break;
  case gfl::grp::RENDER_COLOR_FORMAT_RGB8    :
    {
      texture_format = gfl::grp::g3d::Texture::FORMAT_RGB8;
    }
    break;
  case gfl::grp::RENDER_COLOR_FORMAT_RGBA4   :
    {
      texture_format = gfl::grp::g3d::Texture::FORMAT_RGBA4;
    }
    break;
  case gfl::grp::RENDER_COLOR_FORMAT_RGB5_A1 :
    {
      texture_format = gfl::grp::g3d::Texture::FORMAT_RGBA5551;
    }
    break;
  case gfl::grp::RENDER_COLOR_FORMAT_RGB565  :
    {
      texture_format = gfl::grp::g3d::Texture::FORMAT_RGB565;
    }
    break;
  default:
    {
      GFL_ASSERT_MSG(0, "gfl::grp::RenderColorFormat %dが不明です。\n", render_color_format);
      texture_format = gfl::grp::g3d::Texture::FORMAT_L4;  // NONEがないので1ピクセルあたりのサイズが最小となるフォーマットにしておく。
    }
    break;
  }
  return texture_format;
}

// formatのテクスチャの1ピクセルのビットサイズを得る(バイトサイズではありません)
u8 Texture::GetTextureBitsPerPixel(Format format)
{
  // 値が変わってしまっていないか確認する
  STATIC_ASSERT( FORMAT_RGBA8    ==  0 );
  STATIC_ASSERT( FORMAT_RGB8     ==  1 );
  STATIC_ASSERT( FORMAT_RGBA5551 ==  2 );
  STATIC_ASSERT( FORMAT_RGB565   ==  3 );
  STATIC_ASSERT( FORMAT_RGBA4    ==  4 );
  STATIC_ASSERT( FORMAT_LA8      ==  5 );
  STATIC_ASSERT( FORMAT_HILO8    ==  6 );
  STATIC_ASSERT( FORMAT_L8       ==  7 );
  STATIC_ASSERT( FORMAT_A8       ==  8 );
  STATIC_ASSERT( FORMAT_LA4      ==  9 );
  STATIC_ASSERT( FORMAT_L4       == 10 );
  STATIC_ASSERT( FORMAT_A4       == 11 );
  STATIC_ASSERT( FORMAT_ETC1     == 12 );
  STATIC_ASSERT( FORMAT_ETC1A4   == 13 );
  STATIC_ASSERT( FORMAT_MAX      == 14 );

  static const u8 texture_bits_per_pixel_table[FORMAT_MAX] =
  {
    32,
    24,
    16,
    16,
    16,
    16,
    16,
     8,
     8,
     8,
     4,
     4,
     4,
     8,
  };

  return texture_bits_per_pixel_table[format];
}

  
//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 *                 Createは呼ばれません。
 */
//-----------------------------------------------------------------------------
Texture::Texture(void)
  : m_nw_res_texture(),
    m_is_create(false)
{
  GFL_SINGLETON_INSTANCE(System)->GetTextureManager()->AddObject(this);

  // キャストして直接コマンドとして使っていることもあるので、値が一致しているかの確認をコンパイル時にしておく。
  STATIC_ASSERT( FORMAT_RGBA8    == PICA_DATA_TEX_FORMAT_RGBA8  );
  STATIC_ASSERT( FORMAT_RGB8     == PICA_DATA_TEX_FORMAT_RGB8   );
  STATIC_ASSERT( FORMAT_RGBA5551 == PICA_DATA_TEX_FORMAT_RGB5A1 );
  STATIC_ASSERT( FORMAT_RGB565   == PICA_DATA_TEX_FORMAT_RGB565 );
  STATIC_ASSERT( FORMAT_RGBA4    == PICA_DATA_TEX_FORMAT_RGBA4  );
  STATIC_ASSERT( FORMAT_LA8      == PICA_DATA_TEX_FORMAT_LA8    );
  STATIC_ASSERT( FORMAT_HILO8    == PICA_DATA_TEX_FORMAT_HILO8  );
  STATIC_ASSERT( FORMAT_L8       == PICA_DATA_TEX_FORMAT_L8     );
  STATIC_ASSERT( FORMAT_A8       == PICA_DATA_TEX_FORMAT_A8     );
  STATIC_ASSERT( FORMAT_LA4      == PICA_DATA_TEX_FORMAT_LA4    );
  STATIC_ASSERT( FORMAT_L4       == PICA_DATA_TEX_FORMAT_L4     );
  STATIC_ASSERT( FORMAT_A4       == PICA_DATA_TEX_FORMAT_A4     );
  STATIC_ASSERT( FORMAT_ETC1     == PICA_DATA_TEX_FORMAT_ETC1   );
  STATIC_ASSERT( FORMAT_ETC1A4   == PICA_DATA_TEX_FORMAT_ETC1A4 );

  STATIC_ASSERT( FORMAT_RGBA8    == gfl::grp::g3d::TEXTURE_FORMAT_RGBA8    );
  STATIC_ASSERT( FORMAT_RGB8     == gfl::grp::g3d::TEXTURE_FORMAT_RGB8     );
  STATIC_ASSERT( FORMAT_RGBA5551 == gfl::grp::g3d::TEXTURE_FORMAT_RGBA5551 );
  STATIC_ASSERT( FORMAT_RGB565   == gfl::grp::g3d::TEXTURE_FORMAT_RGB565   );
  STATIC_ASSERT( FORMAT_RGBA4    == gfl::grp::g3d::TEXTURE_FORMAT_RGBA4    );
  STATIC_ASSERT( FORMAT_LA8      == gfl::grp::g3d::TEXTURE_FORMAT_LA8      );
  STATIC_ASSERT( FORMAT_HILO8    == gfl::grp::g3d::TEXTURE_FORMAT_HILO8    );
  STATIC_ASSERT( FORMAT_L8       == gfl::grp::g3d::TEXTURE_FORMAT_L8       );
  STATIC_ASSERT( FORMAT_A8       == gfl::grp::g3d::TEXTURE_FORMAT_A8       );
  STATIC_ASSERT( FORMAT_LA4      == gfl::grp::g3d::TEXTURE_FORMAT_LA4      );
  STATIC_ASSERT( FORMAT_L4       == gfl::grp::g3d::TEXTURE_FORMAT_L4       );
  STATIC_ASSERT( FORMAT_A4       == gfl::grp::g3d::TEXTURE_FORMAT_A4       );
  STATIC_ASSERT( FORMAT_ETC1     == gfl::grp::g3d::TEXTURE_FORMAT_ETC1     );
  STATIC_ASSERT( FORMAT_ETC1A4   == gfl::grp::g3d::TEXTURE_FORMAT_ETC1A4   );

  // 配列添え字として使っているので、値を変更されては困るから、コンパイル時にチェックする。
  STATIC_ASSERT( nw::gfx::res::ResCubeTexture::CUBE_FACE_POSITIVE_X == 0 );
  STATIC_ASSERT( nw::gfx::res::ResCubeTexture::CUBE_FACE_NEGATIVE_X == 1 );
  STATIC_ASSERT( nw::gfx::res::ResCubeTexture::CUBE_FACE_POSITIVE_Y == 2 );
  STATIC_ASSERT( nw::gfx::res::ResCubeTexture::CUBE_FACE_NEGATIVE_Y == 3 );
  STATIC_ASSERT( nw::gfx::res::ResCubeTexture::CUBE_FACE_POSITIVE_Z == 4 );
  STATIC_ASSERT( nw::gfx::res::ResCubeTexture::CUBE_FACE_NEGATIVE_Z == 5 );
  STATIC_ASSERT( nw::gfx::res::ResCubeTexture::MAX_CUBE_FACE        == 6 );      
}
//-----------------------------------------------------------------------------
/*
 *  @brief         デストラクタ
 *                 Destroyを呼びます。
 */
//-----------------------------------------------------------------------------
Texture::~Texture()
{
  Destroy();
  GFL_SINGLETON_INSTANCE(System)->GetTextureManager()->RemoveObject(this);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         生成
 */
//-----------------------------------------------------------------------------
void Texture::Create(
    gfl::heap::NwAllocator*    /*heap_allocator*/,
    gfl::heap::NwAllocator*    device_allocator,
    const Description*         description
)
{
  if(!(m_nw_res_texture.IsValid()))
  {
    Description l_description;
    if( description )
    {
      l_description = *description;
    }
 
    {
      switch( l_description.type )
      {
      case TYPE_IMAGE_TEXTURE:  m_nw_res_texture = CreateImageTexture (device_allocator, &l_description); m_is_create = true; break;
      case TYPE_CUBE_TEXTURE:   m_nw_res_texture = CreateCubeTexture  (device_allocator, &l_description); m_is_create = true; break;
      case TYPE_SHADOW_TEXTURE: m_nw_res_texture = CreateShadowTexture(device_allocator, &l_description); m_is_create = true; break;
      }
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "既にテクスチャを生成済みです。\n");
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         生成
 *                 レンダーターゲットをテクスチャとする
 *                 これで生成したTextureインスタンス生存中は、引数で渡したrender_targetも保持しておいて下さい。
 *                 縦横注意！
 *
 *	@param  description        レンダーターゲット
 */
//-----------------------------------------------------------------------------
void Texture::Create(
    const gfl::grp::RenderTarget*  render_target
)
{
  if(!(m_nw_res_texture.IsValid()))
  {
    m_nw_res_texture = render_target->GetNwResTexture();
  }
  else
  {
    GFL_ASSERT_MSG(0, "既にテクスチャを生成済みです。\n");
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         破棄
 *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
 */
//-----------------------------------------------------------------------------
void Texture::Destroy(void)
{
  {
    if( m_is_create )
    {
      if(m_nw_res_texture.IsValid())
      {
        nw::gfx::res::ResPixelBasedTexture texture = nw::ut::ResStaticCast<nw::gfx::res::ResPixelBasedTexture>(m_nw_res_texture.Dereference());
        switch(texture.GetTypeInfo())
        {
        case nw::gfx::res::ResImageTexture::TYPE_INFO:  DestroyImageTexture(m_nw_res_texture);  break;
        case nw::gfx::res::ResCubeTexture::TYPE_INFO:   DestroyCubeTexture(m_nw_res_texture);   break;
        case nw::gfx::res::ResShadowTexture::TYPE_INFO: DestroyShadowTexture(m_nw_res_texture); break;
        }
      }
    }

    m_nw_res_texture = nw::gfx::res::ResTexture();
    m_is_create = false;
  }
}



//
// privateメンバ関数
//
nw::gfx::res::ResTexture Texture::CreateImageTexture(
    gfl::heap::NwAllocator*    device_allocator,
    const Description*         description
)
{
  nw::gfx::res::ResImageTexture res_texture;
  res_texture = nw::gfx::res::ResImageTexture::DynamicBuilder()
      .Height(description->height)
      .Width(description->width)
      .MipmapSize(description->mipmap_size)
      .LocationFlag(description->memory_area | description->memory_transfer)
      .LocationAddress(description->location_address)
      .Format(static_cast<nw::gfx::res::ResPixelBasedTexture::FormatHW>(description->format))
      .ExecutingMemoryFill(description->executing_memory_fill)
      .DynamicAllocation(description->is_dynamic_allocation)
      .Create(device_allocator);
  res_texture.Setup(device_allocator, nw::gfx::ResGraphicsFile(NULL));
  return res_texture;
}
nw::gfx::res::ResTexture Texture::CreateCubeTexture(
    gfl::heap::NwAllocator*    device_allocator,
    const Description*         description
)
{
  nw::gfx::res::ResCubeTexture res_texture;
  res_texture = nw::gfx::res::ResCubeTexture::DynamicBuilder()
      .Width(description->width)
      .MipmapSize(description->mipmap_size)
      .LocationFlag(description->memory_area | description->memory_transfer)
      .LocationAddress(static_cast<nw::gfx::res::ResCubeTexture::CubeFace>(0), description->cube_face_location_address[0])
      .LocationAddress(static_cast<nw::gfx::res::ResCubeTexture::CubeFace>(1), description->cube_face_location_address[1])
      .LocationAddress(static_cast<nw::gfx::res::ResCubeTexture::CubeFace>(2), description->cube_face_location_address[2])
      .LocationAddress(static_cast<nw::gfx::res::ResCubeTexture::CubeFace>(3), description->cube_face_location_address[3])
      .LocationAddress(static_cast<nw::gfx::res::ResCubeTexture::CubeFace>(4), description->cube_face_location_address[4])
      .LocationAddress(static_cast<nw::gfx::res::ResCubeTexture::CubeFace>(5), description->cube_face_location_address[5])
      .Format(static_cast<nw::gfx::res::ResPixelBasedTexture::FormatHW>(description->format))
      .ExecutingMemoryFill(description->executing_memory_fill)
      .DynamicAllocation(description->is_dynamic_allocation)
      .Create(device_allocator);
  res_texture.Setup(device_allocator, nw::gfx::ResGraphicsFile(NULL));
  return res_texture;
}
nw::gfx::res::ResTexture Texture::CreateShadowTexture(
    gfl::heap::NwAllocator*    device_allocator,
    const Description*         description
)
{
  nw::gfx::res::ResShadowTexture res_texture;
  res_texture = nw::gfx::res::ResShadowTexture::DynamicBuilder()
      .Height(description->height)
      .Width(description->width)
      .LocationFlag(description->memory_area | description->memory_transfer)
      .LocationAddress(description->location_address)
      .ExecutingMemoryFill(description->executing_memory_fill)
      .PerspectiveShadow(description->is_perspective_shadow)  // Orthoカメラの場合はr値による除算を行わないようにします。
      .ShadowZBias(description->shadow_z_bias)                // (定数 * near / (far - near)) くらいがいいかも。
      .DynamicAllocation(description->is_dynamic_allocation)
      .Create(device_allocator);
  res_texture.Setup(device_allocator, nw::gfx::ResGraphicsFile(NULL));
  return res_texture;
}

void Texture::DestroyImageTexture(nw::gfx::res::ResTexture nw_res_texture)
{
  nw::gfx::res::ResImageTexture res_texture = nw::ut::ResDynamicCast<nw::gfx::res::ResImageTexture>(nw_res_texture);
  res_texture.Cleanup();
  res_texture.DynamicDestroy();
}
void Texture::DestroyCubeTexture (nw::gfx::res::ResTexture nw_res_texture)
{
  nw::gfx::res::ResCubeTexture res_texture = nw::ut::ResDynamicCast<nw::gfx::res::ResCubeTexture>(nw_res_texture);
  res_texture.Cleanup();
  res_texture.DynamicDestroy();
}
void Texture::DestroyShadowTexture(nw::gfx::res::ResTexture nw_res_texture)
{
  nw::gfx::res::ResShadowTexture res_texture = nw::ut::ResDynamicCast<nw::gfx::res::ResShadowTexture>(nw_res_texture);
  res_texture.Cleanup();
  res_texture.DynamicDestroy();
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

