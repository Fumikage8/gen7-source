//=============================================================================
/**
 * @file    TownMapFrame.h
 * @brief   タウンマップ画面フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2015.10.13
 */
//=============================================================================
#if !defined( TOWNMAP_FRAME_H_INCLUDED )
#define TOWNMAP_FRAME_H_INCLUDED

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
#include "App/TownMapParts/include/TownMapPartsDraw.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TownMap)

// 前方宣言
struct APP_PARAM;
class TownMapDraw;

//=============================================================================
/**
 * @class TownMapFrame
 * @brief タウンマップ画面フレームクラス
 * @date  2015.10.13
 */
//=============================================================================
class TownMapFrame
  : public applib::frame::CellSuper
  , public TownMapPartsDraw::SubScreenMapListener
{
  GFL_FORBID_COPY_AND_ASSIGN(TownMapFrame);    // コピーコンストラクタ＋代入禁止


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  TownMapFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~TownMapFrame();

  //-----------------------------------------------------------------------------
  /**
   * @func    Setup
   * @brief   パラメータセット
   * @date    2015.10.13
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
   * @date    2015.10.13
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result InitFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFunc
   * @brief   フレーム終了
   * @date    2015.10.13
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result EndFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateFunc
   * @brief   フレーム更新
   * @date    2015.10.13
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   フレーム描画
   * @date    2015.10.13
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
   * @date    2015.10.13
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
   * @date    2015.10.13
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteDraw(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePartsDraw
   * @brief   下画面マップ描画クラス生成
   * @date    2015.10.13
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreatePartsDraw(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeletePartsDraw
   * @brief   描画クラス削除
   * @date    2015.10.13
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeletePartsDraw(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadLayoutResource
   * @brief   レイアウトリソース読み込み
   * @date    2015.10.13
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
   * @date    2015.10.13
   */
  //-----------------------------------------------------------------------------
  void UnloadLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadMessage
   * @brief   メッセージ読み込み
   * @date    2015.10.13
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
   * @date    2015.10.13
   */
  //-----------------------------------------------------------------------------
  void UnloadMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadTownMapData
   * @brief   タウンマップデータ読み込み
   * @date    2015.10.26
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadTownMapData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UnloadTownMapData
   * @brief   タウンマップデータ削除
   * @date    2015.10.26
   */
  //-----------------------------------------------------------------------------
  void UnloadTownMapData(void);

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
  bool CreateWordSetLoader(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteWordSetLoader
   * @brief   WordSetLoader削除
   * @date    2015.10.29
   */
  //-----------------------------------------------------------------------------
  void DeleteWordSetLoader(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFade
   * @brief   初期フェード設定
   * @date    2015.10.15
   */
  //-----------------------------------------------------------------------------
  void InitFade(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFadeIn
   * @brief   フェードイン開始
   * @date    2015.10.15
   */
  //-----------------------------------------------------------------------------
  void SetFadeIn(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFadeOut
   * @brief   フェードアウト開始
   * @date    2015.10.15
   */
  //-----------------------------------------------------------------------------
  void SetFadeOut(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsFadeEnd
   * @brief   フェード終了チェック
   * @date    2015.10.15
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
   * @date    2015.10.13
   *
   * @param   seq   シーケンス
   */
  //-----------------------------------------------------------------------------
  void ChangeSequence( u32 seq );

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqStart
   * @brief   アップデート：開始処理
   * @date    2015.10.14
   */
  //-----------------------------------------------------------------------------
  void Update_SeqStart(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqEnd
   * @brief   アップデート：終了処理
   * @date    2015.10.16
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Update_SeqEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PuttWarpPointMessage
   * @brief   "○○にとびますか？"のメッセージ表示
   * @date    2015.10.29
   */
  //-----------------------------------------------------------------------------
  void PutWarpPointMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutUpperInfo
   * @brief   上画面情報表示
   * @date    2015.10.29
   */
  //-----------------------------------------------------------------------------
  void PutUpperInfo(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OnAction
   * @brief   入力時のリスナー
   * @date    2015.10.13
   *
   * @param   id    動作ID
   *
   * @note  override TownMapPartsDraw::SubScreenMapListener
   */
  //-----------------------------------------------------------------------------
  virtual void OnAction( TownMapPartsDraw::SubScreenMapListener::ActionID id );


private:
  APP_PARAM * m_appParam;                               //!< 外部設定パラメータ
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ

  void * m_pUpperLytResBuff;  //!< 上画面用レイアウトリソース
  void * m_pLowerLytResBuff;  //!< 下画面用レイアウトリソース
  void * m_pRotomLytResBuff;  //!< ロトムレイアウトリソース
  void * m_pMsgBuff;          //!< メッセージバッファ

  WordSetLoader * m_pWordSetLoader; //!< ワードセットローダー

  TownMapDraw * m_pDraw;            //!< 描画クラス（上画面）
  TownMapPartsDraw * m_pPartsDraw;  //!< 描画クラス（下画面）

  void * m_pResZoneData;      //!< タウンマップデータ：ゾーン
  size_t m_resZoneDataLen;    //!< ゾーンデータ数
  void * m_pResIconData;      //!< タウンマップデータ： アイコン
  size_t m_resIconDataLen;    //!< アイコンデータ数

  s32 m_putWarpMsgID;   //!< "そらをとぶ"用場所メッセージID

  u16 m_mainSeq;  //!< メインシーケンス
  u16 m_subSeq;   //!< サブシーケンス
};


GFL_NAMESPACE_END(TownMap)
GFL_NAMESPACE_END(App)


#endif  // TOWNMAP_FRAME_H_INCLUDED
