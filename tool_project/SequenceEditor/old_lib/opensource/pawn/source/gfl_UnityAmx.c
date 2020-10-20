//==============================================================================
/**
 * @file	gfl_UnityPawn.c
 * @brief	
 * @author	kimura_shin
 * @data	2011/02/03, 15:18
 */
// =============================================================================

#include "gfl_Amx.h"

// pawn �K�{�t�@�C��
#include "amx.c"
#include "float.c"

#if GFL_PAWN_PRINT
#include "amxprint.c"
#endif
#if GFL_PAWN_CORE
#ifdef GFL_PLATFORM_3DS // �{���͑啶������������ʂ��Ȃ���r�c�Ȃ��̂��c
#define stricmp(str1,str2) strcmp(str1,str2)
#define SN_TARGET_PS2 // PS2 �̂ӂ������
#endif

#include "amxcore.c" // amxprint �̌�ɂ��Ȃ��ƃR���p�C���G���[

#ifdef GFL_PLATFORM_3DS
#undef SN_TARGET_PS2
#endif

#endif
#if GFL_PAWN_DEBUG
#include "amxdbg.c"
#endif
