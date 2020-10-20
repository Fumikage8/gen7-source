//=============================================================================
/**
 * @file    FieldMenuSubWindowProc.h
 * @brief   フィールドメニューProc
 * @author  Hiroyuki Nakamura
 * @date    2015.06.04
 */
//=============================================================================
#if !defined( FIELD_MENU_SUBWINDOW_PROC_H_INCLUDED )
#define FIELD_MENU_SUBWINDOW_PROC_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "Savedata/include/FieldMenuSave.h"
#include "AppLib/include/Util/app_util_Heap.h"
#include "Field/FieldRo/include/Subscreen/FieldSubScreenProc.h"
#include "Field/FieldRo/include/Event/FieldEventPokemonRide.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

// 前方宣言
class FieldMenuFrame;

//=============================================================================
/**
 * @class FieldMenuProc
 * @brief フィールドメニューProcクラス
 * @date  2015.05.18
 */
//=============================================================================
class FieldMenuSubWindowProc
  : public Field::SubScreen::FieldSubScreenProc
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldMenuSubWindowProc);  // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  FieldMenuSubWindowProc(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~FieldMenuSubWindowProc();

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFunc
   * @brief   Proc初期化処理
   * @date    2015.06.04
   *
   * @param   pManager    プロセス管理マネージャー
   *
   * @return  gfl2::proc::Result
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFunc
   * @brief   Proc終了処理
   * @date    2015.06.04
   *
   * @param   pManager    プロセス管理マネージャー
   *
   * @return  gfl2::proc::Result
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateFunc
   * @brief   Procアップデート処理
   * @date    2015.06.04
   *
   * @param   pManager    プロセス管理マネージャー
   *
   * @return  gfl2::proc::Result
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    PreDrawFunc
   * @brief   Proc描画下準備
   * @date    2015.07.09
   *
   * @param   pManager    プロセス管理マネージャー
   */
  //-----------------------------------------------------------------------------
  virtual void PreDrawFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   Proc描画処理
   * @date    2015.06.04
   *
   * @param   pManager    プロセス管理マネージャー
   * @param   displayNo   描画対象画面
   */
  //-----------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::proc::Manager * pManager, gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    Resume
   * @brief   フィールド復帰　再稼働
   * @date    2015.06.04
   */
  //-----------------------------------------------------------------------------
  virtual void Resume(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Suspend
   * @brief   フィールド削除　一時停止
   * @date    2015.06.04
   */
  //-----------------------------------------------------------------------------
  virtual void Suspend(void);

  //-----------------------------------------------------------------------------
  /**
   *	@brief  下画面の種類を返す
   */
  //-----------------------------------------------------------------------------
  virtual Field::SubScreen::SubWindowId GetSubWindowId() { return Field::SubScreen::WIN_MENU_MAIN; };

  //-----------------------------------------------------------------------------
  /**
   * @func    GetEventRequestID
   * @brief	  起動リクエストを返す
   * @date    2015.09.03
   *
   * @return  リクエストID
   */
  //-----------------------------------------------------------------------------
  virtual Field::SubScreen::FieldMenuOutRequestID GetEventRequestID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func     フィールドメニューフレームを取得する
   * @return   FieldMenuFrame*
   */
  //-----------------------------------------------------------------------------
  FieldMenuFrame* GetFieldMenuFrame(void){ return m_pMenuFrame; }

  //-----------------------------------------------------------------------------
  /**
   * @func    RemakeFrame
   * @brief   フレーム再生成リクエスト
   * @date    2015.09.10
   *
   * @retval  true  = リクエスト成功
   * @retval  false = リクエスト失敗
   *
   * @note  終了リクエストが発行されている場合は失敗します
   */
  //-----------------------------------------------------------------------------
  bool RemakeFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsFrameRemake
   * @brief   フレーム再生成中か
   * @date    2015.09.10
   *
   * @retval  true  = 再生成中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsFrameRemake(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRideID
   * @brief   選択されたライドポケモンのIDを取得
   * @date    2015.09.16
   *
   * @return  ライドポケモンのID
   */
  //-----------------------------------------------------------------------------
  Savedata::FieldMenu::RideID GetRideID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetFieldRideID
   * @brief   フィールドのライドポケモンのIDを取得
   * @date    2015.09.16
   *
   * @param   id  下画面のライドポケモンID
   *
   * @return  フィールドのライドポケモンのID
   */
  //-----------------------------------------------------------------------------
  Field::RIDE_POKEMON_ID GetFieldRideID( Savedata::FieldMenu::RideID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsCreate
   * @brief   フィールドメニューの生成が終了したか
   * @date    2015.11.04
   *
   * @retval  true  = した
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsCreate(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomEvent
   * @brief   ロトムメッセージ表示イベントを開始
   * @date    2015.11.11
   *
   * @param   msg_id  メッセージID
   *
   * @retval  true  = 成功
   * @retval  false = 失敗
   */
  //-----------------------------------------------------------------------------
  bool SetRotomEvent( u32 msg_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRotomEvent
   * @brief   ロトムメッセージ表示イベント中か
   * @date    2015.11.11
   *
   * @retval  true  = イベント中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsRotomEvent(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomPosEvent
   * @brief   ロトムメッセージ表示リクエスト（POSイベント用）
   * @date    2016.01.15
   *
   * @param   msg_id  メッセージID
   */
  //-----------------------------------------------------------------------------
  void SetRotomPosEvent( u32 msg_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsMenuDrawCreate
   * @brief   MenuDrawクラスの生成が完了したか
   * @date    2017.04.05
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   *
   * @note
   *    FieldStaffListクラス生成時に
   *    メッセージのアーカイブへの読み込みが重なってしまうため、
   *    メッセージを読み込んでいるMenuDrawクラスの完成をチェックする
   */
  //-----------------------------------------------------------------------------
  bool IsMenuDrawCreate(void) const;


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    CreateHeap
   * @brief   ヒープ生成
   * @date    2015.07.09
   */
  //-----------------------------------------------------------------------------
  void CreateHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteHeap
   * @brief   ヒープ削除
   * @date    2015.07.09
   */
  //-----------------------------------------------------------------------------
  void DeleteHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateRenderManager
   * @brief   描画マネージャ生成
   * @date    2015.07.09
   */
  //-----------------------------------------------------------------------------
  void CreateRenderManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteRenderManager
   * @brief   描画マネージャ削除
   * @date    2015.07.09
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteRenderManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateFrame
   * @brief   フレーム生成
   * @date    2015.07.09
   */
  //-----------------------------------------------------------------------------
  void CreateFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteFrame
   * @brief   フレーム削除
   * @date    2015.07.09
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteFrame(void);

public:
  //-----------------------------------------------------------------------------
  /**
   * @func    SetRequest
   * @brief    FieldMenuFrameへ変更の指示を出す
   */
  //-----------------------------------------------------------------------------
  void SetRequest( Field::SubScreen::FieldMenuInRequestID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRequest
   * @brief   FieldMenuFrameから変更の指示を得る
   */
  //-----------------------------------------------------------------------------
  Field::SubScreen::FieldMenuOutRequestID GetRequest(void);

private:
  //-----------------------------------------------------------------------------
  /**
   * @func    Update_MenuMain
   * @brief   アップデート：メイン処理
   * @date    2015.06.04
   *
   * @param   pManager  プロセス管理マネージャー
   *
   * @return  gfl2::proc::Result
   */
  //-----------------------------------------------------------------------------
  gfl2::proc::Result Update_MenuMain( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_RemakeFrameInit
   * @brief   アップデート：フレーム再生成初期化
   * @date    2015.09.28
   *
   * @param   pManager  プロセス管理マネージャー
   *
   * @return  gfl2::proc::Result
   */
  //-----------------------------------------------------------------------------
  gfl2::proc::Result Update_RemakeFrameInit( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_RemakeFrameMain
   * @brief   アップデート：フレーム再生成メイン
   * @date    2015.09.28
   *
   * @param   pManager  プロセス管理マネージャー
   *
   * @return  gfl2::proc::Result
   */
  //-----------------------------------------------------------------------------
  gfl2::proc::Result Update_RemakeFrameMain( gfl2::proc::Manager * pManager );


private:
  gfl2::heap::HeapBase * m_pManagerHeap;  //!< マネージャヒープ

  app::util::Heap m_heap;   //!< ローカルヒープ

  app::util::AppRenderingManager * m_pRenderingManager;   //!< 描画マネージャ
  applib::frame::Manager * m_pFrameManager;   //!< フレームマネージャ
  FieldMenuFrame * m_pMenuFrame;              //!< フレーム

  bool m_frameRemakeReq;   //!< フレーム再生成フラグ
  bool m_isFrameRemake;    //!< フレーム再生成中か
};


GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

#endif  // FIELD_MENU_SUBWINDOW_PROC_H_INCLUDED
