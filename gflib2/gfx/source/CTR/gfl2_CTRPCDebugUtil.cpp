//======================================================================
/**
 * @file  gfl2_CTRPCDebugUtil.cpp
 * @date 15/06/08
 * @author  araki_syo
 * @brief  スクリーンショットを保存する関数
 */
//======================================================================
#if (GFL_DEBUG) && defined(GF_PLATFORM_CTR)

#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/CTR/gfl2_CTRGL.h>
#include <gfx/include/gfl2_PCDebugUtil.h>
#include <fs/include/gfl2_FsPcUtil.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <nn/util/util_Float24.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(gfx)

typedef struct PCDebugUtil::tagBITMAPFILEHEADER {
    unsigned short bfType;
    unsigned long  bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned long  bfOffBits;

    tagBITMAPFILEHEADER(u32 width, u32 height, u32 pixelFormatSize) : bfType(0x4D42),
      bfSize(pixelFormatSize * width * height + 54), // 54 bit offset
    bfReserved1(0),
    bfReserved2(0),
    bfOffBits(54) {} // 54 bit offset
} __attribute__ ((packed)) BITMAPFILEHEADER;

typedef struct PCDebugUtil::tagBITMAPINFOHEADER{
    unsigned long  biSize;
    long           biWidth;
    long           biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned long  biCompression;
    unsigned long  biSizeImage;
    long           biXPixPerMeter;
    long           biYPixPerMeter;
    unsigned long  biClrUsed;
    unsigned long  biClrImporant;

    tagBITMAPINFOHEADER(u32 width, u32 height, u32 pixelFormatSize)  : biSize(40),
    biWidth(width),
    biHeight(height),
    biPlanes(1),
    biBitCount(pixelFormatSize * 8),
    biCompression(0),
    biSizeImage(0),
    biXPixPerMeter(0),
    biYPixPerMeter(0),
    biClrUsed(0),
    biClrImporant(0) {}
} __attribute__ ((packed)) BITMAPINFOHEADER;

/**
 * @brief スクリーンショットを24bitビットマップ形式で保存する
 * @param fileName PC側スクリーンショット保存先の絶対パス
 * @param hDisplay 撮影する画面のハンドル
 * @param pHeap スクリーンショットで利用するヒープ
 */
void PCDebugUtil::PrintScreen(const char* fileName, gfl2::gfx::DisplayDeviceHandle hDisplay, gfl2::heap::HeapBase* pHeap)
{
  u32 height = gfl2::gfx::GFGL::GetDisplayHeight(hDisplay);
  u32 width = gfl2::gfx::GFGL::GetDisplayWidth(hDisplay);

  const void* buf = gfl2::gfx::ctr::CTRGL::GetDisplayBuffer_(hDisplay);

  PrintImage(buf, fileName, width, height, s_RGBFormatSize, pHeap);
}

/**
 * @brief 相対深度画像を32bitビットマップ形式で保存する
 * @param fileName PC側深度画像保存先の絶対パス
 * @param hDisplay 撮影する画面のハンドル
 * @param pHeap スクリーンショットで利用するヒープ
 */
void PCDebugUtil::PrintDepthMap(const char* fileName, gfl2::gfx::DisplayDeviceHandle hDisplay, gfl2::heap::HeapBase* pHeap)
{
  // 512 x 256
  u32 height = gfl2::gfx::ctr::CTRGL::GetFrameBufferHeight_();
  u32 width = gfl2::gfx::ctr::CTRGL::GetFrameBufferWidth_();

  const uptr depthStencil = gfl2::gfx::ctr::CTRGL::GetDepthStencilBufferStatic_(hDisplay);

  u8* buf = GFL_NEW_ARRAY(pHeap) u8[s_RGBAFormatSize * width * height];

  Block2Linear(buf, reinterpret_cast<void*>(depthStencil), width, height, 8);
  DepthStencil2Depth(reinterpret_cast<u32*>(buf), width, height);

  PrintImage(reinterpret_cast<void*>(buf), fileName, height, width, s_RGBAFormatSize, pHeap);

  GFL_DELETE_ARRAY(buf);
}

/**
 * @brief 絶対深度画像を32bitビットマップ形式で保存する
 * @param fileName PC側深度画像保存先の絶対パス
 * @param hDisplay 撮影する画面のハンドル
 * @param pHeap スクリーンショットで利用するヒープ
 */
void PCDebugUtil::PrintAbsDepthMap(const char* fileName, gfl2::gfx::DisplayDeviceHandle hDisplay, gfl2::heap::HeapBase* pHeap)
{
  // 512 x 256
  u32 height = gfl2::gfx::ctr::CTRGL::GetFrameBufferHeight_();
  u32 width = gfl2::gfx::ctr::CTRGL::GetFrameBufferWidth_();

  const uptr depthStencil = gfl2::gfx::ctr::CTRGL::GetDepthStencilBufferStatic_(hDisplay);

  u8* buf = GFL_NEW_ARRAY(pHeap) u8[s_RGBAFormatSize * width * height];

  Block2Linear(buf, reinterpret_cast<void*>(depthStencil), width, height, 8);
  DepthStencil2AbsDepth(reinterpret_cast<u32*>(buf), width, height);

  PrintImage(reinterpret_cast<void*>(buf), fileName, height, width, s_RGBAFormatSize, pHeap);

  GFL_DELETE_ARRAY(buf);
}

/**
 * @brief ステンシル画像を保存する
 * @param fileName PC側ステンシル画像保存先の絶対パス
 * @param hDisplay 撮影する画面のハンドル
 * @param pHeap スクリーンショットで利用するヒープ
 */
void PCDebugUtil::PrintStencil(const char* fileName, gfl2::gfx::DisplayDeviceHandle hDisplay, gfl2::heap::HeapBase* pHeap)
{
  // 512 x 256
  u32 height = gfl2::gfx::ctr::CTRGL::GetFrameBufferHeight_();
  u32 width = gfl2::gfx::ctr::CTRGL::GetFrameBufferWidth_();

  const uptr depthStencil = gfl2::gfx::ctr::CTRGL::GetDepthStencilBufferStatic_(hDisplay);

  u8* buf = GFL_NEW_ARRAY(pHeap) u8[s_RGBAFormatSize * width * height];

  Block2Linear(buf, reinterpret_cast<void*>(depthStencil), width, height, 8);
  DepthStencil2Stencil(reinterpret_cast<u32*>(buf), width, height);

  PrintImage(reinterpret_cast<void*>(buf), fileName, height, width, s_RGBAFormatSize, pHeap);

  GFL_DELETE_ARRAY(buf);
}

void PCDebugUtil::PrintImage(const void* buf, const char* fileName, u32 width, u32 height, u32 pixelFormatSize, gfl2::heap::HeapBase* pHeap)
{
  bool flag = false;
  gfl2::fs::Result res = gfl2::fs::PcUtil::IsPcAccessEnableFlag(&flag);

  if(!res.IsSuccess() || !flag)
  {
    GFL_PRINT("Host IO is not available!\n");
    return;
  }

  // 画像ファイルのひな形作成
  BITMAPFILEHEADER fileHeader(width, height, pixelFormatSize);
  BITMAPINFOHEADER infoHeader(width, height, pixelFormatSize);
  u32 offset = sizeof(fileHeader) + sizeof(infoHeader);

  char* bitMapFile = GFL_NEW_ARRAY(pHeap) char[pixelFormatSize * width * height + offset];

  // CTR解像度 左上原点
  fileHeader.bfSize = pixelFormatSize * height * width + offset;
  infoHeader.biWidth = height; // transpsose
  infoHeader.biHeight = -width; // transpose

  // バッファにヘッダをコピー
  std::memcpy(bitMapFile, &fileHeader, sizeof(fileHeader));
  std::memcpy(bitMapFile + sizeof(fileHeader), &infoHeader, sizeof(infoHeader));

  // バッファに画素情報をコピー
  std::memcpy(bitMapFile + offset, buf, pixelFormatSize * height * width);

  // カレントディレクトリの取得
  char path[_MAX_PATH];
  gfl2::fs::PcUtil::SetCurrentWorkDirectory(path, path, _MAX_PATH);

  // HIOでファイルへ書き込み
  gfl2::fs::PcUtil::WriteFile(fileName, bitMapFile, pixelFormatSize * height * width + offset);

  // バッファの開放
  GFL_DELETE_ARRAY bitMapFile;
}

void PCDebugUtil::PrintBlockImage(const void* buf, const char* fileName, u32 width, u32 height, u32 pixelFormatSize, gfl2::heap::HeapBase* pHeap)
{
  u8* tmp_buf = GFL_NEW_ARRAY(pHeap) u8[pixelFormatSize * width * height];

  Block2Linear(tmp_buf, buf, height, width, 8);
  PrintImage(reinterpret_cast<void*>(tmp_buf), fileName, width, height, pixelFormatSize, pHeap);

  GFL_DELETE_ARRAY(tmp_buf);
}

/**
 * @brief ブロック形式のピクセルデータをリニア形式に変換する
 * @param out buf リニア形式に変換されたピクセルデータ
 * @param in src ブロック形式のピクセルデータ
 * @width 画面横解像度
 * @height 画面縦解像度
 * @blockSize ブロックサイズ
 */
void PCDebugUtil::Block2Linear(void* buf, const void* src, u32 width, u32 height, u8 blockSize)
{
  // 各8x8ブロックに対しテクセルからリニア形式へ変換
  for (int i = 0; i < height / blockSize; ++i)
  {
    for (int j = 0; j < width / blockSize; ++j)
    {
      Texel2Linear(reinterpret_cast<u32*>(buf) + i * blockSize * width + j * blockSize, reinterpret_cast<const u32*>(src) + i * (width / blockSize) * (blockSize * blockSize) + j * (blockSize * blockSize), width, height, blockSize);
    }
  }
}

void PCDebugUtil::Texel2Linear(void* buf, const void* src, u32 width, u32 height, u8 blockSize)
{
  // 再帰呼び出しだけど再帰深度は高々3だから許してほしい
  if(blockSize == 1)
  {
    std::memcpy(buf, src, 4);
  }
  else
  {
    Texel2Linear(buf, src, width, height, blockSize / 2);

    //右ブロック
    Texel2Linear(reinterpret_cast<u32*>(buf) + blockSize / 2,
      reinterpret_cast<const u32*>(src) + (blockSize * blockSize) / 4,
      width,
      height,
      blockSize / 2);

    //下ブロック
    Texel2Linear(reinterpret_cast<u32*>(buf) + width * (blockSize / 2),
      reinterpret_cast<const u32*>(src) + (blockSize * blockSize) / 2,
      width,
      height,
      blockSize / 2);

    //右下ブロック
    Texel2Linear(reinterpret_cast<u32*>(buf) + width * (blockSize / 2) + blockSize / 2,
      reinterpret_cast<const u32*>(src) + 3 * (blockSize * blockSize) / 4,
      width,
      height,
      blockSize / 2);
  }
}

/**
 * @brief デプスステンシルバッファのピクセルデータをグレースケールの相対深度値に変換する
 * @param in out buf デプスステンシルバッファ
 * @width 画面横解像度
 * @height 画面縦解像度
 */
void PCDebugUtil::DepthStencil2Depth(u32* buf, u32 width, u32 height)
{
  const u32 maxDepth = 0x00ffffff;
  u32 minDepth = 0x00ffffff;

  // 深度値の下限を取得
  for (int i = 0; i < width * height; i++)
  {
    u32 rawDepth = 0;
    std::memcpy(&rawDepth, buf + i, 3);

    minDepth &= rawDepth;
  }

  // 深度値を正規化
  for (int i = 0; i < width * height; i++)
  {
    u32 rawDepth = 0;
    std::memcpy(&rawDepth, buf + i, 3);
    f32 normalDepth = static_cast<f32>(rawDepth - minDepth) / static_cast<f32>(maxDepth - minDepth);
    u8 depth = static_cast<u8>(normalDepth * 255);

    std::memcpy(reinterpret_cast<u8*>(buf + i), &depth, 1);
    std::memcpy(reinterpret_cast<u8*>(buf + i) + 1, &depth, 1);
    std::memcpy(reinterpret_cast<u8*>(buf + i) + 2, &depth, 1);
  }
}

/**
 * @brief デプスステンシルバッファのピクセルデータをグレースケールの絶対深度値に変換する
 * @param in out buf デプスステンシルバッファ
 * @width 画面横解像度
 * @height 画面縦解像度
 * @detail 絶対深度値の場合、出力された画像が非常に見づらい場合があります
 */
void PCDebugUtil::DepthStencil2AbsDepth(u32* buf, u32 width, u32 height)
{
  for (int i = 0; i < width * height; i++)
  {
    u32 rawDepth = 0;
    std::memcpy(&rawDepth, buf + i, 3);

    u8 depth = rawDepth / 0xffff - 1;
    std::memcpy(reinterpret_cast<u8*>(buf + i), &depth, 1);
    std::memcpy(reinterpret_cast<u8*>(buf + i) + 1, &depth, 1);
    std::memcpy(reinterpret_cast<u8*>(buf + i) + 2, &depth, 1);
  }
}

/**
 * @brief デプスステンシルバッファのピクセルデータをステンシル値に変換する
 * @param in out buf デプスステンシルバッファ
 * @width 画面横解像度
 * @height 画面縦解像度
 */
void PCDebugUtil::DepthStencil2Stencil(u32* buf, u32 width, u32 height)
{
  for (int i = 0; i < width * height; i++)
  {
    u8 stencil = 0;
    std::memcpy(&stencil, reinterpret_cast<u8*>(buf + i) + 3, 1);

    std::memcpy(reinterpret_cast<u8*>(buf + i), &stencil, 1);
    std::memcpy(reinterpret_cast<u8*>(buf + i) + 1, &stencil, 1);
    std::memcpy(reinterpret_cast<u8*>(buf + i) + 2, &stencil, 1);
  }
}

GFL_NAMESPACE_END(gfx)
GFL_NAMESPACE_END(gfl2)

#endif // GFL_DEBUG && GF_PLATFORM_CTR