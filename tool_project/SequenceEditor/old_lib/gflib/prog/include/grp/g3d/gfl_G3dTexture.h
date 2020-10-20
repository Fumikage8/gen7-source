#ifndef __GFL_G3DTEXTURE_H__
#define __GFL_G3DTEXTURE_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dTexture.h
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
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dObject.h>
#include <grp/g3d/gfl_G3dResource.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3Dテクスチャクラス
//=====================================
class Texture : public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(Texture);


  // 下記friendクラスからはprivate関数にもアクセスしてよい
  friend class Model;
  friend class BloomFilter;
  friend class System;


public:
  //---------------------------------------------------------------------------
  /**
   *           定数宣言
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	テクスチャのタイプ
  //=====================================
  enum Type
  {
    TYPE_IMAGE_TEXTURE,  // 画像データによる平面テクスチャ(普通のテクスチャ)
    TYPE_CUBE_TEXTURE,   // キューブテクスチャ
    TYPE_SHADOW_TEXTURE  // シャドウテクスチャ
  };

  //-------------------------------------
  ///	テクスチャのフォーマット
  //=====================================
  enum Format
  {
    FORMAT_RGBA8       = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA8   ,    //!< 1ピクセルが4バイト、RGBAが各8ビットのフォーマットです。
    FORMAT_RGB8        = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGB8    ,    //!< 1ピクセルが3バイト、RGBが各8ビットのフォーマットです。
    FORMAT_RGBA5551    = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA5551,    //!< 1ピクセルが2バイト、RGBが各5ビット、Aが1ビットのフォーマットです。
    FORMAT_RGB565      = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGB565  ,    //!< 1ピクセルが2バイト、RGBが各5,6,5ビットのフォーマットです。
    FORMAT_RGBA4       = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA4   ,    //!< 1ピクセルが2バイト、RGBAが各4ビットのフォーマットです。
    FORMAT_LA8         = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_LA8     ,    //!< 1ピクセルが2バイト、AlphaとLuminanceが各8ビットのフォーマットです 
    FORMAT_HILO8       = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_HILO8   ,    //!< 1ピクセルが2バイト、X,Yが各8ビットのフォーマットです。
    FORMAT_L8          = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_L8      ,    //!< 1ピクセルが1バイト、Luminanceが8ビットのフォーマットです。
    FORMAT_A8          = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_A8      ,    //!< 1ピクセルが1バイト、Alphaが8ビットのフォーマットです。
    FORMAT_LA4         = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_LA4     ,    //!< 1ピクセルが1バイト、AlphaとLuminanceが各4ビットのフォーマットです 
    FORMAT_L4          = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_L4      ,    //!< 1ピクセルが4ビット、Luminanceが4ビットのフォーマットです。
    FORMAT_A4          = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_A4      ,    //!< 1ピクセルが4ビット、Alphaが4ビットのフォーマットです。
    FORMAT_ETC1        = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_ETC1    ,    //!< 1ピクセルが4ビット相当の圧縮フォーマットです。
    FORMAT_ETC1A4      = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_ETC1A4  ,    //!< 1ピクセルが8ビット相当の圧縮フォーマットです。                    
    FORMAT_MAX         = 14
  };

  //-------------------------------------
  ///	キューブテクスチャの面
  //=====================================
  enum CubeFace
  {
    CUBE_FACE_POSITIVE_X = nw::gfx::res::ResCubeTexture::CUBE_FACE_POSITIVE_X,   //!< X 軸の＋方向です。
    CUBE_FACE_NEGATIVE_X = nw::gfx::res::ResCubeTexture::CUBE_FACE_NEGATIVE_X,   //!< X 軸の－方向です。
    CUBE_FACE_POSITIVE_Y = nw::gfx::res::ResCubeTexture::CUBE_FACE_POSITIVE_Y,   //!< Y 軸の＋方向です。
    CUBE_FACE_NEGATIVE_Y = nw::gfx::res::ResCubeTexture::CUBE_FACE_NEGATIVE_Y,   //!< Y 軸の－方向です。
    CUBE_FACE_POSITIVE_Z = nw::gfx::res::ResCubeTexture::CUBE_FACE_POSITIVE_Z,   //!< Z 軸の＋方向です。
    CUBE_FACE_NEGATIVE_Z = nw::gfx::res::ResCubeTexture::CUBE_FACE_NEGATIVE_Z,   //!< Z 軸の－方向です。
    CUBE_FACE_MAX        = nw::gfx::res::ResCubeTexture::MAX_CUBE_FACE           //!< 面の数です。       
  };


public:
  //-----------------------------------------------------------------------------
  /**
   *           staticメンバ関数
   */
  //-----------------------------------------------------------------------------
  // gfl::grp::g3d::Texture::Format -> gfl::grp::RenderColorFormat
  static gfl::grp::RenderColorFormat    ConvertTextureFormatToRenderColorFormat(gfl::grp::g3d::Texture::Format texture_format);
  // gfl::grp::RenderColorFormat    -> gfl::grp::g3d::Texture::Format
  static gfl::grp::g3d::Texture::Format ConvertRenderColorFormatToTextureFormat(gfl::grp::RenderColorFormat    render_color_format);

  // formatのテクスチャの1ピクセルのビットサイズを得る(バイトサイズではありません)
  static u8 GetTextureBitsPerPixel(Format format);


public:
  //---------------------------------------------------------------------------
  /**
   *           構造体宣言
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	設定内容
  //=====================================
  struct Description 
  {
    Type                         type;                   // [TYPE_IMAGE_TEXTURE, TYPE_CUBE_TEXTURE, TYPE_SHADOW_TEXTURE]  // テクスチャのタイプ

    s32                          height;                 // [TYPE_IMAGE_TEXTURE,                    TYPE_SHADOW_TEXTURE]  // 高さ
    s32                          width;                  // [TYPE_IMAGE_TEXTURE, TYPE_CUBE_TEXTURE, TYPE_SHADOW_TEXTURE]  // 幅

    gfl::grp::MemoryArea         memory_area;            // [TYPE_IMAGE_TEXTURE, TYPE_CUBE_TEXTURE, TYPE_SHADOW_TEXTURE]  // メモリ配置場所(location_addressが0以外のときは使用しない)
    gfl::grp::MemoryTransfer     memory_transfer;        // [TYPE_IMAGE_TEXTURE, TYPE_CUBE_TEXTURE, TYPE_SHADOW_TEXTURE]  // メモリ転送タイプ(location_addressが0以外のときは使用しない)
    u32                          location_address;       // [TYPE_IMAGE_TEXTURE,                    TYPE_SHADOW_TEXTURE]  // イメージ展開済みのアドレス(VRAM 上のアドレスでも構いません)
    u32                          cube_face_location_address[CUBE_FACE_MAX];
                                                         // [                    TYPE_CUBE_TEXTURE                     ]  // イメージ展開済みのアドレス(VRAM 上のアドレスでも構いません)
    b32                          executing_memory_fill;  // [TYPE_IMAGE_TEXTURE, TYPE_CUBE_TEXTURE, TYPE_SHADOW_TEXTURE]  // 確保したピクセル領域を0でクリアする場合true
    b32                          is_dynamic_allocation;  // [TYPE_IMAGE_TEXTURE, TYPE_CUBE_TEXTURE, TYPE_SHADOW_TEXTURE]  // 動的にFCRAM上に領域を確保する場合はtrue

    s32                          mipmap_size;            // [TYPE_IMAGE_TEXTURE, TYPE_CUBE_TEXTURE                     ]  // ミップマップサイズ
                                                                                                                          // (ミップマップを使用しないときは1を、ミップマップを使用するときはミップマップ数+1を指定する)
    Format                       format;                 // [TYPE_IMAGE_TEXTURE, TYPE_CUBE_TEXTURE                     ]  // テクスチャのフォーマット

    b32                          is_perspective_shadow;  // [                                       TYPE_SHADOW_TEXTURE]  // 投影シャドウの場合はtrue
    f32                          shadow_z_bias;          // [                                       TYPE_SHADOW_TEXTURE]  // シャドウテクスチャのZバイアス値
    f32                          shadow_z_scale;         // [                                       TYPE_SHADOW_TEXTURE]  // オフセット値に乗算するスケール
     
    //-----------------------------------------------------------------------------
    /**
     *  @brief         コンストラクタ
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : type                  (TYPE_IMAGE_TEXTURE),
        height                (128),
        width                 (256),
        memory_area           (MEMORY_AREA_FCRAM),
        memory_transfer       (MEMORY_TRANSFER_NO_COPY_FCRAM_DMP),
        location_address      (0),
        executing_memory_fill (false),
        is_dynamic_allocation (false),
        mipmap_size           (1),
        format                (FORMAT_RGBA4), 
        is_perspective_shadow (true),
        shadow_z_bias         (0.0f),
        shadow_z_scale        (1.0f)
    {
      for( s32 i=0; i<CUBE_FACE_MAX; ++i )  // 配列になっているメンバ変数は、初期化子リストでは初期化できない。
        cube_face_location_address[i] = 0;
    }
  };

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  Texture(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~Texture();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *	@param	heap_allocator     ヒープアロケータ
   *	@param  device_allocator   デバイスアロケータ
   *	@param  description        設定内容
   */
  //-----------------------------------------------------------------------------
  void Create(
      gfl::heap::NwAllocator*    heap_allocator,
      gfl::heap::NwAllocator*    device_allocator,
      const Description*         description = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *                 レンダーターゲットをテクスチャとする
   *                 これで生成したTextureインスタンス生存中は、引数で渡したrender_targetも保持しておいて下さい。
   *                 縦横注意！
   *
   *	@param  description        レンダーターゲット
   */
  //-----------------------------------------------------------------------------
  void Create(
      const gfl::grp::RenderTarget*  render_target
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         破棄
   *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
   */
  //-----------------------------------------------------------------------------
  void Destroy(void);

private:
  nw::gfx::res::ResTexture CreateImageTexture(
      gfl::heap::NwAllocator*    device_allocator,
      const Description*         description
  );
  nw::gfx::res::ResTexture CreateCubeTexture(
      gfl::heap::NwAllocator*    device_allocator,
      const Description*         description
  );
  nw::gfx::res::ResTexture CreateShadowTexture(
      gfl::heap::NwAllocator*    device_allocator,
      const Description*         description
  );

  void DestroyImageTexture (nw::gfx::res::ResTexture nw_res_texture);
  void DestroyCubeTexture  (nw::gfx::res::ResTexture nw_res_texture);
  void DestroyShadowTexture(nw::gfx::res::ResTexture nw_res_texture);


public:
  nw::gfx::res::ResTexture  GetNwResTexture(void) const  {  return m_nw_res_texture;  }


private:
  nw::gfx::res::ResTexture     m_nw_res_texture;
  b32                          m_is_create;

};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DTEXTURE_H__
