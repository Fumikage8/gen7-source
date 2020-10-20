﻿//=================================================================================================
/**
 *  @file   CollisionSceneNone.cpp
 *  @brief  空コリジョンシーン
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 *  @note   Sceneをコンテナとして使いたいときに利用
 */
//=================================================================================================
#include "Field/FieldRo/include/Collision/CollisionSceneNone.h"

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
CollisionSceneNone::CollisionSceneNone()
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
CollisionSceneNone::~CollisionSceneNone()
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
BaseSolver* CollisionSceneNone::CreateSolver(void)
{
  return 0;
}