//==============================================================================
/**
 * @file  gfl_Amx.h
 *  pawn オリジナルのファイル群 C のファイルのみ
 * @author  kimura_shin
 * @data  2011/02/03, 15:45
 */
// =============================================================================

#ifndef GFL_AMX_H
#define GFL_AMX_H

#ifdef __cplusplus
#include <gflib.h>
#else // pawn オリジナルCファイル用
#include <core/gfl_Config.h>

#include <core/gfl_OsHeader.h>
#include <core/gfl_Typedef.h>

// これらを定義しないとコンパイルが通らない
#define AMX_ANSIONLY

#endif

typedef s16 int16_t;
typedef u16 uint16_t;

#ifdef GFL_PLATFORM_PC
typedef long int          int32_t;
typedef unsigned long int uint32_t;
#endif

#ifdef GFL_PLATFORM_3DS
#define HAVE_STDINT_H
#elif defined(GFL_PLATFORM_WINDOWS)
#define __STDC__ 1
#endif
#define HAVE_ALLOCA_H 0

#include "osdefs.h"

//#define GFL_PAWN_DEBUG GFL_DEBUG
#include <fs/gfl_HioSwitch.h>

#ifndef GFL_PAWN_DEBUG
#if defined( GFL_PLATFORM_3DS )
  #define GFL_PAWN_DEBUG GFL_HIO_ENABLE
#else
  #define GFL_PAWN_DEBUG GFL_DEBUG
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
#include "amxdbg.h"
#endif

#ifdef __cplusplus

typedef char c8; //!< 8ビットの文字変数

// 浮動小数点を扱いたいので
extern "C" {
  extern int AMXEXPORT amx_FloatInit( AMX *amx );
#if GFL_PAWN_PRINT
  typedef char TCHAR;
#include "amxcons.h"
  extern int AMXEXPORT amx_PrintInit( AMX *amx );
#endif
#if GFL_PAWN_CORE
  extern int AMXEXPORT AMXAPI amx_CoreInit(AMX *amx);
#endif
}
#endif

#endif  /* GFL_AMX_H */

