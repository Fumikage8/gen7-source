//======================================================================
/**
 * @file FieldGimmickEncountStandbyActor.cpp
 * @date 2015/08/24 16:55:44
 * @author saita_kazuki
 * @brief ギミックエンカウント待機型アクター
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountStandbyActor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldGimmickEncountAccessor.h"

#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"
#include "Field/FieldRo/include/StaticModel/StaticModel.h"
#include "Field/FieldRo/include/StaticModel/StaticModelDefine.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountDataAccessor.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountData.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountManager.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "GameSys/include/GameManager.h"
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN( Field );

/**
 *  @brief  コンストラクタ
 */
FieldGimmickEncountStandbyActor::FieldGimmickEncountStandbyActor( void )
  : FieldStaticModelActorBase()
  , IFieldGimmickEncountActorBase()
  , m_pData( NULL )
  , m_pCollisionSceneForGimmickEncountAction( NULL )
  , m_pCollisionSceneForGimmickEncount( NULL )
  , m_pActorForGimmickEncountAction( NULL )
  , m_pActorForGimmickEncount( NULL )
  , m_pGimmickEncountManager( NULL)
  , m_pGimmickEncountLocalWork( NULL)
  , m_isDrawOnly( false)
  , m_isAnimationPlaying( false)
{
}

/**
 *  @brief  デストラクタ
 */
FieldGimmickEncountStandbyActor::~FieldGimmickEncountStandbyActor( void )
{
  this->Terminate();
}

/**
 *  @brief  ワーク初期化
 */
void FieldGimmickEncountStandbyActor::InitializeWork( SetupWorkData &setupData )
{
  m_pData                  = setupData.pGimmickEncountData;
  m_pGimmickEncountManager = setupData.pGimmickEncountManager;
  m_isDrawOnly             = setupData.isDrawOnly;

  gfl2::math::Vector3     pos( m_pData->position[ 0 ], m_pData->position[ 1 ], m_pData->position[ 2 ] );
  m_vPos = pos;

  // ギミックエンカウントローカルワークを初期化
  {
    GimmickEncount::DataAccessor gimmickEncountDataAccessor;
    gimmickEncountDataAccessor.SetData( GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetFieldResident()->GetResource( BL_IDX_FIELD_RESIDENT__GIMMICK_ENCOUNT_PACK));

    m_pGimmickEncountLocalWork = m_pGimmickEncountManager->GetEmptyLocalWork();

    GimmickEncount::LocalWork::InitializeDescription desc;
    desc.pTypesData = gimmickEncountDataAccessor.GetData( m_pData->presetID);
    desc.gimmickType = gimmickEncountDataAccessor.GetGimmickType( m_pData->presetID);
    desc.gimmickFlagNo = setupData.gimmickFlagNo;
    desc.firstPosition = pos;
    desc.pActor = this;
    m_pGimmickEncountLocalWork->Initialize( desc);
  }

  // 待機型はローカルワークの現在位置を更新しておく
  {
    GimmickEncount::LocalWork::UpdateDescription desc;
    desc.nowPosition = m_vPos;
    m_pGimmickEncountLocalWork->Update( desc);
  }

  IFieldGimmickEncountActorBase* pThis = this;
  m_pGimmickEncountManager->RegistActor( pThis);
}

/**
 *  @brief  初期化
 */
void FieldGimmickEncountStandbyActor::Initialize( SetupData &setupData )
{
  m_pStaticModelManager                     = setupData.pStaticModelManager;
  m_pCollisionSceneForGimmickEncountAction  = setupData.pCollisionSceneForGimmickEncountAction;
  m_pCollisionSceneForGimmickEncount        = setupData.pCollisionSceneForGimmickEncount;
  m_pTerrainManager                         = setupData.pTerrainManager;
  m_pRenderingPipeLine                      = setupData.pRenderingPipeLine;
  m_pData                                   = setupData.pGimmickEncountData;

  gfl2::math::Vector3     pos( m_pData->position[ 0 ], m_pData->position[ 1 ], m_pData->position[ 2 ] );
  gfl2::math::Quaternion  qua( m_pData->quaternion[ 0 ], m_pData->quaternion[ 1 ], m_pData->quaternion[ 2 ], m_pData->quaternion[ 3 ] );
  m_vPos = pos;

  // 配置モデルの生成
  {
    m_pStaticModel = setupData.pStaticModelManager->CreateModel(
      m_pData->modelID,
      m_pData->eventID,
      pos,
      qua
      );

    m_pStaticModel->SetVisibleAll(false);  // 初期状態は全て非表示
    m_pStaticModel->SetStaticActor(NULL);

  }

  // 表示のみならコリジョンは作成しないで終了
  if( m_isDrawOnly)
  {
    return;
  }

  FieldCollisionAccessor collisionAccessor;
  FieldCollisionAccessor::CollisionData colData;

  FieldGimmickEncountAccessor* pAccessor;
  pAccessor = setupData.pAccessor;

  // エンカウント開始コリジョンの設定
  {
    const void *p_col = pAccessor->GetEncountCollisionData( *( m_pData ) );
    collisionAccessor.Initialize( p_col );
    if( getCollisionData( &colData, collisionAccessor))
    {
      m_pActorForGimmickEncount = m_pCollisionSceneForGimmickEncount->CreateStaticActor( setupData.pAllocator, setupData.pHeap);
      IFieldGimmickEncountActorBase* pBase = this;
      setupCollision( colData, pos, m_pActorForGimmickEncount, pBase);
    }
  }

  // アクション実行コリジョンの設定
  {
    const void *p_col = pAccessor->GetActionCollisionData( *( m_pData ) );
    collisionAccessor.Initialize( p_col );
    if( getCollisionData( &colData, collisionAccessor))
    {
      m_pActorForGimmickEncountAction = m_pCollisionSceneForGimmickEncountAction->CreateStaticActor( setupData.pAllocator, setupData.pHeap);
      IFieldGimmickEncountActorBase* pBase = this;
      setupCollision( colData, pos, m_pActorForGimmickEncountAction, pBase);
    }
  }
}

/**
 *  @brief  起動
 */
bool FieldGimmickEncountStandbyActor::Kick( const KickRequest &req, bool * p_outIsEventCreate )
{
  if( p_outIsEventCreate ) *p_outIsEventCreate = false; 

  if( IsInvalid() ){ return false; }

  return false;
}

/**
 *  @brief  判定
 */
bool FieldGimmickEncountStandbyActor::IsInvalid( void )
{
  if( !m_pData ){ return true; }

  return false;
}

/**
 *  @brief ギミックエンカウント配置情報の取得
 */
const FieldGimmickEncountAccessor::Data* FieldGimmickEncountStandbyActor::GetGimmickEncountData() const
{
  return m_pData;
}

/**
 *  @brief イベント通知
 */
void FieldGimmickEncountStandbyActor::Notify( GimmickEncount::NotifyState notify)
{
  m_pGimmickEncountLocalWork->Notify( notify);
}

/**
 *  @brief 更新
 */
void FieldGimmickEncountStandbyActor::Update()
{
  GimmickEncount::NotifyState state = m_pGimmickEncountLocalWork->GetNotifyState();
  GimmickEncount::NotifyState stateOld = m_pGimmickEncountLocalWork->GetNotifyStateOld();
  switch( state)
  {
  // なし
  case GimmickEncount::NOTIFY_STATE_NONE:
    {
      StaticModel::StaticModel::Handle_IsEndEventAnimation isEndHandle( 0);
      // アニメーション中 かつ 再生終了していたら停止
      if( m_isAnimationPlaying && m_pStaticModel->IsEnd( isEndHandle))
      {
        // 待機アニメーションを停止
        StaticModel::StaticModel::Handle_StopEventAnimation handle( 0);
        m_pStaticModel->Set( &handle );

        GimmickEncount::StandbyData* pData = reinterpret_cast<GimmickEncount::StandbyData*>( m_pGimmickEncountLocalWork->GetTypesData());
        if( pData->actionSEID != SMID_NULL)
        {
          Sound::StopSE( pData->actionSEID);
        }

        m_isAnimationPlaying = false;
      }
    }
    break;

  // 予備動作コリジョンにヒット
  case GimmickEncount::NOTIFY_STATE_ACTION_COLLISION_HIT:
    {
      // アニメーションしていなかったら再生
      if( m_isAnimationPlaying == false)
      {
        StaticModel::StaticModel::Handle_StartEventAnimation handle( 0, 1.0f, true);
        m_pStaticModel->Set( &handle );

        GimmickEncount::StandbyData* pData = reinterpret_cast<GimmickEncount::StandbyData*>( m_pGimmickEncountLocalWork->GetTypesData());
        if( pData->actionSEID != SMID_NULL)
        {
          Sound::PlaySE( pData->actionSEID);
        }

        m_isAnimationPlaying = true;
      }
    }
    break;

  // エンカウントコリジョンにヒット
  case GimmickEncount::NOTIFY_STATE_ENCOUNT_COLLISION_HIT:
    {
      // 前回通知が「予備動作コリジョンにヒット」でSEが鳴っていたらSE停止
      if( stateOld == GimmickEncount::NOTIFY_STATE_ACTION_COLLISION_HIT )
      {
        GimmickEncount::StandbyData* pData = reinterpret_cast<GimmickEncount::StandbyData*>( m_pGimmickEncountLocalWork->GetTypesData());
        if( pData->actionSEID != SMID_NULL && Sound::IsSEPlaying( pData->actionSEID))
        {
          Sound::StopSE( pData->actionSEID);
        }
      }
    }
    break;

  default:
    // 何もしない
    break;
  }
}

/**
 *  @brief  追加
 */
void FieldGimmickEncountStandbyActor::registActor( void )
{
  if( isNeedRegistActor() == false)
  {
    return;
  }

  if( m_pActorForGimmickEncountAction)
  {
    m_pCollisionSceneForGimmickEncountAction->RegistStaticActor( m_pActorForGimmickEncountAction);
  }

  if( m_pActorForGimmickEncount)
  {
    m_pCollisionSceneForGimmickEncount->RegistStaticActor( m_pActorForGimmickEncount);
  }

  this->visibleStaticModel();
}

/**
 *  @brief  削除
 */
void FieldGimmickEncountStandbyActor::unregistActor( void )
{
  if( isNeedUnRegistActor() == false)
  {
    return;
  }

  if( m_pActorForGimmickEncountAction)
  {
    m_pCollisionSceneForGimmickEncountAction->UnRegistStaticActor( m_pActorForGimmickEncountAction);
  }

  if( m_pActorForGimmickEncount)
  {
    m_pCollisionSceneForGimmickEncount->UnRegistStaticActor( m_pActorForGimmickEncount);
  }

  this->invisibleStaticModel();
}

/**
 *  @brief  破棄共通処理
 *  @retval true 破棄処理を行った
 *  @retval false 破棄処理を行っていない
 */
bool FieldGimmickEncountStandbyActor::terminate()
{
  // ループSEの再生を止める
  if( m_pGimmickEncountLocalWork)
  {
    GimmickEncount::StandbyData* pData = reinterpret_cast<GimmickEncount::StandbyData*>( m_pGimmickEncountLocalWork->GetTypesData());
    if( pData)
    {
      u32 seID = pData->actionSEID;
      // 再生されていないSEをStopSEで止めることによる問題はないので問答無用で止める
      if( seID != SMID_NULL)
      {
        Sound::StopSE( seID);
      }
    }

    m_pGimmickEncountLocalWork->Terminate();
  }

  IFieldGimmickEncountActorBase* pThis = this;
  m_pGimmickEncountManager->UnRegistActor( pThis);

  bool isTerminate;
  isTerminate = FieldStaticModelActorBase::terminate();

  GFL_SAFE_DELETE( m_pActorForGimmickEncountAction);
  GFL_SAFE_DELETE( m_pActorForGimmickEncount);

  m_isAnimationPlaying = false;

  return isTerminate;
}

GFL_NAMESPACE_END( Field );
