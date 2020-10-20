#if PM_DEBUG

#if !defined(__IMAGEDB_TEST_LOWER_DRAW_H_INCLUDED__)
#define __IMAGEDB_TEST_LOWER_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    ImageDBTestLowerDraw.h
 * @date    2016/10/24 19:27:01
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

class LowerDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerDraw );

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  LowerDraw( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~LowerDraw( void ) { ; }

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

private:
  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* btn, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneID );

private:
  app::util::Heap*                  m_pHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  DrawListener*   m_pListener;

private:
  u32     m_PhotoNum;
};

GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // __IMAGEDB_TEST_LOWER_DRAW_H_INCLUDED__

#endif // #if PM_DEBUG
