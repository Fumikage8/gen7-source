#if PM_DEBUG

#if !defined(__IMAGEDB_TEST_LOWER_ALBUM_H_INCLUDED__)
#define __IMAGEDB_TEST_LOWER_ALBUM_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    ImageDBTestLowerAlbum.h
 * @date    2016/10/25 17:16:19
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <AppLib/include/UI/UIView.h>
#include <AppLib/include/UI/UIInputListener.h>


//----------------------------------------------------------------------
// @brief   ImageDBテスト
//----------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

class DrawListener;

class LowerAlbumDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerAlbumDraw );

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  LowerAlbumDraw( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~LowerAlbumDraw( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual void Update( void );
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  //-------------------------------------------------------
  // @brief   初期化・終了処理
  //-------------------------------------------------------
  bool InitializeSystem( void );
  void Initialize2D( void* lytDatBuf );
  void InitializeButtonManager( void );
  bool Terminate( void );

  //-------------------------------------------------------
  // @brief   設定関数群
  //-------------------------------------------------------
  void SetDrawListener( DrawListener* listener ) { m_pListener = listener; }

  //-------------------------------------------------------
  // @brief   オープン・クローズ
  //-------------------------------------------------------
  void Open( void );
  void Close( void );

  //-------------------------------------------------------
  // @brief   次のサムネイルシーケンスへ
  //-------------------------------------------------------
  void NextThumbnailSeq( void ) { m_Seq = 3; }

private:
  //-------------------------------------------------------
  // @brief   サムネイル更新
  //-------------------------------------------------------
  void UpdateThumbnail( void );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* btn, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneID );

private:
  static const u32 THUMB_MAX = 16;

private:
  app::util::Heap*                  m_pHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  DrawListener*   m_pListener;

private:
  gfl2::lyt::LytPicture*    m_pThumbPicture[THUMB_MAX];

private:
  u8    m_Seq;

  u32   m_LoadIndex;
  u32   m_LoadCount;

  u32   m_PageNum;
  u32   m_PageMax;
};

GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // __IMAGEDB_TEST_LOWER_ALBUM_H_INCLUDED__

#endif // #if PM_DEBUG
