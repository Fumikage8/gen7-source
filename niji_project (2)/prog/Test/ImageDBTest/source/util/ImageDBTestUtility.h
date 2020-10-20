#if PM_DEBUG

#if !defined(__IMAGEDB_TEST_UTILITY_H_INCLUDED__)
#define __IMAGEDB_TEST_UTILITY_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    ImageDBTestUtility.h
 * @date    2016/10/21 17:27:53
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined(GF_PLATFORM_CTR)

#include <imagedb/include/gfl2_ImageDBAccessor.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_Vector4.h>
#include <Layout/include/gfl2_Layout.h>

#include "Field/FieldRo/include/TrialModel/FieldFinderModel_define.h"


using namespace Field::TrialModel;
using namespace gfl2::imagedb;

//----------------------------------------------------------------------
// @brief   Utility
//----------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

class ImageDBUtil
{
public:
  //-------------------------------------------------------
  // @brief   ImageDBAccessor
  //-------------------------------------------------------
  static ImageDBAccessor* CreateImageDBAccessor( gfl2::heap::HeapBase* workHeap );
  static void             DeleteImageDBAccessor( ImageDBAccessor* accessor );

  //-------------------------------------------------------
  // @brief   エンコード・デコード
  //-------------------------------------------------------
  static void EncodeJpeg( gfl2::heap::HeapBase* workHeap, void* srcColorBuffer, FinderPhotoJpeg* dstJpeg );
  static void DecodeColorBuffer( gfl2::heap::HeapBase* workHeap, FinderPhotoJpeg* srcJpeg, void* dstColorBuffer, nn::jpeg::PixelFormat format );
  static void DecodeThumbnail( gfl2::heap::HeapBase* workHeap, FinderPhotoJpeg* srcJpeg, FinderPhotoThumbnail* dstThumb );

  //-------------------------------------------------------
  // @brief   保存・読込み
  //-------------------------------------------------------
  static s32                         CheckSDCard( ImageDBAccessor::imgDbResult* dstResult, gfl2::heap::HeapBase* workHeap );
  static ImageDBAccessor::ResultCode SaveColorBuffer( ImageDBAccessor::imgDbResult* dstResult, gfl2::heap::HeapBase* workHeap, void* srcColorBuffer, wchar_t* dstFilePath = NULL );

  static bool LoadSnapImage( gfl2::heap::HeapBase* workHeap, const wchar_t* filepath, void* tmpConvBuf, void* dstColorBuffer );
  static bool LoadThumbImage( gfl2::heap::HeapBase* workHeap, const wchar_t* filepath, FinderPhotoThumbnail* tmpThumb, FinderPhotoThumbnail* dstThumb );

  //-------------------------------------------------------
  // @brief   テクスチャの差し替え
  //-------------------------------------------------------
  static void ReplaceTexture_ShootSnap( void* imgBuf, gfl2::lyt::LytPicture* picture );
  static void ReplaceTexture_LoadSnap( void* imgBuf, gfl2::lyt::LytPicture* picture );
  static void ReplaceTexture_LoadThumb( void* imgBuf, gfl2::lyt::LytPicture* picture );

private:
  //-------------------------------------------------------
  // @brief   保存・読込み
  //-------------------------------------------------------
  static ImageDBAccessor::ResultCode LoadImage( ImageDBAccessor::imgDbResult* dstResult, gfl2::heap::HeapBase* workHeap, const wchar_t* filepath, void* dstColorBuffer, u32 width, u32 height, bool thumbnail = false );

  //-------------------------------------------------------
  // @brief   テクスチャの差し替え
  //-------------------------------------------------------
  static void ReplaceTexture( void* imgBuf, gfl2::lyt::LytPicture* picture, const nw::lyt::TexSize texSize, const nw::lyt::TexSize realSize, const nw::lyt::TexFormat format );

public:
  //-------------------------------------------------------
  // @brief   各種バッファの生成・破棄
  //-------------------------------------------------------
  static void* CreateColorBuffer( gfl2::heap::HeapBase* heap, u32 align = 16 );
  static void  DeleteColorBuffer( void* buffer );

  static FinderPhotoJpeg* CreatePhotoJpeg( gfl2::heap::HeapBase* heap );
  static void             DeletePhotoJpeg( FinderPhotoJpeg* jpeg );

  static FinderPhotoSnap* CreatePhotoSnap( gfl2::heap::HeapBase* heap );
  static void             DeletePhotoSnap( FinderPhotoSnap* snap );

  static void* CreatePhotoThumbnail( gfl2::heap::HeapBase* heap );
  static void  DeletePhotoThumbnail( void* thumb );

public:
  static const u32 PHOTO_SNAP_WIDTH    = 512;
  static const u32 PHOTO_SNAP_HEIGHT   = 256;
  static const u32 PHOTO_THUMB_WIDTH   = 128;
  static const u32 PHOTO_THUMB_HEIGHT  = 64;
  static const u32 PHOTO_FILENAME_SIZE = imgdb::imgio::c_nMaxPathSize;
  static const u32 COLOR_BUFFER_SIZE = sizeof( u32 ) * PHOTO_SNAP_WIDTH * PHOTO_SNAP_HEIGHT;
  static const u32 THUMB_BUFFER_SIZE = sizeof( u32 ) * PHOTO_THUMB_WIDTH * PHOTO_THUMB_HEIGHT;
};

GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // defined(GF_PLATFORM_CTR)

#endif // __IMAGEDB_TEST_UTILITY_H_INCLUDED__

#endif // #if PM_DEBUG
