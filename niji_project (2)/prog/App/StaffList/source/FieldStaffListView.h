//=============================================================================
/**
 * @file    FieldStaffListView.h
 * @brief   momiji追加分のスタッフロール: フィールド用ビュー
 * @author  Hiroyuki Nakamura
 * @date    2017.03.06
 */
//=============================================================================
#if !defined( FIELD_STAFFLIST_VIEW_H_INCLUDED )
#define FIELD_STAFFLIST_VIEW_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// module
#include "App/StaffList/include/StaffListDef.h"
// momiji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Util/app_util_Heap.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(StaffList)

// 前方宣言
class StaffListFrame;


//=============================================================================
/**
 * @class FieldStaffList
 * @brief スタッフロール画面フィールド呼び出し
 * @date  2017.03.06
 */
//=============================================================================
class FieldStaffListView
  : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldStaffListView);   // コピーコンストラクタ＋代入禁止

private:
  //! メインシーケンス
  enum MainSeq
  {
    MAINSEQ_INIT = 0,
    MAINSEQ_MAIN,
    MAINSEQ_END,
    MAINSEQ_FINISH,
  };


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param heap                ヒープ
   * @param manager_heap        マネージャ用ヒープ
   * @param rendering_manager   描画マネージャ
   */
  //-----------------------------------------------------------------------------
  FieldStaffListView( app::util::Heap * heap, gfl2::heap::HeapBase * manager_heap, app::util::AppRenderingManager * rendering_manager );

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~FieldStaffListView();

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2017.03.06
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2017.03.06
   *
   * @param   displayNo   描画面
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsModuleFree
   * @brief   クラスを破棄できるか
   * @date    2017.03.06
   *
   * @retval  true  = 可
   * @retval  false = 不可
   */
  //-----------------------------------------------------------------------------
  bool IsModuleFree(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsCreated
   * @brief   生成が完了したか
   * @date    2017.03.06
   *
   * @retval  true  = 完了した
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsCreated(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRequest
   * @brief   動作リスクエスト
   * @date    2017.03.06
   *
   * @param   no    データ番号
   * @param   mode  背景配置モード
   */
  //-----------------------------------------------------------------------------
  void SetRequest( u32 no, BgPutMode mode );

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
   * @func    UpdateView
   * @brief   ビュー更新
   * @date    2017.03.06
   */
  //-----------------------------------------------------------------------------
  void UpdateView(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFunc
   * @brief   初期化処理
   * @date    2017.03.06
   *
   * @retval  true  = 処理中
   * @retval  false = 終了
   */
  //-----------------------------------------------------------------------------
  bool InitFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFunc
   * @brief   終了処理
   * @date    2017.03.06
   *
   * @retval  true  = 処理中
   * @retval  false = 終了
   */
  //-----------------------------------------------------------------------------
  bool EndFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateFunc
   * @brief   アップデート処理
   * @date    2017.03.06
   *
   * @retval  true  = 処理中
   * @retval  false = 終了
   */
  //-----------------------------------------------------------------------------
  bool UpdateFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateFrame
   * @brief   フレーム生成
   * @date    2017.03.06
   */
  //-----------------------------------------------------------------------------
  void CreateFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteFrame
   * @brief   フレーム削除
   * @date    2017.03.06
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteFrame(void);


private:
  gfl2::heap::HeapBase * m_managerHeap;   //!< マネージャ用ヒープ
  app::util::Heap * m_heap;               //!< ローカルヒープ

  app::util::AppRenderingManager * m_renderingManager;   //!< 描画マネージャ
  applib::frame::Manager * m_pFrameManager;              //!< フレームマネージャ
  StaffListFrame * m_pFrame;  //!< フレーム

  MainSeq m_mainSeq;          //!< メインシーケンス


#if PM_DEBUG
public:
  void DebugSetup(void);
#endif  // PM_DEBUG
};


GFL_NAMESPACE_END(StaffList)
GFL_NAMESPACE_END(App)

#endif  // FIELD_STAFFLIST_VIEW_H_INCLUDED
