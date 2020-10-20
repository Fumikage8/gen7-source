#if !defined(__ARCSRC_TOOLDEFINE_H__)
#define __ARCSRC_TOOLDEFINE_H__
#pragma once


//=============================================================================
/**
 * @brief  「garcのソースを記録したファイル」で使用する定義
 * @file   arcsrc_ToolDefine.h
 * @author kawada_koji
 * @date   2014.12.12
 */
//=============================================================================


//#############################################################################
//#############################################################################
//#############################################################################

// ツール以外で使用するので
// GFL_FS_TOOL_DEFINE_USE_IN_TOOL
// を定義しないようにコメントアウトしておく。

//#if !defined(GFL_FS_TOOL_DEFINE_USE_IN_TOOL)
//#define GFL_FS_TOOL_DEFINE_USE_IN_TOOL
//#endif  // GFL_FS_TOOL_DEFINE_USE_IN_TOOL

//#############################################################################
//#############################################################################
//#############################################################################








#ifdef GFL_FS_TOOL_DEFINE_USE_IN_TOOL




///////////////////////////////////////////////////////////////////
//
// ツールで使用するとき
//
///////////////////////////////////////////////////////////////////

namespace arcsrc
{

//! @brief  型定義
typedef unsigned char      u8;
typedef unsigned short int u16;
typedef signed   long int  s32;
typedef unsigned long int  u32;

typedef u32 ARCDATID;    //!< *.gaixファイル内に書かれているID(GARC_*)


// ファイルシステムにおいてツールと共通使用している定義
#include "gfl2_FsToolDefine.h"


}  // arcsrc




#else  // GFL_FS_TOOL_DEFINE_USE_IN_TOOL




///////////////////////////////////////////////////////////////////
//
// ツール以外で使用するとき
//
///////////////////////////////////////////////////////////////////


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


// fsのインクルード

// ファイルシステムにおいてツールと共通使用している定義
#include <fs/include/gfl2_FsToolDefine.h>

#include <fs/include/gfl2_FsArcFile.h>


namespace arcsrc
{

typedef  gfl2::fs::ArcFile::ARCDATID  ARCDATID;    //!< *.gaixファイル内に書かれているID(GARC_*)

typedef  gfl2::fs::ToolDefine         ToolDefine;

}




#endif  // GFL_FS_TOOL_DEFINE_USE_IN_TOOL




#endif // __ARCSRC_TOOLDEFINE_H__

