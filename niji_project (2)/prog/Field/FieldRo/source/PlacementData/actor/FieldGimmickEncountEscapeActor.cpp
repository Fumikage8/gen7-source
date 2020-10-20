//======================================================================
/**
 * @file FieldGimmickEncountEscapeActor.cpp
 * @date 2015/08/19 19:03:22
 * @author saita_kazuki
 * @brief ギミックエンカウント逃走型アクター
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountEscapeActor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldGimmickEncountAccessor.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountDataAccessor.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountData.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountManager.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"

#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficPath.h"

#include "GameSys/include/GameManager.h"

GFL_NAMESPACE_BEGIN( Field );

/**
 *  @brief  コンストラクタ
 */
FieldGimmickEncountEscapeActor::FieldGimmickEncountEscapeActor( void )
  : FieldMoveModelActorBase()
  , IFieldGimmickEncountActorBase()
  , m_pData( NULL )
  , m_pCollisionSceneForGimmickEncountAction( NULL )
  , m_pCollisionSceneForGimmickEncount( NULL )
  , m_pActorForGimmickEncountAction1( NULL )
  , m_pActorForGimmickEncountAction2( NULL )
  , m_pActorForGimmickEncount( NULL )
  , m_pGimmickEncountManager( NULL)
  , m_pGimmickEncountLocalWork( NULL)
  , m_pTrafficPath1( NULL)
  , m_pTrafficPath2( NULL)
{
}

/**
 *  @brief  デストラクタ
 */
FieldGimmickEncountEscapeActor::~FieldGimmickEncountEscapeActor( void )
{
  // PlacementDataManager側でワークを残すか完全破棄かを選択するので、
  // デストラクタでTerminate()は呼ばない
}

/**
 *  @brief  初期化
 */
void FieldGimmickEncountEscapeActor::Initialize( SetupData &setupData )
{
  m_pMoveModelManager                       = setupData.pMoveModelManager;
  m_pData                                   = setupData.pGimmickEncountData;
  m_pCollisionSceneForGimmickEncountAction  = setupData.pCollisionSceneForGimmickEncountAction;
  m_pCollisionSceneForGimmickEncount        = setupData.pCollisionSceneForGimmickEncount;
  m_pGimmickEncountManager                  = setupData.pGimmickEncountManager;

  m_ModelIndex = getMoveModelIndex( m_pData->eventID );

  gfl2::math::Vector3 pos( m_pData->position[ 0 ], m_pData->position[ 1 ], m_pData->position[ 2 ] );
  m_vPos = pos;

  m_MoveModelHeaderWork.moveCodeId  = MoveModel::FIELD_MOVE_CODE_GIMMICK_ENCOUNT_ESCAPE;
  m_MoveModelHeaderWork.eventId     = m_pData->eventID;
  m_MoveModelHeaderWork.zoneId      = setupData.zoneId;

  initializeMoveModelWork();

  // Path生成
  CreateTrafficPath( setupData.pHeap, setupData.pAccessor );

  // ギミックエンカウントローカルワークを初期化
  {
    GimmickEncount::DataAccessor gimmickEncountDataAccessor;
    gimmickEncountDataAccessor.SetData( GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetFieldResident()->GetResource( BL_IDX_FIELD_RESIDENT__GIMMICK_ENCOUNT_PACK));

    m_pGimmickEncountLocalWork = m_pGimmickEncountManager->GetEmptyLocalWork();

    GimmickEncount::LocalWork::InitializeDescription desc;
    desc.pTypesData = gimmickEncountDataAccessor.GetData( m_pData->presetID);
    desc.gimmickType = gimmickEncountDataAccessor.GetGimmickType( m_pData->presetID);
    desc.gimmickFlagNo = setupData.gimmickFlagNo;
    desc.firstPosition = m_vPos;
    desc.pActor = this;
    m_pGimmickEncountLocalWork->Initialize( desc);
  }

  SetupMoveCodeWork();
}

/**
 *  @brief  リソースとコリジョンの設定
 */
void FieldGimmickEncountEscapeActor::CreateResource( const CreateResourceData& createResourceData )
{
  FieldCollisionAccessor collisionAccessor;
  FieldCollisionAccessor::CollisionData colData;
  FieldGimmickEncountAccessor* pAccessor;

  m_pTerrainManager               = createResourceData.pTerrainManager;
  m_pRenderingPipeLine            = createResourceData.pRenderingPipeLine;
  pAccessor                       = createResourceData.pAccessor;

  gfl2::math::Vector3     pos( m_pData->position[ 0 ], m_pData->position[ 1 ], m_pData->position[ 2 ] );
  gfl2::math::Quaternion  qua( m_pData->quaternion[ 0 ], m_pData->quaternion[ 1 ], m_pData->quaternion[ 2 ], m_pData->quaternion[ 3 ] );

  m_MoveModelHeaderResource.characterId                             = m_pData->modelID;
  m_MoveModelHeaderResource.pCollisionSceneForGimmickEncountAction  = m_pCollisionSceneForGimmickEncountAction;
  m_MoveModelHeaderResource.pCollisionSceneForGimmickEncount        = m_pCollisionSceneForGimmickEncount;
  m_MoveModelHeaderResource.pos                                     = pos;
  m_MoveModelHeaderResource.qua                                     = qua;

  // リソース初期化
  initializeMoveModelResource();

  Field::MoveModel::FieldMoveModel *pMoveModel = m_pMoveModelManager->GetFieldMoveModel( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );

  // 非表示
  pMoveModel->SetVisible( false );

  gfl2::math::Vector3 modelPos = pMoveModel->GetCharaDrawInstance()->GetPosition();

  // エンカウント開始コリジョンの設定
  {
    const void *p_col = pAccessor->GetEncountCollisionData( *( m_pData ) );
    collisionAccessor.Initialize( p_col );
    if( getCollisionData( &colData, collisionAccessor))
    {
      m_pActorForGimmickEncount = pMoveModel->GetStaticActor( Field::MoveModel::COLLISION_STATIC_TYPE_GIMMICK_ENCOUNT );
      IFieldGimmickEncountActorBase* pBase = this;
      setupCollision( colData, pos, m_pActorForGimmickEncount, pBase);
      m_pActorForGimmickEncount->SetPosition( modelPos );
    }
  }

  // アクション実行コリジョンの設定
  {
    const void *p_col = pAccessor->GetActionCollisionData( *( m_pData ) );
    collisionAccessor.Initialize( p_col );
    if( getCollisionData( &colData, collisionAccessor))
    {
      m_pActorForGimmickEncountAction1 = m_pCollisionSceneForGimmickEncountAction->CreateStaticActor( createResourceData.pAllocator, createResourceData.pHeap);
      IFieldGimmickEncountActorBase* pBase = this;
      setupCollision( colData, pos, m_pActorForGimmickEncountAction1, pBase);
    }
  }
  // アクション実行コリジョンの設定(逃走型は必ず2つある)
  {
    const void *p_col = pAccessor->GetActionCollisionData2( *( m_pData ) );
    collisionAccessor.Initialize( p_col );
    if( getCollisionData( &colData, collisionAccessor))
    {
      m_pActorForGimmickEncountAction2 = m_pCollisionSceneForGimmickEncountAction->CreateStaticActor( createResourceData.pAllocator, createResourceData.pHeap);
      IFieldGimmickEncountActorBase* pBase = this;
      setupCollision( colData, pos, m_pActorForGimmickEncountAction2, pBase);
    }
  }

  // 接地設定
  if( m_pTrafficPath1 && m_pTrafficPath1->CheckFlags( TrafficPath::FLAG_LANDING ) )
  {
    pMoveModel->SetCollisionSceneContainerForGround( m_pTerrainManager->GetCollsionScene() );
  }
  if( m_pTrafficPath2 && m_pTrafficPath2->CheckFlags( TrafficPath::FLAG_LANDING ) )
  {
    pMoveModel->SetCollisionSceneContainerForGround( m_pTerrainManager->GetCollsionScene() );
  }
}

/**
 *  @brief  起動
 */
bool FieldGimmickEncountEscapeActor::Kick( const KickRequest &req, bool * p_outIsEventCreate )
{
  if( p_outIsEventCreate ) *p_outIsEventCreate = false; 

  if( IsInvalid() ){ return false; }

  return false;
}

/**
 *  @brief  判定
 */
bool FieldGimmickEncountEscapeActor::IsInvalid( void )
{
  if( !m_pData ){ return true; }

  return false;
}

/**
 *  @brief ギミックエンカウント配置情報の取得
 */
const FieldGimmickEncountAccessor::Data* FieldGimmickEncountEscapeActor::GetGimmickEncountData() const
{
  return m_pData;
}

/**
 *  @brief イベント通知
 */
void FieldGimmickEncountEscapeActor::Notify( GimmickEncount::NotifyState notify)
{
  MoveModel::FieldMoveCodeGimmickEncountEscape::Work *pWork;
  pWork = this->GetMoveCodeWork();

  if( notify == GimmickEncount::NOTIFY_STATE_ACTION_COLLISION_HIT)
  {
    if( pWork->state < MoveModel::FieldMoveCodeGimmickEncountEscape::STATE_MOVE_PATH)
    {
      // 逃走型は2つのアクション用コリジョンがあるので、
      // アクション用コリジョンに当たった通知が来たときにStaticActorの衝突結果を見てどちらに当たったかを決定する

      BaseActor* pHitActorForAction1 = m_pActorForGimmickEncountAction1->GetCollisionResult( 0)->pTargetActor;
      BaseActor* pHitActorForAction2 = m_pActorForGimmickEncountAction2->GetCollisionResult( 0)->pTargetActor;

      if( pHitActorForAction1 && pHitActorForAction2 == NULL)
      {
        notify = GimmickEncount::NOTIFY_STATE_ACTION_COLLISION_HIT;
        m_pActorForGimmickEncountAction1->InitCollisionResult();
      }
      else if( pHitActorForAction1 == NULL && pHitActorForAction2)
      {
        notify = GimmickEncount::NOTIFY_STATE_ACTION_COLLISION_2_HIT;
        m_pActorForGimmickEncountAction2->InitCollisionResult();
      }
      else if( pHitActorForAction1 == NULL && pHitActorForAction2 == NULL)
      {
        // 当たった通知なのに両方当たっていない。イベントチェック層かコリジョン側の不具合の可能性がある
        GFL_ASSERT( 0);
        notify = GimmickEncount::NOTIFY_STATE_ACTION_COLLISION_HIT;
      }
      else
      {
        // 両方当たった。許容。コリジョン1が優先
        //GFL_ASSERT( 0);
        notify = GimmickEncount::NOTIFY_STATE_ACTION_COLLISION_HIT;
      }
    }
  }

  m_pGimmickEncountLocalWork->Notify( notify);
}

/**
 *  @brief バニッシュ中か
 *  @retval true バニッシュ中
 *  @retval false それ以外
 */
bool FieldGimmickEncountEscapeActor::IsVanish()
{
  MoveModel::FieldMoveCodeGimmickEncountEscape::Work* pWork;
  pWork = GetMoveCodeWork();

  if( pWork == NULL )
  {
    GFL_ASSERT( 0 );
    return true;
  }

  return ( pWork->state == MoveModel::FieldMoveCodeGimmickEncountEscape::STATE_VANISH );
}

/**
 *  @brief  追加
 */
void FieldGimmickEncountEscapeActor::registActor( void )
{
  if( isRegistActor() == false)
  {
    return;
  }

  if( m_pActorForGimmickEncountAction2)
  {
    m_pCollisionSceneForGimmickEncountAction->RegistStaticActor( m_pActorForGimmickEncountAction2);
  }

  if( m_pActorForGimmickEncountAction1)
  {
    m_pCollisionSceneForGimmickEncountAction->RegistStaticActor( m_pActorForGimmickEncountAction1);
  }

  if( m_pActorForGimmickEncount)
  {
    m_pCollisionSceneForGimmickEncount->RegistStaticActor( m_pActorForGimmickEncount);
  }

  registMoveModel();
}

/**
 *  @brief  削除
 */
void FieldGimmickEncountEscapeActor::unregistActor( void )
{
  if( isUnRegistActor() == false)
  {
    return;
  }

  if( m_pActorForGimmickEncountAction2)
  {
    m_pCollisionSceneForGimmickEncountAction->UnRegistStaticActor( m_pActorForGimmickEncountAction2);
  }

  if( m_pActorForGimmickEncountAction1)
  {
    m_pCollisionSceneForGimmickEncountAction->UnRegistStaticActor( m_pActorForGimmickEncountAction1);
  }

  if( m_pActorForGimmickEncount)
  {
    m_pCollisionSceneForGimmickEncount->UnRegistStaticActor( m_pActorForGimmickEncount);
  }

  unregistMoveModel();
}

/**
 *  @brief  破棄共通処理
 *  @retval true 破棄処理を行った
 *  @retval false 破棄処理を行っていない
 */
bool FieldGimmickEncountEscapeActor::terminate()
{
  m_pGimmickEncountLocalWork->Terminate();

  DeleteTrafficPath();

  bool isTerminate =  FieldMoveModelActorBase::terminate();

  GFL_SAFE_DELETE( m_pActorForGimmickEncountAction1 );
  GFL_SAFE_DELETE( m_pActorForGimmickEncountAction2 );

  return isTerminate;
}

/**
 *  @brief  MoveCodeWorkに対してこのクラスでの設定を行う
 */
void FieldGimmickEncountEscapeActor::SetupMoveCodeWork()
{
  MoveModel::FieldMoveCodeGimmickEncountEscape::Work *pWork;
  pWork = this->GetMoveCodeWork();

  if( pWork)
  {
    pWork->pActor = this;
    pWork->pPath[ MoveModel::FieldMoveCodeGimmickEncountEscape::PATH_TYPE_1 ] = m_pTrafficPath1;
    pWork->pPath[ MoveModel::FieldMoveCodeGimmickEncountEscape::PATH_TYPE_2 ] = m_pTrafficPath2;
  }
}

/**
 *  @brief  MoveCodeWorkの取得
 */
MoveModel::FieldMoveCodeGimmickEncountEscape::Work* FieldGimmickEncountEscapeActor::GetMoveCodeWork()
{
  if( m_ModelIndex >= MoveModel::FIELD_MOVE_MODEL_MAX)
  {
    return NULL;
  }

  MoveModel::FieldMoveModel* pChara = m_pMoveModelManager->GetFieldMoveModel( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );

  if( pChara == NULL)
  {
    return NULL;
  }

  u32 size = sizeof( MoveModel::FieldMoveCodeGimmickEncountEscape::Work);
  void* work = MoveModel::FieldMoveCodeManager::GetMoveCodeWork( pChara->GetMoveCodeWork(), size);
  MoveModel::FieldMoveCodeGimmickEncountEscape::Work *pWork;
  pWork = static_cast<Field::MoveModel::FieldMoveCodeGimmickEncountEscape::Work*>( work);

  return pWork;
}

/**
 *  @brief  パスの生成
 */
void FieldGimmickEncountEscapeActor::CreateTrafficPath( gfl2::heap::HeapBase *pHeap, FieldGimmickEncountAccessor *pAccessor )
{
  {
    const Field::Traffic::PathCoreData *pCoreData = pAccessor->GetPathCoreData( *m_pData );
    // リソースからパスデータの取得を行う
    if( pCoreData )
    {
      m_pTrafficPath1 = GFL_NEW( pHeap ) Field::TrafficPath();
      m_pTrafficPath1->Initialize( pHeap );
      m_pTrafficPath1->CreatePath( pCoreData );
    }
  }

  {
    const Field::Traffic::PathCoreData *pCoreData = pAccessor->GetPathCoreData2( *m_pData );
    // リソースからパスデータの取得を行う
    if( pCoreData )
    {
      m_pTrafficPath2 = GFL_NEW( pHeap ) Field::TrafficPath();
      m_pTrafficPath2->Initialize( pHeap );
      m_pTrafficPath2->CreatePath( pCoreData );
    }
  }
}

/**
 *  @brief  パスの破棄
 */
void FieldGimmickEncountEscapeActor::DeleteTrafficPath()
{
  if( m_pTrafficPath1)
  {
    m_pTrafficPath1->EndUse();
    m_pTrafficPath1->DeletePath();
    // Terminateはデストラクタで行うので明示的に呼ばない
    // (Terminate呼ぶとヒープをNULLチェックせずに殺しにいくのでデストラクタで落ちる)
    GFL_DELETE( m_pTrafficPath1);
    m_pTrafficPath1 = NULL;
  }

  if( m_pTrafficPath2)
  {
    m_pTrafficPath2->EndUse();
    m_pTrafficPath2->DeletePath();
    // Terminateはデストラクタで行うので明示的に呼ばない
    // (Terminate呼ぶとヒープをNULLチェックせずに殺しにいくのでデストラクタで落ちる)
    GFL_DELETE( m_pTrafficPath2);
    m_pTrafficPath2 = NULL;
  }
}

GFL_NAMESPACE_END( Field );
