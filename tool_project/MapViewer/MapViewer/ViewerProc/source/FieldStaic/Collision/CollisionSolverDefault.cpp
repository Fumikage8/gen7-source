//=================================================================================================
/**
 *  @file   CollisionSolverDefault.h
 *  @brief  デフォルトソルバー
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include <Collision/include/gfl2_CollisionModel.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CollisionMeshResourceNode.h>
#include <math/include/gfl2_math_control.h>

#include "ViewerProc/include/FieldStatic/Collision/BaseShape.h"
#include "ViewerProc/include/FieldStatic/Collision/CollisionSolverDefault.h"
#include "ViewerProc/include/FieldStatic/Collision/StaticActor.h"
#include "ViewerProc/include/FieldStatic/Collision/DynamicActor.h"

//----------------------------------------------------------------------------------------
//! @brief ソルバーベース
//----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
CollisionSolverDefault::CollisionSolverDefault()
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
CollisionSolverDefault::~CollisionSolverDefault()
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
void CollisionSolverDefault::Solved(gfl2::renderingengine::scenegraph::instance::InstanceNode* pRootStaticActorRoot,
                                    gfl2::renderingengine::scenegraph::instance::InstanceNode* pRootDynamicActorRoot)
{
  // 初期化
  for( u32 i=0 ; i<pRootStaticActorRoot->GetChildCount() ; i++ )
  {
    StaticActor* pStaticActor = pRootStaticActorRoot->GetChild(i)->SafeCast<StaticActor>();
    if( pStaticActor )
    {
      pStaticActor->InitCollisionResult();
    }
  }
  for( u32 i=0 ; i<pRootDynamicActorRoot->GetChildCount() ; i++ )
  {
    DynamicActor* pDynamicActor = pRootDynamicActorRoot->GetChild(i)->SafeCast<DynamicActor>();
    if( pDynamicActor )
    {
      pDynamicActor->InitCollisionResult();
    }
  }

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
      gfl2::math::Vector3 adjustVec(0.0f,0.0f,0.0f);

      // dynamic sphere vs static mesh
      if( pDynamicActor->GetShapeType() == BaseActor::SPHERE 
       && pStaticActor->GetShapeType()  == BaseActor::MESH  )
      {
        isHit = TestSphereMesh( pDynamicActor,
                                static_cast<SphereShape*>(pDynamicActor->GetShape()),
                                pStaticActor,
                                static_cast<MeshShape*>  (pStaticActor->GetShape()),
                                &adjustVec );
      }
      // dynamic cylinder vs static cylinder
      else if( pDynamicActor->GetShapeType() == BaseActor::CYLINDER 
            && pStaticActor->GetShapeType()  == BaseActor::CYLINDER )
      {
        isHit = TestCylinderCylinder( pDynamicActor,
                                      static_cast<CylinderShape*>(pDynamicActor->GetShape()),
                                      pStaticActor,
                                      static_cast<CylinderShape*>(pStaticActor->GetShape()),
                                      &adjustVec );
      }
      // dynamic cylinder vs static Box
      else if( pDynamicActor->GetShapeType() == BaseActor::CYLINDER 
            && pStaticActor->GetShapeType()  == BaseActor::BOX )
      {
        isHit = TestCylinderBox(pDynamicActor,
                                static_cast<CylinderShape*>(pDynamicActor->GetShape()),
                                pStaticActor,
                                static_cast<BoxShape*>(pStaticActor->GetShape()),
                                &adjustVec );
      }
      // dynamic cylinder vs static Line
      else if( pDynamicActor->GetShapeType() == BaseActor::CYLINDER 
            && pStaticActor->GetShapeType()  == BaseActor::LINE )
      {
        isHit = TestCylinderLine(pDynamicActor,
                                 static_cast<CylinderShape*>(pDynamicActor->GetShape()),
                                 pStaticActor,
                                 static_cast<LineShape*>(pStaticActor->GetShape()),
                                 &adjustVec );
      }

      // 衝突している
      if( isHit )
      {
        // ダイナミックアクターの空いている衝突結果ワークを取得
        BaseActor::CollisionResult* pDynamicActorResult = NULL;
        for( u32 i=0 ; i<BaseActor::MAX_COLLISION_RESULT_CNT ; i++ )
        {
          if( pDynamicActor->GetCollisionResult(i)->isFree() )
          {
            pDynamicActorResult = pDynamicActor->GetCollisionResult(i);
            break;
          }
        }

        // スタティックアクターの空いている衝突結果ワークを取得
        BaseActor::CollisionResult* pSaticActorResult = NULL;
        for( u32 i=0 ; i<BaseActor::MAX_COLLISION_RESULT_CNT ; i++ )
        {
          if( pStaticActor->GetCollisionResult(i)->isFree() )
          {
            pSaticActorResult = pStaticActor->GetCollisionResult(i);
            break;
          }
        }

        // 当たった相手を取得
        if( pDynamicActorResult )
        {
          pDynamicActorResult->pTargetActor = pStaticActor;
        }
        if( pSaticActorResult )
        {
          pSaticActorResult->pTargetActor   = pDynamicActor;
        }

        // アクターの座標更新
        gfl2::math::Vector3 newDynamicActorPos( adjustVec + pDynamicActor->GetPosition() );
        pDynamicActor->SetPosition( newDynamicActorPos );
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
*  @param  pAdjustVec           ダイナミックアクターへの補正量
*  @return 1なら衝突している、0なら衝突していない
*/
//-----------------------------------------------------------------------------
int CollisionSolverDefault::TestSphereMesh(BaseActor*           pDynamicActor,
                                           SphereShape*         pDynamicActorSphere,
                                           BaseActor*           pStaticActor,
                                           MeshShape*           pStaticActorMesh,
                                           gfl2::math::Vector3* pAdjustVec)
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
                                  pDynamicActorSphere->m_pos + pDynamicActor->GetPosition(),
                                  gfl2::math::Vector3( trianglePos0 + pStaticActor->GetPosition() ),
                                  gfl2::math::Vector3( trianglePos1 + pStaticActor->GetPosition() ),
                                  gfl2::math::Vector3( trianglePos2 + pStaticActor->GetPosition() ),
                                  &tempClosestPt );

    if( !hit )
    {
      continue;
    }

    // メッシュ全体での最近接点を取る
    gfl2::math::Vector3 def       = tempClosestPt - ( pDynamicActorSphere->m_pos + pDynamicActor->GetPosition() );
    f32                 defLength = def.Length();
    if( minDefLength < 0.0f || defLength < minDefLength )
    {
      // 最近接点の距離を更新
      minDefLength = defLength;

      // 押し返しベクトルの取得
      gfl2::math::Vector3 adjustVec    = def.Normalize() * -1.0f;
      f32                 adjustVecLen = pDynamicActorSphere->m_r - defLength;
      *pAdjustVec = adjustVec * (adjustVecLen+MARJIN);
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
int CollisionSolverDefault::TestSphereTriangle( const f32                  r,
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
int CollisionSolverDefault::TestCylinderCylinder(BaseActor*           pDynamicActor,
                                                 CylinderShape*       pDynamicActorCylinder,
                                                 BaseActor*           pStaticActor,
                                                 CylinderShape*       pStaticActorCylinder,
                                                 gfl2::math::Vector3* pAdjustVec )
{
  // DynamicActorの座標を取得
  gfl2::math::Vector3 dynamicActorPos( pDynamicActor->GetPosition() );

  // StaticActorの座標を取得
  gfl2::math::Vector3 staticActorPos( pStaticActor->GetPosition() );

  // 二つのスフィア間の距離を取得
  gfl2::math::Vector3 worldDynamicActorSphere( dynamicActorPos + pDynamicActorCylinder->m_pos ); worldDynamicActorSphere.y = 0.0f;
  gfl2::math::Vector3 worldStaticActorSphere ( staticActorPos  +  pStaticActorCylinder->m_pos ); worldStaticActorSphere.y  = 0.0f;
  gfl2::math::Vector3 vecFromStaticActorToDynamicActor = worldDynamicActorSphere - worldStaticActorSphere;
  f32                 lenFromStaticActorToDynamicActor = vecFromStaticActorToDynamicActor.Length();

  // 衝突判定
  if( lenFromStaticActorToDynamicActor < pDynamicActorCylinder->m_r + pStaticActorCylinder->m_r )
  {
    // 押し返しベクトルの作成
    f32 osikaeshi = (pDynamicActorCylinder->m_r + pStaticActorCylinder->m_r) - lenFromStaticActorToDynamicActor;
    gfl2::math::Vector3 normalVecFromStaticActorToDynamicActor = vecFromStaticActorToDynamicActor.Normalize();
    *pAdjustVec = normalVecFromStaticActorToDynamicActor * (osikaeshi + MARJIN);

    return true;
  }
  return false;
}

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
int CollisionSolverDefault::TestCylinderBox(BaseActor*           pDynamicActor,
                                            CylinderShape*       pDynamicActorCylinder,
                                            BaseActor*           pStaticActor,
                                            BoxShape*            pStaticActorBox,
                                            gfl2::math::Vector3* pAdjustVec )
{
  // DynamicActorの座標を取得
  gfl2::math::Vector3 dynamicActorPos( pDynamicActor->GetPosition() );

  // StaticActorの座標を取得
  gfl2::math::Vector3 staticActorPos( pStaticActor->GetPosition() );

  // 内外判定
  gfl2::math::Vector3 worldDynamicActorSphere( dynamicActorPos + pDynamicActorCylinder->m_pos ); worldDynamicActorSphere.y = 0.0f;
  gfl2::math::Vector3 worldStaticActorBox    ( staticActorPos  +       pStaticActorBox->m_pos );     worldStaticActorBox.y = 0.0f;
  f32 dotX = pStaticActorBox->m_u0.Dot( worldStaticActorBox - worldDynamicActorSphere );
  f32 dotY = pStaticActorBox->m_u1.Dot( worldStaticActorBox - worldDynamicActorSphere );

  // 衝突判定
  if( gfl2::math::Abs(dotX) > pStaticActorBox->m_e0 + pDynamicActorCylinder->m_r 
   || gfl2::math::Abs(dotY) > pStaticActorBox->m_e1 + pDynamicActorCylinder->m_r )
  {
    return false;
  }

  // X軸に直行する辺に近いのか、Y軸に直行する辺に近いのか調査
  f32 defX = (pStaticActorBox->m_e0 + pDynamicActorCylinder->m_r) - gfl2::math::Abs(dotX);
  f32 defY = (pStaticActorBox->m_e1 + pDynamicActorCylinder->m_r) - gfl2::math::Abs(dotY);

  // X軸に直行する辺に近い
  if( defX < defY )
  {
    // X軸に直行する辺の左辺に当たっている
    if( dotX>0.0f )
    {
      *pAdjustVec = gfl2::math::Vector3(-1.0f*defX*pStaticActorBox->m_u0.x,
                                        -1.0f*defX*pStaticActorBox->m_u0.y,
                                        -1.0f*defX*pStaticActorBox->m_u0.z );
    }
    // X軸に直行する辺の右辺に当たっている
    else
    {
      *pAdjustVec = gfl2::math::Vector3(defX*pStaticActorBox->m_u0.x,
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
      *pAdjustVec =  gfl2::math::Vector3(-1.0f*defY*pStaticActorBox->m_u1.x,
                                         -1.0f*defY*pStaticActorBox->m_u1.y,
                                         -1.0f*defY*pStaticActorBox->m_u1.z );
    }
    // Y軸に直行する辺の下辺に当たっている
    else
    {
      *pAdjustVec =  gfl2::math::Vector3(defY*pStaticActorBox->m_u1.x,
                                         defY*pStaticActorBox->m_u1.y,
                                         defY*pStaticActorBox->m_u1.z );
    }
  }

  return true;
}

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
int CollisionSolverDefault::TestCylinderLine    ( BaseActor*           pDynamicActor,
                                                  CylinderShape*       pDynamicActorCylinder,
                                                  BaseActor*           pStaticActor,
                                                  LineShape*           pStaticActorLine,
                                                  gfl2::math::Vector3* pAdjustVec)
{
  // 最近接点の取得
  gfl2::math::Vector3 worldDynamicActorSphere( pDynamicActor->GetPosition() + pDynamicActorCylinder->m_pos  ); worldDynamicActorSphere.y = 0.0f;
  gfl2::math::Vector3 worldStaticActorLine0  (  pStaticActor->GetPosition() +      pStaticActorLine->m_pos0 ); worldStaticActorLine0.y   = 0.0f;
  gfl2::math::Vector3 worldStaticActorLine1  (  pStaticActor->GetPosition() +      pStaticActorLine->m_pos1 ); worldStaticActorLine1.y   = 0.0f;
  gfl2::math::Vector3 closestPt              ( ClosestPtPointSegment( worldDynamicActorSphere , worldStaticActorLine0 , worldStaticActorLine1 ) );

  // 衝突判定
  gfl2::math::Vector3 def( worldDynamicActorSphere - closestPt );
  if( def.Length() > pDynamicActorCylinder->m_r )
  {
    return false;
  }

  // 押し返しベクトルの作成
  f32 osikaeshi = pDynamicActorCylinder->m_r - def.Length();
  gfl2::math::Vector3 normal = def.Normalize();
  *pAdjustVec = normal * (osikaeshi + MARJIN);

  return true;
}

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
                                                                  const gfl2::math::Vector3& b )
{
  // パラメータ化されている位置 d(t) = a + t*(b-a)の計算によりabにcを射影
  gfl2::math::Vector3 ab = b-a;
  gfl2::math::Vector3 ac = p-a;
  f32 t = ac.Dot(ab) / ab.Dot(ab);

  // 線分の外側にある場合、t(従ってd)を最近接点までクランプ
  if( t<0.0f ) t = 0.0f;
  if( t>1.0f ) t = 1.0f;

  // クランプされているtからの射影されている位置を計算
  gfl2::math::Vector3 d(a.x + t * ab.x,
                        a.y + t * ab.y,
                        a.z + t * ab.z);
  return d;
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
gfl2::math::Vector3 CollisionSolverDefault::ClosestPtPointTriangle(const gfl2::math::Vector3& p,
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
gfl2::math::Vector3 CollisionSolverDefault::ClosestPtPointRect(const gfl2::math::Vector3& p,
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