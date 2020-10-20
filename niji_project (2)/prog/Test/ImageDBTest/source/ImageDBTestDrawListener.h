#if PM_DEBUG

#if !defined(__IMAGEDB_TEST_DRAW_LISTENER_H_INCLUDED__)
#define __IMAGEDB_TEST_DRAW_LISTENER_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    ImageDBTestDrawListener.h
 * @date    2016/10/21 17:09:48
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <Layout/include/gfl2_Layout.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

class DrawListener
{
public:
  //-------------------------------------------------------
  // @brief   終了通知
  //-------------------------------------------------------
  virtual void OnEndImageDBTest( void ) { ; }

  //-------------------------------------------------------
  // @brief   リクエスト
  //-------------------------------------------------------
  virtual void OnRequestPhotoShoot( void ) { ; }
  virtual void OnRequestImgdbSave( void ) { ; }
  virtual void OnRequestImgdbLoad_Snap( u32 idx ) { ; }
  virtual void OnRequestImgdbLoad_Thumb( u32 thumbIdx, u32 paneIndex, gfl2::lyt::LytPicture* picture ) { ; }
  virtual void OnRequestImgdbSearch( void ) { ; }
  virtual void OnRequestOpenAlbum( void ) { ; }
  virtual void OnRequestCloseAlbum( void ) { ; }
};

GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // __IMAGEDB_TEST_DRAW_LISTENER_H_INCLUDED__

#endif // #if PM_DEBUG
