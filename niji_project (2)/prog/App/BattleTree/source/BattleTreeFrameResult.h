// ============================================================================
/*
 * @file BattleTreeFrameResult.h
 * @brief バトルツリーアプリのリザルトコードを定義します。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_TREE_FRAMERESULT_H_INCLUDE )
#define BATTLE_TREE_FRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)


  enum SampleResultCode
  {
    FRAME_RESULT_SELECT = 0,
    FRAME_RESULT_BACK,
  };


GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)

#endif // BATTLE_TREE_FRAMERESULT_H_INCLUDE
