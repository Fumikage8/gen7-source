//=============================================================================
/**
 * @file    StaffListFrame.h
 * @brief   momiji追加分のスタッフロール画面フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2017.03.02
 */
//=============================================================================
#if !defined( STAFFLIST_FRAME_H_INCLUDED )
#define STAFFLIST_FRAME_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Str.h>
// momiji
#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"
#include "App/FinderStudioStatic/include/FinderStudioMessageMenuView.h"
// module
#include "StaffListDraw.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(StaffList)

//=============================================================================
/**
 * @class StaffListFrame
 * @brief スタッフロール画面フレームクラス
 * @date  2017.03.02
 */
//=============================================================================
class StaffListFrame
  : public applib::frame::CellSuper
#if PM_DEBUG
  , public StaffListDraw::Listener
#endif // PM_DEBUG
{
  GFL_FORBID_COPY_AND_ASSIGN(StaffListFrame);   // コピーコンストラクタ＋代入禁止

public:
  static const u32 REQ_END = 0xffffffff;  //!< 終了リクエスト


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  StaffListFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~StaffListFrame();

  //-----------------------------------------------------------------------------
  /**
   * @func    Setup
   * @brief   パラメータセット
   * @date    2017.03.02
   *
   * @parma   heap        ヒープ
   * @param   render_man  描画マネージャー
   */
  //-----------------------------------------------------------------------------
  void Setup( app::util::Heap * heap, app::util::AppRenderingManager * render_man );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsCreated
   * @brief   生成が完了したか
   * @date    2017.03.16
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsCreated(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFunc
   * @brief   フレーム初期化
   * @date    2017.03.02
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result InitFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFunc
   * @brief   フレーム終了
   * @date    2017.03.02
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result EndFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateFunc
   * @brief   フレーム更新
   * @date    2017.03.02
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   フレーム描画
   * @date    2017.03.02
   *
   * @param   no  描画先のディスプレイ番号
   */
  //-----------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRequest
   * @brief   動作リスクエスト
   * @date    2017.03.03
   *
   * @param   no    データ番号
   * @param   mode  背景配置モード ※デフォルト = BG_PUT_RIGHT
   */
  //-----------------------------------------------------------------------------
  void SetRequest( u32 no, BgPutMode mode = BG_PUT_RIGHT );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsAllRequestEnd
   * @brief   全てのリクエストが終了したか
   * @date    2017.03.17
   *
   * @retval  true  = 終了した
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsAllRequestEnd(void);


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    CreateDraw
   * @brief   描画クラス生成
   * @date    2017.03.02
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
   * @date    2017.03.02
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
   * @date    2017.03.02
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
   * @date    2017.03.02
   */
  //-----------------------------------------------------------------------------
  void UnloadLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadMessage
   * @brief   メッセージ読み込み
   * @date    2017.03.02
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
   * @date    2017.03.02
   */
  //-----------------------------------------------------------------------------
  void UnloadMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadListItemData
   * @brief   リストデータ読み込み
   * @date    2017.03.03
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadListItemData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UnloadListItemData
   * @brief   リストデータ削除
   * @date    2017.03.03
   */
  //-----------------------------------------------------------------------------
  void UnloadListItemData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeSequence
   * @brief   シーケンス切り替え
   * @date    2017.03.02
   *
   * @param   seq   シーケンス
   */
  //-----------------------------------------------------------------------------
  void ChangeSequence( u32 seq );

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqMain
   * @brief   アップデート：メイン処理
   * @date    2017.03.02
   */
  //-----------------------------------------------------------------------------
  void Update_SeqMain(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqEnd
   * @brief   アップデート：終了処理
   * @date    2017.03.02
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Update_SeqEnd(void);


private:
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ

  void * m_pLytResBuff;     //!< 下画面用レイアウトリソース
  void * m_pMsgBuff;        //!< メッセージバッファ

  StaffListDraw * m_pDraw;  //!< 描画クラス

  void * m_pListBuf;  //!< リストデータバッファ
  const StaffListDraw::LIST_ITEM_DATA * m_listData; //!< リストデータ
  u32 m_listDataMax;  //!< リストデータ数

  bool m_isUpdateFunc;  //!< true = UpdateFunc

  u16 m_mainSeq;  //!< メインシーケンス
  u16 m_subSeq;   //!< サブシーケンス


#if PM_DEBUG
public:
  void DebugSetup(void);

private:
  //-----------------------------------------------------------------------------
  /**
   * @func    OnAction
   * @brief   動作リスナー
   * @date    2017.03.02
   *
   * @param   id    動作ID
   *
   * @note  override StaffListDraw::Listener
   */
  //-----------------------------------------------------------------------------
  virtual void OnAction( StaffListDraw::Listener::ActionID id );

  bool m_isDebugInput;

#endif // PM_DEBUG
};

GFL_NAMESPACE_END(StaffList)
GFL_NAMESPACE_END(App)

#endif  // STAFFLIST_FRAME_H_INCLUDED
