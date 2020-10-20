#if PM_DEBUG

#if !defined(__IMAGEDB_TEST_LOAD_PICTURE_H_INCLUDED__)
#define __IMAGEDB_TEST_LOAD_PICTURE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    ImageDBTestLoadPicture.h
 * @date    2016/10/25 12:09:48
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined(GF_PLATFORM_CTR)

#include <Test/ImageDBTest/source/util/ImageDBTestUtility.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

class ImageDBLoadListener
{
public:
  virtual void OnEndLoadSnap( void ) { ; }
  virtual void OnEndLoadThumbnail( void ) { ; }
};

class ImageDBLoad
{
public:
  ImageDBLoad( gfl2::heap::HeapBase* heap );
  ~ImageDBLoad( void );

public:
  void Update( void );
  
  void RequestLoadPhoto_Snap( const wchar_t* filepath, gfl2::lyt::LytPicture* picture );
  void RequestLoadPhoto_Thumb( const wchar_t* filepath, gfl2::lyt::LytPicture* picture, u32 paneIndex );

  void SetListener( ImageDBLoadListener* listener ) { m_pListener = listener; }

  void* GetColorBuffer( void ) const { return m_pColorBuffer; }

private:
  void Update_Wait( void );
  void Update_Replace( void );

private:
  gfl2::heap::HeapBase*     m_pHeap;
  ImageDBLoadListener*      m_pListener;

  void*   m_pColorBuffer;
  void*   m_pThumBuffer[16];

  void*   m_pTmpLoadBuff;
  void*   m_pTmpConvBuffer;

  gfl2::lyt::LytPicture*    m_pPicture;

  u8    m_DelayTime;
  u8    m_LoadSnapSeq;
  u8    m_LoadThumbSeq;
  u8    m_ThumbPaneIndex;
};

GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // defined(GF_PLATFORM_CTR)

#endif // __IMAGEDB_TEST_LOAD_PICTURE_H_INCLUDED__

#endif // #if PM_DEBUG
