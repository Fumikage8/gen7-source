//=============================================================================
/**
 * @file    ParurePokeListFrame.h
 * @brief   ポケパルレポケモン選択画面フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2015.11.12
 */
//=============================================================================
#if !defined( PARURE_POKELIST_FRAME_H_INCLUDED )
#define PARURE_POKELIST_FRAME_H_INCLUDED

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
#include "ParurePokeListDraw.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(PokeParure)

// 前方宣言
struct APP_PARAM;


//=============================================================================
/**
 * @class ParurePokeListFrame
 * @brief ポケパルレポケモン選択画面フレームクラス
 * @date  2015.10.13
 */
//=============================================================================
class ParurePokeListFrame
  : public applib::frame::CellSuper
  , public ParurePokeListDraw::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN(ParurePokeListFrame);    // コピーコンストラクタ＋代入禁止


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  ParurePokeListFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~ParurePokeListFrame();

  //-----------------------------------------------------------------------------
  /**
   * @func    Setup
   * @brief   パラメータセット
   * @date    2015.11.12
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
   * @date    2015.11.12
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result InitFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFunc
   * @brief   フレーム終了
   * @date    2015.11.12
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result EndFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateFunc
   * @brief   フレーム更新
   * @date    2015.11.12
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   フレーム描画
   * @date    2015.11.12
   *
   * @param   no  描画先のディスプレイ番号
   */
  //-----------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    CreateDraw
   * @brief   描画クラス生成
   * @date    2015.11.12
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
   * @date    2015.11.12
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
   * @date    2015.11.12
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
   * @date    2015.11.12
   */
  //-----------------------------------------------------------------------------
  void UnloadLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadMessage
   * @brief   メッセージ読み込み
   * @date    2015.11.12
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
   * @date    2015.11.12
   */
  //-----------------------------------------------------------------------------
  void UnloadMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFade
   * @brief   初期フェード設定
   * @date    2015.11.12
   */
  //-----------------------------------------------------------------------------
  void InitFade(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFadeIn
   * @brief   フェードイン開始
   * @date    2015.11.12
   */
  //-----------------------------------------------------------------------------
  void SetFadeIn(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFadeOut
   * @brief   フェードアウト開始
   * @date    2015.11.12
   */
  //-----------------------------------------------------------------------------
  void SetFadeOut(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsFadeEnd
   * @brief   フェード終了チェック
   * @date    2015.11.12
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
   * @date    2015.11.12
   *
   * @param   seq   シーケンス
   */
  //-----------------------------------------------------------------------------
  void ChangeSequence( u32 seq );

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqStart
   * @brief   アップデート：開始処理
   * @date    2015.11.12
   */
  //-----------------------------------------------------------------------------
  void Update_SeqStart(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqEnd
   * @brief   アップデート：終了処理
   * @date    2015.11.12
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
   * @date    2015.11.12
   *
   * @param   pos   選択位置
   *
   * @note  override ParurePokeListDraw::Listener
   */
  //-----------------------------------------------------------------------------
  virtual void OnAction( u32 pos );


private:
  APP_PARAM * m_appParam;                               //!< 外部設定パラメータ
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ

  void * m_pLytResBuff;       //!< 上画面用レイアウトリソース
  void * m_pMsgBuff;          //!< メッセージバッファ
  void * m_pParticleResBuff;  //!< パーティクルリソース

//  WordSetLoader * m_pWordSetLoader; //!< ワードセットローダー

  ParurePokeListDraw * m_pDraw;            //!< 描画クラス（上画面）

  u16 m_mainSeq;  //!< メインシーケンス
  u16 m_subSeq;   //!< サブシーケンス
};


GFL_NAMESPACE_END(PokeParure)
GFL_NAMESPACE_END(App)


#endif  // PARURE_POKELIST_FRAME_H_INCLUDED
