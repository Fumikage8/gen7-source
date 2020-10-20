#if !defined( __APP_TOOL_STATUS_UPPER_DRAW_H_INCLUDED__ )
#define __APP_TOOL_STATUS_UPPER_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    AppToolStatusUpperDraw.h
 * @date    2015/12/16 17:42:58
 * @author  fukushima_yuya
 * @brief   フィールドステータス：メイン画面(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/Tool/AppToolTrainerIconRendering.h>

#include  <Field/FieldStatic/include/BattleFes/BattleFesTool.h>

//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class AppCommonGrpIconData;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)
class FieldStatusUpperInfo;
class FieldStatusUpperRadarChart;
GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//------------------------------------------------------------------------------
// @brief   フィールドステータス：メイン画面(上)
//------------------------------------------------------------------------------
class FieldStatusUpperDraw
  : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN( FieldStatusUpperDraw );

public:
  //! @brief    生成パラメータ
  struct CreateParam
  {
    app::util::Heap*                  heap;           //!< ヒープ
    app::util::AppRenderingManager*   renderMan;      //!< レンダリングマネージャ
    app::tool::AppCommonGrpIconData*  cmnIcon;        //!< 共通アイコン
    app::tool::AppToolTrainerIconRendering*
                                      trainerIconPtr;

    gfl2::str::MsgData*               msgData;        //!< メッセージデータ
    print::WordSet*                   wordSet;        //!< ワードセット

    void*                             lytBuff;                //!< レイアウトバッファ
    bool                              helpFlag;               //!< ヘルプボタンの表示フラグ(true = 表示、false = 非表示)
    bool                              battleFesCircleLayout;  //!< BFC用のレイアウトにする

    /*  ctor  */
    CreateParam(void)
      : heap(NULL)
      , renderMan(NULL)
      , cmnIcon(NULL)
      , trainerIconPtr(NULL)
      , msgData(NULL)
      , wordSet(NULL)
      , lytBuff(NULL)
      , helpFlag(true)
      , battleFesCircleLayout(false)
    {
    }
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   param   生成パラメータ
   */
  //------------------------------------------------------------------
  FieldStatusUpperDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~FieldStatusUpperDraw( void ) { ; }

public:
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

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool InitFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool EndFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新処理
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool UpdateFunc( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2Dの生成
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool _Create2D( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモン情報の生成
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool _CreatePokeInfo( void );

  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートの生成
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool _CreateRadarChart( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパラメータの設定
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetPokemonParam( const pml::pokepara::PokemonParam* pp, const BattleFesTool::BFCTrainerInfo* pBFCTrainerInfo = NULL );

  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートの表示切替
   *
   * @param   visible   表示フラグ
   */
  //------------------------------------------------------------------
  void SetVisibleRadarChart( bool visible );

  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートの表示モード切替
   */
  //------------------------------------------------------------------
  void ChangeRadarChartMode( void );

private:
  enum SequenceKind {
    SEQ_KIND_INIT,    //!< 初期化処理
    SEQ_KIND_INFO,    //!< ポケモン情報
    SEQ_KIND_CHART,   //!< レーダーチャート
    SEQ_KIND_NUM,
  };

private:
  app::util::Heap*                    m_pHeap;

  app::util::AppRenderingManager*     m_pAppRenderingManager;

  app::tool::AppCommonGrpIconData*    m_pCommonIcon;

  app::tool::AppToolTrainerIconRendering*
                                      m_pTrainerIcon;

  gfl2::str::MsgData*                 m_pMsgData;

  print::WordSet*                     m_pWordSet;

  void*                               m_pLytBuff;

private:
  FieldStatusUpperInfo*         m_pUpperInfo;

  FieldStatusUpperRadarChart*   m_pUpperRadarChart;

private:
  u8      m_Seq[SEQ_KIND_NUM];

  bool    m_HelpVisible;
  bool    m_IsBattleFesCircleLayout;  //!< BFC用のレイアウトにする
};

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

#endif // __APP_TOOL_STATUS_UPPER_DRAW_H_INCLUDED__