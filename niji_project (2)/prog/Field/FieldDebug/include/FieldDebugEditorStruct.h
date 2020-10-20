//======================================================================
/**
 * @file FieldDebugEditorStruct.h
 * @date 2016/04/12 22:42:28
 * @author ikuta_junya
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELDDEBUGEDITOR_H_INCLUDED__
#define __FIELDDEBUGEDITOR_H_INCLUDED__
#pragma once


// イベントデバッグ機能の有効無効定義
#if defined( DEBUG_ONLY_FOR_ikuta_junya )
#define GF_ENABLE_DEBUG_EDITOR
#else
#if defined(GF_PLATFORM_WIN32)
#define GF_ENABLE_DEBUG_EDITOR
#endif
#endif

#endif // __FIELDDEBUGEDITOR_H_INCLUDED__