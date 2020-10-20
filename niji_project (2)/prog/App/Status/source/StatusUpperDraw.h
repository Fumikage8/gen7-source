#if !defined( __APP_STATUS_UPPER_DRAW_H_INCLUDED__ )
#define __APP_STATUS_UPPER_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusUpperDraw.h
 * @date    2015/11/30 18:14:06
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：メイン画面(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/Tool/AppToolTrainerIconRendering.h>

#include <Field/FieldStatic/include/BattleFes/BattleFesTool.h>

//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)
struct APP_PARAM;
class DrawerBase;
class StatusDrawListener;
class WazaExplain;
GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_BEGIN(Tool)
class FieldStatusUpperDraw;
GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class AppCommonGrpIconData;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(Fade)
class FadeManager;
GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(gfl2)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   メイン画面(上)
//------------------------------------------------------------------------------
class UpperMainDraw
  : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN( UpperMainDraw );

public:
  //! @brief    生成パラメータ
  struct CreateParam
  {
    app::util::Heap*                  heap;
    app::util::AppRenderingManager*   renderMan;
    gfl2::Fade::FadeManager*          fadeMan;
    app::tool::AppCommonGrpIconData*  cmnIcon;
    app::tool::AppToolTrainerIconRendering*
                                      trainerIconPtr;
  };

  //! @brief    初期化パラメータ
  struct InitParam
  {
    APP_PARAM*                  appParam;
    void*                       appLytBuff;
    gfl2::str::MsgData*         baseMsgData;
    gfl2::str::MsgData*         wazaMsgData;
    print::WordSet*             wordSet;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  UpperMainDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~UpperMainDraw( void ) { ; }

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
  virtual void Draw( gfl2::gfx::CtrDisplayNo no ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   破棄チェック
   *
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool IsExit( void ) { return true; }

  //------------------------------------------------------------------
  /**
   * @brief   準備完了かどうか
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsReady( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   メインシーケンスかどうか
   *
   * @return  "true  = メインシーケンス"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsMainSeq( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパラメータのセット
   *
   * @param   pp    表示する対象のポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetPokemonParam(const pml::pokepara::PokemonParam* pp, const BattleFesTool::BFCTrainerInfo* pBFCTrainerInfo = NULL);

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
   * @brief   オープン中？
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsOpen( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  void Initialize2D( InitParam& param );

private:
  //------------------------------------------------------------------
  /**
   * @brief   準備シーケンス
   *
   * @return  "true  = 準備完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool Sequence_Ready( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   技の説明を開く
   *
   * @param   wazaNo    技No
   *
   * @return  "true  = 開くことに成功"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool OpenWazaExplain( u32 wazaNo );

  //------------------------------------------------------------------
  /**
   * @brief   技の説明を閉じる
   */
  //------------------------------------------------------------------
  void CloseWazaExplain( void );

  //------------------------------------------------------------------
  /**
   * @brief   表示する技の設定
   *
   * @param   wazaNo    技No
   */
  //------------------------------------------------------------------
  void SetWazaExplain( u32 wazaNo );

  //------------------------------------------------------------------
  /**
   * @brief   技説明ウィンドウが閉じきったかどうか
   *
   * @return  "true  == 閉じ終わった"
   * @return  "false == 継続"
   */
  //------------------------------------------------------------------
  bool IsClosedWazaExplain( void ) const;

public:
  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートを表示
   */
  //------------------------------------------------------------------
  void VisibleRadarChart( void );

  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートを非表示
   */
  //------------------------------------------------------------------
  void InvisibleRadarChart( void );

  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートの表示モード切替
   */
  //------------------------------------------------------------------
  void ChangeRadarChartMode( void );

private:
  app::util::Heap*                    m_pHeap;

  app::util::AppRenderingManager*     m_pAppRenderingManager;

  gfl2::Fade::FadeManager*            m_pFadeManager;

  StatusDrawListener*                 m_uiListener;

  app::tool::AppCommonGrpIconData*    m_pCommonIcon;

  app::tool::AppToolTrainerIconRendering*
                                      m_pTrainerIcon;

  App::Tool::FieldStatusUpperDraw*    m_pUpperDraw;

private:
  u8                    m_Seq;

  APP_PARAM*            m_pAppParam;

  DrawerBase*           m_pDrawerBase;

  WazaExplain*          m_pWazaExplain;
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_UPPER_DRAW_H_INCLUDED__
