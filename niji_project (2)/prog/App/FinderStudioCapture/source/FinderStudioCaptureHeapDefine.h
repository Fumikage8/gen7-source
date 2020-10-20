#if !defined( __FINDER_STUDIO_CAPTURE_HEAP_DEFINE_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_HEAP_DEFINE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureHeapDefine.h
 * @date    2016/11/09 17:22:53
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：ヒープ定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioHeapDefine.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )
GFL_NAMESPACE_BEGIN( HeapDefine )

//////////////////////////////////////////////////////////////////////////
/// ---[ ヒープサイズ定義 begin ]---

static const u32 APP_SYS_HEAP_SIZE = 0x580000;      //!< アプリ用システムヒープサイズ：5.5MB
static const u32 APP_DEV_HEAP_SIZE = 0x300000;      //!< アプリ用デバイスヒープサイズ：3MB
static const u32 PHOTO_TEX_HEAP_SIZE = 0x300400;    //!< 写真テクスチャ用ヒープサイズ：3MB+1KB

#if PM_DEBUG
static void DebugPrint_AllHeapSize( void )
{
  GFL_PRINT( "\n//****************************************//\n" );
  Static::HeapDefine::DebugPrint_StaticHeapSize();
  GFL_PRINT( "AppSysHeapSize=[%#x]\n", APP_SYS_HEAP_SIZE );
  GFL_PRINT( "AppDevHeapSize=[%#x]\n", APP_DEV_HEAP_SIZE );
  GFL_PRINT( "PhotoTextureHeapSize[%#x]\n", PHOTO_TEX_HEAP_SIZE );
  GFL_PRINT( "PaddingHeapSize=[%#x]\n",
    Static::HeapDefine::ROOT_HEAP_SIZE - ( Static::HeapDefine::PROC_HEAP_SIZE +
    Static::HeapDefine::MANAGER_HEAP_SIZE +
    Static::HeapDefine::IMAGEDB_HEAP_SIZE +
    Static::HeapDefine::VIEWER_HEAP_SIZE +
    APP_SYS_HEAP_SIZE +
    APP_DEV_HEAP_SIZE +
    PHOTO_TEX_HEAP_SIZE ) );
  GFL_PRINT( "//****************************************//\n" );
}
#endif

/// ---[ ヒープサイズ定義 end ]---
//////////////////////////////////////////////////////////////////////////

GFL_NAMESPACE_END( HeapDefine )
GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_HEAP_DEFINE_H_INCLUDED__