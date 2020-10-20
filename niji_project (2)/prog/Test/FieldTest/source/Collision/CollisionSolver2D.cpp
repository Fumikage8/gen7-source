//=================================================================================================
/**
 *  @file   CollisionSolver2D.h
 *  @brief  デフォルト2Dソルバー
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include <Collision/include/gfl2_CollisionModel.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CollisionMeshResourceNode.h>

#include "../../include/Collision/BaseShape.h"
#include "../../include/Collision/CollisionSolver2D.h"
#include "../../include/Collision/StaticActor.h"
#include "../../include/Collision/DynamicActor.h"
#include "../../include/movemodel.h"
#include "../../include/field_transform.h"

//----------------------------------------------------------------------------------------
//! @brief ソルバーベース
//----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
CollisionSolver2D::CollisionSolver2D()
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
CollisionSolver2D::~CollisionSolver2D()
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  ツリー間の衝突を解決
*
*  @param  pRootStaticActorRoot  衝突されるツリー
*  @param  pRootStaticActorRoot  衝突するツリー
*/
//-----------------------------------------------------------------------------
void CollisionSolver2D::Solved(gfl2::renderingengine::scenegraph::instance::InstanceNode* pRootStaticActorRoot,
                               gfl2::renderingengine::scenegraph::instance::InstanceNode* pRootDynamicActorRoot)
{
  // 解決
  for( u32 i=0 ; i<pRootStaticActorRoot->GetChildCount() ; i++ )
  {
    StaticActor* pStaticActor = pRootStaticActorRoot->GetChild(i)->SafeCast<StaticActor>();
    if( !pStaticActor )
    {
      continue;
    }

    for( u32 i=0 ; i<pRootDynamicActorRoot->GetChildCount() ; i++ )
    {
      DynamicActor* pDynamicActor = pRootDynamicActorRoot->GetChild(i)->SafeCast<DynamicActor>();
      if( !pDynamicActor )
      {
        continue;
      }

      b32 isHit = false;

      // dynamic sphere2D vs static sphere2D
      if( pDynamicActor->getShapeType() == BaseActor::SPHERE_2D 
       && pStaticActor->getShapeType()  == BaseActor::SPHERE_2D )
      {
        isHit = TestSphereSphere( pDynamicActor,
                                  static_cast<SphereShape2D*>(pDynamicActor->getShape()),
                                  pStaticActor,
                                  static_cast<SphereShape2D*>(pStaticActor->getShape()) );
      }
      // dynamic sphere2D vs static Box2D
      else if( pDynamicActor->getShapeType() == BaseActor::SPHERE_2D 
            && pStaticActor->getShapeType()  == BaseActor::BOX_2D )
      {
        isHit = TestSphereBox( pDynamicActor,
                               static_cast<SphereShape2D*>(pDynamicActor->getShape()),
                               pStaticActor,
                               static_cast<BoxShape2D*>(pStaticActor->getShape()) );
      }

      // 衝突している
      if( isHit )
      {
        BaseActor::CollisionResult* pDynamicActorResult = pDynamicActor->getCollisionResult();
        BaseActor::CollisionResult* pSaticActorResult   =  pStaticActor->getCollisionResult();

        // 当たった相手を取得
        pDynamicActorResult->pTargetActor = pStaticActor;
        pSaticActorResult->pTargetActor   = pDynamicActor;

        // アクターの座標更新
        gfl2::math::Vector3 newDynamicActorPos( pDynamicActorResult->adjustVec + pDynamicActor->getPosition() );
        gfl2::math::Vector3 newStaticActorPos (   pSaticActorResult->adjustVec +  pStaticActor->getPosition() );
        pDynamicActor->setPosition( newDynamicActorPos );
        pStaticActor->setPosition ( newStaticActorPos  );
      }
    }
  }
}

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
int CollisionSolver2D::TestSphereSphere( BaseActor*       pDynamicActor,
                                         SphereShape2D*   pDynamicActorSphere,
                                         BaseActor*       pStaticActor,
                                         SphereShape2D*   pStaticActorSphere )
{
  // DynamicActorの座標を取得
  gfl2::math::Vector3 dynamicActorPos( pDynamicActor->getPosition() );

  // StaticActorの座標を取得
  gfl2::math::Vector3 staticActorPos( pStaticActor->getPosition() );

  // 二つのスフィア間の距離を取得
  gfl2::math::Vector3 worldDynamicActorSphere( dynamicActorPos + pDynamicActorSphere->m_pos ); worldDynamicActorSphere.y = 0.0f;
  gfl2::math::Vector3 worldStaticActorSphere ( staticActorPos  +  pStaticActorSphere->m_pos ); worldStaticActorSphere.y  = 0.0f;
  gfl2::math::Vector3 vecFromStaticActorToDynamicActor = worldDynamicActorSphere - worldStaticActorSphere;
  f32                 lenFromStaticActorToDynamicActor = vecFromStaticActorToDynamicActor.Length();

  // 衝突判定
  if( lenFromStaticActorToDynamicActor < pDynamicActorSphere->m_r + pStaticActorSphere->m_r )
  {
    BaseActor::CollisionResult* pDynamicActorResult = pDynamicActor->getCollisionResult();
    BaseActor::CollisionResult* pSaticActorResult   =  pStaticActor->getCollisionResult();

    // 押し返しベクトルの作成
    f32 osikaeshi = (pDynamicActorSphere->m_r + pStaticActorSphere->m_r) - lenFromStaticActorToDynamicActor;
    gfl2::math::Vector3 normalVecFromStaticActorToDynamicActor = vecFromStaticActorToDynamicActor.Normalize();
    pDynamicActorResult->adjustVec = normalVecFromStaticActorToDynamicActor * (osikaeshi + MARJIN);
    pSaticActorResult->adjustVec   = gfl2::math::Vector3(0.0f,0.0f,0.0f);

    // 最近接点を取得
    gfl2::math::Vector3 normalVecFromDynamicActorToStaticActor = normalVecFromStaticActorToDynamicActor * -1.0f;
    pDynamicActorResult->closestPt = normalVecFromDynamicActorToStaticActor * pDynamicActorSphere->m_r;
    pSaticActorResult->closestPt   = pDynamicActorResult->closestPt;

    return true;
  }
  return false;
}

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
int CollisionSolver2D::TestSphereBox( BaseActor*          pDynamicActor,
                                      SphereShape2D*      pDynamicActorSphere,
                                      BaseActor*          pStaticActor,
                                      BoxShape2D*         pStaticActorBox )
{
  // DynamicActorの座標を取得
  gfl2::math::Vector3 dynamicActorPos( pDynamicActor->getPosition() );

  // StaticActorの座標を取得
  gfl2::math::Vector3 staticActorPos( pStaticActor->getPosition() );

  // 内外判定
  gfl2::math::Vector3 worldDynamicActorSphere( dynamicActorPos + pDynamicActorSphere->m_pos ); worldDynamicActorSphere.y = 0.0f;
  gfl2::math::Vector3 worldStaticActorBox    ( staticActorPos  +     pStaticActorBox->m_pos );     worldStaticActorBox.y = 0.0f;
  f32 dotX = pStaticActorBox->m_u0.Dot( worldStaticActorBox - worldDynamicActorSphere );
  f32 dotY = pStaticActorBox->m_u1.Dot( worldStaticActorBox - worldDynamicActorSphere );

  // 衝突判定
  if( gfl2::math::Abs(dotX) > pStaticActorBox->m_e0 + pDynamicActorSphere->m_r 
   || gfl2::math::Abs(dotY) > pStaticActorBox->m_e1 + pDynamicActorSphere->m_r )
  {
    return false;
  }

  BaseActor::CollisionResult* pDynamicActorResult = pDynamicActor->getCollisionResult();
  BaseActor::CollisionResult* pSaticActorResult   =  pStaticActor->getCollisionResult();

  // X軸に直行する辺に近いのか、Y軸に直行する辺に近いのか調査
  f32 defX = (pStaticActorBox->m_e0 + pDynamicActorSphere->m_r) - gfl2::math::Abs(dotX);
  f32 defY = (pStaticActorBox->m_e1 + pDynamicActorSphere->m_r) - gfl2::math::Abs(dotY);

  // X軸に直行する辺に近い
  if( defX < defY )
  {
    // X軸に直行する辺の左辺に当たっている
    if( dotX>0.0f )
    {
      pDynamicActorResult->adjustVec = gfl2::math::Vector3( -1.0f*defX*pStaticActorBox->m_u0.x,
                                                            -1.0f*defX*pStaticActorBox->m_u0.y,
                                                            -1.0f*defX*pStaticActorBox->m_u0.z );
    }
    // X軸に直行する辺の右辺に当たっている
    else
    {
      pDynamicActorResult->adjustVec = gfl2::math::Vector3( defX*pStaticActorBox->m_u0.x,
                                                            defX*pStaticActorBox->m_u0.y,
                                                            defX*pStaticActorBox->m_u0.z );
    }
  }
  // Y軸に直行する辺に近い
  else
  {
    // Y軸に直行する辺の上辺に当たっている
    if( dotY>0.0f )
    {
      pDynamicActorResult->adjustVec =  gfl2::math::Vector3( -1.0f*defY*pStaticActorBox->m_u1.x,
                                                             -1.0f*defY*pStaticActorBox->m_u1.y,
                                                             -1.0f*defY*pStaticActorBox->m_u1.z );
    }
    // Y軸に直行する辺の下辺に当たっている
    else
    {
      pDynamicActorResult->adjustVec =  gfl2::math::Vector3( defY*pStaticActorBox->m_u1.x,
                                                             defY*pStaticActorBox->m_u1.y,
                                                             defY*pStaticActorBox->m_u1.z );
    }
  }
  pSaticActorResult->adjustVec = gfl2::math::Vector3(0.0f,0.0f,0.0f);

  // 最近接点を取得 todo
  pDynamicActorResult->closestPt = gfl2::math::Vector3(0.0f,0.0f,0.0f);
  pSaticActorResult->closestPt   = pDynamicActorResult->closestPt;

  return true;
}

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
gfl2::math::Vector3 CollisionSolver2D::ClosestPtPointRect( const gfl2::math::Vector3& p,
                                                           const gfl2::math::Vector3& rectC,
                                                           const gfl2::math::Vector3& rectU0,
                                                           const gfl2::math::Vector3& rectU1,
                                                           const f32                  rectE0,
                                                           const f32                  rectE1)
{
  gfl2::math::Vector3 d = p - rectC;

  // ボックスの中心における結果から開始、そこから段階的に進める
  gfl2::math::Vector3 closestPt( rectC );

  // 各長方形の軸に対して
  for( u32 i=0 ; i<2 ; i++ )
  {
    gfl2::math::Vector3 rectU( i==0 ? rectU0 : rectU1 );
    f32                 rectE( i==0 ? rectE0 : rectE1 );

    // ...dをその軸に射影して
    // 長方形の中心からdの軸に沿った距離を得る
    f32 dist = d.Dot(rectU);

    // 長方形の範囲よりも距離が大きい場合、長方形までクランプ
    if( dist >  rectE )
    {
      dist =  rectE;
    }
    if( dist < -rectE )
    {
      dist = -rectE;
    }

    // ワールド座標を得るためにその距離だけ軸に沿って進める
    closestPt += gfl2::math::Vector3( dist*rectU.x , dist*rectU.y, dist*rectU.z);
  }

  return closestPt;
}