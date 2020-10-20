/**
* @brief  フィールドデバッグメニュー、地形関連
* @file   FieldDebugMenuTerrain.h
* @author ikuta_junya
* @data   2015.03.08
*/

#if !defined( __FIELDDEBUGMENUTERRAIN_H_INCLUDED__ )
#define __FIELDDEBUGMENUTERRAIN_H_INCLUDED__ 
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
GFL_NAMESPACE_BEGIN( Menu );

/**
* @brief 地形メニュー
*/
class Terrain
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( Terrain );

public:

  /**
  * @brief コンストラクタ
  * フィールドデバッグメニューの構築を実行します
  *
  * @param  pHeap         メニュー構築に使用するヒープ
  * @param  pParentGroup  Terrainを追加するグループ
  *
  * @return 無し
  */ 
  Terrain( gfl2::heap::HeapBase* pHeap, gfl2::debug::DebugWinGroup* pParentGroup, gfl2::str::MsgData* pMsgData );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~Terrain( void );

  /**
  * @brief   地形項目のTOPグループを取得
  */
  gfl2::debug::DebugWinGroup* GetTopGroup( void ){ return m_pTerrainGrop; }

  /**
  * @brief   地形項目用のヒープを取得
  */
  gfl2::heap::HeapBase* GetHeap( void ) { return m_pHeap; }

  /**
  * @brief   メッセージデータを取得
  */
  gfl2::str::MsgData* GetMessageData( void ) { return m_pMsgData; }

private:

  gfl2::heap::HeapBase*       m_pHeap;            // メニュー構築に使用するヒープ
  gfl2::debug::DebugWinGroup* m_pParentGroup;     // 親
  gfl2::debug::DebugWinGroup* m_pTerrainGrop;
  gfl2::str::MsgData*         m_pMsgData;         // メッセージデータ

private:


};  // class FieldDebugMenu

GFL_NAMESPACE_END( Menu );
GFL_NAMESPACE_END( Debug );
GFL_NAMESPACE_END( Field );

#endif // if PM_DEBUG
#endif // __FIELDDEBUGMENUTERRAIN_H_INCLUDED__