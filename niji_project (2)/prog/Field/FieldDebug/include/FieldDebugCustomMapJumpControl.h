//======================================================================
/**
* @file FieldDebugCustomMapJumpControl.h
* @author ikuta_junya
* @brief ユーザーカスタマイズ可能なマップジャンプ機能管理
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if PM_DEBUG

#if !defined __FIELDDEBUGCUSTOMMAPJUMP_H_INCLUDED__
#define __FIELDDEBUGCUSTOMMAPJUMP_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( debug );
class DebugWinGroup;
class DebugWinItem;
GFL_NAMESPACE_END( debug );
GFL_NAMESPACE_END( gfl2 );

GFL_NAMESPACE_BEGIN(Field);
GFL_NAMESPACE_BEGIN(Debug);

/**
* @brief  ユーザーカスタマイズ可能なマップジャンプ機能管理
* @note   機能1. 項目をデバッグメニューに追加する
* @note   機能2. 項目生成破棄時に情報をロード・破棄する
*/
class CustomMapJumpControl
{
  GFL_FORBID_COPY_AND_ASSIGN(CustomMapJumpControl);

public:

  CustomMapJumpControl();
  virtual ~CustomMapJumpControl();

  /**
  * @breaf 項目をデバッグメニューに生成する
  * @param pParent 追加先
  * @param pHeap      デバッグメニューに追加する際に使用するヒープ
  * @param pHeapCore  デバッグメニューの項目が開かれた際に使用するヒープ
  */
  void CreateItem( gfl2::debug::DebugWinGroup* pParent, gfl2::heap::HeapBase* pHeap,  gfl2::heap::HeapBase* pHeapCore );

private:

  /**
  * @breaf 実態を生成する
  */
  void CreateCore( void );
  
  /**
  * @breaf 実態を破棄する
  */
  void DestroyCore( void );

  /**
  * @breaf カスタムマップジャンプグループ、生成破棄コールバック関数
  * @param userWork 対象グループを管理するCustomMapJumpControlインスタンス
  */
  static void CreateGroup( void *userWork, gfl2::debug::DebugWinItem *item );
  static void DestroyGroup( void *userWork, gfl2::debug::DebugWinItem *item );


  // デバッグメニュー関連
  gfl2::debug::DebugWinGroup* m_pParentGroup;     // 親
  gfl2::debug::DebugWinGroup* m_pGrop;            // 自分
  
  class ControlCore;
  gfl2::heap::HeapBase*       m_pHeapCore;        // 実態生成に使用するヒープ
  ControlCore*                m_pCore;            // 実態、デバッグ項目のOpenCloseに合わせて生成・破棄される
};

GFL_NAMESPACE_END(Debug)
  GFL_NAMESPACE_END(Field)

#endif // __FIELDDEBUGCUSTOMMAPJUMP_H_INCLUDED__
#endif // #if PM_DEBUG