//======================================================================
/**
 * @file  gfl2_WinOpenGLPCDebugUtil.cpp
 * @date 15/05/28
 * @author  araki_syo
 * @brief  スクリーンショットを保存する関数
 */
//======================================================================
#if (GFL_DEBUG) && defined(GF_PLATFORM_WIN32)

#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_PCDebugUtil.h>
#include <fs/include/gfl2_FsPcUtil.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <gfx/include/gfl2_ImageData.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(gfx)

#pragma pack(1) // prevent padding insertion for Win32
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
} BITMAPFILEHEADER;
#pragma pack()

#pragma pack(1) // prevent padding insertion for Win32
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
} BITMAPINFOHEADER;
#pragma pack()

/**
 * @brief スクリーンショットを保存する
 * @param fileName PC側スクリーンショット保存先の絶対パス
 * @param hDisplay 撮影する画面のハンドル
 * @param pHeap スクリーンショットで利用するヒープ
 */
void PCDebugUtil::PrintScreen(const char* fileName, gfl2::gfx::DisplayDeviceHandle hDisplay, gfl2::heap::HeapBase* pHeap)
{
  u32 height = gfl2::gfx::GFGL::GetDisplayHeight(hDisplay);
  u32 width = gfl2::gfx::GFGL::GetDisplayWidth(hDisplay);

  u32 margin = 0;

  // 指定された画面をアクティブに
  if(width == gfl2::gfx::GFGLUtil::GetDisplayWidthByDisplayMode(gfl2::gfx::DisplayMode::CTR_LEFT))
  {
    glBindFramebuffer( GL_FRAMEBUFFER, 1);
  }
  else
  {
    glBindFramebuffer( GL_FRAMEBUFFER, 0);
    margin = s_marginWidth;
  }
  glViewport(0, 0, width, height);
  glScissor(0, 0, width, height);

  char* buf = GFL_NEW_ARRAY(pHeap) char[s_RGBFormatSize * width * height];

  glReadPixels(margin, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, buf); // bitmapはBGR

  PrintImage(buf, fileName, width, height, s_RGBFormatSize, pHeap);

  // バッファの開放
  GFL_DELETE_ARRAY buf;
}

/**
 * @brief 相対深度マップを保存する
 * @param fileName PC側深度マップ保存先の絶対パス
 * @param hDisplay 撮影する画面のハンドル
 * @param pHeap スクリーンショットで利用するヒープ
 */
void PCDebugUtil::PrintDepthMap(const char* fileName, gfl2::gfx::DisplayDeviceHandle hDisplay, gfl2::heap::HeapBase* pHeap)
{
  u32 height = gfl2::gfx::GFGL::GetDisplayHeight(hDisplay);
  u32 width = gfl2::gfx::GFGL::GetDisplayWidth(hDisplay);

  u32 margin = 0;

  // 指定された画面をアクティブに
  if(width == gfl2::gfx::GFGLUtil::GetDisplayWidthByDisplayMode(gfl2::gfx::DisplayMode::CTR_LEFT))
  {
    glBindFramebuffer( GL_FRAMEBUFFER, 1);
  }
  else
  {
    glBindFramebuffer( GL_FRAMEBUFFER, 0);
    margin = s_marginWidth;
  }
  glViewport(0, 0, width, height);
  glScissor(0, 0, width, height);

  u16* buf1 = GFL_NEW_ARRAY(pHeap) u16[width * height];
  u8* buf2 = GFL_NEW_ARRAY(pHeap) u8[s_RGBFormatSize * width * height];

  // 深度情報を取得
  glReadPixels(margin, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, buf1);

  for(u32 i = 0; i < width * height; ++i){
    buf2[i * s_RGBFormatSize] = static_cast<u8>(buf1[i]);
    buf2[i * s_RGBFormatSize + 1] = static_cast<u8>(buf1[i]);
    buf2[i * s_RGBFormatSize + 2] = static_cast<u8>(buf1[i]);
  }

  PrintImage(buf2, fileName, width, height, s_RGBFormatSize, pHeap);

  GFL_DELETE_ARRAY(buf1);
  GFL_DELETE_ARRAY(buf2);
}

/**
 * @brief 深度マップを保存する
 * @param fileName PC側深度マップ保存先の絶対パス
 * @param hDisplay 撮影する画面のハンドル
 * @param pHeap スクリーンショットで利用するヒープ
 */
void PCDebugUtil::PrintAbsDepthMap(const char* fileName, gfl2::gfx::DisplayDeviceHandle hDisplay, gfl2::heap::HeapBase* pHeap)
{
  // WIN32版では深度値は自動で正規化される
  PrintDepthMap(fileName, hDisplay, pHeap);
}

/**
 * @brief ステンシル画像を保存する
 * @param fileName PC側深度マップ保存先の絶対パス
 * @param hDisplay 撮影する画面のハンドル
 * @param pHeap スクリーンショットで利用するヒープ
 */
void PCDebugUtil::PrintStencil(const char* fileName, gfl2::gfx::DisplayDeviceHandle hDisplay, gfl2::heap::HeapBase* pHeap)
{
  u32 height = gfl2::gfx::GFGL::GetDisplayHeight(hDisplay);
  u32 width = gfl2::gfx::GFGL::GetDisplayWidth(hDisplay);

  u32 margin = 0;

  // 指定された画面をアクティブに
  if(width == gfl2::gfx::GFGLUtil::GetDisplayWidthByDisplayMode(gfl2::gfx::DisplayMode::CTR_LEFT))
  {
    glBindFramebuffer( GL_FRAMEBUFFER, 1);
  }
  else
  {
    glBindFramebuffer( GL_FRAMEBUFFER, 0);
    margin = s_marginWidth;
  }
  glViewport(0, 0, width, height);
  glScissor(0, 0, width, height);

  u8* buf1 = GFL_NEW_ARRAY(pHeap) u8[width * height];

  // ステンシル情報を取得
  glReadPixels(margin, 0, width, height, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, buf1);

  u8* buf2 = GFL_NEW_ARRAY(pHeap) u8[s_RGBFormatSize * width * height];

  for (u32 i = 0; i < width * height; i++)
  {
    buf2[i * s_RGBFormatSize] = 255 * buf1[i];
    buf2[i * s_RGBFormatSize + 1] = 255 * buf1[i];
    buf2[i * s_RGBFormatSize + 2] = 255 * buf1[i];
  }

  PrintImage(buf2, fileName, width, height, s_RGBFormatSize, pHeap);

  GFL_DELETE_ARRAY(buf1);
  GFL_DELETE_ARRAY(buf2);
}

/**
 * @brief 画素情報をビットマップとしてPCに保存する
 * @param fileName PC側画像保存先の絶対パス
 * @param width 横解像度
 * @param height 縦解像度
 * @param pHeap スクリーンショットで利用するヒープ
 */
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

  // Win32解像度 左下原点
  fileHeader.bfSize = pixelFormatSize * width * height + offset;
  infoHeader.biWidth = width;
  infoHeader.biHeight = height;

  // バッファにヘッダをコピー
  memcpy(bitMapFile, &fileHeader, sizeof(fileHeader));
  memcpy(bitMapFile + sizeof(fileHeader), &infoHeader, sizeof(infoHeader));

  memcpy(bitMapFile + offset, buf, pixelFormatSize * width * height);

  // カレントディレクトリの取得
  char path[_MAX_PATH];
  gfl2::fs::PcUtil::SetCurrentWorkDirectory(path, path, _MAX_PATH);

  // HIOでファイルへ書き込み
  gfl2::fs::PcUtil::WriteFile(fileName, bitMapFile, pixelFormatSize * height * width + offset);

  // バッファの開放
  GFL_DELETE_ARRAY bitMapFile;
}

/**
 * @brief スクリーンショットを保存する
 * @param fileName PC側スクリーンショット保存先の絶対パス
 * @param hDisplay 撮影する画面のハンドル
 * @param pHeap スクリーンショットで利用するヒープ
 */
void PCDebugUtil::PrintScreenAsTga(const char* fileName, gfl2::gfx::DisplayDeviceHandle hDisplay, gfl2::heap::HeapBase* pHeap)
{
  u32 height = gfl2::gfx::GFGL::GetDisplayHeight(hDisplay);
  u32 width = gfl2::gfx::GFGL::GetDisplayWidth(hDisplay);

  u32 margin = 0;

  // 指定された画面をアクティブに
  if(width == gfl2::gfx::GFGLUtil::GetDisplayWidthByDisplayMode(gfl2::gfx::DisplayMode::CTR_LEFT))
  {
    glBindFramebuffer( GL_FRAMEBUFFER, 1);
  }
  else
  {
    glBindFramebuffer( GL_FRAMEBUFFER, 0);
    margin = s_marginWidth;
  }
  glViewport(0, 0, width, height);
  glScissor(0, 0, width, height);

  char* buf = GFL_NEW_ARRAY(pHeap) char[s_RGBAFormatSize * width * height];

  glReadPixels(margin, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, buf); // tgaでアルファつきで出力

  PrintImageAsTga(buf, fileName, width, height, s_RGBAFormatSize, pHeap);

  // バッファの開放
  GFL_DELETE_ARRAY buf;
}

/**
 * @brief 画素情報をビットマップとしてPCに保存する
 * @param fileName PC側画像保存先の絶対パス
 * @param width 横解像度
 * @param height 縦解像度
 * @param pHeap スクリーンショットで利用するヒープ
 */
void PCDebugUtil::PrintImageAsTga(const void* buf, const char* fileName, u32 width, u32 height, u32 pixelFormatSize, gfl2::heap::HeapBase* pHeap)
{
  bool flag = false;
  gfl2::fs::Result res = gfl2::fs::PcUtil::IsPcAccessEnableFlag(&flag);

  if(!res.IsSuccess() || !flag)
  {
    GFL_PRINT("Host IO is not available!\n");
    return;
  }

  // 画像ファイルのひな形作成
  gfl2::gfx::ImageData::TargaHeader targaHeader;
  targaHeader.idLength = 0x00;
  targaHeader.colorMapType = 0x00;
  targaHeader.imageType = 0x02;
  targaHeader.colorMapOrigin[0] = 0x00;
  targaHeader.colorMapOrigin[1] = 0x00;
  targaHeader.colorMapLength[0] = 0x00;
  targaHeader.colorMapLength[1] = 0x00;
  targaHeader.colorMapEntrySize = 0x00;
  targaHeader.xOrigin = 0x00;
  targaHeader.yOrigin = 0x00;
  targaHeader.width = width;
  targaHeader.height = height;
  targaHeader.bpp = 0x20;
  targaHeader.attribute = 0x00;

  u32 offset = sizeof(targaHeader);

  char* bitMapFile = GFL_NEW_ARRAY(pHeap) char[pixelFormatSize * width * height + offset];

  // バッファにヘッダをコピー
  memcpy(bitMapFile, &targaHeader, sizeof(targaHeader));

  memcpy(bitMapFile + offset, buf, pixelFormatSize * width * height);

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
  PrintImage(buf, fileName, width, height, pixelFormatSize, pHeap);
}

GFL_NAMESPACE_END(gfx)
GFL_NAMESPACE_END(gfl2)

#endif // GFL_DEBUG && GF_PLATFORM_WIN32