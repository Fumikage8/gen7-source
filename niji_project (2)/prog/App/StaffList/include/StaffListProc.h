//=============================================================================
/**
 * @file    StaffListProc.h
 * @brief   momiji追加分のスタッフロールProc処理
 * @author  Hiroyuki Nakamura
 * @date    2017.03.02
 */
//=============================================================================
#if !defined( STAFFLIST_PROC_H_INCLUDED )
#define STAFFLIST_PROC_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// momiji
#include "GameSys/include/GameProc.h"
#include "AppLib/include/Util/app_util_Heap.h"
#include "AppLib/include/Ui/UIView.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(StaffList)

// 前方宣言
class FieldStaffList;


//=============================================================================
/**
 * @class StaffListProc
 * @brief スタッフロール画面Procクラス
 * @date  2017.03.02
 */
//=============================================================================
class StaffListProc
  : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(StaffListProc);  // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  StaffListProc(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~StaffListProc();

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFunc
   * @brief   Proc初期化処理
   * @date    2017.03.02
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
   * @date    2017.03.02
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
   * @date    2017.03.02
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
   * @date    2017.03.02
   *
   * @param   pManager    プロセス管理マネージャー
   */
  //-----------------------------------------------------------------------------
  virtual void PreDrawFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   Proc描画処理
   * @date    2017.03.02
   *
   * @param   pManager    プロセス管理マネージャー
   * @param   displayNo   描画対象画面
   */
  //-----------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::proc::Manager * pManager, gfl2::gfx::CtrDisplayNo displayNo );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    CreateHeap
   * @brief   ヒープ生成
   * @date    2017.03.02
   */
  //-----------------------------------------------------------------------------
  void CreateHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteHeap
   * @brief   ヒープ削除
   * @date    2017.03.02
   */
  //-----------------------------------------------------------------------------
  void DeleteHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateRenderManager
   * @brief   描画マネージャ生成
   * @date    2017.03.02
   */
  //-----------------------------------------------------------------------------
  void CreateRenderManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteRenderManager
   * @brief   描画マネージャ削除
   * @date    2017.03.02
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteRenderManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateView
   * @brief   親ビュー生成
   * @date    2017.03.07
   */
  //-----------------------------------------------------------------------------
  void CreateView(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteView
   * @brief   親ビュー削除
   * @date    2017.03.07
   */
  //-----------------------------------------------------------------------------
  void DeleteView(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateFieldStaffList
   * @brief   フィールドスタッフリスト生成
   * @date    2017.03.07
   */
  //-----------------------------------------------------------------------------
  void CreateFieldStaffList(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteFieldStaffList
   * @brief   フィールドスタッフリスト削除
   * @date    2017.03.07
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteFieldStaffList(void);

  //=============================================================================
  /**
   * @class DummyView
   * @brief 親ビュークラス（ダミー）
   * @date  2017.03.07
   */
  //=============================================================================
  class DummyView
    : public app::ui::UIView
  {
  public:
    DummyView( app::util::Heap * heap, gfl2::heap::NwAllocator * sys, gfl2::heap::NwAllocator * dev )
      : app::ui::UIView( heap, sys, dev )
    {
    }
    virtual ~DummyView(){}
    virtual void Update(void)
    {
      app::ui::UIView::Update();
    }
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo ){}
  };


private:
  DummyView * m_pView;  //!< 親ビュー
  gfl2::heap::HeapBase * m_pManagerHeap;  //!< マネージャ用ヒープ
  app::util::Heap m_heap;   //!< ローカルヒープ
  app::util::AppRenderingManager * m_pRenderingManager;   //!< 描画マネージャ
  FieldStaffList * m_pFieldStaffList;   //!< フィールドスタッフリスト
};


GFL_NAMESPACE_END(StaffList)
GFL_NAMESPACE_END(App)

#endif  // STAFFLIST_PROC_H_INCLUDED
