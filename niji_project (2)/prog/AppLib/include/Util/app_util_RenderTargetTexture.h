#if !defined(APP_UTIL_RENDERTARGETTEXTURE_H_INCLUDED)
#define APP_UTIL_RENDERTARGETTEXTURE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    app_util_RenderTargetTexture.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.07.31
 @brief   レンダーターゲットテクスチャ
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <Layout/include/gfl2_Layout.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
  GFL_NAMESPACE_BEGIN(gfx)
    class Texture;
    class Surface;
  GFL_NAMESPACE_END(gfx)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(gfl2)
  GFL_NAMESPACE_BEGIN(util)
    class GLHeapAllocator;
  GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(System)
  class nijiAllocator;
GFL_NAMESPACE_END(System)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


//==============================================================================
/**
 @class     RenderTargetTexture
 @brief     レンダーターゲットテクスチャ
 */
//==============================================================================
class RenderTargetTexture
{
  GFL_FORBID_COPY_AND_ASSIGN(RenderTargetTexture);

public:
  //! @brief  タイプ
  class Type
  {
  public:
    enum Tag
    {
      DEPTH_BUFFER_NONE,    //!< デプスバッファなし
      DEPTH_BUFFER_RENTAL,  //!< デプスバッファをフレームバッファから借りる
      DEPTH_BUFFER_CREATE,  //!< デプスバッファを生成し専用利用する
    };
  };

  //! @brief  設定内容
  class Description
  {
  public:
    u32               width;        // 2のべき乗のサイズでなくともよいが、メモリを確保する際は縦横とも2のべき乗のサイズで確保します。
    u32               height;       // CTR特有の縦横入れ替えはしなくていいです。
    gfl2::gfx::Format colorFormat;
    gfl2::gfx::Format depthFormat;  // typeがDEPTH_BUFFER_CREATEのときだけ使用する。
    Type::Tag         type;
  public:
    Description(void)
      : width(0),
        height(0),
        colorFormat(gfl2::gfx::Format::A8R8G8B8),
        depthFormat(gfl2::gfx::Format::D24S8),
        type(Type::DEPTH_BUFFER_RENTAL)
    {}
  };

public:
  //! @brief  コンストラクタ(まだ生成されていません。生成はCreateを呼んだときにされます。)
  RenderTargetTexture(void);
  //! @brief  デストラクタ(Destroyを呼んでいます。)
  virtual ~RenderTargetTexture();

  //! @brief  生成
  //! @param[in]  vramAllocator    VRAMアロケータ(VRAMから取らなけ ないものはここから取ります)
  //! @param[in]  devGLAllocator   デバイスメモリから確保するアロケー田(VRAMでなくてもいいものはここから取ります)
  //! @param[in]  desc             設定内容
  void Create(
      System::nijiAllocator*       vramAllocator,
      gfl2::util::GLHeapAllocator* devGLAllocator,
      const Description&           desc
  );
  //! @brief  破棄
  void Destroy(void);

  //! @brief  テクスチャの利用領域のサイズを取得する(Descriptionで指定したサイズ)
  u32 GetWidth(void) const { return m_desc.width; }
  u32 GetHeight(void) const { return m_desc.height; }
  
  //! @brief  テクスチャのサイズを取得する(2のべき乗のサイズ)
  u32 GetRealWidth(void) const { return m_realWidth; }
  u32 GetRealHeight(void) const { return m_realHeight; }

  //! @brief  カラーバッファからレイアウト用のテクスチャの情報を取得する
  //! @param[out]  textureInfo  レイアウト用のテクスチャの情報を受け取るオブジェクト
  void GetColorTextureInfo(gfl2::lyt::LytResTextureInfo* textureInfo) const;

  //! @brief  カラーバッファを取得する
  gfl2::gfx::Surface* GetColorSurface(void) const;
  //! @brief  デプスバッファを取得する
  gfl2::gfx::Surface* GetDepthSurface(void) const;

  // テストに使った関数。普段はコメントアウト。
  //gfl2::gfx::Texture* GetColorTexture(void) const { return m_colorTexture; }

  //! @brief  デプステクスチャを取得する
  gfl2::gfx::Texture* GetDepthTexture(void) const;

  //  テクスチャーバッファを取得
  void* GetTextureBuffer();

  //! @brief  クリアカラーを設定する  // gfl2::gfx::Colorは0=黒,1=白
  void SetClearColor(const gfl2::gfx::Color& clearColor) { m_clearColor = clearColor; }
  //! @brief  クリアカラーを取得する
  const gfl2::gfx::Color& GetClearColor(void) const { return m_clearColor; }

private:
  //! @brief  valを2のべき乗に切り上げる。valが元々2のべき乗ならそのまま。
  static u32 roundUpToPowerOf2(u32 val);

private:
  System::nijiAllocator*       m_vramAllocator;    // 解放時に必要になるので覚えておく
  gfl2::util::GLHeapAllocator* m_devGLAllocator;   // 解放時に必要になるので覚えておく
  Description                  m_desc;
  u32                          m_realWidth;
  u32                          m_realHeight;
  gfl2::gfx::Texture*          m_colorTexture;
  gfl2::gfx::Texture*          m_depthTexture;
  gfl2::gfx::Surface*          m_depthSurface;
  gfl2::gfx::Color             m_clearColor;
};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


#endif  // APP_UTIL_RENDERTARGETTEXTURE_H_INCLUDED

