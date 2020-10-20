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

#include "Field/FieldRo/include/Collision/BaseShape.h"
#include "Field/FieldRo/include/Collision/CollisionSolverDefault.h"
#include "Field/FieldRo/include/Collision/StaticActor.h"
#include "Field/FieldRo/include/Collision/DynamicActor.h"

//----------------------------------------------------------------------------------------
//! @brief ソルバーベース
//----------------------------------------------------------------------------------------

#define BROADPHASE 1

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
  // 解決
  for( u32 i=0 ; i<pRootStaticActorRoot->GetChildCount() ; i++ )
  {
    StaticActor* pStaticActor = pRootStaticActorRoot->GetChild(i)->SafeCast<StaticActor>();
    if( !pStaticActor || !pStaticActor->IsActive() )
    {
      continue;
    }

    for( u32 j=0 ; j<pRootDynamicActorRoot->GetChildCount() ; j++ )
    {
      DynamicActor* pDynamicActor = pRootDynamicActorRoot->GetChild(j)->SafeCast<DynamicActor>();
      if( !pDynamicActor || !pDynamicActor->IsActive() )
      {
        continue;
      }

      b32 isHit = false;
      gfl2::math::Vector3 adjustVec(0.0f,0.0f,0.0f);

      // dynamic sphere vs static mesh
      if( pDynamicActor->GetShapeType() == BaseActor::SHAPE_SPHERE 
       && pStaticActor->GetShapeType()  == BaseActor::SHAPE_MESH  )
      {
        isHit = TestSphereMesh( pDynamicActor,
                                static_cast<SphereShape*>(pDynamicActor->GetShape()),
                                pStaticActor,
                                static_cast<MeshShape*>  (pStaticActor->GetShape()),
                                &adjustVec );
      }
      // dynamic cylinder vs static cylinder
      else if( pDynamicActor->GetShapeType() == BaseActor::SHAPE_CYLINDER 
            && pStaticActor->GetShapeType()  == BaseActor::SHAPE_CYLINDER )
      {
        isHit = TestCylinderCylinder( pDynamicActor,
                                      static_cast<CylinderShape*>(pDynamicActor->GetShape()),
                                      pStaticActor,
                                      static_cast<CylinderShape*>(pStaticActor->GetShape()),
                                      &adjustVec );
      }
      // dynamic cylinder vs static Box
      else if( pDynamicActor->GetShapeType() == BaseActor::SHAPE_CYLINDER 
            && pStaticActor->GetShapeType()  == BaseActor::SHAPE_BOX )
      {
        isHit = TestCylinderBox(pDynamicActor,
                                static_cast<CylinderShape*>(pDynamicActor->GetShape()),
                                pStaticActor,
                                static_cast<BoxShape*>(pStaticActor->GetShape()),
                                &adjustVec );
      }
      // dynamic cylinder vs static Line
      else if( pDynamicActor->GetShapeType() == BaseActor::SHAPE_CYLINDER 
            && pStaticActor->GetShapeType()  == BaseActor::SHAPE_LINE )
      {
        isHit = TestCylinderLine(pDynamicActor,
                                 static_cast<CylinderShape*>(pDynamicActor->GetShape()),
                                 pStaticActor,
                                 static_cast<LineShape*>(pStaticActor->GetShape()),
                                 &adjustVec );
      }
      // dynamic sphere vs static Obb
      else if( pDynamicActor->GetShapeType() == BaseActor::SHAPE_SPHERE 
            && pStaticActor->GetShapeType()  == BaseActor::SHAPE_OBB )
      {
        isHit = TestSphereObb(pDynamicActor,
                              static_cast<SphereShape*>(pDynamicActor->GetShape()),
                              pStaticActor,
                              static_cast<ObbShape*>(pStaticActor->GetShape()),
                              &adjustVec );
      }
      // dynamic sphere vs static Triangle
      else if( pDynamicActor->GetShapeType() == BaseActor::SHAPE_SPHERE
            && pStaticActor->GetShapeType()  == BaseActor::SHAPE_TRIANGLE )
      {
        isHit = TestSphereTriangle(pDynamicActor,
                                   static_cast<SphereShape*>(pDynamicActor->GetShape()),
                                   pStaticActor,
                                   static_cast<TriangleShape*>(pStaticActor->GetShape()),
                                   &adjustVec );
      }
      // dynamic cylinder vs static Triangle
      else if( pDynamicActor->GetShapeType() == BaseActor::SHAPE_CYLINDER
            && pStaticActor->GetShapeType()  == BaseActor::SHAPE_TRIANGLE )
      {
        isHit = TestCylinderTriangle(pDynamicActor,
                                     static_cast<CylinderShape*>(pDynamicActor->GetShape()),
                                     pStaticActor,
                                     static_cast<TriangleShape*>(pStaticActor->GetShape()),
                                     &adjustVec );
      }

      // 衝突している
      if( isHit )
      {
        // ダイナミックアクターの空いている衝突結果ワークを取得
        BaseActor::CollisionResult* pDynamicActorResult = NULL;
        for( u32 k=0 ; k<BaseActor::MAX_COLLISION_RESULT_CNT ; k++ )
        {
          if( pDynamicActor->GetCollisionResult(k)->isFree() )
          {
            pDynamicActorResult = pDynamicActor->GetCollisionResult(k);
            break;
          }
        }

        // スタティックアクターの空いている衝突結果ワークを取得
        BaseActor::CollisionResult* pSaticActorResult = NULL;
        for( u32 k=0 ; k<BaseActor::MAX_COLLISION_RESULT_CNT ; k++ )
        {
          if( pStaticActor->GetCollisionResult(k)->isFree() )
          {
            pSaticActorResult = pStaticActor->GetCollisionResult(k);
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
        if( pDynamicActor->IsEnableAdjustVector() )
        {
          gfl2::math::Vector3 newDynamicActorPos( adjustVec + pDynamicActor->GetPosition() );
          pDynamicActor->SetPosition( newDynamicActorPos );
        }
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  引数のツリーに登録されているメッシュ形状を軽量化し保持する
*
*  @param  pActorRoot    軽量化対象のツリー
*  @param  pos           ポリゴンを収集する中心値
*  @param  collectionR   ポリゴンを収集する範囲
*/
//-----------------------------------------------------------------------------
void CollisionSolverDefault::MeshCooking(gfl2::renderingengine::scenegraph::instance::InstanceNode* pActorRoot,gfl2::math::Vector3 pos,f32 collectionR)
{
#if USE_MESH_COOKING
  for( u32 i=0 ; i<pActorRoot->GetChildCount() ; i++ )
  {
    StaticActor* pActor = pActorRoot->GetChild(i)->SafeCast<StaticActor>();
    if( pActor 
     && pActor->IsActive()
     && pActor->GetCookingData()
     && pActor->GetShapeType() == BaseActor::SHAPE_MESH )
    {
      MeshCookingSub( pActor , static_cast<MeshShape*>(pActor->GetShape()) , pos , collectionR );
    }
  }
#endif
}

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
void CollisionSolverDefault::MeshCookingSub(StaticActor* pActor,MeshShape* pActorMesh,gfl2::math::Vector3 pos,f32 collectionR)
{
  // データ不正チェック
  if( !pActor
   || !pActorMesh->m_pCollisionModel
   || !pActorMesh->m_pCollisionModel->GetCollisionMeshNode() )
  {
    return;
  }

  // クッキングデータの初期化
  s32* pCookingData = pActor->GetCookingData();
  for( u32 i=0 ; i<StaticActor::MAX_COOKING_SIZE ; i++ )
  {
    pCookingData[i] = -1;
  }

  // コリジョン用ジオメトリの取得
  gfl2::renderingengine::scenegraph::resource::CollisionMeshResourceNode* pColMeshNode = pActorMesh->m_pCollisionModel->GetCollisionMeshNode();
  const gfl2::renderingengine::scenegraph::resource::CollisionTriangle*   pTriangle    = NULL;

  // 全トライアングルと衝突検出
  u32                 triangleCount(pColMeshNode->GetCollisionTriangleCount());
  gfl2::math::Vector3 localTrianglePos0 (0.0f,0.0f,0.0f);
  gfl2::math::Vector3 localTrianglePos1 (0.0f,0.0f,0.0f);
  gfl2::math::Vector3 localTrianglePos2 (0.0f,0.0f,0.0f);
  gfl2::math::Vector3 globalTrianglePos0(0.0f,0.0f,0.0f);
  gfl2::math::Vector3 globalTrianglePos1(0.0f,0.0f,0.0f);
  gfl2::math::Vector3 globalTrianglePos2(0.0f,0.0f,0.0f);
  gfl2::math::Vector3 tempClosestPt(0.0f,0.0f,0.0f);
  int                 hit          (0);
  u32                 cookingCnt   (0);

  for (u32 i = 0; i<triangleCount ; ++i)
  {
    // 衝突しているか？
    pTriangle      = pColMeshNode->GetCollisionTriangle(i);
    localTrianglePos0.x = pTriangle->m_Positions[0].x; localTrianglePos0.y = pTriangle->m_Positions[0].y; localTrianglePos0.z = pTriangle->m_Positions[0].z;
    localTrianglePos1.x = pTriangle->m_Positions[1].x; localTrianglePos1.y = pTriangle->m_Positions[1].y; localTrianglePos1.z = pTriangle->m_Positions[1].z;
    localTrianglePos2.x = pTriangle->m_Positions[2].x; localTrianglePos2.y = pTriangle->m_Positions[2].y; localTrianglePos2.z = pTriangle->m_Positions[2].z;
    globalTrianglePos0  = localTrianglePos0 + pActor->GetPosition();
    globalTrianglePos1  = localTrianglePos1 + pActor->GetPosition();
    globalTrianglePos2  = localTrianglePos2 + pActor->GetPosition();

    hit = TestSphereTriangleBroadPhase( collectionR,
                                        pos,
                                        globalTrianglePos0,
                                        globalTrianglePos1,
                                        globalTrianglePos2 );
    if( hit )
    {
      pCookingData[cookingCnt] = (s32)i;
      cookingCnt++;
      if( cookingCnt >= StaticActor::MAX_COOKING_SIZE )
      {
        return;
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
int CollisionSolverDefault::TestSphereMesh(DynamicActor*        pDynamicActor,
                                           SphereShape*         pDynamicActorSphere,
                                           StaticActor*         pStaticActor,
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
  const gfl2::renderingengine::scenegraph::resource::CollisionTriangle*   pTriangle    = NULL;

  // 全トライアングルと衝突検出
  u32                 triangleCount(pColMeshNode->GetCollisionTriangleCount());
  gfl2::math::Vector3 localTrianglePos0 (0.0f,0.0f,0.0f);
  gfl2::math::Vector3 localTrianglePos1 (0.0f,0.0f,0.0f);
  gfl2::math::Vector3 localTrianglePos2 (0.0f,0.0f,0.0f);
  gfl2::math::Vector3 globalTrianglePos0(0.0f,0.0f,0.0f);
  gfl2::math::Vector3 globalTrianglePos1(0.0f,0.0f,0.0f);
  gfl2::math::Vector3 globalTrianglePos2(0.0f,0.0f,0.0f);
  gfl2::math::Vector3 tempClosestPt     (0.0f,0.0f,0.0f);
  f32                 minDefLength      (-1.0f);

#if USE_MESH_COOKING
  b32 isEnableMeshCooking = pDynamicActor->IsUseCookingData() && pStaticActor->GetCookingData();
#else
  b32 isEnableMeshCooking = false;
#endif

  // クッキングされたデータを使う
  if( isEnableMeshCooking )
  {
    s32* pCookingData = pStaticActor->GetCookingData();
    for (u32 i = 0; i<StaticActor::MAX_COOKING_SIZE ; i++)
    {
      // クッキングデータを全検索したかどうか
      if( pCookingData[i] == -1 )
      {
        break;
      }

      // 衝突しているか？
      pTriangle           = pColMeshNode->GetCollisionTriangle(pCookingData[i]);
      localTrianglePos0.x = pTriangle->m_Positions[0].x; localTrianglePos0.y = pTriangle->m_Positions[0].y; localTrianglePos0.z = pTriangle->m_Positions[0].z;
      localTrianglePos1.x = pTriangle->m_Positions[1].x; localTrianglePos1.y = pTriangle->m_Positions[1].y; localTrianglePos1.z = pTriangle->m_Positions[1].z;
      localTrianglePos2.x = pTriangle->m_Positions[2].x; localTrianglePos2.y = pTriangle->m_Positions[2].y; localTrianglePos2.z = pTriangle->m_Positions[2].z;
      globalTrianglePos0  = localTrianglePos0 + pStaticActor->GetPosition();
      globalTrianglePos1  = localTrianglePos1 + pStaticActor->GetPosition();
      globalTrianglePos2  = localTrianglePos2 + pStaticActor->GetPosition();
      if( !TestSphereTriangle( pDynamicActorSphere->m_r, pDynamicActorSphere->m_pos + pDynamicActor->GetPosition(), globalTrianglePos0, globalTrianglePos1, globalTrianglePos2, &tempClosestPt ) )
      {
        continue;
      }

      // メッシュ全体での最近接点を取る
      gfl2::math::Vector3 def       = tempClosestPt - ( pDynamicActorSphere->m_pos + pDynamicActor->GetPosition() );
      f32                 defLength = def.Length();
      if( defLength <= FLT_EPSILON )
      {
        continue;
      }

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
  }
  // クッキングデータを使わない
  else
  {
    for (u32 i = 0; i<triangleCount ; i++)
    {
      // 衝突しているか？
      pTriangle           = pColMeshNode->GetCollisionTriangle(i);
      localTrianglePos0.x = pTriangle->m_Positions[0].x; localTrianglePos0.y = pTriangle->m_Positions[0].y; localTrianglePos0.z = pTriangle->m_Positions[0].z;
      localTrianglePos1.x = pTriangle->m_Positions[1].x; localTrianglePos1.y = pTriangle->m_Positions[1].y; localTrianglePos1.z = pTriangle->m_Positions[1].z;
      localTrianglePos2.x = pTriangle->m_Positions[2].x; localTrianglePos2.y = pTriangle->m_Positions[2].y; localTrianglePos2.z = pTriangle->m_Positions[2].z;
      globalTrianglePos0  = localTrianglePos0 + pStaticActor->GetPosition();
      globalTrianglePos1  = localTrianglePos1 + pStaticActor->GetPosition();
      globalTrianglePos2  = localTrianglePos2 + pStaticActor->GetPosition();
      if( !TestSphereTriangle( pDynamicActorSphere->m_r, pDynamicActorSphere->m_pos + pDynamicActor->GetPosition(), globalTrianglePos0, globalTrianglePos1, globalTrianglePos2, &tempClosestPt ) )
      {
        continue;
      }

      // メッシュ全体での最近接点を取る
      gfl2::math::Vector3 def       = tempClosestPt - ( pDynamicActorSphere->m_pos + pDynamicActor->GetPosition() );
      f32                 defLength = def.Length();
      if( defLength <= FLT_EPSILON )
      {
        continue;
      }

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
  }

  // 衝突しているかどうかを返す
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
#if BROADPHASE
  // ブロードフェイズ
  if( !TestSphereTriangleBroadPhase(r,pos,a,b,c) )
  {
    return 0;
  }
#endif

  // 球の中心に対する最近接点である三角形ABC上にある点Pを見つける
  *pClosestPt = ClosestPtPointTriangle( pos , a,b,c );

  // 球と三角形が交差するのは、球の中心から点Pまでの(平方した)距離が(平方した)球の半径よりも小さい場合
  gfl2::math::Vector3 v = *pClosestPt - pos;
  return v.Dot(v) <= r * r;
}

//-----------------------------------------------------------------------------
/**
*  @brief  Spehere Vs Triangle (BroadPhace)
*
*  @param  r          スフィアの半径
*  @param  pos        スフィアの中心位置
*  @param  a          トライアングルの座標1
*  @param  b          トライアングルの座標2
*  @param  c          トライアングルの座標3
*  @param  pClosestPt 最近接点
*  @return trueなら衝突している、falseなら衝突していない
*/
//-----------------------------------------------------------------------------
b32 CollisionSolverDefault::TestSphereTriangleBroadPhase(const f32                  r,
                                                         const gfl2::math::Vector3& pos,
                                                         const gfl2::math::Vector3& a,
                                                         const gfl2::math::Vector3& b,
                                                         const gfl2::math::Vector3& c)
{
  //===========================================================================================================
  // note: nijiは壁用ポリゴンは全て垂直に作られているので、ここの処理はそれを前提に記述しています！
  //===========================================================================================================

  // ポリゴン情報を収集
  gfl2::math::Vector3 convPos       (pos.x,0.0f,pos.z);
  gfl2::math::Vector3 trianglePos0  (  a.x,0.0f,  a.z);
  gfl2::math::Vector3 trianglePos1  (  b.x,0.0f,  b.z);
  gfl2::math::Vector3 trianglePos2  (  c.x,0.0f,  c.z);
  gfl2::math::Vector3 def0To1       (trianglePos1 - trianglePos0);
  gfl2::math::Vector3 def1To2       (trianglePos2 - trianglePos1);
  gfl2::math::Vector3 def2To0       (trianglePos0 - trianglePos2);
  f32                 def0To1Length (def0To1.Length());
  f32                 def1To2Length (def1To2.Length());
  f32                 def2To0Length (def2To0.Length());

  // 最長の辺を調べる
  gfl2::math::Vector3 maxDef      ( def0To1       );
  gfl2::math::Vector3 start       ( trianglePos0  );
  f32                 maxDefLength( def0To1Length );
  if( def1To2Length > maxDefLength )
  {
    maxDef       = def1To2;
    maxDefLength = def1To2Length;
    start        = trianglePos1; 
  }
  if( def2To0Length > maxDefLength )
  {
    maxDef       = def2To0;
    maxDefLength = def2To0Length;
    start        = trianglePos2;
  }

  // ポリゴンを包括するスフィアを作成
  f32                 boudingSphereR  ( maxDefLength/2.0f   );
  gfl2::math::Vector3 boudingSpherePos( start + maxDef/2.0f );

  // ポリゴンを包括するスフィアとアクターのスフィアの衝突チェック
  gfl2::math::Vector3 def( convPos - boudingSpherePos );
  if( def.Length() < r + boudingSphereR )
  {
    return true;
  }
  return false;
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

  // 高さの事前チェック
  f32 worldDynamicActorCylinderMin = dynamicActorPos.y + pDynamicActorCylinder->m_pos.y;
  f32 worldDynamicActorCylinderMax = dynamicActorPos.y + pDynamicActorCylinder->m_pos.y + pDynamicActorCylinder->m_height;
  f32 worldStaticActorCylinderMin  =  staticActorPos.y +  pStaticActorCylinder->m_pos.y;
  f32 worldStaticActorCylinderMax  =  staticActorPos.y +  pStaticActorCylinder->m_pos.y +  pStaticActorCylinder->m_height;
  if( worldDynamicActorCylinderMin > worldStaticActorCylinderMax || worldDynamicActorCylinderMax < worldStaticActorCylinderMin )
  {
    return false;
  }

  // 二つのスフィア間の距離を取得
  gfl2::math::Vector3 worldDynamicActorSphere( dynamicActorPos + pDynamicActorCylinder->m_pos ); worldDynamicActorSphere.y = 0.0f;
  gfl2::math::Vector3 worldStaticActorSphere ( staticActorPos  +  pStaticActorCylinder->m_pos ); worldStaticActorSphere.y  = 0.0f;
  gfl2::math::Vector3 vecFromStaticActorToDynamicActor = worldDynamicActorSphere - worldStaticActorSphere;
  f32                 lenFromStaticActorToDynamicActor = vecFromStaticActorToDynamicActor.Length();

  // 衝突判定
  if( lenFromStaticActorToDynamicActor > FLT_EPSILON 
   && lenFromStaticActorToDynamicActor < pDynamicActorCylinder->m_r + pStaticActorCylinder->m_r )
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

  // 高さの事前チェック
  f32 worldDynamicActorCylinderMin = dynamicActorPos.y + pDynamicActorCylinder->m_pos.y;
  f32 worldDynamicActorCylinderMax = dynamicActorPos.y + pDynamicActorCylinder->m_pos.y + pDynamicActorCylinder->m_height;
  f32 worldStaticActorBoxMin       =  staticActorPos.y +       pStaticActorBox->m_pos.y;
  f32 worldStaticActorBoxMax       =  staticActorPos.y +       pStaticActorBox->m_pos.y +       pStaticActorBox->m_height;
  if( worldDynamicActorCylinderMin > worldStaticActorBoxMax || worldDynamicActorCylinderMax < worldStaticActorBoxMin )
  {
    return false;
  }

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

  // 余白を追加
  gfl2::math::Vector3 unitAdjustVec(*pAdjustVec);
  unitAdjustVec.Normalize();
  *pAdjustVec = *pAdjustVec + (unitAdjustVec * MARJIN);

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
  // DynamicActorの座標を取得
  gfl2::math::Vector3 dynamicActorPos( pDynamicActor->GetPosition() );
  // StaticActorの座標を取得
  gfl2::math::Vector3 staticActorPos( pStaticActor->GetPosition() );

  // 高さの事前チェック
  f32 worldDynamicActorCylinderMin = dynamicActorPos.y + pDynamicActorCylinder->m_pos.y;
  f32 worldDynamicActorCylinderMax = dynamicActorPos.y + pDynamicActorCylinder->m_pos.y  + pDynamicActorCylinder->m_height;
  f32 worldStaticActorLineMin      =  staticActorPos.y +      pStaticActorLine->m_pos0.y;
  f32 worldStaticActorLineMax      =  staticActorPos.y +      pStaticActorLine->m_pos0.y +      pStaticActorLine->m_height;
  if( worldDynamicActorCylinderMin > worldStaticActorLineMax || worldDynamicActorCylinderMax < worldStaticActorLineMin )
  {
    return false;
  }

  // 最近接点の取得
  gfl2::math::Vector3 worldDynamicActorSphere( dynamicActorPos + pDynamicActorCylinder->m_pos  ); worldDynamicActorSphere.y = 0.0f;
  gfl2::math::Vector3 worldStaticActorLine0  (  staticActorPos +      pStaticActorLine->m_pos0 ); worldStaticActorLine0.y   = 0.0f;
  gfl2::math::Vector3 worldStaticActorLine1  (  staticActorPos +      pStaticActorLine->m_pos1 ); worldStaticActorLine1.y   = 0.0f;
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
int CollisionSolverDefault::TestSphereObb    ( BaseActor*           pDynamicActor,
                                               SphereShape*         pDynamicActorSphere,
                                               BaseActor*           pStaticActor,
                                               ObbShape*            pStaticActorObb,
                                               gfl2::math::Vector3* pAdjustVec)
{
  // 最近接点を取得
  gfl2::math::Vector3 closestPt( ClosestPtPointObb( (pDynamicActor->GetPosition() + pDynamicActorSphere->m_pos),
                                                    (pStaticActor->GetPosition()  +     pStaticActorObb->m_pos),
                                                    pStaticActorObb->m_u0,
                                                    pStaticActorObb->m_u1,
                                                    pStaticActorObb->m_u2,
                                                    pStaticActorObb->m_e0,
                                                    pStaticActorObb->m_e1,
                                                    pStaticActorObb->m_e2 ) );

  // 最近接点がスフィアに入っているか
  gfl2::math::Vector3  v( closestPt - (pDynamicActor->GetPosition() + pDynamicActorSphere->m_pos) );
  int hit = (v.Dot(v) <= pDynamicActorSphere->m_r * pDynamicActorSphere->m_r);

  // 補正量を取得
  if( hit == 1 )
  {
    gfl2::math::Vector3 adjustVec( (pDynamicActor->GetPosition() + pDynamicActorSphere->m_pos) - closestPt );
    f32 adjustLength( pDynamicActorSphere->m_r - adjustVec.Length() );
    *pAdjustVec = adjustVec.Normalize() * adjustLength;
  }

  return hit;
}

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
int CollisionSolverDefault::TestSphereTriangle( DynamicActor*        pDynamicActor,
                                                SphereShape*         pDynamicActorSphere,
                                                StaticActor*         pStaticActor,
                                                TriangleShape*       pStaticActorTriangle,
                                                gfl2::math::Vector3* pAdjustVec)
{
  // 衝突しているか？
  gfl2::math::Vector3 localTrianglePos0 (0.0f,0.0f,0.0f);
  gfl2::math::Vector3 localTrianglePos1 (0.0f,0.0f,0.0f);
  gfl2::math::Vector3 localTrianglePos2 (0.0f,0.0f,0.0f);
  gfl2::math::Vector3 globalTrianglePos0(0.0f,0.0f,0.0f);
  gfl2::math::Vector3 globalTrianglePos1(0.0f,0.0f,0.0f);
  gfl2::math::Vector3 globalTrianglePos2(0.0f,0.0f,0.0f);
  gfl2::math::Vector3 tempClosestPt     (0.0f,0.0f,0.0f);
  localTrianglePos0.x = pStaticActorTriangle->m_pos0.x; localTrianglePos0.y = pStaticActorTriangle->m_pos0.y; localTrianglePos0.z = pStaticActorTriangle->m_pos0.z;
  localTrianglePos1.x = pStaticActorTriangle->m_pos1.x; localTrianglePos1.y = pStaticActorTriangle->m_pos1.y; localTrianglePos1.z = pStaticActorTriangle->m_pos1.z;
  localTrianglePos2.x = pStaticActorTriangle->m_pos2.x; localTrianglePos2.y = pStaticActorTriangle->m_pos2.y; localTrianglePos2.z = pStaticActorTriangle->m_pos2.z;
  globalTrianglePos0  = localTrianglePos0 + pStaticActor->GetPosition();
  globalTrianglePos1  = localTrianglePos1 + pStaticActor->GetPosition();
  globalTrianglePos2  = localTrianglePos2 + pStaticActor->GetPosition();

  // 球の中心に対する最近接点である三角形ABC上にある点Pを見つける
  tempClosestPt = ClosestPtPointTriangle( pDynamicActorSphere->m_pos + pDynamicActor->GetPosition() ,globalTrianglePos0,globalTrianglePos1,globalTrianglePos2);

  // 球と三角形が交差するのは、球の中心から点Pまでの(平方した)距離が(平方した)球の半径よりも小さい場合
  gfl2::math::Vector3 v = tempClosestPt - (pDynamicActorSphere->m_pos + pDynamicActor->GetPosition());
  if( !(v.Dot(v) <= pDynamicActorSphere->m_r * pDynamicActorSphere->m_r) )
  {
    return 0;
  }

  // メッシュ全体での最近接点を取る
  gfl2::math::Vector3 def       = tempClosestPt - ( pDynamicActorSphere->m_pos + pDynamicActor->GetPosition() );
  f32                 defLength = def.Length();
  if( defLength <= FLT_EPSILON )
  {
    return 0;
  }

  // 押し返しベクトルの取得
  gfl2::math::Vector3 adjustVec    = def.Normalize() * -1.0f;
  f32                 adjustVecLen = pDynamicActorSphere->m_r - defLength;
  *pAdjustVec = adjustVec * (adjustVecLen+MARJIN);
  return 1;
}

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
int CollisionSolverDefault::TestCylinderTriangle( BaseActor*           pDynamicActor,
                                                  CylinderShape*       pDynamicActorCylinder,
                                                  BaseActor*           pStaticActor,
                                                  TriangleShape*       pStaticActorTriangle,
                                                  gfl2::math::Vector3* pAdjustVec)
{
  {
    // シリンダーの中心に対する最近接点である三角形ABC上にある点Pを見つける
    gfl2::math::Vector3 tempClosestPt        (0.0f,0.0f,0.0f);
    gfl2::math::Vector3 globalTrianglePos0   (pStaticActorTriangle->m_pos0 + pStaticActor->GetPosition());
    gfl2::math::Vector3 globalTrianglePos1   (pStaticActorTriangle->m_pos1 + pStaticActor->GetPosition());
    gfl2::math::Vector3 globalTrianglePos2   (pStaticActorTriangle->m_pos2 + pStaticActor->GetPosition());
    gfl2::math::Vector3 globalDynamicActorPos(pDynamicActorCylinder->m_pos + pDynamicActor->GetPosition());
    globalTrianglePos0.y    = 0.0f;
    globalTrianglePos1.y    = 0.0f;
    globalTrianglePos2.y    = 0.0f;
    globalDynamicActorPos.y = 0.0f;
    tempClosestPt = ClosestPtPointTriangle( globalDynamicActorPos,globalTrianglePos0,globalTrianglePos1,globalTrianglePos2);

    // XZ平面のチェック
    gfl2::math::Vector3 def(tempClosestPt - globalDynamicActorPos);
    if( def.Length() > pDynamicActorCylinder->m_r)
    {
      return 0;
    }
  }

  {
    // シリンダーの中心に対する最近接点である三角形ABC上にある点Pを見つける
    gfl2::math::Vector3 tempClosestPt        (0.0f,0.0f,0.0f);
    gfl2::math::Vector3 globalTrianglePos0   (pStaticActorTriangle->m_pos0 + pStaticActor->GetPosition());
    gfl2::math::Vector3 globalTrianglePos1   (pStaticActorTriangle->m_pos1 + pStaticActor->GetPosition());
    gfl2::math::Vector3 globalTrianglePos2   (pStaticActorTriangle->m_pos2 + pStaticActor->GetPosition());
    gfl2::math::Vector3 globalDynamicActorPos(pDynamicActorCylinder->m_pos + pDynamicActor->GetPosition());
    tempClosestPt = ClosestPtPointTriangle(globalDynamicActorPos,globalTrianglePos0,globalTrianglePos1,globalTrianglePos2);

    /// 高さチェック
    f32 worldDynamicActorCylinderMin = globalDynamicActorPos.y;
    f32 worldDynamicActorCylinderMax = globalDynamicActorPos.y + pDynamicActorCylinder->m_height;
    if( worldDynamicActorCylinderMin > tempClosestPt.y || tempClosestPt.y > worldDynamicActorCylinderMax )
    {
      return 0;
    }

    // 押し返しベクトルの作成
    gfl2::math::Vector3 def( globalDynamicActorPos - tempClosestPt );
    f32 osikaeshi = pDynamicActorCylinder->m_r - def.Length();
    gfl2::math::Vector3 normal = def.Normalize();
    *pAdjustVec = normal * (osikaeshi + MARJIN);
  }

  return 1;
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
gfl2::math::Vector3 CollisionSolverDefault::ClosestPtPointObb( const gfl2::math::Vector3& p,
                                                               const gfl2::math::Vector3& rectC,
                                                               const gfl2::math::Vector3& rectU0,
                                                               const gfl2::math::Vector3& rectU1,
                                                               const gfl2::math::Vector3& rectU2,
                                                               const f32                  rectE0,
                                                               const f32                  rectE1,
                                                               const f32                  rectE2)
{
  gfl2::math::Vector3 d = p - rectC;

  // ボックスの中心における結果から開始、そこから段階的に進める
  gfl2::math::Vector3 closestPt( rectC );

  // 各長方形の軸に対して
  gfl2::math::Vector3 rectU[3] = { rectU0,rectU1,rectU2 };
  f32                 rectE[3] = { rectE0,rectE1,rectE2 };
  for( u32 i=0 ; i<3 ; i++ )
  {
    // ...dをその軸に射影して
    // 長方形の中心からdの軸に沿った距離を得る
    f32 dist = d.Dot(rectU[i]);

    // 長方形の範囲よりも距離が大きい場合、長方形までクランプ
    if( dist >  rectE[i] )
    {
      dist =  rectE[i];
    }
    if( dist < -rectE[i] )
    {
      dist = -rectE[i];
    }

    // ワールド座標を得るためにその距離だけ軸に沿って進める
    closestPt += gfl2::math::Vector3( dist*rectU[i].x , dist*rectU[i].y, dist*rectU[i].z);
  }

  return closestPt;
}