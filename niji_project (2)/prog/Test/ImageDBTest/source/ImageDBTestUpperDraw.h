#if PM_DEBUG

#if !defined(__IMAGEDB_TEST_UPPER_DRAW_H_INCLUDED__)
#define __IMAGEDB_TEST_UPPER_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    ImageDBTestUpperDraw.h
 * @date    2016/10/21 16:27:41
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <AppLib/include/UI/UIView.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

class DrawListener;

//----------------------------------------------------------------------
// @brief   ImageDBテスト
//----------------------------------------------------------------------
class UpperDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( UpperDraw );

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  UpperDraw( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~UpperDraw( void ) { ; }

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
  void InitializePane( void );
  bool Terminate( void );

  //-------------------------------------------------------
  // @brief   設定関数群
  //-------------------------------------------------------
  void SetDrawListener( DrawListener* listener ) { m_pListener = listener; }

  //-------------------------------------------------------
  // @brief   取得関数群
  //-------------------------------------------------------
  gfl2::lyt::LytPicture* GetPicturePane_Portrait( void ) const { return m_pPicPortrait; }
  gfl2::lyt::LytPicture* GetPicturePane_Sidewise( void ) const { return m_pPicSidewise; }

private:
  app::util::Heap*                  m_pHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  DrawListener*   m_pListener;

  gfl2::lyt::LytPicture*    m_pPicPortrait;
  gfl2::lyt::LytPicture*    m_pPicSidewise;
};

GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // __IMAGEDB_TEST_UPPER_DRAW_H_INCLUDED__

#endif // #if PM_DEBUG
