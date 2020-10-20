#if !defined( __APP_STATUS_FRAME_H_INCLUDED__ )
#define __APP_STATUS_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusFrame.h
 * @date    2015/11/27 16:21:49
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：Frame
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusDrawListener.h"

#include <AppLib/include/Frame/AppFrameCellSuper.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Fade/include/gfl2_FadeManager.h>
#include <fs/include/gfl2_Fs.h>
#include <str/include/gfl2_Str.h>
#include <ui/include/gfl2_UI.h>
#include <AppLib/include/Tool/AppToolTrainerIconRendering.h>

//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)
struct APP_PARAM;
class MenuCursor;
class PokemonContainer;
class UpperMainDraw;
class UpperMemoDraw;
class LowerMainDraw;
class LowerBoxMarkDraw;
GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN( tool )
class AppCommonGrpIconData;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_BEGIN(ui)
class UIView;
GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(print)
class WordSet;
GFL_NAMESPACE_END(print)

class WordSetLoader;

GFL_NAMESPACE_BEGIN(pml)
GFL_NAMESPACE_BEGIN(pokepara)
class PokemonParam;
GFL_NAMESPACE_END(pokepara)
GFL_NAMESPACE_END(pml)

GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)
class BaseCamera;
GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN(renderingengine)
GFL_NAMESPACE_BEGIN(scenegraph)
GFL_NAMESPACE_BEGIN(instance)
class DrawEnvNode;
GFL_NAMESPACE_END(instance)
GFL_NAMESPACE_END(scenegraph)
GFL_NAMESPACE_END(renderingengine)
GFL_NAMESPACE_END( gfl2 )


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   Frame
//------------------------------------------------------------------------------
class StatusFrame
  : public applib::frame::CellSuper
  , public StatusDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN( StatusFrame );

public:
  //! @brief  セットアップパラメータ
  struct SetupParam
  {
    APP_PARAM*                      appParam;
    app::util::Heap*                heap;
    app::util::AppRenderingManager* renderMan;
    gfl2::Fade::FadeManager*        fadeMan;
    gfl2::fs::AsyncFileManager*     fileMan;
    gfl2::ui::DeviceManager*        devMan;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief    コンストラクタ
   */
  //------------------------------------------------------------------
  StatusFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~StatusFrame( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief    動作に必要な引数を登録
   *
   * @param   param   セットアップパラメータ
   */
  //------------------------------------------------------------------
  void Setup( SetupParam& param );

public:
  //------------------------------------------------------------------
  /**
   * @brief    PROCの初期化を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief    PROCのメイン更新を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief    PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief    PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result EndFunc( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   レイアウトデータの読み込み
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool LoadLayoutData( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージデータの読み込み
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool LoadMsgData( void );

  //------------------------------------------------------------------
  /**
   * @brief   ワードセットローダーの生成
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool CreateWordSetLoader( void );

  //------------------------------------------------------------------
  /**
   * @brief   メニューカーソルの生成
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool CreateMenuCursor( void );

  //------------------------------------------------------------------
  /**
   * @brief   共通アイコンの生成
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool CreateCommonIcon( void );

  //------------------------------------------------------------------
  /**
   * @brief   トレーナーアイコンの生成
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool CreateTrainerIcon( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画環境の生成
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool CreateDrawEnv( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンコンテナの生成
   */
  //------------------------------------------------------------------
  void CreatePokemonContainer( void );

  //------------------------------------------------------------------
  /**
   * @brief   メイン画面(上)の生成
   */
  //------------------------------------------------------------------
  void CreateUpperMainDraw( void );

  //------------------------------------------------------------------
  /**
   * @brief   メモ画面(上)の生成
   */
  //------------------------------------------------------------------
  void CreateUpperMemoDraw( void );

  //------------------------------------------------------------------
  /**
   * @brief   メイン画面(下)の生成
   */
  //------------------------------------------------------------------
  void CreateLowerMainDraw( void );

  //------------------------------------------------------------------
  /**
   * @brief   ボックスマーク変更画面(下)の生成
   */
  //------------------------------------------------------------------
  void CreateLowerBoxMarkDraw( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   すべての画面の準備が完了したか？
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsReady( void );

  //------------------------------------------------------------------
  /**
   * @brief   すべての画面の破棄が可能か？
   *
   * @return  "true  = 可"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsExit( void );

  //------------------------------------------------------------------
  /**
   * @brief   制限時間超過か？
   *
   * @return  "true  = 超過"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsTimeOut( void );

  //------------------------------------------------------------------
  /**
   * @brief   開く
   */
  //------------------------------------------------------------------
  void StatusOpen( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパラメータのセット
   *
   * @param   index   次に表示するインデックス
   */
  //------------------------------------------------------------------
  void SetPokemonParam( u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパラメータの反映
   */
  //------------------------------------------------------------------
  void UpdatePokemonParam( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新禁止かどうか
   *
   * @param   pokeIndex   ポケモンインデックス
   *
   * @return  "true  = 禁止"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsUpdateProhibition( u32 pokeIndex );

private:  //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   終了通知
   */
  //------------------------------------------------------------------
  virtual void EndStatus( void );

  //------------------------------------------------------------------
  /**
   * @brief   キャンセルボタンの押下
   */
  //------------------------------------------------------------------
  virtual void OnCancelBtn( void );

  //------------------------------------------------------------------
  /**
   * @brief   決定ボタンの押下
   */
  //------------------------------------------------------------------
  virtual void OnDecideBtn( void );

  //------------------------------------------------------------------
  /**
   * @brief   閉じる
   */
  //------------------------------------------------------------------
  virtual void StatusClose( void );

  //------------------------------------------------------------------
  /**
   * @brief   画面モードの設定
   *
   * @param   mode    画面モード
   */
  //------------------------------------------------------------------
  virtual void SetDispMode( u8 mode );

  //------------------------------------------------------------------
  /**
   * @brief   画面モードの取得
   *
   * @return  画面モード
   */
  //------------------------------------------------------------------
  virtual u8 GetDispMode( void );

  //------------------------------------------------------------------
  /**
   * @brief   タブモードの取得
   *
   * @return  タブモード
   */
  //------------------------------------------------------------------
  virtual u8 GetTabMode( void );

  //------------------------------------------------------------------
  /**
   * @brief   レーダーチャートの表示モード切替
   */
  //------------------------------------------------------------------
  virtual void ChangeRadarChartMode( void );

private:  //!< タブ関連リスナー
  //------------------------------------------------------------------
  /**
   * @brief   技タブへの変更
   *
   * @return  "true  = 変更成功"
   * @return  "false = 変更失敗"
   */
  //------------------------------------------------------------------
  virtual bool ChangeWazaTab( void );

  //------------------------------------------------------------------
  /**
   * @brief   メモタブへの変更
   *
   * @return  "true  = 変更成功"
   * @return  "false = 変更失敗"
   */
  //------------------------------------------------------------------
  virtual bool ChangeMemoTab( void );

  //------------------------------------------------------------------
  /**
   * @brief   タマゴ用メモタブへの変更
   *
   * @return  "true  = 変更成功"
   * @return  "false = 変更失敗"
   */
  //------------------------------------------------------------------
  virtual bool ChangeEggMemoTab( void );

private:  //!< ポケモンパラメータ関連リスナー
  //------------------------------------------------------------------
  /**
   * @brief   次のポケモンへ
   *
   * @note    十字キー切り替え
   */
  //------------------------------------------------------------------
  virtual void NextPokemon( void );

  //------------------------------------------------------------------
  /**
   * @brief   前のポケモンへ
   *
   * @note    十字キー切り替え
   */
  //------------------------------------------------------------------
  virtual void PrevPokemon( void );

  //------------------------------------------------------------------
  /**
   * @brief   指定したポケモンに変更
   *
   * @note    手持ち表示でボールをタッチした時のみ使用
   */
  //------------------------------------------------------------------
  virtual void SetPokemon( u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパラメータの更新
   */
  //------------------------------------------------------------------
  virtual void UpdatePokeParam( void );

private:  //!< ボックスマーク変更画面関連リスナー
  //------------------------------------------------------------------
  /**
   * @brief   ボックスマーク変更画面のオープン
   *
   * @param   updateProhibition   更新禁止フラグ
   */
  //------------------------------------------------------------------
  virtual void OpenBoxMark( bool updateProhibition );

  //------------------------------------------------------------------
  /**
   * @brief   ボックスマーク変更画面のクローズ
   */
  //------------------------------------------------------------------
  virtual void CloseBoxMark( void );

private:  //!< 技説明画面関連リスナー
  //------------------------------------------------------------------
  /**
   * @brief   技説明画面のオープン
   *
   * @param   index   インデックス
   *
   * @return  "true  = 開いた"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  virtual bool OpenWazaExplain( u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   技説明画面のクローズ
   */
  //------------------------------------------------------------------
  virtual void CloseWazaExplain( void );

  //------------------------------------------------------------------
  /**
   * @brief   表示する技を変更
   *
   * @param   wazaNo    技No
   */
  //------------------------------------------------------------------
  virtual void SetWazaExplain( u32 wazaNo );

  //------------------------------------------------------------------
  /**
   * @brief   技説明が閉じきったかどうか
   *
   * @param   wazaNo    技No
   */
  //------------------------------------------------------------------
  virtual bool IsWazaExplainClosed( void );

private:  //!< リボン説明画面関連リスナー
  //------------------------------------------------------------------
  /**
   * @brief   リボン説明画面のオープン
   *
   * @param   ribbonID    リボンID
   *
   * @return  "true  = 開いた"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  virtual bool OpenRibbonExplain( u32 ribbonID );

  //------------------------------------------------------------------
  /**
   * @brief   リボン説明画面のクローズ
   */
  //------------------------------------------------------------------
  virtual void CloseRibbonExplain( void );

  //------------------------------------------------------------------
  /**
   * @brief   表示するリボンを変更
   *
   * @param   ribbonID    リボンID
   */
  //------------------------------------------------------------------
  virtual void SetRibbonExplain( u32 ribbonID );

  //------------------------------------------------------------------
  /**
   * @brief   リボンを未所持かどうか
   *
   * @param   none    true = 未所持、false = それ以外
   */
  //------------------------------------------------------------------
  virtual void SetRibbonNone( bool none );

protected:
  int   m_subseq;     //!< プロセス関数の動作シーケンス
  bool  m_bEnd;       //!< Endフラグ

private:
  enum MsgKind {
    MSG_KIND_STATUS,
    MSG_KIND_WAZA_INFO,
    MSG_KIND_ITEM_INFO,
    MSG_KIND_TOKUSEI_INFO,
    MSG_KIND_TRAINER_MEMO,
    MSG_KIND_RIBBON,
    MSG_KIND_NUM,
  };

  enum CursorKind {
    CUR_KIND_WAZA,
    CUR_KIND_RIBBON,
    CUR_KIND_NUM,
  };

private:
  app::util::Heap*                  m_pAppHeap;

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  SetupParam                        m_SetupParam;

  gfl2::str::MsgData*               m_pMsgData[MSG_KIND_NUM];
  
  print::WordSet*                   m_pWordSet;

  WordSetLoader*                    m_pWordSetLoader;

  app::tool::AppCommonGrpIconData*  m_pCommonIcon;

  app::tool::AppToolTrainerIconRendering*
                                    m_pTrainerIcon;

private:
  APP_PARAM*                        m_pAppParam;

  PokemonContainer*                 m_pPokeContainer;

  pml::pokepara::PokemonParam*      m_pPokeParam;

  app::ui::UIView*                  m_pParentView;

  UpperMainDraw*                    m_pUpperMainDraw;

  UpperMemoDraw*                    m_pUpperMemoDraw;

  LowerMainDraw*                    m_pLowerMainDraw;

  LowerBoxMarkDraw*                 m_pLowerBoxMarkDraw;

private:
  void*           m_pArcReadBuff;

  void*           m_pMsgReadBuff[MSG_KIND_NUM];

  MenuCursor*     m_pMenuCursor[CUR_KIND_NUM];

  u8              m_LytLoadSeq;

  u8              m_MsgDataCount;

  u8              m_MsgLoadSeq;

  u8              m_WordSetSeq;

  u8              m_CursorCount;

  u8              m_CursorSeq;

  u8              m_CmnIconSeq;

  u8              m_TrainerIconSeq;   /**<  trainerIcon初期化Seq  */

  u8              m_EnvSeq;

private:
  u8              m_DispMode;

  u8              m_TabMode;

  u32             m_PokeIndex;

  bool            m_UpdateProhibition;

  bool            m_RibbonNone;

private:
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   m_pCameraDrawEnvNode;   //!< カメラを含む環境
  poke_3d::model::BaseCamera*                                 m_pCamera;              //!< カメラ

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   m_pDrawEnvNode;         //!< ライトを含む環境
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_FRAME_H_INCLUDED__
