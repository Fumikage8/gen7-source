//==============================================================================
/**
 * @file  gfl_UnityPawn.c
 * @brief 
 * @author  kimura_shin
 * @data  2011/02/03, 15:18
 */
// =============================================================================

#if defined __WIN32__ || defined _WIN32 || defined WIN32 || defined __MSDOS__
  #include <malloc.h>
#pragma warning(disable:4127)
#pragma warning(disable:4101)
#endif

#include "pawn/include/gfl2_pawn.h"
#include "pawn/include/gfl2_Amx.h"

#ifdef GF_PLATFORM_CTR
//CTRの場合、GCC拡張機能goto *expr関連の警告を抑制
#pragma diag_suppress 1211
#endif
// pawn 必須ファイル
#include "amx.c"
#include "float.c"

#if GFL_PAWN_PRINT
#include "amxprint.c"
#endif
#if GFL_PAWN_CORE
#ifdef GF_PLATFORM_CTR // 本当は大文字小文字を区別しない比較…ないのか…
#define stricmp(str1,str2) strcmp(str1,str2)
#define SN_TARGET_PS2 // PS2 のふりをする
#endif

#include "amxcore.c" // amxprint の後にしないとコンパイルエラー

#ifdef GF_PLATFORM_CTR
#undef SN_TARGET_PS2
#endif

#endif
#if GFL_PAWN_DEBUG
#include "amxdbg.c"
#endif
