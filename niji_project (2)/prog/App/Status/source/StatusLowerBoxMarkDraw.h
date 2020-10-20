#if !defined( __APP_STATUS_LOWER_BOX_MARK_DRAW_H_INCLUDED__ )
#define __APP_STATUS_LOWER_BOX_MARK_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusLowerBoxMarkDraw.h
 * @date    2015/11/27 19:07:36
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：ボックスマーク変更画面(下)
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
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)
class DrawerBase;
class StatusDrawListener;
GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   ボックスマーク変更画面(下)
//------------------------------------------------------------------------------
class LowerBoxMarkDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerBoxMarkDraw );

public:
  //! @brief    生成パラメータ
  struct CreateParam
  {
    app::util::Heap*                  heap;
    app::util::AppRenderingManager*   renderMan;
  };

  //! @brief    初期化パラメータ
  struct InitParam
  {
    void*                 appLytBuff;
    gfl2::str::MsgData*   baseMsgData;
    print::WordSet*       wordSet;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  LowerBoxMarkDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~LowerBoxMarkDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   このビュー専用のUIリスナー設定
   *
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( StatusDrawListener* listener );

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  void Init( InitParam& param );

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
   *
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool IsExit( void );

  //------------------------------------------------------------------
  /**
   * @brief   準備完了か？
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsReady( void ) const;

public:
  //------------------------------------------------------------------
  /**
   * @brief   オープン
   *
   * @param   updateProhibition   更新禁止フラグ
   */
  //------------------------------------------------------------------
  void Open( bool updateProhibition );

  //------------------------------------------------------------------
  /**
   * @brief   クローズ
   */
  //------------------------------------------------------------------
  void Close( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパラメータのセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetPokemonParam( pml::pokepara::PokemonParam* pp );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   param     初期化パラメータ
   */
  //------------------------------------------------------------------
  void Initialize2D( InitParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

  //------------------------------------------------------------------
  /**
   * @brief   ボックスマークのセットアップ
   *
   * @param   updateProhibition   更新禁止フラグ
   */
  //------------------------------------------------------------------
  void SetupBoxMark( bool updateProhibition );

  //------------------------------------------------------------------
  /**
   * @brief   ボックスマークの更新
   * 
   * @param   markID    ボックスマークID
   *
   * @note    無色 -> 青 -> 赤 -> 無色
   */
  //------------------------------------------------------------------
  void UpdateBoxMark( const u32 markID );

  //------------------------------------------------------------------
  /**
   * @brief   ボックスマークの色の取得
   * 
   * @param   markID    ボックスマークID
   * @param   color     ボックスマークカラー
   */
  //------------------------------------------------------------------
  void SetBoxMarkColor( pml::pokepara::BoxMark mark, pml::pokepara::BoxMarkColor color );

  //------------------------------------------------------------------
  /**
   * @brief   ボックスマークの色の取得
   * 
   * @param   markID    ボックスマークID
   *
   * @return  ボックスマークの色
   */
  //------------------------------------------------------------------
  pml::pokepara::BoxMarkColor GetBoxMarkColor( pml::pokepara::BoxMark mark );

private:    //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション通知用リスナ
   * @param   paneId
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );

private:   //!< ボタンマネージャ関連
  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   */
  //------------------------------------------------------------------
  void CreateButtonManager( void );

private:
  app::util::Heap*                    m_pHeap;

  app::util::AppRenderingManager*     m_pAppRenderingManager;

  StatusDrawListener*          m_uiListener;

  pml::pokepara::PokemonParam*        m_pPokeParam;

private:
  DrawerBase*     m_pDrawerBase;

  u8              m_Seq;
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_LOWER_BOX_MARK_DRAW_H_INCLUDED__
