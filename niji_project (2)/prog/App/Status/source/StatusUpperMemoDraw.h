#if !defined( __APP_STATUS_UPPER_MEMO_DRAW_H_INCLUDED__ )
#define __APP_STATUS_UPPER_MEMO_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusUpperMemoDraw.h
 * @date    2015/11/30 19:17:47
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：メモ画面(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/ui/UIView.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)
class DrawerBase;
class StatusDrawListener;
class TrainerMemo;
class RibbonExplain;
GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   メモ画面(上)
//------------------------------------------------------------------------------
class UpperMemoDraw
  : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN( UpperMemoDraw );

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
    void*                       appLytBuff;
    gfl2::str::MsgData*         baseMsgData;
    gfl2::str::MsgData*         memoMsgData;
    gfl2::str::MsgData*         ribbonMsgData;
    print::WordSet*             wordSet;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  UpperMemoDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~UpperMemoDraw( void ) { ; }

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
   * @brief   ポケモンパラメータのセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetPokemonParam( const pml::pokepara::PokemonParam* pp );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  void Initialize2D( InitParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   リボンの説明を開く
   *
   * @param   ribbonID    表示するリボンのID
   *
   * @param   "true  = 開くことに成功"
   * @param   "false = それ以外"
   */
  //------------------------------------------------------------------
  bool OpenRibbonExplain( u32 ribbonID );

  //------------------------------------------------------------------
  /**
   * @brief   リボンの説明を閉じる
   */
  //------------------------------------------------------------------
  void CloseRibbonExplain( void );

  //------------------------------------------------------------------
  /**
   * @brief   表示するリボンの設定
   *
   * @param   ribbonID    表示するリボンのID
   */
  //------------------------------------------------------------------
  void SetRibbonExplain( u32 ribbonID );

private:
  app::util::Heap*                    m_pHeap;

  app::util::AppRenderingManager*     m_pAppRenderingManager;

  StatusDrawListener*          m_uiListener;

private:
  DrawerBase*     m_pDrawerBase;

  TrainerMemo*    m_pTrainerMemo;

  RibbonExplain*  m_pRibbonExplain;

  u8              m_Seq;
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // #define __APP_STATUS_UPPER_MEMO_DRAW_H_INCLUDED__
