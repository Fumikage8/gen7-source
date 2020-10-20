// ============================================================================
/*
 * @file BattleTreeResourceID.h
 * @brief バトルツリーアプリで使用するリソースのIDです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_TREE_RESOURCEID_H_INCLUDE )
#define BATTLE_TREE_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)


  enum BattleTreeResourceID
  {
    BATTLE_TREE_RESOURCE_ID_LAYOUT,
    BATTLE_TREE_RESOURCE_ID_MESSAGE,
    BATTLE_TREE_RESOURCE_MAX,
  };

GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)

#endif // BattleTreeRESOURCEID_H_INCLUDE
