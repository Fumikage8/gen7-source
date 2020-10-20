//===========================================================================
/**
 * @file    gfl_debug_print.cpp
 * @brief   デバッグプリント処理
 * @author  GAMEFREAK inc.
 * @date    2014.03.03
 */
//===========================================================================

#ifdef GF_PLATFORM_WIN32
#define NOMINMAX
#include <windows.h>
#endif


#ifdef GF_PLATFORM_CTR
#include <nn/dbg.h>
#endif


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>




GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(debug)

static const int defaultPrintFlag = PRINT_FLAG_LEVEL1;
  
static u16 printFlag = defaultPrintFlag;
static u16 printEnable = 1;

//----------------------------------------------------------------------
/**
 * @brief   フラグの初期化 GFL_PRINT_FLAG_LEVEL1が設定されます
 */
//----------------------------------------------------------------------
void ResetPrintFlag()
{
  printFlag = defaultPrintFlag;
}
  
//----------------------------------------------------------------------
/**
 * @brief   フラグの設定(上書き)
 *
 * @param[in]    flag フラグビット
 */
//----------------------------------------------------------------------
void SetPrintFlag( const int flag )
{
  printFlag = flag;
}
bool TogglePrintFlg(void)
{
  printEnable = 1 - printEnable;
  return printEnable ? true : false;
}

  
//----------------------------------------------------------------------
/**
 * @brief   フラグの設定(追加)
 *
 * @param[in]    flag フラグビット
 */
//----------------------------------------------------------------------
void AddPrintFlag( const int flag )
{
  printFlag |= flag;
}
  
//----------------------------------------------------------------------
/**
 * @brief   フラグの設定(削除)
 *
 * @param[in]    flag フラグビット
 */
//----------------------------------------------------------------------
void SubPrintFlag( const int flag )
{
  printFlag -= (printFlag&flag);
}

void PrintCore(const int flag , const char* pszFormat,...)
{
  if(!printEnable)
  {
    return;
  }
  if( flag == (printFlag&flag) )
  {
#if defined(GF_PLATFORM_WIN32)
    const size_t bufSize = 512;
    va_list	argp;
    char pszBuf[bufSize];
    va_start(argp, pszFormat);
    vsprintf_s( pszBuf, bufSize, pszFormat, argp);
    va_end(argp);
    ::OutputDebugStringA( pszBuf);	
#elif defined(GF_PLATFORM_CTR)
    va_list	argp;
    va_start(argp, pszFormat);
  
    //プラットフォームごとの処理

    //可変引数が渡せないので内部関数使用
    (void)nn::dbg::detail::VPrintf(pszFormat,argp);

    //プラットフォームごとの処理 ここまで

    va_end(argp);
#endif
  }
}

#if GFL_DEBUG_PRINT

void UserPrint( const char * fmt, ... )
{
  if(!printEnable)
  {
    return;
  }
#if defined(GF_PLATFORM_WIN32)
  const size_t bufSize = 512;
  va_list	argp;
  char pszBuf[bufSize];
  va_start(argp, fmt);
  vsprintf_s( pszBuf, bufSize, fmt, argp);
  va_end(argp);
  ::OutputDebugStringA( pszBuf);	
#elif defined(GF_PLATFORM_CTR)
  va_list	argp;
  va_start(argp, fmt);
  
  //プラットフォームごとの処理

  //可変引数が渡せないので内部関数使用
  (void)nn::dbg::detail::VPrintf(fmt,argp);

  //プラットフォームごとの処理 ここまで

  va_end(argp);
#endif
}

#else  //GFL_DEBUG_PRINT
void UserPrint( const char * fmt, ... ){GFL_UNUSED( fmt );}
#endif  //GFL_DEBUG_PRINT

#if GFL_DEBUG


//----------------------------------------------------------------------
/**
* @brief   ログ出力(RELEASEでも動作する版)
* @note    一時確認の使用を想定しているもの、使用箇所は基本的にコミットしないでください
* @note    【コミットしても問題ないケース】
* @note    ・デバッグメニューの先で特定の操作を行った際に通るシーケンス
* @note    ・ゲーム起動時のシステム使用量
*
* @param[in]    fmt  出力(可変引数対応)
*/
//----------------------------------------------------------------------
void ReleasePrintCore( const char* pszFormat , ... )
{
#if defined(GF_PLATFORM_WIN32)
  const size_t bufSize = 512;
  va_list	argp;
  char pszBuf[bufSize];
  va_start(argp, pszFormat);
  vsprintf_s( pszBuf, bufSize, pszFormat, argp);
  va_end(argp);
  ::OutputDebugStringA( pszBuf);	
#elif defined(GF_PLATFORM_CTR)
  va_list	argp;
  va_start(argp, pszFormat);

  //プラットフォームごとの処理

  //可変引数が渡せないので内部関数使用
  (void)nn::dbg::detail::VPrintf(pszFormat,argp);

  //プラットフォームごとの処理 ここまで

  va_end(argp);
#endif
}

#endif // #if GFL_DEBUG


GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl2)

