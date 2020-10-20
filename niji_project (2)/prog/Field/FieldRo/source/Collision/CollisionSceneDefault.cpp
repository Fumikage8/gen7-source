//=================================================================================================
/**
 *  @file   CollisionSceneDefault.cpp
 *  @brief  デフォルトコリジョンシーン
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include "Field/FieldRo/include/Collision/CollisionSceneDefault.h"
#include "Field/FieldRo/include/Collision/CollisionSolverDefault.h"

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
CollisionSceneDefault::CollisionSceneDefault()
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
CollisionSceneDefault::~CollisionSceneDefault()
{
}

//----------------------------------------------------------------------------
/**
*  @brief   解決者の作成
*
*  @return  solver
*  @note    この関数をオーバーライドすることにより衝突の挙動を変更できます
*/
//-----------------------------------------------------------------------------
BaseSolver* CollisionSceneDefault::CreateSolver(void)
{
  // 狙ってデフォルトnewを使用しています。(親クラスのm_pAllocatorから取られます)
  return GFL_NEW(GetHeap()) CollisionSolverDefault();
}