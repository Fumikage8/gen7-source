#if PM_DEBUG

#if !defined(__IMAGEDB_TEST_FRAME_H_INCLUDED__)
#define __IMAGEDB_TEST_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    ImageDBTestFrame.h
 * @date    2016/10/21 15:44:40
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <AppLib/include/Frame/AppFrameCellSuper.h>

#include "ImageDBTestDrawListener.h"
#include "pic/ImageDBTestLoadPicture.h"

#include <macro/include/gfl2_Macros.h>


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
// @brief   ImageDBテスト
//----------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

class ImageDBSave;
class ImageDBLoad;
class UpperDraw;
class LowerDraw;
class LowerAlbumDraw;

class Frame
  : public applib::frame::CellSuper
  , public DrawListener
#if defined(GF_PLATFORM_CTR)
  , public ImageDBLoadListener
#endif
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame );

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  Frame( void );
  virtual ~Frame( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );
  virtual applib::frame::Result EndFunc( void );
  virtual applib::frame::Result UpdateFunc( void );
  virtual void                  DrawFunc( gfl2::gfx::CtrDisplayNo no );

private:
  //-------------------------------------------------------
  // @brief   ロード関数群
  //-------------------------------------------------------
  bool LoadLayoutData( void );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual void OnEndImageDBTest( void );
  virtual void OnRequestPhotoShoot( void );
  virtual void OnRequestImgdbSave( void );
  virtual void OnRequestImgdbLoad_Snap( u32 idx );
  virtual void OnRequestImgdbLoad_Thumb( u32 thumbIdx, u32 paneIndex, gfl2::lyt::LytPicture* picture );
  virtual void OnRequestImgdbSearch( void );
  virtual void OnRequestOpenAlbum( void );
  virtual void OnRequestCloseAlbum( void );

  virtual void OnEndLoadThumbnail( void );

public:
  //-------------------------------------------------------
  // @brief   設定関数群
  //-------------------------------------------------------
  void SetAppHeap( app::util::Heap* heap ) { m_pAppHeap = heap; }
  void SetAppRenderingManager( app::util::AppRenderingManager* renderMan ) { m_pAppRenderingManager = renderMan; }

private:
  app::util::Heap*                  m_pAppHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;
  gfl2::heap::HeapBase*             m_pImgdbHeap;

  ImageDBSave*      m_pImgdbSave;
  ImageDBLoad*      m_pImgdbLoad;
  UpperDraw*        m_pUpperDraw;
  LowerDraw*        m_pLowerDraw;
  LowerAlbumDraw*   m_pLowerAlbumDraw;

private:
  void*     m_pLytDataBuff;

  u8        m_LoadSeq;
};

GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // __IMAGEDB_TEST_FRAME_H_INCLUDED__

#endif // #if PM_DEBUG
