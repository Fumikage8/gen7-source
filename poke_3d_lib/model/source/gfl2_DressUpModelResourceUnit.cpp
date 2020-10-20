#include "gfl2_DressUpModelResourceUnit.h"

#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>

#include <debug/include/gfl2_DebugPrint.h>

#if 0
  #define DRESSUP_DEBUG_PRINT(...) TAKEBE_PRINT(__VA_ARGS__)
#else
  #define DRESSUP_DEBUG_PRINT(...)
#endif

namespace poke_3d { namespace model {

// 1フレームで処理するピクセル数
static const u32 MAX_TEXTURE_PIXEL_COUNT = (256 * 256) / 2;
static const u32 MAX_TEXTURE_PIXEL_COUNT_FACE = (256 * 256) / 3 + 1;

// マスクテクスチャには判別用にフッターを付けている
struct MaskTextureFooter
{
  u8 originalNameLength;  // オリジナルテクスチャの名前サイズ
  u8 maskTextureId;       // マスクテクスチャID
};

#if defined(GF_PLATFORM_WIN32)
struct RGBA8888
{
  u8 r;
  u8 g;
  u8 b;
  u8 a;
};
struct RGB565
{
  u16 r :5;
  u16 g :6;
  u16 b :5;
};
struct RGBA5551
{
  u16 r :5;
  u16 g :5;
  u16 b :5;
  u16 a :1;
};
struct A8
{
  u8 a;
};
struct LA88
{
  u8 l;
  u8 a;
};
#elif defined(GF_PLATFORM_CTR)
struct RGBA8888
{
  u8 a;
  u8 b;
  u8 g;
  u8 r;
};
struct RGB565
{
  u16 b :5;
  u16 g :6;
  u16 r :5;
};
struct RGBA5551
{
  u16 a :1;
  u16 b :5;
  u16 g :5;
  u16 r :5;
};
struct A8
{
  u8 a;
};
struct LA88
{
  u8 a;
  u8 l;
};
#else
#error "error invalid platform"
#endif

static const char DRESSUP_HAIR_ON[] = {'_', 'o', 'n'};
static const char DRESSUP_HAIR_OFF[] = {'_', 'o', 'f', 'f'};

// マスクテクスチャとの乗算
template <typename ColorFormat, typename MaskFormat>
static bool MultMaskTextureCore(ColorFormat* pColorBuffer, MaskFormat* pMaskBuffer, u32 width, u32 height, const gfl2::math::Vector3& color, u32* inout_pTexturePixel)
{
  u32 loopCount = width * height;

  u32 tmpLoopCount = loopCount;
  if (tmpLoopCount > MAX_TEXTURE_PIXEL_COUNT)
  {
    tmpLoopCount = (*inout_pTexturePixel) + MAX_TEXTURE_PIXEL_COUNT;
    if (tmpLoopCount >= loopCount)
    {
      tmpLoopCount = loopCount;
    }
  }
  DRESSUP_DEBUG_PRINT("MultMaskTextureCore loopCount %u, tmpLoopCount %u\n", loopCount, tmpLoopCount);

  pColorBuffer += *inout_pTexturePixel;
  pMaskBuffer += *inout_pTexturePixel;

  for (u32 i = *inout_pTexturePixel; i < tmpLoopCount; ++i)
  {
    f32 mask = pMaskBuffer->a / 255.0f;

    pColorBuffer->r = gfl2::math::Lerp(pColorBuffer->r, pColorBuffer->r * color.x, mask);
    pColorBuffer->g = gfl2::math::Lerp(pColorBuffer->g, pColorBuffer->g * color.y, mask);
    pColorBuffer->b = gfl2::math::Lerp(pColorBuffer->b, pColorBuffer->b * color.z, mask);

    ++pColorBuffer;
    ++pMaskBuffer;
  }

  *inout_pTexturePixel = tmpLoopCount;
  return (*inout_pTexturePixel >= loopCount);
}

// テンプレート解決用
template <typename ColorFormat>
static bool MultMaskTextureSelectMaskFormat(ColorFormat* pColorBuffer, void* pMaskBuffer, gfl2::gfx::Format maskFormat, u32 width, u32 height, const gfl2::math::Vector3& color, u32* inout_pTexturePixel)
{
  switch (maskFormat)
  {
  case gfl2::gfx::Format::L8A8:
    return MultMaskTextureCore(pColorBuffer, static_cast<LA88*>(pMaskBuffer), width, height, color, inout_pTexturePixel);

  case gfl2::gfx::Format::L8:
  case gfl2::gfx::Format::A8:
    return MultMaskTextureCore(pColorBuffer, static_cast<A8*>(pMaskBuffer), width, height, color, inout_pTexturePixel);

  case gfl2::gfx::Format::A8R8G8B8:
    return MultMaskTextureCore(pColorBuffer, static_cast<RGBA8888*>(pMaskBuffer), width, height, color, inout_pTexturePixel);

  default:
    // 未対応のフォーマット
    GFL_ASSERT_MSG(0, "maskFormat != L8 or A8");
    break;
  }

  return true;
}

// テンプレート解決用
static bool MultMaskTextureSelectColorFormat(void* pColorBuffer, gfl2::gfx::Format colorFormat, void* pMaskBuffer, gfl2::gfx::Format maskFormat, u32 width, u32 height, const gfl2::math::Vector3& color, u32* inout_pTexturePixel)
{
  switch (colorFormat)
  {
  case gfl2::gfx::Format::A8R8G8B8:
    return MultMaskTextureSelectMaskFormat(static_cast<RGBA8888*>(pColorBuffer), pMaskBuffer, maskFormat, width, height, color, inout_pTexturePixel);

  case gfl2::gfx::Format::R5G6B5:
    return MultMaskTextureSelectMaskFormat(static_cast<RGB565*>(pColorBuffer), pMaskBuffer, maskFormat, width, height, color, inout_pTexturePixel);

  case gfl2::gfx::Format::R5G5B5A1:
    return MultMaskTextureSelectMaskFormat(static_cast<RGBA5551*>(pColorBuffer), pMaskBuffer, maskFormat, width, height, color, inout_pTexturePixel);

  default:
    // 未対応のフォーマット
    GFL_ASSERT(0);
    break;
  }

  return true;
}

// マスクテクスチャとのカラー乗算
static bool MultMaskTexture(gfl2::gfx::Texture* pColorTexture,
                            gfl2::gfx::Texture* pMaskTexture,
                            const gfl2::math::Vector3& color,
                            u32* inout_pTexturePixel)
{
  // テクスチャサイズが同じじゃなければおかしい
  GFL_ASSERT(pColorTexture->GetTextureDesc().width == pMaskTexture->GetTextureDesc().width);
  GFL_ASSERT(pColorTexture->GetTextureDesc().height == pMaskTexture->GetTextureDesc().height);

  void* pColorBuffer = pColorTexture->Lock();
  void* pMaskBuffer = pMaskTexture->Lock();

  bool result = MultMaskTextureSelectColorFormat(
    pColorBuffer,
    pColorTexture->GetTextureDesc().format,
    pMaskBuffer,
    pMaskTexture->GetTextureDesc().format,
    pColorTexture->GetTextureDesc().width,
    pColorTexture->GetTextureDesc().height,
    color,
    inout_pTexturePixel);

  pColorTexture->UnLock();
  pMaskTexture->UnLock();

  return result;
}

// マスクテクスチャとの乗算：face用
template <typename ColorFormat, typename MaskFormat>
static bool MultMaskTextureCoreFace_1(ColorFormat* pColorBuffer, u32 width, u32 height,
                                      MaskFormat* pMaskBuffer0,
                                      const gfl2::math::Vector3& color0,
                                      u32* inout_pTexturePixel)
{
  u32 loopCount = width * height;

  u32 tmpLoopCount = loopCount;
  if (tmpLoopCount > MAX_TEXTURE_PIXEL_COUNT_FACE)
  {
    tmpLoopCount = (*inout_pTexturePixel) + MAX_TEXTURE_PIXEL_COUNT_FACE;
    if (tmpLoopCount >= loopCount)
    {
      tmpLoopCount = loopCount;
    }
  }
  DRESSUP_DEBUG_PRINT("MultMaskTextureCoreFace_1 loopCount %u, tmpLoopCount %u\n", loopCount, tmpLoopCount);

  pColorBuffer += *inout_pTexturePixel;
  pMaskBuffer0 += *inout_pTexturePixel;

  for (u32 i = *inout_pTexturePixel; i < tmpLoopCount; ++i)
  {
    f32 mask = pMaskBuffer0->a / 255.0f;

    pColorBuffer->r = gfl2::math::Lerp(pColorBuffer->r, pColorBuffer->r * color0.x, mask);
    pColorBuffer->g = gfl2::math::Lerp(pColorBuffer->g, pColorBuffer->g * color0.y, mask);
    pColorBuffer->b = gfl2::math::Lerp(pColorBuffer->b, pColorBuffer->b * color0.z, mask);

    ++pColorBuffer;
    ++pMaskBuffer0;
  }

  *inout_pTexturePixel = tmpLoopCount;
  return (*inout_pTexturePixel >= loopCount);
}

// マスクテクスチャとの乗算：face用
template <typename ColorFormat, typename MaskFormat>
static bool MultMaskTextureCoreFace_2(ColorFormat* pColorBuffer, u32 width, u32 height,
                                      MaskFormat* pMaskBuffer0, MaskFormat* pMaskBuffer1,
                                      const gfl2::math::Vector3& color0, const gfl2::math::Vector3& color1,
                                      u32* inout_pTexturePixel)
{
  u32 loopCount = width * height;

  u32 tmpLoopCount = loopCount;
  if (tmpLoopCount > MAX_TEXTURE_PIXEL_COUNT_FACE)
  {
    tmpLoopCount = (*inout_pTexturePixel) + MAX_TEXTURE_PIXEL_COUNT_FACE;
    if (tmpLoopCount >= loopCount)
    {
      tmpLoopCount = loopCount;
    }
  }
  DRESSUP_DEBUG_PRINT("MultMaskTextureCoreFace_2 loopCount %u, tmpLoopCount %u\n", loopCount, tmpLoopCount);

  pColorBuffer += *inout_pTexturePixel;
  pMaskBuffer0 += *inout_pTexturePixel;
  pMaskBuffer1 += *inout_pTexturePixel;

  for (u32 i = *inout_pTexturePixel; i < tmpLoopCount; ++i)
  {
    f32 r = 1.0f;
    f32 g = 1.0f;
    f32 b = 1.0f;

    // mask0
    {
      f32 mask = pMaskBuffer0->a / 255.0f;
      r = gfl2::math::Lerp(r, color0.x, mask);
      g = gfl2::math::Lerp(g, color0.y, mask);
      b = gfl2::math::Lerp(b, color0.z, mask);
    }

    // mask1
    {
      f32 mask = pMaskBuffer1->a / 255.0f;
      r = gfl2::math::Lerp(r, color1.x, mask);
      g = gfl2::math::Lerp(g, color1.y, mask);
      b = gfl2::math::Lerp(b, color1.z, mask);
    }

    // color
    {
      pColorBuffer->r *= r;
      pColorBuffer->g *= g;
      pColorBuffer->b *= b;
    }

    ++pColorBuffer;
    ++pMaskBuffer0;
    ++pMaskBuffer1;
  }

  *inout_pTexturePixel = tmpLoopCount;
  return (*inout_pTexturePixel >= loopCount);
}

// マスクテクスチャとの乗算：face用
template <typename ColorFormat, typename MaskFormat>
static bool MultMaskTextureCoreFace_3(ColorFormat* pColorBuffer, u32 width, u32 height,
                                      MaskFormat* pMaskBuffer0, MaskFormat* pMaskBuffer1, MaskFormat* pMaskBuffer2,
                                      const gfl2::math::Vector3& color0, const gfl2::math::Vector3& color1, const gfl2::math::Vector3& color2,
                                      u32* inout_pTexturePixel)
{
  u32 loopCount = width * height;

  u32 tmpLoopCount = loopCount;
  if (tmpLoopCount > MAX_TEXTURE_PIXEL_COUNT_FACE)
  {
    tmpLoopCount = (*inout_pTexturePixel) + MAX_TEXTURE_PIXEL_COUNT_FACE;
    if (tmpLoopCount >= loopCount)
    {
      tmpLoopCount = loopCount;
    }
  }
  DRESSUP_DEBUG_PRINT("MultMaskTextureCoreFace_3 loopCount %u, tmpLoopCount %u\n", loopCount, tmpLoopCount);

  pColorBuffer += *inout_pTexturePixel;
  pMaskBuffer0 += *inout_pTexturePixel;
  pMaskBuffer1 += *inout_pTexturePixel;
  pMaskBuffer2 += *inout_pTexturePixel;

  for (u32 i = *inout_pTexturePixel; i < tmpLoopCount; ++i)
  {
    f32 r = 1.0f;
    f32 g = 1.0f;
    f32 b = 1.0f;

    // mask0
    {
      f32 mask = pMaskBuffer0->a / 255.0f;
      r = gfl2::math::Lerp(r, color0.x, mask);
      g = gfl2::math::Lerp(g, color0.y, mask);
      b = gfl2::math::Lerp(b, color0.z, mask);
    }

    // mask1
    {
      f32 mask = pMaskBuffer1->a / 255.0f;
      r = gfl2::math::Lerp(r, color1.x, mask);
      g = gfl2::math::Lerp(g, color1.y, mask);
      b = gfl2::math::Lerp(b, color1.z, mask);
    }

    // mask2
    {
      f32 mask = pMaskBuffer2->a / 255.0f;
      r = gfl2::math::Lerp(r, color2.x, mask);
      g = gfl2::math::Lerp(g, color2.y, mask);
      b = gfl2::math::Lerp(b, color2.z, mask);
    }

    // color
    {
      pColorBuffer->r *= r;
      pColorBuffer->g *= g;
      pColorBuffer->b *= b;
    }

    ++pColorBuffer;
    ++pMaskBuffer0;
    ++pMaskBuffer1;
    ++pMaskBuffer2;
  }

  *inout_pTexturePixel = tmpLoopCount;
  return (*inout_pTexturePixel >= loopCount);
}

// マスクテクスチャとの乗算：face用
template <typename ColorFormat, typename MaskFormat>
static bool MultMaskTextureCoreFace_4(ColorFormat* pColorBuffer, u32 width, u32 height,
                                      MaskFormat* pMaskBuffer0, MaskFormat* pMaskBuffer1, MaskFormat* pMaskBuffer2, MaskFormat* pMaskBuffer3,
                                      const gfl2::math::Vector3& color0, const gfl2::math::Vector3& color1, const gfl2::math::Vector3& color2, const gfl2::math::Vector3& color3,
                                      u32* inout_pTexturePixel)
{
  u32 loopCount = width * height;

  u32 tmpLoopCount = loopCount;
  if (tmpLoopCount > MAX_TEXTURE_PIXEL_COUNT_FACE)
  {
    tmpLoopCount = (*inout_pTexturePixel) + MAX_TEXTURE_PIXEL_COUNT_FACE;
    if (tmpLoopCount >= loopCount)
    {
      tmpLoopCount = loopCount;
    }
  }
  DRESSUP_DEBUG_PRINT("MultMaskTextureCoreFace_4 loopCount %u, tmpLoopCount %u\n", loopCount, tmpLoopCount);

  pColorBuffer += *inout_pTexturePixel;
  pMaskBuffer0 += *inout_pTexturePixel;
  pMaskBuffer1 += *inout_pTexturePixel;
  pMaskBuffer2 += *inout_pTexturePixel;
  pMaskBuffer3 += *inout_pTexturePixel;

  for (u32 i = *inout_pTexturePixel; i < tmpLoopCount; ++i)
  {
    f32 r = 1.0f;
    f32 g = 1.0f;
    f32 b = 1.0f;

    // mask0
    {
      f32 mask = pMaskBuffer0->a / 255.0f;
      r = gfl2::math::Lerp(r, color0.x, mask);
      g = gfl2::math::Lerp(g, color0.y, mask);
      b = gfl2::math::Lerp(b, color0.z, mask);
    }

    // mask1
    {
      f32 mask = pMaskBuffer1->a / 255.0f;
      r = gfl2::math::Lerp(r, color1.x, mask);
      g = gfl2::math::Lerp(g, color1.y, mask);
      b = gfl2::math::Lerp(b, color1.z, mask);
    }

    // mask2
    {
      f32 mask = pMaskBuffer2->a / 255.0f;
      r = gfl2::math::Lerp(r, color2.x, mask);
      g = gfl2::math::Lerp(g, color2.y, mask);
      b = gfl2::math::Lerp(b, color2.z, mask);
    }

    // mask3
    {
      f32 mask = pMaskBuffer3->a / 255.0f;
      r = gfl2::math::Lerp(r, color3.x, mask);
      g = gfl2::math::Lerp(g, color3.y, mask);
      b = gfl2::math::Lerp(b, color3.z, mask);
    }

    // color
    {
      pColorBuffer->r *= r;
      pColorBuffer->g *= g;
      pColorBuffer->b *= b;
    }

    ++pColorBuffer;
    ++pMaskBuffer0;
    ++pMaskBuffer1;
    ++pMaskBuffer2;
    ++pMaskBuffer3;
  }

  *inout_pTexturePixel = tmpLoopCount;
  return (*inout_pTexturePixel >= loopCount);
}

// テンプレート解決用：face用
template <typename ColorFormat, typename MaskFormat>
static bool MultMaskTextureSelectMaskFormatFaceCount(ColorFormat* pColorBuffer, void** pMaskBufferList, u32 width, u32 height, const gfl2::math::Vector3 colorList[], u32 maskCount, u32* inout_pTexturePixel)
{
  switch (maskCount)
  {
  case 1:
    return MultMaskTextureCoreFace_1(pColorBuffer, width, height, 
      static_cast<MaskFormat*>(pMaskBufferList[0]),
      colorList[0],
      inout_pTexturePixel);

  case 2:
    return MultMaskTextureCoreFace_2(pColorBuffer, width, height,
      static_cast<MaskFormat*>(pMaskBufferList[0]), static_cast<MaskFormat*>(pMaskBufferList[1]),
      colorList[0], colorList[1],
      inout_pTexturePixel);

  case 3:
    return MultMaskTextureCoreFace_3(pColorBuffer, width, height,
      static_cast<MaskFormat*>(pMaskBufferList[0]), static_cast<MaskFormat*>(pMaskBufferList[1]), static_cast<MaskFormat*>(pMaskBufferList[2]),
      colorList[0], colorList[1], colorList[2],
      inout_pTexturePixel);

  case 4:
    return MultMaskTextureCoreFace_4(pColorBuffer, width, height,
      static_cast<MaskFormat*>(pMaskBufferList[0]), static_cast<MaskFormat*>(pMaskBufferList[1]), static_cast<MaskFormat*>(pMaskBufferList[2]), static_cast<MaskFormat*>(pMaskBufferList[3]),
      colorList[0], colorList[1], colorList[2], colorList[3],
      inout_pTexturePixel);

  default:
    // 未対応のマスク数
    GFL_ASSERT_MSG(0, "maskCount > 4");
    break;
  }

  return true;
}

// テンプレート解決用：face用
template <typename ColorFormat>
static bool MultMaskTextureSelectMaskFormatFace(ColorFormat* pColorBuffer, void** pMaskBufferList, gfl2::gfx::Format maskFormat, u32 width, u32 height, const gfl2::math::Vector3 colorList[], u32 maskCount, u32* inout_pTexturePixel)
{
  switch (maskFormat)
  {
  case gfl2::gfx::Format::L8A8:
    return MultMaskTextureSelectMaskFormatFaceCount<ColorFormat, LA88>(pColorBuffer, pMaskBufferList, width, height, colorList, maskCount, inout_pTexturePixel);

  case gfl2::gfx::Format::L8:
  case gfl2::gfx::Format::A8:
    return MultMaskTextureSelectMaskFormatFaceCount<ColorFormat, A8>(pColorBuffer, pMaskBufferList, width, height, colorList, maskCount, inout_pTexturePixel);

  case gfl2::gfx::Format::A8R8G8B8:
    return MultMaskTextureSelectMaskFormatFaceCount<ColorFormat, RGBA8888>(pColorBuffer, pMaskBufferList, width, height, colorList, maskCount, inout_pTexturePixel);

  default:
    // 未対応のフォーマット
    GFL_ASSERT_MSG(0, "maskFormat != L8 or A8");
    break;
  }

  return true;
}

// テンプレート解決用：face用
static bool MultMaskTextureSelectColorFormatFace(void* pColorBuffer, gfl2::gfx::Format colorFormat, void** pMaskBufferList, gfl2::gfx::Format maskFormat, u32 width, u32 height, const gfl2::math::Vector3 colorList[], u32 maskCount, u32* inout_pTexturePixel)
{
  switch (colorFormat)
  {
  case gfl2::gfx::Format::A8R8G8B8:
    return MultMaskTextureSelectMaskFormatFace(static_cast<RGBA8888*>(pColorBuffer), pMaskBufferList, maskFormat, width, height, colorList, maskCount, inout_pTexturePixel);

  case gfl2::gfx::Format::R5G6B5:
    return MultMaskTextureSelectMaskFormatFace(static_cast<RGB565*>(pColorBuffer), pMaskBufferList, maskFormat, width, height, colorList, maskCount, inout_pTexturePixel);

  case gfl2::gfx::Format::R5G5B5A1:
    return MultMaskTextureSelectMaskFormatFace(static_cast<RGBA5551*>(pColorBuffer), pMaskBufferList, maskFormat, width, height, colorList, maskCount, inout_pTexturePixel);

  default:
    // 未対応のフォーマット
    GFL_ASSERT(0);
    break;
  }

  return true;
}


// マスクテクスチャとのカラー乗算：face用
static bool MultMaskTextureFace(gfl2::gfx::Texture* pColorTexture, gfl2::gfx::Texture** pMaskTextureList, const gfl2::math::Vector3 colorList[], u32 maskCount, u32* inout_pTexturePixel)
{
  // マスクテクスチャが存在しないのはおかしい
  GFL_ASSERT(maskCount > 0);

  // テクスチャサイズが同じじゃなければおかしい
  for (u32 i = 0; i < maskCount; ++i)
  {
    GFL_ASSERT(pColorTexture->GetTextureDesc().width == pMaskTextureList[i]->GetTextureDesc().width);
    GFL_ASSERT(pColorTexture->GetTextureDesc().height == pMaskTextureList[i]->GetTextureDesc().height);
  }
  
  // マスクテクスチャのフォーマットが全て同じじゃなければおかしい
  gfl2::gfx::Format maskFormat = pMaskTextureList[0]->GetTextureDesc().format;
  for (u32 i = 1; i < maskCount; ++i)
  {
    GFL_ASSERT(maskFormat == pMaskTextureList[i]->GetTextureDesc().format);
  }

  void* pColorBuffer = pColorTexture->Lock();
  void* pMaskBufferList[DressUpModelResourceUnit::MASK_TEXTURE_ID_COUNT] = {NULL};
  for (u32 i = 0; i < maskCount; ++i)
  {
    pMaskBufferList[i] = pMaskTextureList[i]->Lock();
  }

  bool result = MultMaskTextureSelectColorFormatFace(
    pColorBuffer,
    pColorTexture->GetTextureDesc().format,
    pMaskBufferList,
    pMaskTextureList[0]->GetTextureDesc().format,
    pColorTexture->GetTextureDesc().width,
    pColorTexture->GetTextureDesc().height,
    colorList,
    maskCount,
    inout_pTexturePixel);

  pColorTexture->UnLock();
  for (u32 i = 0; i < maskCount; ++i)
  {
    pMaskTextureList[i]->UnLock();
  }

  return result;
}


/**
 * @brief コンストラクタ
 */
DressUpModelResourceUnit::DressUpModelResourceUnit() :
  m_ItemIndex(-1),
  m_ItemOptionBit(0)
{
}

/**
 * @brief デストラクタ
 */
DressUpModelResourceUnit::~DressUpModelResourceUnit()
{
}

/**
 * @brief 読み込んだ静的データの開放：派生クラス用
 */
void DressUpModelResourceUnit::UnloadStaticDataDerived()
{
  for (u32 i = 0; i < MASK_TEXTURE_ID_COUNT; ++i)
  {
    for (u32 submask = 0; submask < SUBMASK_TEXTURE_ID_COUNT; ++submask)
    {
      m_MaskTextureResourceList[i][submask].Unload();
    }
  }
}

/**
 * @brief 着せ替えパーツのセットアップ
 *
 * @param pFileManager ファイルマネージャ
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 *
 * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
 */
bool DressUpModelResourceUnit::SetupDressUpParts(gfl2::fs::AsyncFileManager* pFileManager, gfl2::gfx::IGLAllocator* pGfxAllocator)
{
  // マスクテクスチャセットアップ
  u32 binLinkerDataCount = this->GetBinLinkerDataCount(pFileManager);
  for (u32 i = DRESSUP_BIN_LINKER_INDEX_MASK; i < binLinkerDataCount; ++i)
  {
    // BinLinkerからマスクテクスチャデータを取得する
    const void* pMaskTextureData = this->GetBinLinkerData(pFileManager, i);
    if (pMaskTextureData == NULL)
    {
      return false;
    }

    // データサイズ取得
    u32 dataSize = this->GetBinLinkerDataSize(pFileManager, i);
    if (dataSize == 0)
    {
      return false;
    }

    // フッター取得
    const MaskTextureFooter* pFooter = reinterpret_cast<const MaskTextureFooter*>(reinterpret_cast<uptr>(pMaskTextureData) + (dataSize - sizeof(MaskTextureFooter)));

    // オリジナルテクスチャ名
    const char* pOriginalTextureName = reinterpret_cast<const char*>(reinterpret_cast<uptr>(pFooter) - pFooter->originalNameLength);
    u8 originalTextureNameLength = pFooter->originalNameLength;

    // p1_face01mouth, p2_face01mouthだけは特殊処理
    u32 submask = SUBMASK_TEXTURE_ID_NORMAL;
    if (originalTextureNameLength >= 14)
    {
      if ((::std::memcmp(pOriginalTextureName, "p1_face01mouth", 14) == 0) || (::std::memcmp(pOriginalTextureName, "p2_face01mouth", 14) == 0))
      {
        submask = SUBMASK_TEXTURE_ID_FACEMOUTH;
      }
    }

    // すでに作成済みなら何もしない
    MaskTextureResource* pMaskTextureResource = &m_MaskTextureResourceList[pFooter->maskTextureId][submask];
    if (pMaskTextureResource->pResourceNode != NULL)
    {
      // submask1が空いていたらそちらを使ってみる
      if (submask == 0)
      {
        pMaskTextureResource = &m_MaskTextureResourceList[pFooter->maskTextureId][1];
      }

      if (pMaskTextureResource->pResourceNode != NULL)
      {
        continue;
      }
    }

    // リソースノードを生成する
    gfl2::renderingengine::scenegraph::resource::GfBinaryTexData maskTexData;
    maskTexData.SetTextureData((c8*)pMaskTextureData);

    pMaskTextureResource->pResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeIndex(pGfxAllocator, &maskTexData, 0);

    // オリジナルテクスチャ名
    pMaskTextureResource->pOriginalTextureName = pOriginalTextureName;
    pMaskTextureResource->originalTextureNameLength = originalTextureNameLength;
  }

  return true;
}

/**
 * @brief テクスチャカラーの変更
 *
 * @param maskTextureId マスクテクスチャID
 * @param submaskTextureId サブマスクテクスチャID
 * @param color カラー(RGB 0.0f〜1.0f で指定)
 * @param inout_pTexturePixel テクスチャの処理位置
 * @param out_pComplete true:色替え完了 false:処理中
 *
 * @return ture:色替え対象のテクスチャが存在する false:色替え対象のテクスチャが存在しない
 */
bool DressUpModelResourceUnit::ChangeTextureColor(MaskTextureId maskTextureId, SubMaskTextureId submaskTextureId, const gfl2::math::Vector3& color, u32* inout_pTexturePixel, bool* out_pComplete)
{
  MaskTextureResource* pMaskTextureResource = NULL;
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pOriginalTextureResourceNode = NULL;

  // マスクテクスチャとそれに対応するテクスチャリソースを探す
  if (!this->FindMaskTextureResourceAndOriginalTextureResourceNode(maskTextureId, submaskTextureId, &pMaskTextureResource, &pOriginalTextureResourceNode))
  {
    // 見つからない
    *out_pComplete = true;
    return false;
  }

  // 色を乗算する
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pMaskTextureNode = pMaskTextureResource->pResourceNode->SafeCast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode>();
  *out_pComplete = MultMaskTexture(const_cast<gfl2::gfx::Texture*>(pOriginalTextureResourceNode->GetTexture()), const_cast<gfl2::gfx::Texture*>(pMaskTextureNode->GetTexture()), color, inout_pTexturePixel);

  // 色替えを行った
  return true;
}

/**
 * @brief テクスチャカラーの変更：face専用処理
 *
 * @param colorList カラーリスト(RGB 0.0f〜1.0f で指定)
 * @param hasLipColor true:リップカラーあり, false:リップカラー無し
 * @param submaskTextureId サブマスクテクスチャID
 * @param inout_pTexturePixel テクスチャの処理位置
 * @param out_pComplete true:色替え完了 false:処理中
 *
 * @return ture:色替え対象のテクスチャが存在する false:色替え対象のテクスチャが存在しない
 */
bool DressUpModelResourceUnit::ChangeTextureColorFace(const gfl2::math::Vector3 colorList[], bool hasLipColor, SubMaskTextureId submaskTextureId, u32* inout_pTexturePixel, bool* out_pComplete)
{
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pOriginalTextureResourceNode = NULL;
  MaskTextureResource* pMaskTextureResourceList[MASK_TEXTURE_ID_COUNT] = {NULL};

  // skin
  // マスクテクスチャとそれに対応するテクスチャリソースを探す
  if (!this->FindMaskTextureResourceAndOriginalTextureResourceNode(MASK_TEXTURE_ID_SKIN, submaskTextureId, &pMaskTextureResourceList[MASK_TEXTURE_ID_SKIN], &pOriginalTextureResourceNode))
  {
    // 見つからない
    *out_pComplete = true;
    return false;
  }

  // eye
  pMaskTextureResourceList[MASK_TEXTURE_ID_EYE] = &m_MaskTextureResourceList[MASK_TEXTURE_ID_EYE][submaskTextureId];

  // eyeblow
  pMaskTextureResourceList[MASK_TEXTURE_ID_EYEBLOW] = &m_MaskTextureResourceList[MASK_TEXTURE_ID_EYEBLOW][submaskTextureId];

  // lip
  if (hasLipColor)
  {
    pMaskTextureResourceList[MASK_TEXTURE_ID_LIP] = &m_MaskTextureResourceList[MASK_TEXTURE_ID_LIP][submaskTextureId];
  }

  // 配列化
  gfl2::gfx::Texture* pMaskTextureNodeList[MASK_TEXTURE_ID_COUNT] = {NULL};
  gfl2::math::Vector3 maskColorList[MASK_TEXTURE_ID_COUNT];
  u32 maskCount = 0;
  for (u32 i = 0; i < MASK_TEXTURE_ID_COUNT; ++i)
  {
    if (pMaskTextureResourceList[i] == NULL)
    {
      continue;
    }
    if (pMaskTextureResourceList[i]->pResourceNode == NULL)
    {
      continue;
    }
    
    gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pMaskTextureNode = pMaskTextureResourceList[i]->pResourceNode->SafeCast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode>();
    if (pMaskTextureNode == NULL)
    {
      continue;
    }

    pMaskTextureNodeList[maskCount] = const_cast<gfl2::gfx::Texture*>(pMaskTextureNode->GetTexture());

    maskColorList[maskCount] = colorList[i];

    ++maskCount;
  }

  *out_pComplete = MultMaskTextureFace(const_cast<gfl2::gfx::Texture*>(pOriginalTextureResourceNode->GetTexture()), pMaskTextureNodeList, maskColorList, maskCount, inout_pTexturePixel);

  // 色替えを行った
  return true;
}

/**
 * @brief マスクテクスチャに対応するテクスチャリソースを探す
 */
gfl2::renderingengine::scenegraph::resource::TextureResourceNode* DressUpModelResourceUnit::FindTextureResourceNode(gfl2::renderingengine::scenegraph::DagNode* pModelResourceNode, MaskTextureResource* pMaskTextureResource)
{
  for (u32 i = 0; i < pModelResourceNode->GetChildCount(); ++i)
  {
    gfl2::renderingengine::scenegraph::DagNode* pChildNode = pModelResourceNode->GetChild(i);
    gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pOriginalTextureResourceNode = pChildNode->SafeCast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode>();

    if (pOriginalTextureResourceNode == NULL)
    {
      continue;
    }

    const c8* pName = pOriginalTextureResourceNode->GetName();

    // 文字数比較
    u32 nameLength = ::std::strlen(pName);
    nameLength -= 4;  // .tgaは要らない
    if (nameLength < pMaskTextureResource->originalTextureNameLength)
    {
      continue;
    }
    // 文字列比較
    if (::std::memcmp(pName, pMaskTextureResource->pOriginalTextureName, pMaskTextureResource->originalTextureNameLength) != 0)
    {
      continue;
    }
    // 文字数が一致しない？
    if (nameLength != pMaskTextureResource->originalTextureNameLength)
    {
      // 髪の影用に 「_on」 か 「_off」が含まれる可能性
      const c8* pLastName = &pName[pMaskTextureResource->originalTextureNameLength];
      if (!((::std::memcmp(pLastName, DRESSUP_HAIR_ON, sizeof(DRESSUP_HAIR_ON)) == 0) || (::std::memcmp(pLastName, DRESSUP_HAIR_OFF, sizeof(DRESSUP_HAIR_OFF)) == 0)))
      {
        continue;
      }
    }

    return pOriginalTextureResourceNode;
  }

  return NULL;
}

/**
 * @brief マスクテクスチャとそのマスクに対応するテクスチャリソースを探す
 */
bool DressUpModelResourceUnit::FindMaskTextureResourceAndOriginalTextureResourceNode(MaskTextureId maskTextureId, u32 submask, MaskTextureResource** out_ppMaskTextureResource, gfl2::renderingengine::scenegraph::resource::TextureResourceNode** out_ppOriginalTextureResourceNode)
{
  // マスクテクスチャ
  *out_ppMaskTextureResource = &m_MaskTextureResourceList[maskTextureId][submask];
  if ((*out_ppMaskTextureResource)->pResourceNode == NULL)
  {
    // 見つからない
    return false;
  }


  // オリジナルテクスチャ
  gfl2::renderingengine::scenegraph::DagNode* pModelResourceNode = this->GetModelResourceNode();
  *out_ppOriginalTextureResourceNode = this->FindTextureResourceNode(pModelResourceNode, *out_ppMaskTextureResource);
  if (*out_ppOriginalTextureResourceNode == NULL)
  {
    // 見つからない
    return false;
  }

  // 見つかった
  return true;
}

/**
 * @brief ロケータの取得
 */
const DressUpModelResourceUnit::DressUpLocator* DressUpModelResourceUnit::GetDressUpLocator() const
{
  // BinLinkerからロケータデータを取得する
  return static_cast<const DressUpLocator*>(this->GetBinLinkerData(NULL, DRESSUP_BIN_LINKER_INDEX_LOCATOR));
}

/**
 * @brief アイテムごとのオプションビット設定
 *
 * @param optionBit 例外設定などのオプションビット
 */
void DressUpModelResourceUnit::SetItemOptionBit(u8 optionBit)
{
  m_ItemOptionBit = optionBit;
}

/**
 * @brief アイテムごとのオプションビットが立っているか確認
 *
 * @param checkOptionBit 確認するオプション
 * @return true:立っている false:立っていない
 */
bool DressUpModelResourceUnit::IsItemOption(OptionBit checkOptionBit) const
{
  return (m_ItemOptionBit & (1 << checkOptionBit));
}

/**
 * @brief 対応する着せ替えのアイテム番号を設定
 *
 * @param itemIndex 着せ替えアイテム番号
 */
void DressUpModelResourceUnit::SetItemIndex(s16 itemIndex)
{
  m_ItemIndex = itemIndex;
}

/**
 * @brief 対応する着せ替えのアイテム番号を取得
 *
 * @return 着せ替えアイテム番号
 */
s16 DressUpModelResourceUnit::GetItemIndex() const
{
  return m_ItemIndex;
}

}}
