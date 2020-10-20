//===========================================================================
/**
 * @file    gfl2_assert_CLR.cpp
 * @brief   アサート処理
 */
//===========================================================================
#include <clr/include/debug/gfl2_assert_CLR.h>

namespace gfl2 { namespace clr { namespace debug {

/**
 * @brief ASSERTループ内コールバック関数設定
 * @param GFL_ASSERTで動作を停止した際、閉じる対象となるウインドウハンドル
 */
void Assert::SetUpAssertLoopFunc(System::IntPtr handle)
{
  NativeAssert::SetHWND(reinterpret_cast<HWND>(handle.ToPointer()));
  gfl2::debug::GFL_ASSERT_SetLoopFunc(NativeAssert::AssertLoopFunc);
}

HWND NativeAssert::m_Hwnd;  //!< @brief GFL_ASSERTで動作を停止した際、閉じる対象となるウインドウハンドル

/**
 * @brief C#側でGFL_ASSERTで停止した際、閉じる対象となるウインドウハンドルの設定
 * @param HWND hwnd ウインドウハンドル
 */
void NativeAssert::SetHWND(HWND hwnd)
{
  m_Hwnd = hwnd;
}

/**
 * @brief GFL_ASSERTで停止した際呼び出される関数
 */
void NativeAssert::AssertLoopFunc(const c8* pbuff, b32 bStop)
{
  MessageBoxA(NULL, pbuff, "GFL_ASSERT", MB_ICONERROR);
  // アプリケーションを強制終了
  System::Environment::Exit(0);
}

}}}
