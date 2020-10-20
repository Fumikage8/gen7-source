//======================================================================
/**
 * @file  gfl2_PCDebugUtil.h
 * @date 15/05/28
 * @author  araki_syo
 * @brief  PCにデバッグ用スクリーンショットを保存する関数
 * @detail gfl2_(CTR|WinOpenGL)PCDebugUtil.cppにインクルードされます
 */
//======================================================================
#if (GFL_DEBUG)

#if !defined GFLIB2_GFX_DEBUG_UTIL
#define GFLIB2_GFX_DEBUG_UTIL
#pragma once

#if !defined(_MAX_PATH)
#define _MAX_PATH 260 // max path length of Windows system (defined in <stdlib.h> of Win32)
#endif

#include <gfx/include/gfl2_types.h>
#include <heap/include/gfl2_heap_manager.h>

namespace gfl2 { namespace gfx {

class PCDebugUtil
{
public:
  /**
   * @brief スクリーンショットを保存する
   * @param fileName PC側スクリーンショット保存先の絶対パス
   * @param hDisplay 撮影する画面のハンドル
   * @param pHeap スクリーンショットで利用するヒープ
   */
  static void PrintScreen(const char* fileName, gfl2::gfx::DisplayDeviceHandle hDisplay, gfl2::heap::HeapBase* pHeap);

  /**
   * @brief スクリーンショットをTgaで保存する
   * @param fileName PC側スクリーンショット保存先の絶対パス
   * @param hDisplay 撮影する画面のハンドル
   * @param pHeap スクリーンショットで利用するヒープ
   */
  static void PrintScreenAsTga(const char* fileName, gfl2::gfx::DisplayDeviceHandle hDisplay, gfl2::heap::HeapBase* pHeap);

  /**
   * @brief 相対深度マップを保存する
   * @param fileName PC側深度マップ保存先の絶対パス
   * @param hDisplay 撮影する画面のハンドル
   * @param pHeap スクリーンショットで利用するヒープ
   */
  static void PrintDepthMap(const char* fileName, gfl2::gfx::DisplayDeviceHandle hDisplay, gfl2::heap::HeapBase* pHeap);

  /**
   * @brief 絶対深度マップを保存する
   * @param fileName PC側深度マップ保存先の絶対パス
   * @param hDisplay 撮影する画面のハンドル
   * @param pHeap スクリーンショットで利用するヒープ
   */
  static void PrintAbsDepthMap(const char* fileName, gfl2::gfx::DisplayDeviceHandle hDisplay, gfl2::heap::HeapBase* pHeap);

  /**
   * @brief ステンシル画像を保存する
   * @param fileName PC側深度マップ保存先の絶対パス
   * @param hDisplay 撮影する画面のハンドル
   * @param pHeap スクリーンショットで利用するヒープ
   */
  static void PrintStencil(const char* fileName, gfl2::gfx::DisplayDeviceHandle hDisplay, gfl2::heap::HeapBase* pHeap);

private:
  PCDebugUtil(); // forbid constructor
  PCDebugUtil(const PCDebugUtil&); // forbid copy constructor
  ~PCDebugUtil(); // forbid destructor

  PCDebugUtil& operator = (const PCDebugUtil&); // forbid assign operator

public:
  static void PrintImage(const void* buf, const char* fileName, u32 width, u32 height, u32 pixelFormatSize, gfl2::heap::HeapBase* pHeap);
  static void PrintImageAsTga(const void* buf, const char* fileName, u32 width, u32 height, u32 pixelFormatSize, gfl2::heap::HeapBase* pHeap);
  static void PrintBlockImage(const void* buf, const char* fileName, u32 width, u32 height, u32 pixelFormatSize, gfl2::heap::HeapBase* pHeap);

private:
  // for CTR
  static void Block2Linear(void* buf,const void* src, u32 width, u32 height, u8 blockSize);
  static void Texel2Linear(void* buf, const void* src, u32 width, u32 height, u8 blockSize); 
  static void DepthStencil2Depth(u32* buf, u32 width, u32 height);
  static void DepthStencil2AbsDepth(u32* buf, u32 width, u32 height);
  static void DepthStencil2Stencil(u32* buf, u32 width, u32 height);

private:
  struct tagBITMAPFILEHEADER;
  struct tagBITMAPINFOHEADER;

  static const u32 s_RGBFormatSize = 3;
  static const u32 s_RGBAFormatSize = 4;
  static const u32 s_marginWidth = 40; // (400 - 320) / 2
};

} // gfx
} // gfl2

#endif // GFLIB2_GFX_DEBUG_UTIL

#endif // GFL_DEBUG