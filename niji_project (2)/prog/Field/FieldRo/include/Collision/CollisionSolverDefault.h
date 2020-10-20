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
#include "Field/FieldRo/include/Collision/BaseSolver.h"

class SphereShape;
class MeshShape;
class CylinderShape;
class BoxShape;
class ObbShape;
class LineShape;
class TriangleShape;
class StaticActor;
class DynamicActor;

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

  //----------------------------------------------------------------------------
  /**
  *  @brief  引数のツリーに登録されているメッシュ形状を軽量化し保持する
  *
  *  @param  pActorRoot    軽量化対象のツリー
  *  @param  pos           ポリゴンを収集する中心値
  *  @param  collectionR   ポリゴンを収集する範囲
  */
  //-----------------------------------------------------------------------------
  virtual void MeshCooking(gfl2::renderingengine::scenegraph::instance::InstanceNode* pActorRoot,gfl2::math::Vector3 pos,f32 collectionR);

private:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  引数のツリーに登録されているメッシュ形状を軽量化し保持する(サブ)
  *
  *  @param  pStaticActor         アクター
  *  @param  pStaticActorMesh     アクターが持つメッシュ
  *  @param  pos                  ポリゴンを収集する中心値
  *  @param  collectionR          ポリゴンを収集する範囲
  */
  //-----------------------------------------------------------------------------
  void MeshCookingSub(StaticActor* pActor,MeshShape* pActorMesh,gfl2::math::Vector3 pos,f32 collectionR);

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
  int TestSphereMesh( DynamicActor*        pDynamicActor,
                      SphereShape*         pDynamicActorSphere,
                      StaticActor*         pStaticActor,
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
  *  @brief  Spehere Vs Triangle (BroadPhace)
  *
  *  @param  r          スフィアの半径
  *  @param  pos        スフィアの中心位置
  *  @param  a          トライアングルの座標1
  *  @param  b          トライアングルの座標2
  *  @param  c          トライアングルの座標3
  *  @return 1なら衝突している、0なら衝突していない
  */
  //-----------------------------------------------------------------------------
  b32 TestSphereTriangleBroadPhase(const f32                  r,
                                   const gfl2::math::Vector3& pos,
                                   const gfl2::math::Vector3& a,
                                   const gfl2::math::Vector3& b,
                                   const gfl2::math::Vector3& c);

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
  *  @brief  DynamicSphere Vs StaticObb
  *
  *  @param  pDynamicActor         ダイナミックアクター
  *  @param  pDynamicActorSphere   ダイナミックアクターが持つスフィア
  *  @param  pStaticActor          スタティックアクター
  *  @param  pStaticActorObb       スタティックアクターが持つObb
  *  @param  pAdjustVec            ダイナミックアクターへの補正量
  *  @return 1なら衝突している、0なら衝突していない
  */
  //-----------------------------------------------------------------------------
  int TestSphereObb       ( BaseActor*           pDynamicActor,
                            SphereShape*         pDynamicActorSphere,
                            BaseActor*           pStaticActor,
                            ObbShape*            pStaticActorObb,
                            gfl2::math::Vector3* pAdjustVec);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  DynamicSphere Vs StaticTriangle
  *
  *  @param  pDynamicActor         ダイナミックアクター
  *  @param  pDynamicActorSphere   ダイナミックアクターが持つスフィア
  *  @param  pStaticActor          スタティックアクター
  *  @param  pStaticActorObb       スタティックアクターが持つトライアングル
  *  @param  pAdjustVec            ダイナミックアクターへの補正量
  *  @return 1なら衝突している、0なら衝突していない
  */
  //-----------------------------------------------------------------------------
  int TestSphereTriangle  ( DynamicActor*        pDynamicActor,
                            SphereShape*         pDynamicActorSphere,
                            StaticActor*         pStaticActor,
                            TriangleShape*       pStaticActorTriangle,
                            gfl2::math::Vector3* pAdjustVec);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  DynamicCylinder Vs StaticTriangle
  *
  *  @param  pDynamicActor         ダイナミックアクター
  *  @param  pDynamicActorCylinder ダイナミックアクターが持つシリンダー
  *  @param  pStaticActor          スタティックアクター
  *  @param  pStaticActorLine      スタティックアクターが持つトライアングル
  *  @param  pAdjustVec            ダイナミックアクターへの補正量
  *  @return 1なら衝突している、0なら衝突していない
  */
  //-----------------------------------------------------------------------------
  int TestCylinderTriangle ( BaseActor*           pDynamicActor,
                             CylinderShape*       pDynamicActorCylinder,
                             BaseActor*           pStaticActor,
                             TriangleShape*       pStaticActorTriangle,
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
  gfl2::math::Vector3 ClosestPtPointSegment(const gfl2::math::Vector3& p,
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

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定点からのObbの最近接点をとる
  *
  *  @param  p          指定点
  *  @param  rectC      長方形の中心点
  *  @param  rectU0     長方形のローカルなX軸を表す単位ベクトル
  *  @param  rectU1     長方形のローカルなY軸を表す単位ベクトル
  *  @param  rectU2     長方形のローカルなZ軸を表す単位ベクトル
  *  @param  rectE0     X軸にそった長方形の長さの半分
  *  @param  rectE1     Y軸にそった長方形の長さの半分
  *  @param  rectE2     Y軸にそった長方形の長さの半分
  *  @return 最近接点
  */
  //-----------------------------------------------------------------------------
  gfl2::math::Vector3 ClosestPtPointObb( const gfl2::math::Vector3& p,
                                         const gfl2::math::Vector3& rectC,
                                         const gfl2::math::Vector3& rectU0,
                                         const gfl2::math::Vector3& rectU1,
                                         const gfl2::math::Vector3& rectU2,
                                         const f32                  rectE0,
                                         const f32                  rectE1,
                                         const f32                  rectE2);

};
