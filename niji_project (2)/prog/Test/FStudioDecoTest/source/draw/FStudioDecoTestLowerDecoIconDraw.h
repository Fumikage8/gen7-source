#if PM_DEBUG

#if !defined(__FINDER_STUDIO_DECO_TEST_LOWER_DECO_ICON_DRAW_H_INCLUDED__)
#define __FINDER_STUDIO_DECO_TEST_LOWER_DECO_ICON_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FStudioDecoTestLowerDecoIconDraw.h
 * @date    2016/10/28 11:49:50
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <AppLib/include/UI/UIView.h>
#include <AppLib/include/UI/UIInputListener.h>

#include <Test/FStudioDecoTest/source/FStudioDecoTestDefine.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

class UIUtility;
class DrawListener;

class LowerDecoIconDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerDecoIconDraw );

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  LowerDecoIconDraw( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~LowerDecoIconDraw( void ) { ; }

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
  void InitializeButtonManager( void );
  bool Terminate( void );

  //-------------------------------------------------------
  // @brief   開閉処理
  //-------------------------------------------------------
  void Open( DecoItemKind type );
  void Close( void );
  bool IsOpen( void ) const { return m_isOpen; }

  //-------------------------------------------------------
  // @brief   デコアイコン
  //-------------------------------------------------------
  void          ChangeDecoType( DecoItemKind type );
  DecoItemKind  GetDecoType( void ) const { return m_DecoType; }

  //-------------------------------------------------------
  // @brief   設定関数群
  //-------------------------------------------------------
  void SetDrawListener( DrawListener* listener ) { m_pListener = listener; }

private:
  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* btn, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneLongPressEvent( const u32 paneID );

private:
  static const u32 ITEM_SELECT_FRAME = 5;

private:
  app::util::Heap*                  m_pHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  UIUtility*      m_pUtil;
  DrawListener*   m_pListener;

private:
  DecoItemKind    m_DecoType;
  bool            m_isOpen;

private:
  gfl2::lyt::LytPane*       m_pNullBGMask;
  gfl2::lyt::LytPane*       m_pNullDeco[DECO_ITEM_KIND_NUM];

private:
  gfl2::math::VEC3      m_TouchPos;
};

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // __FINDER_STUDIO_DECO_TEST_LOWER_DECO_ICON_DRAW_H_INCLUDED__

#endif // #if PM_DEBUG
