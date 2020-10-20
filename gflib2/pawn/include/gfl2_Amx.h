//==============================================================================
/**
 * @file  gfl2_Amx.h
 *  pawn オリジナルのファイル群 C のファイルのみ
 * @author  kimura_shin
 * @data  2011/02/03, 15:45
 */
// =============================================================================

#if !defined(__GFL_AMX_H__) // 重複定義防止
#define __GFL_AMX_H__   // 重複定義防止マクロ
#pragma once


#include "typedef.h"


#ifdef GF_PLATFORM_CTR
  #define HAVE_STDINT_H
#elif defined(GF_PLATFORM_WIN32)
//  #define __STDC__ 1
//  #include <tchar.h>
#endif

#define HAVE_ALLOCA_H 0

#include "osdefs.h"

//#define GFL_PAWN_DEBUG GFL_DEBUG
#if 0 //★★未対応
#include "fs/gfl_HioSwitch.h"
#endif

#ifndef GFL_PAWN_DEBUG
  #if defined( GF_PLATFORM_CTR )
    #define GFL_PAWN_DEBUG GFL_DEBUG_PRINT
  #else
    #define GFL_PAWN_DEBUG GFL_DEBUG_PRINT
  #endif
#endif

#ifndef GFL_PAWN_CORE
  #define GFL_PAWN_CORE 1
  //#define AMX_NORANDOM // pawn の random を使用しない場合に宣言
  #define AMX_NOPROPLIST // pawn 間の通信を使用しない場合に宣言
#endif

#ifndef GFL_PAWN_PRINT
  #define GFL_PAWN_PRINT 1
#endif

#include "amx.h"

#if GFL_PAWN_DEBUG
#ifdef GF_PLATFORM_WIN32
//Win32の場合はWarning抑止のpragmaで囲む
#pragma warning(push)
#pragma warning(disable:4510)
#pragma warning(disable:4512)
#pragma warning(disable:4610)
#include "amxdbg.h"
#pragma warning(pop)
#else
//CTRの場合はそのままinclude
#include "amxdbg.h"
#endif
#endif

#ifdef __cplusplus
 
typedef char c8; //!< 8ビットの文字変数

// 浮動小数点を扱いたいので
extern "C" {
  extern int AMXEXPORT amx_FloatInit( AMX *amx );
  #if GFL_PAWN_PRINT
    #ifdef GFL2_PAWN_PROJECT
      typedef char TCHAR; //pawnプロジェクトでだけ定義する
      #include "amxcons.h"
    #endif  
    extern int AMXEXPORT amx_PrintInit( AMX *amx );
  #endif
    
  #if GFL_PAWN_CORE
    extern int AMXEXPORT AMXAPI amx_CoreInit(AMX *amx);
  #endif
}

#endif  //__cplusplus


#endif // __GFL_AMX_H__ 重複定義防止
