/**
* @brief  フィールドデバッグメニュー
* @file   FieldDebugMenu.h
* @author ikuta_junya
* @data   2015.03.08
*/

#if !defined( __FIELDDEBUGMENU_H_INCLUDED__ )
#define __FIELDDEBUGMENU_H_INCLUDED__ 
#pragma once


#if PM_DEBUG

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

// 前方宣言
GFL_NAMESPACE_BEGIN( GameSys );
class GameManager;
GFL_NAMESPACE_END( GameSys );

GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( debug );
class DebugWinGroup;
GFL_NAMESPACE_END( debug );
GFL_NAMESPACE_END( gfl2 );


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( debug );

/**
* @brief フィールドデバッグメニュー
*/
class FieldDebugMenu
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldDebugMenu );

public:

  /**
  * @brief コンストラクタ
  * フィールドデバッグメニューの構築を実行します
  *
  * @param  pHeap         メニュー構築に使用するヒープ
  * @param  pGameManager  ゲーム管理
  *
  * @return 無し
  */ 
  FieldDebugMenu( gfl2::heap::HeapBase* pHeap, GameSys::GameManager* pGameManager );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~FieldDebugMenu( void );


private:

  gfl2::heap::HeapBase* m_pHeap;          // メニュー構築に使用するヒープ
  GameSys::GameManager* m_pGameManager;   // ゲーム管理


  gfl2::debug::DebugWinGroup* m_pFieldRootGroup;  // フィールド関連メニューのルート


};  // class FieldDebugMenu

GFL_NAMESPACE_END( debug );
GFL_NAMESPACE_END( Field );

#endif // if PM_DEBUG
#endif // __FIELDDEBUGMENU_H_INCLUDED__