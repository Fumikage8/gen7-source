//======================================================================
/**
 * @file FieldGimmickEncountPatrolActor.cpp
 * @date 2015/08/19 19:03:22
 * @author saita_kazuki
 * @brief ギミックエンカウント巡回型アクター
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountPatrolActor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldGimmickEncountAccessor.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficPath.h"

#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountDataAccessor.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountData.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountManager.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"

#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeGimmickEncountPatrol.h"

#include "GameSys/include/GameManager.h"

GFL_NAMESPACE_BEGIN( Field );

/**
 *  @brief  コンストラクタ
 */
FieldGimmickEncountPatrolActor::FieldGimmickEncountPatrolActor( void )
  : FieldMoveModelActorBase()
  , IFieldGimmickEncountActorBase()
  , m_pData( NULL )
  , m_pCollisionSceneForGimmickEncountAction( NULL )
  , m_pCollisionSceneForGimmickEncount( NULL )
  , m_pActorForGimmickEncount( NULL )
  , m_pGimmickEncountManager( NULL)
  , m_pGimmickEncountLocalWork( NULL)
{
}

/**
 *  @brief  デストラクタ
 */
FieldGimmickEncountPatrolActor::~FieldGimmickEncountPatrolActor( void )
{
  // PlacementDataManager側でワークを残すか完全破棄かを選択するので、
  // デストラクタでTerminate()は呼ばない
}

/**
 *  @brief  初期化
 */
void FieldGimmickEncountPatrolActor::Initialize( SetupData &setupData )
{
  m_pMoveModelManager                       = setupData.pMoveModelManager;
  m_pData                                   = setupData.pGimmickEncountData;
  m_pCollisionSceneForGimmickEncount        = setupData.pCollisionSceneForGimmickEncount;
  m_pGimmickEncountManager                  = setupData.pGimmickEncountManager;

  m_ModelIndex = getMoveModelIndex( m_pData->eventID );

  gfl2::math::Vector3 pos( m_pData->position[ 0 ], m_pData->position[ 1 ], m_pData->position[ 2 ] );
  m_vPos = pos;

  m_MoveModelHeaderWork.moveCodeId  = MoveModel::FIELD_MOVE_CODE_GIMMICK_ENCOUNT_PATROL;
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
void FieldGimmickEncountPatrolActor::CreateResource( const CreateResourceData& createResourceData )
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

  // 往来接地設定
  if( m_pTrafficPath && m_pTrafficPath->CheckFlags( TrafficPath::FLAG_LANDING ) )
  {
    pMoveModel->SetCollisionSceneContainerForGround( m_pTerrainManager->GetCollsionScene() );
  }
}

/**
 *  @brief  起動
 */
bool FieldGimmickEncountPatrolActor::Kick( const KickRequest &req, bool * p_outIsEventCreate )
{
  if( p_outIsEventCreate ) *p_outIsEventCreate = false; 

  if( IsInvalid() ){ return false; }

  return false;
}

/**
 *  @brief  判定
 */
bool FieldGimmickEncountPatrolActor::IsInvalid( void )
{
  if( !m_pData ){ return true; }

  return false;
}

/**
 *  @brief ギミックエンカウント配置情報の取得
 */
const FieldGimmickEncountAccessor::Data* FieldGimmickEncountPatrolActor::GetGimmickEncountData() const
{
  return m_pData;
}

/**
 *  @brief イベント通知
 */
void FieldGimmickEncountPatrolActor::Notify( GimmickEncount::NotifyState notify)
{
  m_pGimmickEncountLocalWork->Notify( notify);
}

/**
 *  @brief  追加
 */
void FieldGimmickEncountPatrolActor::registActor( void )
{
  if( isRegistActor() == false)
  {
    return;
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
void FieldGimmickEncountPatrolActor::unregistActor( void )
{
  if( isUnRegistActor() == false)
  {
    return;
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
bool FieldGimmickEncountPatrolActor::terminate()
{
  m_pGimmickEncountLocalWork->Terminate();

  DeleteTrafficPath();

  return FieldMoveModelActorBase::terminate();
}

/**
 *  @brief  パスの生成
 */
void FieldGimmickEncountPatrolActor::CreateTrafficPath( gfl2::heap::HeapBase *pHeap, FieldGimmickEncountAccessor *pAccessor )
{
  const Field::Traffic::PathCoreData *pCoreData = pAccessor->GetPathCoreData( *m_pData );
  // リソースからパスデータの取得を行う
  if( pCoreData )
  {
    m_pTrafficPath = GFL_NEW( pHeap ) Field::TrafficPath();
    m_pTrafficPath->Initialize( pHeap );
    m_pTrafficPath->CreatePath( pCoreData );
    m_pTrafficPath->StartUse();
  }
}

/**
 *  @brief  パスの破棄
 */
void FieldGimmickEncountPatrolActor::DeleteTrafficPath()
{
  if( m_pTrafficPath)
  {
    m_pTrafficPath->EndUse();
    m_pTrafficPath->DeletePath();
    // Terminateはデストラクタで行うので明示的に呼ばない
    // (Terminate呼ぶとヒープをNULLチェックせずに殺しにいくのでデストラクタで落ちる)
    GFL_DELETE( m_pTrafficPath);
    m_pTrafficPath = NULL;
  }
}

/**
 *  @brief  MoveCodeWorkに対してこのクラスでの設定を行う
 */
void FieldGimmickEncountPatrolActor::SetupMoveCodeWork()
{
  Field::MoveModel::FieldMoveModel* pChara = m_pMoveModelManager->GetFieldMoveModel( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );
  u32 size = sizeof( Field::MoveModel::FieldMoveCodeGimmickEncountPatrol::Work);
  void* work = MoveModel::FieldMoveCodeManager::GetMoveCodeWork( pChara->GetMoveCodeWork(), size);
  Field::MoveModel::FieldMoveCodeGimmickEncountPatrol::Work *pWork;
  pWork = static_cast<Field::MoveModel::FieldMoveCodeGimmickEncountPatrol::Work*>( work);
  pWork->pActor = this;
  pWork->pPath = m_pTrafficPath;
}

GFL_NAMESPACE_END( Field );
