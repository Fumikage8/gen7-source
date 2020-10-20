//==============================================================================
/**
 * @file	gfl_UnityPawn.c
 * @brief	
 * @author	kimura_shin
 * @data	2011/02/03, 15:18
 */
// =============================================================================

#include "gfl_Amx.h"

// pawn 必須ファイル
#include "amx.c"
#include "float.c"

#if GFL_PAWN_PRINT
#include "amxprint.c"
#endif
#if GFL_PAWN_CORE
#ifdef GFL_PLATFORM_3DS // 本当は大文字小文字を区別しない比較…ないのか…
#define stricmp(str1,str2) strcmp(str1,str2)
#define SN_TARGET_PS2 // PS2 のふりをする
#endif

#include "amxcore.c" // amxprint の後にしないとコンパイルエラー

#ifdef GFL_PLATFORM_3DS
#undef SN_TARGET_PS2
#endif

#endif
#if GFL_PAWN_DEBUG
#include "amxdbg.c"
#endif
