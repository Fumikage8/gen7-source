#if PM_DEBUG

#if !defined(__FINDER_STUDIO_DECO_TEST_LOWER_TAB_DRAW_H_INCLUDED__)
#define __FINDER_STUDIO_DECO_TEST_LOWER_TAB_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FStudioDecoTestLowerTabDraw.h
 * @date    2016/10/28 11:40:14
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
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
GFL_NAMESPACE_BEGIN( FSDeco )

class UIUtility;
class DrawListener;

class LowerTabDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerTabDraw );

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  LowerTabDraw( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~LowerTabDraw( void ) { ; }

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
  enum BtnID {
    BTN_ID_DECO_00,
    BTN_ID_DECO_01,
    BTN_ID_DECO_02,
    BTN_ID_DECO_03,
    BTN_ID_NUM,
  };

private:
  app::util::Heap*                  m_pHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  UIUtility*      m_pUtil;
  DrawListener*   m_pListener;
};

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // __FINDER_STUDIO_DECO_TEST_LOWER_TAB_DRAW_H_INCLUDED__

#endif // #if PM_DEBUG
