#pragma once
//=================================================================================================
/**
 *  @file   CollisionSolver3D.h
 *  @brief  デフォルト3Dソルバー
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include <math/include/gfl2_Vector3.h>
#include "../../include/Collision/BaseSolver.h"

class SphereShape3D;
class MeshShape;

//----------------------------------------------------------------------------------------
//! @brief ソルバーベース
//----------------------------------------------------------------------------------------
class CollisionSolver3D : public BaseSolver
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  CollisionSolver3D();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~CollisionSolver3D();

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
  *  @param  pStaticActorMesh     スタティックアクターが持つメッシュ
  *  @param  pResult              最近接点等の衝突結果
  *  @return 1なら衝突している、0なら衝突していない
  */
  //-----------------------------------------------------------------------------
  int TestSphereMesh( BaseActor*       pDynamicActor,
                      SphereShape3D*   pDynamicActorSphere,
                      BaseActor*       pStaticActor,
                      MeshShape*       pStaticActorMesh );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  Spehere Vs Triangle
  *
  *  @param  r          スフィアの半径
  *  @param  pos        スフィアの中心位置
  *  @param  a          トライアングルの座標1
  *  @param  b          トライアングルの座標2
  *  @param  c          トライアングルの座標3
  *  @param  pClosestPt 最近接点
  *  @return 1なら衝突している、0なら衝突していない
  */
  //-----------------------------------------------------------------------------
  int TestSphereTriangle( const f32                  r,
                          const gfl2::math::Vector3& pos,
                          const gfl2::math::Vector3& a,
                          const gfl2::math::Vector3& b,
                          const gfl2::math::Vector3& c,
                          gfl2::math::Vector3*       pClosestPt );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定点からのTriangleの最近接点をとる
  *
  *  @param  p          指定点
  *  @param  a          トライアングルの座標1
  *  @param  b          トライアングルの座標2
  *  @param  c          トライアングルの座標3
  *  @return 最近接点
  */
  //-----------------------------------------------------------------------------
  gfl2::math::Vector3 ClosestPtPointTriangle( const gfl2::math::Vector3& p,
                                              const gfl2::math::Vector3& a,
                                              const gfl2::math::Vector3& b,
                                              const gfl2::math::Vector3& c);


};
