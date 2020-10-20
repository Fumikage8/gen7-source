#if PM_DEBUG

#ifndef __TEST_FUKUSHIMA_POKE_MODEL_TEST_DRAW_H_INCLUDED__
#define __TEST_FUKUSHIMA_POKE_MODEL_TEST_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file PokeModelTestDraw.h
 * @date 2015/09/07 11:22:09
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )
class PokeModelTestDrawListener;
class PokeModel;
GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

GFL_NAMESPACE_BEGIN( GameSys )
class GameManager;
GFL_NAMESPACE_END( GameSys )

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( Fade )
class FadeManager;
GFL_NAMESPACE_END( Fade )
GFL_NAMESPACE_BEGIN( str )
class MsgData;
GFL_NAMESPACE_END( str )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( print )
class WordSet;
GFL_NAMESPACE_END( print )


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   アイコンドロー
//------------------------------------------------------------------------------
class PokeModelTestDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeModelTestDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  PokeModelTestDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeModelTestDraw( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  このビュー専用のUIリスナー設定
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( Test::Fukushima::PokeModelTestDrawListener* pListener );

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   pAppParam
   * @param   pAppLytBuff   レイアウトバイナリ
   */
  //------------------------------------------------------------------
  void Init( void* pAppLytBuff, gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet );

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
   * @brief   オープン
   */
  //------------------------------------------------------------------
  void Open( void );

  //------------------------------------------------------------------
  /**
   * @brief   クローズ
   */
  //------------------------------------------------------------------
  void Close( void );

  //------------------------------------------------------------------
  /**
   * @brief   イン・アウトアニメが終了したかどうか
   * @param   in    インアニメかどうか
   * @return  true = 終了、false = 継続
   */
  //------------------------------------------------------------------
  bool IsEndInOutAnim( bool in );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pHeap         ヒープ
   * @param   pAppLytBuff   レイアウトバイナリデータ
   */
  //------------------------------------------------------------------
  void Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff, gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet );


private:  //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   リソース生成前に必要なデータを登録。フォントリソースなど
   */
  //------------------------------------------------------------------
  virtual void OnPreLoadResource( void );

  //------------------------------------------------------------------
  /**
   * @brief   キー入力通知用リスナ
   * @param   pKey
   * @param   pStick
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション通知用リスナ
   * @param   paneId
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );

private:
  app::util::Heap*    m_pHeap;        //!< ヒープ

  Test::Fukushima::PokeModelTestDrawListener*   m_uiListener;     //!< UIイベントリスナー

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  GameSys::GameManager*       m_pGameManager;

  gfl2::Fade::FadeManager*    m_pFadeManager;

  u32   m_LayoutID;
  u8    m_Seq;

  PokeModel*      m_pPokeModel;

  u8              m_ModelIndex;
};

GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // __TEST_FUKUSHIMA_POKE_MODEL_TEST_DRAW_H_INCLUDED__

#endif // PM_DEBUG