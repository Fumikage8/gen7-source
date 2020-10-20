#pragma once
//=================================================================================================
/**
 *  @file   CollisionSolver2D.h
 *  @brief  デフォルト2Dソルバー
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include <math/include/gfl2_Vector3.h>
#include "../../include/Collision/BaseSolver.h"

class SphereShape2D;
class BoxShape2D;
class MeshShape;

//----------------------------------------------------------------------------------------
//! @brief ソルバーベース
//----------------------------------------------------------------------------------------
class CollisionSolver2D : public BaseSolver
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  CollisionSolver2D();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~CollisionSolver2D();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ツリー間の衝突を解決
  *
  *  @param  pRootStaticActor3DRoot  衝突されるツリー
  *  @param  pRootStaticActor3DRoot  衝突するツリー
  */
  //-----------------------------------------------------------------------------
  virtual void Solved(gfl2::renderingengine::scenegraph::instance::InstanceNode* pRootStaticActorRoot,
                      gfl2::renderingengine::scenegraph::instance::InstanceNode* pRootDynamicActorRoot);

private:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  DynamicSphere Vs StaticMesh
  *
  *  @param  pDynamicActor        ダイナミックアクター
  *  @param  pDynamicActorSphere  ダイナミックアクターが持つスフィア
  *  @param  pStaticActor         スタティックアクター
  *  @param  pStaticActorSphere   スタティックアクターが持つスフィア
  *  @return 1なら衝突している、0なら衝突していない
  */
  //-----------------------------------------------------------------------------
  int TestSphereSphere    ( BaseActor*       pDynamicActor,
                            SphereShape2D*   pDynamicActorSphere,
                            BaseActor*       pStaticActor,
                            SphereShape2D*   pStaticActorSphere );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  DynamicSphere Vs StaticBox
  *
  *  @param  pDynamicActor        ダイナミックアクター
  *  @param  pDynamicActorSphere  ダイナミックアクターが持つスフィア
  *  @param  pStaticActor         スタティックアクター
  *  @param  pStaticActorSphere   スタティックアクターが持つスフィア
  *  @return 1なら衝突している、0なら衝突していない
  */
  //-----------------------------------------------------------------------------
  int TestSphereBox       ( BaseActor*          pDynamicActor,
                            SphereShape2D*      pDynamicActorSphere,
                            BaseActor*          pStaticActor,
                            BoxShape2D*         pStaticActorBox );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定点からのRectの最近接点をとる
  *
  *  @param  p          指定点
  *  @param  rectC      長方形の中心点
  *  @param  rectU0     長方形のローカルなX軸を表す単位ベクトル
  *  @param  rectU1     長方形のローカルなY軸を表す単位ベクトル
  *  @param  rectE0     X軸にそった長方形の長さの半分
  *  @param  rectE1     Y軸にそった長方形の長さの半分
  *  @return 最近接点
  */
  //-----------------------------------------------------------------------------
  gfl2::math::Vector3 ClosestPtPointRect( const gfl2::math::Vector3& p,
                                          const gfl2::math::Vector3& RectC,
                                          const gfl2::math::Vector3& RectU0,
                                          const gfl2::math::Vector3& RectU1,
                                          const f32                  RectE0,
                                          const f32                  RectE1);

};
