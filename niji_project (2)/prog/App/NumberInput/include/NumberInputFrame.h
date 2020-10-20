#if !defined( __NUMBER_INPUT_FRAME_H_INCLUDED__ )
#define __NUMBER_INPUT_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    NumberInputFrame.h
 * @date    2015/11/24 12:24:22
 * @author  fukushima_yuya
 * @brief   英数字入力アプリ：Frame
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/NumberInput/source/NumberInputDef.h>

#include <AppLib/include/Frame/AppFrameCellSuper.h>
#include <AppLib/include/Util/app_util_heap.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(NumberInput)
struct APP_PARAM;
class NumberInputDraw;
class UI;
GFL_NAMESPACE_END(NumberInput)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(GameSys)
class GameManager;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(Fade)
class FadeManager;
GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_BEGIN(ui)
class DeviceManager;
GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_END(gfl2)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(NumberInput)

//------------------------------------------------------------------------------
// @brief   構造体
//------------------------------------------------------------------------------
//! @brief    サブワーク
struct SUB_WORK
{
  NumberInputDraw* graWork;     //!< グラフィックワーク
  UI*              ui;          //!< UI
};

//! @brief    メインワーク
struct MAIN_WORK
{
  APP_PARAM*    prm;            //!< 外部設定パラメータ

  GameSys::GameManager*       gameManager;    //!< ゲームマネージャ
  gfl2::ui::DeviceManager*    devManager;     //!< 入力デバイス管理オブジェクト
  gfl2::Fade::FadeManager*    fadeManager;    //!< フェードマネージャ

  app::util::AppRenderingManager*   renderManager;    //!< レンダリングマネージャ

  u32   mainSeq;                //!< メインシーケンス
  u32   nextSeq;                //!< メインシーケンス遷移用シーケンス
  u32   fadeNextSeq;            //!< フェード後に遷移するシーケンス

  s32   inputData[INPUT_MAX];   //!< 入力されたデータ

  u16   inputPos;               //!< 入力位置
  u16   inputMax;               //!< 入力データ最大数
};


//------------------------------------------------------------------------------
// @brief   Frameクラス
//------------------------------------------------------------------------------
class NumberInputFrame
  : public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN( NumberInputFrame );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  NumberInputFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~NumberInputFrame( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   Proc初期化処理
   *
   * @param   proc_manager    gfl2::proc::Manager
   *
   * @return  gfl2::proc::Result
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   Proc終了処理
   *
   * @param   proc_manager    gfl2::proc::Manager
   *
   * @return  gfl2::proc::Result
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result EndFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   Procアップデート処理
   *
   * @param   proc_manager    gfl2::proc::Manager
   *
   * @return  gfl2::proc::Result
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   Proc描画処理
   *
   * @param   no    ディスプレイNo
   *
   * @return  none
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ゲームマネージャ関連取得
   *
   * @param   man   GameSys::GameManagerクラスポインタ
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void SetGameManager( GameSys::GameManager* man );

  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   *
   * @param   prm   外部設定パラメータ
   * @param   man   レンダリングマネージャ
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void Setup( APP_PARAM* prm, app::util::AppRenderingManager* man );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ヒープの生成
   *
   * @param   none
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool CreateHeap( void );

  //------------------------------------------------------------------
  /**
   * @brief   レイアウトデータの読み込み
   *
   * @param   none
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool LoadLayoutData( void );

  //------------------------------------------------------------------
  /**
   * @brief   サブワークの作成
   *
   * @param   none
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void CreateSubWork( void );

  //------------------------------------------------------------------
  /**
   * @brief   サブワークの削除
   *
   * @param   none
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool DeleteSubWork( void );

  //------------------------------------------------------------------
  /**
   * @brief   フェードインのセット
   *
   * @param   next    フェード後のシーケンス
   *
   * @return  MAINSEQ_FADE
   */
  //------------------------------------------------------------------
  u32 SetFadeIn( u32 next );

  //------------------------------------------------------------------
  /**
   * @brief   フェードアウトのセット
   *
   * @param   next    フェード後のシーケンス
   *
   * @return  MAINSEQ_FADE
   */
  //------------------------------------------------------------------
  u32 SetFadeOut( u32 next );

  //------------------------------------------------------------------
  /**
   * @brief   キーボード入力
   *
   * @param   id    入力データID
   *
   * @return  次のシーケンス
   */
  //------------------------------------------------------------------
  u32 DecideKeyBoard( u32 id );

  //------------------------------------------------------------------
  /**
   * @brief   終了パラメータ設定
   *
   * @param   none
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void SetEndParam( void );

  //------------------------------------------------------------------
  /**
   * @brief   入力データ初期化
   *
   * @param   none
   *
   * @return  "true  = 外部から指定されたデータで初期化"
   * @return  "false = 指定されたデータがないので何もしなかった"
   */
  //------------------------------------------------------------------
  bool InitInputData( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   メインシーケンス：初期化
   *
   * @param   none
   *
   * @return  次のシーケンス
   *
   * 定義：MAINSEQ_INIT
   */
  //------------------------------------------------------------------
  u32 MainSeq_Init( void );

  //------------------------------------------------------------------
  /**
   * @brief   メインシーケンス：解放
   *
   * @param   none
   *
   * @return  次のシーケンス
   *
   * 定義：MAINSEQ_RELEASE
   */
  //------------------------------------------------------------------
  u32 MainSeq_Release( void );

  //------------------------------------------------------------------
  /**
   * @brief   メインシーケンス：フェード待ち
   *
   * @param   none
   *
   * @return  次のシーケンス
   *
   * 定義：MAINSEQ_FADE
   */
  //------------------------------------------------------------------
  u32 MainSeq_Fade( void );

  //------------------------------------------------------------------
  /**
   * @brief   メインシーケンス：開始待ち
   *
   * @param   none
   *
   * @return  次のシーケンス
   *
   * 定義：MAINSEQ_START_WAIT
   */
  //------------------------------------------------------------------
  u32 MainSeq_StartWait( void );

  //------------------------------------------------------------------
  /**
   * @brief   メインシーケンス：メイン処理
   *
   * @param   none
   *
   * @return  次のシーケンス
   *
   * 定義：MAINSEQ_MAIN
   */
  //------------------------------------------------------------------
  u32 MainSeq_Main( void );

private:
  SUB_WORK*           m_SubWork;      //!< サブワーク

  MAIN_WORK           m_MainWork;     //!< メインワーク
  
  app::util::Heap     m_Heap;         //!< ヒープ

  void*               m_pArcReadBuff; //!< レイアウトデータ

  u8                  m_HeapSeq;      //!< ヒープ生成シーケンス

  u8                  m_LytLoadSeq;   //!< レイアウト読み込みシーケンス
};

GFL_NAMESPACE_END(NumberInput)
GFL_NAMESPACE_END(App)

#endif // __NUMBER_INPUT_FRAME_H_INCLUDED__