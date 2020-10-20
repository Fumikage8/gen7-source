#if PM_DEBUG

#if !defined(__IMAGEDB_TEST_SAVE_PICTURE_H_INCLUDED__)
#define __IMAGEDB_TEST_SAVE_PICTURE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    ImageDBTestSavePicture.h
 * @date    2016/10/24 11:18:24
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined(GF_PLATFORM_CTR)

#include <Test/ImageDBTest/source/util/ImageDBTestUtility.h>


//----------------------------------------------------------------------
// @brief   前方宣言
//----------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )
// ---[ end ]---


//----------------------------------------------------------------------
// @brief   写真撮影・保存
//----------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

class ImageDBSave
{
public:
  ImageDBSave( gfl2::heap::HeapBase* heap );
  ~ImageDBSave( void );

public:
  void Update( void );

  void RequestPhotoShoot_Only( app::util::AppRenderingManager* renderMan );
  void RequestPhotoShoot_Save( app::util::AppRenderingManager* renderMan );

private:
  void Update_Wait( void );
  void Update_Only( void );
  void Update_Save( void );

  void SaveForSDCard( void );

private:
  gfl2::heap::HeapBase*   m_pHeap;
  
  void*   m_pColorBuffer;

  u8    m_OnlySeq;
  u8    m_SaveSeq;
  u8    m_DelayTime;
};

GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // defined(GF_PLATFORM_CTR)

#endif // __IMAGEDB_TEST_SAVE_PICTURE_H_INCLUDED__

#endif // #if PM_DEBUG
