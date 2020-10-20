#if PM_DEBUG

#if !defined(__FINDER_STUDIO_DECO_TEST_FRAME_H_INCLUDED__)
#define __FINDER_STUDIO_DECO_TEST_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FStudioDecoTestFrame.h
 * @date    2016/10/26 20:10:37
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FStudioDecoTestDrawListener.h"
#include "FStudioDecoTestDefine.h"

#include <AppLib/include/Frame/AppFrameCellSuper.h>

#include <Test/ImageDBTest/source/pic/ImageDBTestLoadPicture.h>

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

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )
class ImageDBSave;
class ImageDBLoad;
GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )
// ---[ end ]---


//----------------------------------------------------------------------
// @brief   ImageDBテスト
//----------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

class UpperDraw;
class LowerDraw;
class LowerTabDraw;
class LowerDecoIconDraw;

class Frame
  : public applib::frame::CellSuper
  , public DrawListener
#if defined(GF_PLATFORM_CTR)
  , public ImageDB::ImageDBLoadListener
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
  bool LoadPhotoData( void );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual void OnEndDecoTest( void );
  virtual void OnRequestShootAndSave( void );
  virtual void OnOpenDecoWindow( DecoItemKind type );
  virtual void OnUpdateTouchPoint( gfl2::math::VEC3 vec );
  virtual void OnAddDecoItem( void );
  virtual void OnRemoveDecoItem( void );
  virtual void OnSetDecoItem( u32 index );

#if defined(GF_PLATFORM_CTR)
  virtual void OnEndLoadSnap( void );
#endif

public:
  //-------------------------------------------------------
  // @brief   設定関数群
  //-------------------------------------------------------
  void SetAppHeap( app::util::Heap* heap ) { m_pAppHeap = heap; }
  void SetAppRenderingManager( app::util::AppRenderingManager* renderMan ) { m_pAppRenderingManager = renderMan; }

private:
  enum {
    MM_SEQ_MAIN,
    MM_SEQ_END,
  };

private:
  static const u32 IMGDB_HEAP_SIZE = 0x480000;

private:
  app::util::Heap*                  m_pAppHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  gfl2::heap::HeapBase*     m_pImgdbHeap;

  ImageDB::ImageDBSave*  m_pImgdbSave;
  ImageDB::ImageDBLoad*  m_pImgdbLoad;

  UpperDraw*            m_pUpperDraw;
  LowerDraw*            m_pLowerDraw;
  LowerTabDraw*         m_pLowerTabDraw;
  LowerDecoIconDraw*    m_pLowerDecoIconDraw;

private:
  void*     m_pLytDataBuff;

  u8        m_InitSeq;
  u8        m_LoadSeq;
  u8        m_ImgLoadSeq;
};

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // __FINDER_STUDIO_DECO_TEST_FRAME_H_INCLUDED__

#endif // #if PM_DEBUG
