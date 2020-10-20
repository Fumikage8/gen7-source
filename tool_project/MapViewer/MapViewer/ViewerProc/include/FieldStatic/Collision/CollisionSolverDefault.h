#pragma once
//=================================================================================================
/**
 *  @file   CollisionSolverDefault.h
 *  @brief  デフォルトソルバー
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include <math/include/gfl2_Vector3.h>
#include "ViewerProc/include/FieldStatic/Collision/BaseSolver.h"

class SphereShape;
class MeshShape;
class CylinderShape;
class BoxShape;
class LineShape;

//----------------------------------------------------------------------------------------
//! @brief ソルバーベース
//----------------------------------------------------------------------------------------
class CollisionSolverDefault : public BaseSolver
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  CollisionSolverDefault();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~CollisionSolverDefault();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ツリー間の衝突を解決
  *
  *  @param  pRootStaticActorRoot  衝突されるツリー
  *  @param  pRootStaticActorRoot  衝突するツリー
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
  *  @param  pAdjustVec           ダイナミックアクターへの補正量
  *  @return 1なら衝突している、0なら衝突していない
  */
  //-----------------------------------------------------------------------------
  int TestSphereMesh( BaseActor*           pDynamicActor,
                      SphereShape*         pDynamicActorSphere,
                      BaseActor*           pStaticActor,
                      MeshShape*           pStaticActorMesh,
                      gfl2::math::Vector3* pAdjustVec);

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
  *  @brief  DynamicCylinder Vs StaticCylinder
  *
  *  @param  pDynamicActor         ダイナミックアクター
  *  @param  pDynamicActorCylinder ダイナミックアクターが持つシリンダー
  *  @param  pStaticActor          スタティックアクター
  *  @param  pStaticActorCylinder  スタティックアクターが持つシリンダー
  *  @param  pAdjustVec            ダイナミックアクターへの補正量
  *  @return 1なら衝突している、0なら衝突していない
  */
  //-----------------------------------------------------------------------------
  int TestCylinderCylinder( BaseActor*           pDynamicActor,
                            CylinderShape*       pDynamicActorCylinder,
                            BaseActor*           pStaticActor,
                            CylinderShape*       pStaticActorCylinder,
                            gfl2::math::Vector3* pAdjustVec);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  DynamicCylinder Vs StaticBox
  *
  *  @param  pDynamicActor         ダイナミックアクター
  *  @param  pDynamicActorCylinder ダイナミックアクターが持つシリンダー
  *  @param  pStaticActor          スタティックアクター
  *  @param  pStaticActorBox       スタティックアクターが持つボックス
  *  @param  pAdjustVec            ダイナミックアクターへの補正量
  *  @return 1なら衝突している、0なら衝突していない
  */
  //-----------------------------------------------------------------------------
  int TestCylinderBox     ( BaseActor*           pDynamicActor,
                            CylinderShape*       pDynamicActorCylinder,
                            BaseActor*           pStaticActor,
                            BoxShape*            pStaticActorBox,
                            gfl2::math::Vector3* pAdjustVec);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  DynamicCylinder Vs StaticLine
  *
  *  @param  pDynamicActor         ダイナミックアクター
  *  @param  pDynamicActorCylinder ダイナミックアクターが持つシリンダー
  *  @param  pStaticActor          スタティックアクター
  *  @param  pStaticActorLine      スタティックアクターが持つライン
  *  @param  pAdjustVec            ダイナミックアクターへの補正量
  *  @return 1なら衝突している、0なら衝突していない
  */
  //-----------------------------------------------------------------------------
  int TestCylinderLine    ( BaseActor*           pDynamicActor,
                            CylinderShape*       pDynamicActorCylinder,
                            BaseActor*           pStaticActor,
                            LineShape*           pStaticActorLine,
                            gfl2::math::Vector3* pAdjustVec);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定点からのLineの最近接点をとる
  *
  *  @param  p          指定点
  *  @param  a          ラインの先端
  *  @param  b          ラインの終端
  *  @return 最近接点
  */
  //-----------------------------------------------------------------------------
  gfl2::math::Vector3 CollisionSolverDefault::ClosestPtPointSegment(const gfl2::math::Vector3& p,
                                                                    const gfl2::math::Vector3& a,
                                                                    const gfl2::math::Vector3& b );

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
