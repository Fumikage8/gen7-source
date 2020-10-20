/**
* @brief  フィールドデバッグメニュー
* @file   FieldDebugMenuCameraGroup.h
* @author ikuta_junya
* @data   2015.03.08
*/

#if !defined( __FIELDDEBUGMENUCAMERAGROUP_H_INCLUDED__ )
#define __FIELDDEBUGMENUCAMERAGROUP_H_INCLUDED__ 
#pragma once

#if PM_DEBUG

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( debug );
class DebugWinGroup;
GFL_NAMESPACE_END( debug );
GFL_NAMESPACE_END( gfl2 );

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Debug );

/**
* @brief フィールドデバッグメニュー
*/
class DebugMenuCameraGroup
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( DebugMenuCameraGroup );

public:

  /**
  * @brief コンストラクタ
  * フィールドデバッグメニューの構築を実行します
  *
  * @param  pHeap         メニュー構築に使用するヒープ
  * @param  pParentGroup  CameraGroupを追加するグループ
  *
  * @return 無し
  */ 
  DebugMenuCameraGroup( gfl2::heap::HeapBase* pHeap, gfl2::debug::DebugWinGroup* pParentGroup, gfl2::str::MsgData* pMsgData );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~DebugMenuCameraGroup( void );

  /**
  * @brief カメラグループを取得
  *
  * @return 無し
  */
  gfl2::debug::DebugWinGroup* GetTopGroup( void ){ return m_pGrop; }

private:

  gfl2::heap::HeapBase*       m_pHeap;            // メニュー構築に使用するヒープ
  gfl2::debug::DebugWinGroup* m_pParentGroup;     // 親
  gfl2::debug::DebugWinGroup* m_pGrop;            // 自分
  gfl2::str::MsgData*         m_pMsgData;
private:


};  // class FieldDebugMenu

GFL_NAMESPACE_END( Debug );
GFL_NAMESPACE_END( Field );

#endif // if PM_DEBUG
#endif // __FIELDDEBUGMENUCAMERAGROUP_H_INCLUDED__