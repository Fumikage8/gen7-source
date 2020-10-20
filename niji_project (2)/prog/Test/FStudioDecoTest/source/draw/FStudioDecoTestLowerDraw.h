#if PM_DEBUG

#if !defined(__FINDER_STUDIO_DECO_TEST_LOWER_DRAW_H_INCLUDED__)
#define __FINDER_STUDIO_DECO_TEST_LOWER_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FStudioDecoTestLowerDraw.h
 * @date    2016/10/27 13:03:28
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <AppLib/include/UI/UIView.h>
#include <AppLib/include/UI/UIInputListener.h>

#include <Test/FStudioDecoTest/source/FStudioDecoTestDefine.h>


//----------------------------------------------------------------------
// @brief   ImageDBテスト
//----------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

class UIUtility;
class DrawListener;
class ItemList;

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
  void InitializeMember( void );
  void InitializeButtonManager( void );
  void InitializePhoto( void* photoBuf );
  bool Terminate( void );

  //-------------------------------------------------------
  // @brief   デコアイテムの追加と削除
  //-------------------------------------------------------
  void AddDecoItem( void );
  void RemoveDecoItem( void );
  void SetDecoItem( u32 index );

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
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneID );

private:
  enum BtnID {
    BTN_ID_CANCEL,
    BTN_ID_SHOOT,
    BTN_ID_NUM,
  };

private:
  app::util::Heap*                  m_pHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  UIUtility*      m_pUtil;
  DrawListener*   m_pListener;

  ItemList*       m_pItemList;

private:
  gfl2::lyt::LytPicture*    m_pActiveDeco;
  gfl2::lyt::LytPane*       m_pNullDecoParent;
  u8                        m_Wait;

  struct MoveParam
  {
    bool    move;
    u32     decoIdx;
    gfl2::math::VEC3 pos;
    
    MoveParam() : move( false ), decoIdx( 0 ) { ; }
  };
  MoveParam   m_MoveParam;
};

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // __FINDER_STUDIO_DECO_TEST_LOWER_DRAW_H_INCLUDED__

#endif // #if PM_DEBUG
