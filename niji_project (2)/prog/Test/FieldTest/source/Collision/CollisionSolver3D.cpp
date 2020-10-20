//=================================================================================================
/**
 *  @file   CollisionSolver3D.h
 *  @brief  デフォルト3Dソルバー
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include <Collision/include/gfl2_CollisionModel.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CollisionMeshResourceNode.h>

#include "../../include/Collision/BaseShape.h"
#include "../../include/Collision/CollisionSolver3D.h"
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
CollisionSolver3D::CollisionSolver3D()
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
CollisionSolver3D::~CollisionSolver3D()
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
void CollisionSolver3D::Solved(gfl2::renderingengine::scenegraph::instance::InstanceNode* pRootStaticActorRoot,
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

      // dynamic sphere3D vs static mesh3D
      if( pDynamicActor->getShapeType() == BaseActor::SPHERE_3D 
       && pStaticActor->getShapeType()  == BaseActor::MESH_3D  )
      {
        isHit = TestSphereMesh( pDynamicActor,
                                static_cast<SphereShape3D*>(pDynamicActor->getShape()),
                                pStaticActor,
                                static_cast<MeshShape*>  (pStaticActor->getShape()) );
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
*  @param  pStaticActorMesh     スタティックアクターが持つメッシュ
*  @param  pResult              最近接点等の衝突結果
*  @return 1なら衝突している、0なら衝突していない
*/
//-----------------------------------------------------------------------------
int CollisionSolver3D::TestSphereMesh( BaseActor*       pDynamicActor,
                                       SphereShape3D*   pDynamicActorSphere,
                                       BaseActor*       pStaticActor,
                                       MeshShape*       pStaticActorMesh )
{
  // データ不正チェック
  if( !pDynamicActor
   || !pDynamicActorSphere
   || !pStaticActorMesh
   || !pStaticActorMesh
   || !pStaticActorMesh->m_pCollisionModel
   || !pStaticActorMesh->m_pCollisionModel->GetCollisionMeshNode() )
  {
    return 0;
  }

  // コリジョン用ジオメトリの取得
  gfl2::renderingengine::scenegraph::resource::CollisionMeshResourceNode* pColMeshNode = pStaticActorMesh->m_pCollisionModel->GetCollisionMeshNode();

  // 全トライアングルと衝突検出
  gfl2::math::Vector3 tempClosestPt;
  f32                 minDefLength  = -1.0f;
  u32                 triangleCount = pColMeshNode->GetCollisionTriangleCount();
  for (u32 i = 0; i<triangleCount ; ++i)
  {
    // 衝突しているか？
    const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle = pColMeshNode->GetCollisionTriangle(i);
    const gfl2::math::Vector3 trianglePos0( pTriangle->m_Positions[0].x , pTriangle->m_Positions[0].y , pTriangle->m_Positions[0].z );
    const gfl2::math::Vector3 trianglePos1( pTriangle->m_Positions[1].x , pTriangle->m_Positions[1].y , pTriangle->m_Positions[1].z );
    const gfl2::math::Vector3 trianglePos2( pTriangle->m_Positions[2].x , pTriangle->m_Positions[2].y , pTriangle->m_Positions[2].z );

    int hit = TestSphereTriangle( pDynamicActorSphere->m_r,
                                  pDynamicActorSphere->m_pos + pDynamicActor->getPosition(),
                                  gfl2::math::Vector3( trianglePos0 + pStaticActor->getPosition() ),
                                  gfl2::math::Vector3( trianglePos1 + pStaticActor->getPosition() ),
                                  gfl2::math::Vector3( trianglePos2 + pStaticActor->getPosition() ),
                                  &tempClosestPt );

    if( !hit )
    {
      continue;
    }

    // メッシュ全体での最近接点を取る
    gfl2::math::Vector3 def       = tempClosestPt - ( pDynamicActorSphere->m_pos + pDynamicActor->getPosition() );
    f32                 defLength = def.Length();
    if( minDefLength < 0.0f || defLength < minDefLength )
    {
      BaseActor::CollisionResult* pDynamicActorResult = pDynamicActor->getCollisionResult();
      BaseActor::CollisionResult* pSaticActorResult   =  pStaticActor->getCollisionResult();

      // 最近接点の距離を更新
      minDefLength = defLength;

      // 現状でのメッシュ全体の最近接点を取得
      pDynamicActorResult->closestPt = tempClosestPt;
      pSaticActorResult->closestPt   = pDynamicActorResult->closestPt;

      // 押し返しベクトルの取得
      gfl2::math::Vector3 adjustVec    = def.Normalize() * -1.0f;
      f32                 adjustVecLen = pDynamicActorSphere->m_r - defLength;
      pDynamicActorResult->adjustVec   = adjustVec * (adjustVecLen+MARJIN);
      pSaticActorResult->adjustVec     = gfl2::math::Vector3(0.0f,0.0f,0.0f);
    }
  }

  if( minDefLength < 0.0f )
  {
    return 0;
  }
  return 1;
}

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
int CollisionSolver3D::TestSphereTriangle(const f32                  r,
                                          const gfl2::math::Vector3& pos,
                                          const gfl2::math::Vector3& a,
                                          const gfl2::math::Vector3& b,
                                          const gfl2::math::Vector3& c,
                                          gfl2::math::Vector3*       pClosestPt )
{
  // 球の中心に対する最近接点である三角形ABC上にある点Pを見つける
  *pClosestPt = ClosestPtPointTriangle( pos , a,b,c );

  // 球と三角形が交差するのは、球の中心から点Pまでの(平方した)距離が(平方した)球の半径よりも小さい場合
  gfl2::math::Vector3 v = *pClosestPt - pos;
  return v.Dot(v) <= r * r;
}

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
gfl2::math::Vector3 CollisionSolver3D::ClosestPtPointTriangle( const gfl2::math::Vector3& p,
                                                               const gfl2::math::Vector3& a,
                                                               const gfl2::math::Vector3& b,
                                                               const gfl2::math::Vector3& c)
{
  // PがAの外側の頂点領域の中にあるかどうかチェック
  gfl2::math::Vector3 ab = b-a;
  gfl2::math::Vector3 ac = c-a;
  gfl2::math::Vector3 ap = p-a;
  f32                 d1 = ab.Dot(ap);
  f32                 d2 = ac.Dot(ap);
  if( d1 <= 0.0f && d2 <= 0.0f )
  {
    return a; // 重心座標(1,0,0)
  }

  // PがBの外側の頂点領域の中にあるかどうかチェック
  gfl2::math::Vector3 bp = p-b;
  f32                 d3 = ab.Dot(bp);
  f32                 d4 = ac.Dot(bp);
  if( d3 >= 0.0f && d4 <= d3 )
  {
    return b;  // 重心座標(0,1,0)
  }

  // PがABの辺領域の中にあるかどうかチェックし、あればPのAB上に対する射影を返す
  f32 vc = d1*d4 - d3*d2;
  if( vc <= 0.0f && d1 > 0.0f && d3 <= 0.0f )
  {
    f32 v = d1 / (d1-d3);
    //return a + v * ab; // 重心座標(1-v,v,0)
    gfl2::math::Vector3 temp0( v*ab.x, v*ab.y, v*ab.z ); // 重心座標(1-v,v,0)
    gfl2::math::Vector3 temp1( a+temp0 );                // 重心座標(1-v,v,0)
    return temp1;
  }

  // PがCの外側の頂点領域の中にあるかどうかチェック
  gfl2::math::Vector3 cp = p-c;
  f32 d5 = ab.Dot(cp);
  f32 d6 = ac.Dot(cp);
  if( d6 >= 0.0f && d5 <= d6 )
  {
    return c; // 重心座標(0,0,1)
  }

  // PがACの辺領域の中にあるかどうかチェックし、あればPのAC上に対する射影を返す
  f32 vb = d5*d2 - d1*d6;
  if( vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
  {
    f32 w = d2 / (d2-d6);
    //return a + w * ac; // 重心座標(1-w,0,0)
    gfl2::math::Vector3 temp0( w*ac.x, w*ac.y, w*ac.z ); // 重心座標(1-w,0,0)
    gfl2::math::Vector3 temp1( a+temp0 );                // 重心座標(1-w,0,0)
    return temp1;
  }

  // PがBCの辺領域の中にあるかどうかチェックし、あればPのBC上に対する射影を返す
  f32 va = d3*d6 - d5*d4;
  if( va <= 0.0f && (d4-d3) >= 0.0f && (d5-d6) >= 0.0f )
  {
    f32 w = (d4-d3) / ((d4-d3) + (d5-d6));
    //return b + w * (c-b); // 重心座標(0,1-w,w)
    gfl2::math::Vector3 temp0( c-b );      // 重心座標(1-v,v,0)
    gfl2::math::Vector3 temp1( w*temp0.x,  w*temp0.y,  w*temp0.z); // 重心座標(1-v,v,0)
    gfl2::math::Vector3 temp2( b+temp1 );  // 重心座標(1-v,v,0)
    return temp2;
  }

  // Pは面領域の中にある。Qをその重心座標(u,v,w)を用いて計算
  f32 denom = 1.0f / (va+vb+vc);
  f32 v = vb * denom;
  f32 w = vc * denom;
  return a + ab * v + ac * w;
}