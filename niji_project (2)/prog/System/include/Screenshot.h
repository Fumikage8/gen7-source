//======================================================================
/**
 * @file  Screenshot.h
 * @date 15/05/28
 * @author  araki_syo
 * @brief  PCにスクリーンショットを保存する関数
 */
//======================================================================
#if (PM_DEBUG)

#if !defined SYSTEM_SCREENSHOT
#define SYSTEM_SCREENSHOT
#pragma once

#include <System/include/GflUse.h>

namespace System { 

#if defined(GF_PLATFORM_CTR)
#define MAX_PATH 260 // パスの最大長
#endif // GF_PLATFORM_CTR

  class Screenshot
  {
  public:
    /**
     * @brief スクリーンショットを保存する
     * @param fileName ファイル名
     * @param displayMode スクリーンショットを撮影する画面
     */
    static void PrintScreen(const char* fileName, gfl2::gfx::DisplayMode displayMode);

    /**
     * @brief 相対深度画像を保存する
     * @param fileName ファイル名
     * @param displayMode 深度画像を撮影する画面 
     * @detail 深度値は画像として見やすいよう補正されます　他のシーンと深度画像の比較を行う場合は絶対深度画像を使用してください
     */
    static void PrintDepthMap(const char* fileName, gfl2::gfx::DisplayMode displayMode);

    /**
     * @brief 絶対深度画像を保存する
     * @param fileName ファイル名
     * @param displayMode 深度画像を撮影する画面
     * @detail 絶対深度画像は非常に見づらい場合があります
     */
    static void PrintAbsDepthMap(const char* fileName, gfl2::gfx::DisplayMode displayMode);

    /**
     * @brief ステンシル画像を保存する
     * @param fileName ファイル名
     * @param displayMode ステンシル画像を撮影する画面 
     */
    static void PrintStencil(const char* fileName, gfl2::gfx::DisplayMode displayMode);

  private:
    Screenshot(); // forbid constructor
    Screenshot(const Screenshot&); //copy constructor
    ~Screenshot(); // forbid destructor

    Screenshot& operator = (const Screenshot&); // forbid assign operator

    /**
     * @brief キャプチャ画像保存先の絶対パスを取得する
     */
    static void GetTargetPath(char* abspath, const char* filename);
  };
} // System

#endif // SYSTEM_SCREENSHOT

#endif // PM_DEBUG
