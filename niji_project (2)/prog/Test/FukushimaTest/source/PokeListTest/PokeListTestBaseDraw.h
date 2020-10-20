#if PM_DEBUG

#ifndef __TEST_FUKUSHIMA_TEST_POKELIST_BASE_DRAW_H__
#define __TEST_FUKUSHIMA_TEST_POKELIST_BASE_DRAW_H__
#pragma once
//==============================================================================
/**
 * @file    PokeListTestBaseDraw.h
 * @author  fukushima_yuya
 * @date    2015.06.22
 * @brief   ポケモンリストベース描画
 */
//==============================================================================

// niji
#include <App/PokeList/source/PokeListConst.h>
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class PokeListDrawListener;   //!< ポケモンリストリスナークラス
class DrawerBase;
class StaticWork;
class LocalWork;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   ポケモンリストクラス
//------------------------------------------------------------------------------
class TestPokeListBaseDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( TestPokeListBaseDraw );

public:
  enum Sequence {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END_READY,
    SEQ_END
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  TestPokeListBaseDraw( app::util::Heap * heap, app::util::AppRenderingManager * renderingManager );

  //------------------------------------------------------------------
  /**
   * @brief	  デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~TestPokeListBaseDraw( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  このビュー専用のUIリスナー設定
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( App::PokeList::PokeListDrawListener* pListener );

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   pAppParam
   * @param   pAppLytBuff   レイアウトバイナリ
   * @param   pStaticWork   スタティックワーク
   */
  //------------------------------------------------------------------
  void Init( App::PokeList::APP_PARAM* pAppParam, void* pAppLytBuff, App::PokeList::StaticWork* pStaticWork, App::PokeList::LocalWork* pLocalWork );

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
   * @brief   ポケモンリスト画面のオープン
   */
  //------------------------------------------------------------------
  void PokeListOpen( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリスト画面のクローズ
   */
  //------------------------------------------------------------------
  void PokeListClose( void );

  //------------------------------------------------------------------
  /**
   * @brief   イン・アウトアニメが終了したかどうか
   * @param   in    インアニメかどうか
   * @return  true = 終了、false = 継続
   */
  //------------------------------------------------------------------
  bool IsEndInOutAnim( bool in );

protected:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pHeap         ヒープ
   * @param   pAppLytBuff   レイアウトバイナリデータ
   */
  //------------------------------------------------------------------
  void Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  virtual void InitializeText( void ) { ; }

protected:   //!< ボタンマネージャ関連
  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   */
  //------------------------------------------------------------------
  void CreateButtonManager( void );


protected:
  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  virtual bool EndFunc( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   更新処理
   */
  //------------------------------------------------------------------
  virtual void UpdateFunc( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   描画処理
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   UIのセットアップ
   */
  //------------------------------------------------------------------
  virtual void SetupUI( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   ボタンのセットアップ
   */
  //------------------------------------------------------------------
  virtual void SetupButton( app::tool::ButtonManager* pBtnMan ) = 0;


protected:
  app::util::Heap*        m_pHeap;        //!< ヒープ
  App::PokeList::PokeListDrawListener*   m_uiListener;   //!< UIイベントリスナー

  App::PokeList::APP_PARAM*  m_pAppParam;

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  gfl2::Fade::FadeManager*    m_pFadeManager;

  u32   m_LayoutID;
  u8    m_Seq;

  App::PokeList::DrawerBase*   m_pDrawerBase;

  App::PokeList::StaticWork*     m_pStaticWork;

  App::PokeList::LocalWork*      m_pLocalWork;
};


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif  // __TEST_FUKUSHIMA_TEST_POKELIST_DRAW_H__

#endif // PM_DEBUG