//=============================================================================
/**
 * @file    FieldStaffList.h
 * @brief   momiji追加分のスタッフロール: フィールド呼び出し処理
 * @author  Hiroyuki Nakamura
 * @date    2017.03.06
 */
//=============================================================================
#if !defined( FIELD_STAFFLIST_H_INCLUDED )
#define FIELD_STAFFLIST_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// module
#include "App/StaffList/include/StaffListDef.h"
// momiji
#include "AppLib/include/Util/app_util_Heap.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(Field)
class Fieldmap;
GFL_NAMESPACE_END(Field)

#if PM_DEBUG
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(ui)
class UIView;
GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)
#endif  // PM_DEBUG


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(StaffList)

// 前方宣言
class FieldStaffListView;


//=============================================================================
/**
 * @class FieldStaffList
 * @brief スタッフロール画面フィールド呼び出し
 * @date  2017.03.06
 */
//=============================================================================
class FieldStaffList
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldStaffList);   // コピーコンストラクタ＋代入禁止

private:
  static const u32 MANAGER_HEAP_SIZE = 0x1000;          //!< マネージャヒープサイズ
  static const u32 LOCAL_HEAP_SIZE        = 0x00018000; //!< ローカルヒープサイズ
  static const u32 LOCAL_DEVICE_HEAP_SIZE = 0x00088000; //!< ローカルデバイスヒープサイズ


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param   fieldmap  フィールドマップ
   */
  //-----------------------------------------------------------------------------
  FieldStaffList( Field::Fieldmap * fieldmap );

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~FieldStaffList();

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
   * @func    CreateHeap
   * @brief   ヒープ生成
   * @date    2017.03.06
   */
  //-----------------------------------------------------------------------------
  void CreateHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteHeap
   * @brief   ヒープ削除
   * @date    2017.03.06
   */
  //-----------------------------------------------------------------------------
  void DeleteHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateView
   * @brief   ビュー生成
   * @date    2017.03.06
   *
   * @param   fieldmap  フィールドマップ
   */
  //-----------------------------------------------------------------------------
  void CreateView( Field::Fieldmap * fieldmap );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteView
   * @brief   ビュー削除
   * @date    2017.03.06
   */
  //-----------------------------------------------------------------------------
  void DeleteView(void);


private:
  gfl2::heap::HeapBase * m_pManagerHeap;  //!< マネージャ用ヒープ
  app::util::Heap m_heap;   //!< ローカルヒープ

  FieldStaffListView * m_pView;   //!< ビュー


#if PM_DEBUG
public:
  FieldStaffList( app::ui::UIView * view, app::util::AppRenderingManager * man );
  void DebugSetup(void);
private:
  void DebugCreateHeap(void);
  void DebugCreateView( app::ui::UIView * view, app::util::AppRenderingManager * man );
#endif  // PM_DEBUG

};


GFL_NAMESPACE_END(StaffList)
GFL_NAMESPACE_END(App)

#endif  // FIELD_STAFFLIST_H_INCLUDED
