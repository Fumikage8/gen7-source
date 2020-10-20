#if PM_DEBUG

#ifndef __TEST_FUKUSHIMA_TEST_DYNAMIC_HEADER_TEST_DRAW_H__
#define __TEST_FUKUSHIMA_TEST_DYNAMIC_HEADER_TEST_DRAW_H__
#pragma once
//======================================================================
/**
 * @file    DynamicHeaderTestDraw.h
 * @date    2015/10/06 13:23:07
 * @author  fukushima_yuya
 * @brief   DynamicHeaderテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( LangSelect )
class LangSelectDrawListener;
class DrawerBase;
GFL_NAMESPACE_END( LangSelect )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

class DynamicHeaderTestDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( DynamicHeaderTestDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  DynamicHeaderTestDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~DynamicHeaderTestDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   */
  //------------------------------------------------------------------
  void Init( void* arcBuf, void* msgBuf );

  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  bool End( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  virtual void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   破棄チェック
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool IsExit( void );

  //------------------------------------------------------------------
  /**
   * @brief	  このビュー専用のUIリスナー設定
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( App::LangSelect::LangSelectDrawListener* pListener );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   */
  //------------------------------------------------------------------
  void Initialize2D( void* arcBuf, void* msgBuf );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   キー入力通知用リスナ
   * @param   pKey
   * @param   pStick
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

private:
  app::util::Heap*          m_pHeap;        //!< ヒープ

  App::LangSelect::LangSelectDrawListener*   m_uiListener;   //!< UIイベントリスナー

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  gfl2::str::MsgData* m_pMsgData;

  print::WordSet*     m_pWordSet;

  App::LangSelect::DrawerBase*    m_pDrawerBase;
};

GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // __TEST_FUKUSHIMA_TEST_DYNAMIC_HEADER_TEST_DRAW_H__

#endif // PM_DEBUG