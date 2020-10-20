#if !defined( __FINDER_STUDIO_HEAP_DEFINE_H_INCLUDED__ )
#define __FINDER_STUDIO_HEAP_DEFINE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioHeapDefine.h
 * @date    2016/11/24 10:50:59
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：ヒープ定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )
GFL_NAMESPACE_BEGIN( HeapDefine )

//////////////////////////////////////////////////////////////////////////
/// ---[ ヒープサイズ定義 begin ]---

static const u32 ROOT_HEAP_SIZE     = 0x2400000;    //!< メインヒープサイズ：36MB
static const u32 PROC_HEAP_SIZE     = 0x10000;      //!< プロック用ヒープサイズ：64KB
static const u32 MANAGER_HEAP_SIZE  = 0x1000;       //!< マネージャヒープサイズ：4KB
static const u32 IMAGEDB_HEAP_SIZE  = 0x380000;     //!< ImageDB用ヒープサイズ：3.5MB
static const u32 VIEWER_HEAP_SIZE   = 0x1400000;    //!< 3Dビューワー用ヒープサイズ：20MB

#if PM_DEBUG
static void DebugPrint_StaticHeapSize( void )
{
  GFL_PRINT( "RootHeapSize=[%#x]\n", ROOT_HEAP_SIZE );
  GFL_PRINT( "ProcHeapSize=[%#x]\n", PROC_HEAP_SIZE );
  GFL_PRINT( "ManagerHeapSize=[%#x]\n", MANAGER_HEAP_SIZE );
  GFL_PRINT( "ImageDBHeapSize=[%#x]\n", IMAGEDB_HEAP_SIZE );
  GFL_PRINT( "ViewerHeapSize=[%#x]\n", VIEWER_HEAP_SIZE );
}
#endif

/// ---[ ヒープサイズ定義 end ]---
//////////////////////////////////////////////////////////////////////////

GFL_NAMESPACE_END( HeapDefine )
GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_HEAP_DEFINE_H_INCLUDED__
