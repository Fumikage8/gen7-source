#if !defined(GFLIB2_CLR_DEBUG_ASSERT_CLR_H_INCLUDED)
#define GFLIB2_CLR_DEBUG_ASSERT_CLR_H_INCLUDED
#pragma once
//===========================================================================
/**
 * @file    gfl2_assert_CLR.h
 * @brief   アサート処理
 */
//===========================================================================
#include <debug/include/gfl2_Assert.h>
#include <Windows.h>
#using <System.dll>
#using <System.Windows.Forms.dll>

namespace gfl2 { namespace clr { namespace debug {
public ref class Assert
{
public:
  /**
   * @brief ASSERTループ内コールバック関数設定
   */
  static void SetUpAssertLoopFunc(System::IntPtr handle);
  static System::Windows::Forms::Form^ m_ApplicationCore;
};

public class NativeAssert
{
public:
  /**
   * @brief C#側でGFL_ASSERTで停止した際、閉じる対象となるウインドウハンドルの設定
   * @param HWND hwnd ウインドウハンドル
   */
  static void SetHWND(HWND hwnd);

  /**
   * @brief GFL_ASSERTで停止した際呼び出される関数
   */
  static void AssertLoopFunc(const c8* pbuff, b32 bStop);

private:
  static HWND m_Hwnd;
};

}}}

#endif