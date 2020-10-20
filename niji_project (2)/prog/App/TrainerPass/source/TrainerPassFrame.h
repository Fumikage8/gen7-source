//=============================================================================
/**
 * @file    TrainerPassFrame.h
 * @brief   トレーナーパス画面フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2015.11.06
 */
//=============================================================================
#if !defined( TRAINER_PASS_FRAME_H_INCLUDED )
#define TRAINER_PASS_FRAME_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Str.h>
// pmlib
#include <pml/include/pmlib.h>
// niji
#include "AppLib/include/Frame/AppFrameCellSuper.h"
// module
#include "App/TrainerPass/include/TrainerPassAppParam.h"
#include "TrainerPassDraw.h"



// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TrainerPass)

// 前方宣言
class PlayData;
#if PM_DEBUG
class TrainerPassDebugWin;
#endif  // PM_DEBUG

//=============================================================================
/**
 * @class TrainerPassFrame
 * @brief トレーナーパス画面フレームクラス
 * @date  2015.11.06
 */
//=============================================================================
class TrainerPassFrame
  : public applib::frame::CellSuper
  , public TrainerPassDraw::DrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN(TrainerPassFrame);    // コピーコンストラクタ＋代入禁止

private:
  static const s32 STAMP_INIT_WAIT = 30;    //!< スタンプデモの初期ウェイト
  static const s32 STAMP_PUT_WAIT  = 0;     //!< スタンプデモの押したときのウェイト
  static const s32 STAMP_END_WAIT  = 30*2;  //!< スタンプデモの終了ウェイト


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  TrainerPassFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~TrainerPassFrame();

  //-----------------------------------------------------------------------------
  /**
   * @func    Setup
   * @brief   パラメータセット
   * @date    2015.11.06
   *
   * @param   param       外部設定パラメータ
   * @parma   heap        ヒープ
   * @param   render_man  描画マネージャー
   */
  //-----------------------------------------------------------------------------
  void Setup( APP_PARAM * param, app::util::Heap * heap, app::util::AppRenderingManager * render_man );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFunc
   * @brief   フレーム初期化
   * @date    2015.11.06
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result InitFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFunc
   * @brief   フレーム終了
   * @date    2015.11.06
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result EndFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateFunc
   * @brief   フレーム更新
   * @date    2015.11.06
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   フレーム描画
   * @date    2015.11.06
   *
   * @param   no  描画先のディスプレイ番号
   */
  //-----------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    GetGameData
   * @brief   GameData関連取得
   * @date    2015.11.10
   */
  //-----------------------------------------------------------------------------
  void GetGameData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateDraw
   * @brief   描画クラス生成
   * @date    2015.11.06
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateDraw(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteDraw
   * @brief   描画クラス削除
   * @date    2015.11.06
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteDraw(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadLayoutResource
   * @brief   レイアウトリソース読み込み
   * @date    2015.11.06
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UnloadLayourResource
   * @brief   レイアウトリソース削除
   * @date    2015.11.06
   */
  //-----------------------------------------------------------------------------
  void UnloadLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadMessage
   * @brief   メッセージ読み込み
   * @date    2015.11.06
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UnloadMessage
   * @brief   メッセージ削除
   * @date    2015.11.06
   */
  //-----------------------------------------------------------------------------
  void UnloadMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateWordSetLoader
   * @brief   WordSetLoader生成
   * @date    2015.10.29
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
//  bool CreateWordSetLoader(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteWordSetLoader
   * @brief   WordSetLoader削除
   * @date    2015.10.29
   */
  //-----------------------------------------------------------------------------
//  void DeleteWordSetLoader(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFade
   * @brief   初期フェード設定
   * @date    2015.11.06
   */
  //-----------------------------------------------------------------------------
  void InitFade(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFadeIn
   * @brief   フェードイン開始
   * @date    2015.11.06
   */
  //-----------------------------------------------------------------------------
  void SetFadeIn(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFadeOut
   * @brief   フェードアウト開始
   * @date    2015.11.06
   */
  //-----------------------------------------------------------------------------
  void SetFadeOut(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsFadeEnd
   * @brief   フェード終了チェック
   * @date    2015.11.06
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsFadeEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeSequence
   * @brief   シーケンス切り替え
   * @date    2015.11.06
   *
   * @param   seq   シーケンス
   */
  //-----------------------------------------------------------------------------
  void ChangeSequence( u32 seq );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetEnd
   * @brief   終了設定
   * @date    2015.11.10
   *
   * @param   mode  終了モード
   */
  //-----------------------------------------------------------------------------
  void SetEnd( EndMode mode );

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqStart
   * @brief   アップデート：開始処理
   * @date    2015.11.06
   */
  //-----------------------------------------------------------------------------
  void Update_SeqStart(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqPageChangeL
   * @brief   アップデート：ページ切り替え(-)
   * @date    2015.11.09
   */
  //-----------------------------------------------------------------------------
  void Update_SeqPageChangeL(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqPageChangeR
   * @brief   アップデート：ページ切り替え(+)
   * @date    2015.11.09
   */
  //-----------------------------------------------------------------------------
  void Update_SeqPageChangeR(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqStampInit
   * @brief   アップデート：スタンプ用初期処理
   * @date    2015.11.09
   */
  //-----------------------------------------------------------------------------
  void Update_SeqStampInit(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqStampChangeL
   * @brief   アップデート：スタンプ用ページ切り替え(-)
   * @date    2015.11.09
   */
  //-----------------------------------------------------------------------------
  void Update_SeqStampChangeL(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqStampChangeR
   * @brief   アップデート：スタンプ用ページ切り替え(+)
   * @date    2015.11.09
   */
  //-----------------------------------------------------------------------------
  void Update_SeqStampChangeR(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqStampPut
   * @brief   アップデート：スタンプ用押す処理
   * @date    2015.11.09
   */
  //-----------------------------------------------------------------------------
  void Update_SeqStampPut(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqEnd
   * @brief   アップデート：終了処理
   * @date    2015.11.06
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Update_SeqEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OnAction
   * @brief   入力時のリスナー
   * @date    2015.11.06
   *
   * @param   id    動作ID
   *
   * @note  override TrainerPassDraw::DrawListener
   */
  //-----------------------------------------------------------------------------
  virtual void OnAction( TrainerPassDraw::DrawListener::ActionID id );


private:
  APP_PARAM * m_appParam;                               //!< 外部設定パラメータ
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ

  PlayData * m_playData;  //!< ゲーム中に保持し続けるデータ

  void * m_pLytResBuff;   //!< レイアウトリソース
  void * m_pMsgBuff;      //!< メッセージバッファ

//  WordSetLoader * m_pWordSetLoader; //!< ワードセットローダー

  TrainerPassDraw * m_pDraw;  //!< 描画クラス

  s16 m_stampPage;  //!< スタンプページ
  s16 m_stampWait;  //!< スタンプデモ用ウェイト

  u16 m_mainSeq;  //!< メインシーケンス
  u16 m_subSeq;   //!< サブシーケンス

#if PM_DEBUG
  void CreateDebugWin(void);
  void DeleteDebugWin(void);
  TrainerPassDebugWin * m_pDebugWin;
  gfl2::heap::HeapBase * m_pDebugWinHeap;
#endif  // PM_DEBUG
};


GFL_NAMESPACE_END(TrainerPass)
GFL_NAMESPACE_END(App)


#endif  // TRAINER_PASS_FRAME_H_INCLUDED
