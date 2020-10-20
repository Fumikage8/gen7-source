//=================================================================================================
/**
 *  @file   BaseCollisionScene.cpp
 *  @brief  ベースコリジョンシーン
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include "ViewerProc/include/FieldStatic/Collision/BaseShape.h"
#include "ViewerProc/include/FieldStatic/Collision/BaseActor.h"
#include "ViewerProc/include/FieldStatic/Collision/BaseSolver.h"
#include "ViewerProc/include/FieldStatic/Collision/BaseCollisionScene.h"
#include "ViewerProc/include/FieldStatic/Collision/StaticActor.h"
#include "ViewerProc/include/FieldStatic/Collision/DynamicActor.h"


//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
BaseCollisionScene::BaseCollisionScene()
: m_pSolver(NULL)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
BaseCollisionScene::~BaseCollisionScene()
{
  Terminate();
}

//-----------------------------------------------------------------------------
/**
*  @brief  初期化
*
*  @param  pAllocator  利用するアロケータ
*  @param  pHeap       利用するヒープ
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::Setup(gfl2::gfx::IGLAllocator* pAllocator,gfl2::heap::HeapBase* pHeap)
{
  m_pAllocator = pAllocator;
  m_pHeap      = pHeap;

  // ルートはあくまでツリー形成のためなのでベースアクター決め打ちで問題ない
  // 場所をここから変更する際は使用されるアロケータも変わってしまうことに注意してください
  m_rootStaticActorRoot  = GLNew(m_pAllocator) BaseActor();
  m_rootDynamicActorRoot = GLNew(m_pAllocator) BaseActor();

  // 場所をここから変更する際は使用されるアロケータも変わってしまうことに注意してください
  m_pSolver = CreateSolver();
}

//-----------------------------------------------------------------------------
/**
*  @brief  削除
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::Terminate(void)
{
  if( m_rootStaticActorRoot )
  {
    delete m_rootStaticActorRoot;
    m_rootStaticActorRoot = NULL;
  }
  if( m_rootDynamicActorRoot )
  {
    delete m_rootDynamicActorRoot;
    m_rootDynamicActorRoot = NULL;
  }
  if( m_pSolver )
  {
    delete m_pSolver;
    m_pSolver = NULL;
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  コリジョン判定
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::Update(void)
{
  // AABB構築

  // AABBを利用したざっくりとした判定

  // 詳細判定
  if( m_pSolver )
  {
    m_pSolver->Solved( m_rootStaticActorRoot , m_rootDynamicActorRoot );
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突されるアクターの作成
*
*  @return アクター
*/
//-----------------------------------------------------------------------------
StaticActor* BaseCollisionScene::CreateStaticActor(void)
{
  StaticActor* pStaticActor = GLNew(m_pAllocator) StaticActor();
  pStaticActor->SetHeap( m_pHeap );
  return pStaticActor;
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突するアクターの作成
*
*  @return アクター
*/
//-----------------------------------------------------------------------------
DynamicActor* BaseCollisionScene::CreateDynamicActor(void)
{
  DynamicActor* pDynamicActor = GLNew(m_pAllocator) DynamicActor();
  pDynamicActor->SetHeap( m_pHeap );
  return pDynamicActor;
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突されるアクターの追加
*
*  @param  pActor  コリジョンシステムに登録したいアクター
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::RegistStaticActor(StaticActor *pActor)
{
  m_rootStaticActorRoot->AddChild( pActor );
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突するアクターの追加
*
*  @param  pActor  コリジョンシステムに登録したいアクター
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::RegistDynamicActor(DynamicActor *pActor)
{
  m_rootDynamicActorRoot->AddChild( pActor );
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突されるアクターの削除
*
*  @param  pActor  コリジョンシステムから削除したいアクター
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::UnRegistStaticActor(StaticActor *pActor)
{
  m_rootStaticActorRoot->RemoveChild(pActor);
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突するアクターの削除
*
*  @param  pActor  コリジョンシステムから削除したいアクター
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::UnRegistDynamicActor(DynamicActor *pActor)
{
  m_rootDynamicActorRoot->RemoveChild(pActor);
}

//----------------------------------------------------------------------------
/**
*  @brief   解決者の作成
*
*  @return  solver
*  @note    この関数をオーバーライドすることにより衝突の挙動を変更できます
*/
//-----------------------------------------------------------------------------
BaseSolver* BaseCollisionScene::CreateSolver(void)
{
  return GFL_NEW(m_pHeap) BaseSolver();
}

//----------------------------------------------------------------------------
/**
*  @brief  シーンに登録されている衝突されるメッシュに対してのレイキャスト
*
*  @param  start     レイ開始位置
*  @param  end       レイ終了位置
*  @param  pHitData  レイの結果を受け取るための構造体(スタートからの最近接点が返ってきます)
*  @return           レイキャストがヒットしたかどうか
*/
//-----------------------------------------------------------------------------
b32 BaseCollisionScene::RaycastFromStaticActorsMesh(gfl2::math::Vector4& start,gfl2::math::Vector4& end,RaycastCustomCallback::HIT_DATA* pHitData)
{
  b32 isFirst = true;

  for( u32 i=0 ; i<m_rootStaticActorRoot->GetChildCount() ; i++ )
  {
    // コンテナからメッシュ形状を持つスタティックアクターを取得
    StaticActor* pStaticActor = m_rootStaticActorRoot->GetChild(i)->SafeCast<StaticActor>();
    if( !pStaticActor || pStaticActor->GetShapeType() != BaseActor::MESH )
    {
      continue;
    }

    // スタティックアクターからメッシュ形状を取得
    MeshShape* pStaticActorMesh = static_cast<MeshShape*>(pStaticActor->GetShape());
		if (pStaticActorMesh == NULL)
		{
			GFL_ASSERT(1);
		}
		if(pStaticActorMesh->m_pCollisionModel == NULL)
		{
			GFL_ASSERT(1);
		}

    if( !pStaticActorMesh || !pStaticActorMesh->m_pCollisionModel )
    {
      continue;
    }

    // メッシュ形状を利用しレイキャスト
    RaycastCustomCallback callback;
    pStaticActorMesh->m_pCollisionModel->GetIntersection( start,end,NULL, &callback );
    if( !callback.IsHit() )
    {
      continue;
    }

    // レイキャストの開始点に一番近いものを確保
    gfl2::math::Vector4 temp0( callback.GetHitData().intersection - start );
    gfl2::math::Vector4 temp1( pHitData->intersection             - start );
    if( isFirst || temp0.Length3() < temp1.Length3() )
    {
      isFirst = false;
      *pHitData = callback.GetHitData();
    }
  }

  return (isFirst) ? false : true;
}
