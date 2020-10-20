/**
 *  GAME FREAK inc.
 *
 *  @file   FieldMoveModelActor.cpp
 *  @brief  マップジャンプ用アクター
 *  @author miyachi_soichi
 *  @date   2015.05.28
 */

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldMoveModelActor.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeNpc.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"

// Script
#include "FieldScript/include/EventScriptCall.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// fureai
#include "niji_conv_header\field\script\inc\field_pokemon_fureai.inc"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/PokemonFureai/FieldPokemonFureaiDataAccessor.h"
#include "Field/FieldRo/include/PokemonFureai/FieldPokemonFureaiTool.h"


GFL_NAMESPACE_BEGIN( Field );

/**
 *  @brief  コンストラクタ
 */
FieldMoveModelActor::FieldMoveModelActor( void )
  : FieldMoveModelActorBase()
  , m_pData( NULL )
  , m_pDataForContact( NULL )
  , m_pCollisionSceneForTerrain( NULL )
  , m_pCollisionSceneForStaticModel( NULL )
  , m_pCollisionSceneForEventTalk( NULL )
  , m_pActorForTerrain( NULL )
  , m_pActorForStaticModel( NULL )
  , m_pActorForEventTalk( NULL )
  , m_pActorForEventFureaiTalk( NULL )
  , m_DataZoneId(0)
#if PM_DEBUG
	, m_pDebugFureaiMoveAreaActor( NULL )
#endif // PM_DEBUG

{
	// @fix cov_ctr[12601]:初期化対応追加
	m_FreaiMoveArea.type = FieldCollisionAccessor::TYPE_MAX;
}

/**
 *  @brief  デストラクタ
 */
FieldMoveModelActor::~FieldMoveModelActor( void )
{
  // PlacementDataManager側でワークを残すか完全破棄かを選択するので、
  // デストラクタでTerminate()は呼ばない
}

/**
 *  @brief  初期化
 */
void FieldMoveModelActor::Initialize( MoveModelSetupData &setupData )
{
  m_pMoveModelManager             = setupData.pMoveModelManager;
  m_pData                         = setupData.pMoveModelData;
  m_pDataForContact               = setupData.pContactPokemonData;            // ふれあい用（ふれあいではない場合はNULL）
  m_pCollisionSceneForTerrain     = setupData.pCollisionSceneForTerrain;
  m_pCollisionSceneForStaticModel = setupData.pCollisionSceneForStaticModel;
  m_pCollisionSceneForEventTalk   = setupData.pCollisionSceneForEventTalk;

  gfl2::math::Vector3     pos( m_pData->position[ 0 ], m_pData->position[ 1 ], m_pData->position[ 2 ] );
  gfl2::math::Quaternion  qua( m_pData->quaternion[ 0 ], m_pData->quaternion[ 1 ], m_pData->quaternion[ 2 ], m_pData->quaternion[ 3 ] );
  m_vPos = pos;

  // 復帰処理かチェック
  m_ModelIndex = m_pMoveModelManager->GetFieldMoveModelIndexFromEventId( m_pData->eventID );  // Indexが帰ってきたら復帰処理

  m_MoveModelHeaderWork.moveCodeId                        = FieldMoveModelActorBase::GetPlacementMoveCodeToMoveCode( m_pData->moveCode );

  // ふれあいは特定のコードに
  if (setupData.pContactPokemonData)
  {
    m_MoveModelHeaderWork.moveCodeId = MoveModel::FIELD_MOVE_CODE_FUREAI_A;
  }

  m_MoveModelHeaderWork.eventId                           = m_pData->eventID;
  m_MoveModelHeaderWork.zoneId                            = setupData.zoneId;

  m_MoveModelHeaderResource.pos                           = pos;
  m_MoveModelHeaderResource.qua                           = qua;
  m_MoveModelHeaderResource.characterId                   = m_pData->characterId;
  m_MoveModelHeaderResource.pDressUpParam                 = NULL;
  m_MoveModelHeaderResource.pCollisionSceneForTerrain     = setupData.pCollisionSceneForTerrain;
  m_MoveModelHeaderResource.pCollisionSceneForStaticModel = setupData.pCollisionSceneForStaticModel;
  m_MoveModelHeaderResource.pCollisionSceneForEventPos    = setupData.pCollisionSceneForEventPos;
  m_MoveModelHeaderResource.pCollisionSceneForEventTalk   = setupData.pCollisionSceneForEventTalk;
  m_MoveModelHeaderResource.pCollisionSceneForEventPush   = setupData.pCollisionSceneForEventPush;

  initializeMoveModelWork();
  m_DataZoneId = setupData.dataZoneId;

  GetMoveModel()->SetActorUserWork( this );
}

/**
 *  @brief  リソースとコリジョンの設定
 */
void FieldMoveModelActor::CreateResource( const MoveModelCreateResourceData& createResourceData )
{
  FieldCollisionAccessor collisionAccessor;
  FieldCollisionAccessor::CollisionData colData;
  FieldMoveModelAccessor * pAccessor;
  const EventWork*        cpEventWork;

  m_pTerrainManager               = createResourceData.pTerrainManager;
  m_pRenderingPipeLine            = createResourceData.pRenderingPipeLine;

  // ふれあいの場合はこのアクセサを継承した『FieldContactPokemonAccessor』が渡されます。
  // ┗ 移動領域/予備領域データを取得する際は『FieldContactPokemonAccessor』として扱う必要があります。（ GetMoveAreaCollisionData / GetReserveAreaCollisionData ）
  pAccessor                       = createResourceData.pAccessor;
  cpEventWork                     = createResourceData.cpEventWork;

  // 着せ替えパラメータが設定されていた場合
  if( pAccessor->GetDressupParam() != NULL )
  {
    m_MoveModelHeaderResource.pDressUpParam = pAccessor->GetDressupParam();
    ICHI_PRINT("DressUp設定の動作モデル\n");
  }

  Field::MoveModel::FieldMoveModel *pCharaModel = m_pMoveModelManager->GetFieldMoveModel( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );

  // デフォルトアイドルの設定
  pCharaModel->SetDefaultIdleAnimationId( pAccessor->GetDefaultWaitMotionId( *cpEventWork, m_pData->defaultAnimeData ) );

  // モデル作成
  initializeMoveModelResource( pAccessor->GetDressUpModelResourceManager() );


  // 接地できるよう下準備
  pCharaModel->SetCollisionSceneContainerForGround( m_pTerrainManager->GetCollsionScene() );

  // 非表示
  pCharaModel->SetVisible( false );

  // バイナリデータのポジションからの親座標からのコリジョンのオフセット座標を求めるため、ここは描画InstanceではなくデータのPOSを指定する。
  gfl2::math::Vector3     pos( m_pData->position[ 0 ], m_pData->position[ 1 ], m_pData->position[ 2 ] );
  // 衝突用コリジョンの設定
  {
    const void *p_col = pAccessor->GetCollisionData( *( m_pData ) );
    collisionAccessor.Initialize( p_col );
    if( getCollisionData( &colData, collisionAccessor))
    {
      // 自身を押し返す処理用
      m_pActorForTerrain = pCharaModel->GetDynamicActor( Field::MoveModel::COLLISION_TYPE_TERRAIN );
      setupCollision( colData, pos, m_pActorForTerrain, NULL);
      // 主人公との判定用
      m_pActorForStaticModel = pCharaModel->GetStaticActor( Field::MoveModel::COLLISION_STATIC_TYPE_STATIC_MODEL );
      setupCollision( colData, pos, m_pActorForStaticModel, NULL);
    }
  }

  // 話しかけコリジョンの設定
  {
    const void *p_col = pAccessor->GetTalkCollisionData( *( m_pData ) );
    collisionAccessor.Initialize( p_col );
    if( getCollisionData( &colData, collisionAccessor))
    {

      m_pActorForEventTalk = pCharaModel->GetStaticActor( Field::MoveModel::COLLISION_STATIC_TYPE_EVENT_TALK );
      setupCollision( colData, pos, m_pActorForEventTalk, this);
      
      // 話しかけコリジョン
      if (IsContactPokemon())
      {
        m_pActorForEventFureaiTalk = pCharaModel->GetStaticActor(Field::MoveModel::COLLISION_STATIC_TYPE_EVENT_FUREAI_TALK);
        colData.cylinder.radius = 300;
        setupCollision(colData, pos, m_pActorForEventFureaiTalk, this);
      }
    }
  }

  if ( IsContactPokemon() )
  {
    FieldContactPokemonAccessor* pContactPokemonAccessor = static_cast<FieldContactPokemonAccessor*>(pAccessor);
    const void *p_col = pContactPokemonAccessor->GetMoveAreaCollisionData(*(m_pDataForContact));
    collisionAccessor.Initialize(p_col);
    if (getCollisionData( &m_FreaiMoveArea, collisionAccessor))
    {
#if PM_DEBUG
			gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);
			System::nijiAllocator* allocator = System::GflUse::GetDebugAllocator();
			m_pDebugFureaiMoveAreaActor = m_pCollisionSceneForStaticModel->CreateStaticActor(allocator, heap);
			if (m_pDebugFureaiMoveAreaActor)
			{
				gfl2::math::Vector3 pos_work(0.0f, 0.0f, 0.0f);
				setupCollision(m_FreaiMoveArea, pos_work, m_pDebugFureaiMoveAreaActor, NULL);
				m_pDebugFureaiMoveAreaActor->SetActive( false );
			}
#endif // PM_DEBUG
    }


    // 処理負荷対策、自動接地設置をOFF
    if (m_pDataForContact->is_forceLanding)
    {
      pCharaModel->SetForceLanding(true);
    }
    else
    {
      pCharaModel->SetForceLanding(false);
    }
  }

  // 処理負荷対策:アニメーション更新をOFF @note CreateResource直後はUnRegist扱い
  if (IsAnimationStopTarget())
  {
    pCharaModel->OnStateBit(Field::MoveModel::FIELD_MOVE_MODEL_ANIM_STOP_ACTOR_UNREGIST);
  }
}

void FieldMoveModelActor::TerminateResource(void)
{
#if PM_DEBUG
	if (m_pDebugFureaiMoveAreaActor)
	{
		m_pCollisionSceneForStaticModel->UnRegistStaticActor(m_pDebugFureaiMoveAreaActor);
	  GFL_SAFE_DELETE(m_pDebugFureaiMoveAreaActor);
	}
#endif // PM_DEBUG
}

/**
 *  @brief  起動
 */
bool FieldMoveModelActor::Kick( const KickRequest &req, bool * p_outIsEventCreate )
{
  if( p_outIsEventCreate ) *p_outIsEventCreate = false; 
  if( !m_pData ){ return false; }
  if (IsContactPokemon() )
  {
    if (m_pData->scriptID == FieldScript::SCRID_NULL)
    {
      EventScriptCall::CallScript(GameSys::GameManager::GetInstance(), SCRID_FIELD_POKEMON_FUREAI, this, this->GetMoveModel());
    }
    else
    {
      EventScriptCall::CallScript(GameSys::GameManager::GetInstance(), m_pData->scriptID, this, this->GetMoveModel());
    }
  }
  else
  {
    // NPC会話を実行した
    if (Field::MoveModel::FieldMoveCodeManager::IsNpcMoveCode(GetMoveModel()->GetMoveCodeId()))
    {
      MoveModel::FieldMoveCodeNpc::TalkStart(GetMoveModel()->GetMoveCodeWork());
    }

    EventScriptCall::CallScript(GameSys::GameManager::GetInstance(), m_pData->scriptID, this, this->GetMoveModel());
  }
  if (p_outIsEventCreate) *p_outIsEventCreate = true;
  return true;
}

/**
 *  @brief  判定
 */
bool FieldMoveModelActor::IsInvalid( void )
{
  if( !m_pData ){ return true; }
  if( !IsContactPokemon() && m_pData->scriptID == FieldScript::SCRID_NULL ){ return true; }

  return false;
}

/**
 *  @brief  動作モデル取得
 */
MoveModel::FieldMoveModel *FieldMoveModelActor::GetMoveModel( void ) const
{
  return m_pMoveModelManager->GetFieldMoveModel( static_cast<Field::MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );
}

/**
*  @brief  動作モデル初期化判定
*/
bool FieldMoveModelActor::IsSetupMoveModel( void ) const
{
  if( m_ModelIndex >= Field::MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    return false;
  }

  MoveModel::FieldMoveModel* pMoveModel = GetMoveModel();
  if( pMoveModel == NULL || !pMoveModel->IsSetup() )
  {
    return false;
  }

  return true;
}

/**
*  @brief  NPC判定
*/
bool FieldMoveModelActor::IsNpc( void ) const
{
  return Field::MoveModel::FieldMoveCodeManager::IsNpcMoveCode( GetMoveModel()->GetMoveCodeId() );
}

/**
*  @brief  NPCの注目を停止する
*/
void FieldMoveModelActor::StopNpcInterest( void )
{
  if( Field::MoveModel::FieldMoveCodeManager::IsNpcMoveCode( GetMoveModel()->GetMoveCodeId() ) )
  {
    MoveModel::FieldMoveCodeNpc::StopInterest( GetMoveModel()->GetMoveCodeWork() );
  }
}

/**
*  @brief  ふれあいポケモンのねむり状態を開始する
*/
void FieldMoveModelActor::StartFrureaiSleep(void)
{
	// 話しかけ範囲を狭くする
	if (m_pActorForEventFureaiTalk)
	{
		m_pActorForEventFureaiTalk->SetActive(false);
	}
}

/**
 *  @brief  ふれあいポケモンのねむり状態を停止する
 */
void FieldMoveModelActor::StopFrureaiSleep(void)
{
  // 話しかけ範囲を広くする
  if (m_pActorForEventFureaiTalk)
  {
    m_pActorForEventFureaiTalk->SetActive(true);
  }
}

/**
*  @brief  ふれあいポケモンの注目を停止する
*/
void FieldMoveModelActor::StopContactInterest(void)
{
  if (IsContactPokemon())
  {
    MoveModel::FieldMoveCodeFureaiTypeA::StopInterest(GetMoveModel()->GetMoveCodeWork());
  }
}

/**
*  @brief  アクターの状態をバニッシュフラグに伝播する
*/
void FieldMoveModelActor::ApplyActorToVanishFlag( EventWork* pEventWork )
{
  if( !m_pData ){ return; }

  u32 flagwork = m_pData->flagwork;
  if( flagwork != 0 && EventWork::CheckVanishiFlagRenge( flagwork ) )
  {
    if( IsRegist() )
    {
      pEventWork->ResetEventFlag( flagwork );   // バニッシュフラグを下げる
    }
    else
    {
      pEventWork->SetEventFlag( flagwork );     // バニッシュフラグを立てる
    }
  }
}

/**
*  @brief  ふれあい表現用かどうか判定
*/
bool FieldMoveModelActor::IsContactPokemon(void) const
{
  if (m_pDataForContact == NULL)
  {
    return false;
  }
  return true;
}

/**
*  @brief  ふれあいポケモンの移動領域データを取得
*/
void FieldMoveModelActor::GetFreaiMoveAreaData(FieldCollisionAccessor::CollisionData* pData)
{
  *pData = m_FreaiMoveArea;
}


/**
*  @brief  ふれあいポケモンのベース位置を取得
*/
void FieldMoveModelActor::GetFureaiBasePosition(gfl2::math::Vector3* pOut)
{
  gfl2::math::Vector3     pos(m_pData->position[0], m_pData->position[1], m_pData->position[2]);
  *pOut = pos;
}

/**
*  @brief  ふれあいポケモンのベース向きを取得
*/
void FieldMoveModelActor::GetFureaiBaseQuaternion(gfl2::math::Quaternion* pOut)
{
  gfl2::math::Quaternion  qua( m_pData->quaternion[ 0 ], m_pData->quaternion[ 1 ], m_pData->quaternion[ 2 ], m_pData->quaternion[ 3 ] );
	*pOut = qua;
}

const Field::PokemonFureai::POKEMON_SETTING_DATA* FieldMoveModelActor::GetFureaiPokemonSettingData(void)
{
  // ふれあい設定
  Field::FieldResident* pFieldResident = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldResident();
  Field::PokemonFureai::DataAccessor dataAccessor(pFieldResident->GetResource(BL_IDX_FIELD_RESIDENT__POKEMON_FUREAI_BIN));
  return dataAccessor.GetSettingData( GetMoveModel()->GetCharacterId());
}

  /**
*  @brief 主人公がアクションを行った事をふれあいアクターに通知
*  @note イベント中にふれあいポケモンが喜ぶ動作を行った場合、コール
*/
void FieldMoveModelActor::EventPlayerActionReq( s32 type )
{
	MoveModel::FieldMoveCodeFureaiTypeA::EventPlayerActionReq(GetMoveModel()->GetMoveCodeWork(), type );
}

/**
 *  @brief  追加
 */
void FieldMoveModelActor::registActor( void )
{
  // ユーザー指定のRegist:true　Culling:falseのときのみ本当に登録
  if( isRegistActor())
  {
    if( m_pActorForTerrain && m_pActorForStaticModel )
    {
      m_pCollisionSceneForTerrain->RegistDynamicActor( m_pActorForTerrain );
      m_pCollisionSceneForStaticModel->RegistStaticActor( m_pActorForStaticModel );
    }
    if( m_pActorForEventTalk )
    {
      m_pCollisionSceneForEventTalk->RegistStaticActor( m_pActorForEventTalk );
    }
    if( m_pActorForEventFureaiTalk )
    {
      m_pCollisionSceneForEventTalk->RegistStaticActor( m_pActorForEventFureaiTalk );
    }

#if PM_DEBUG
			if (m_pDebugFureaiMoveAreaActor)
			{
				m_pCollisionSceneForStaticModel->RegistStaticActor(m_pDebugFureaiMoveAreaActor);
			}
#endif // PM_DEBUG

    registMoveModel();


    // 処理負荷対策ニメーション更新をON
    Field::MoveModel::FieldMoveModel* pMoveModel = GetMoveModel();
    if (pMoveModel->CheckStateBit(Field::MoveModel::FIELD_MOVE_MODEL_ANIM_STOP_ACTOR_UNREGIST))
    {
      pMoveModel->OffStateBit(Field::MoveModel::FIELD_MOVE_MODEL_ANIM_STOP_ACTOR_UNREGIST );
    }
  }
}

/**
 *  @brief  削除
 */
void FieldMoveModelActor::unregistActor( void )
{
  // ユーザー指定のRegist:false　か　Culling:trueのときは、unregist
  if( isUnRegistActor() )
  {
    if( m_pActorForTerrain && m_pActorForStaticModel )
    {
      m_pCollisionSceneForTerrain->UnRegistDynamicActor( m_pActorForTerrain );
      m_pCollisionSceneForStaticModel->UnRegistStaticActor( m_pActorForStaticModel );
    }
    if( m_pActorForEventTalk )
    {
      m_pCollisionSceneForEventTalk->UnRegistStaticActor( m_pActorForEventTalk );
    }
    if( m_pActorForEventFureaiTalk )
    {
      m_pCollisionSceneForEventTalk->UnRegistStaticActor( m_pActorForEventFureaiTalk );
    }

#if PM_DEBUG
		if (m_pDebugFureaiMoveAreaActor)
		{
			m_pCollisionSceneForTerrain->UnRegistStaticActor(m_pDebugFureaiMoveAreaActor);
		}
#endif // PM_DEBUG

    unregistMoveModel();

    // 処理負荷対策:アニメーション更新をOFF
    Field::MoveModel::FieldMoveModel* pMoveModel = GetMoveModel();
    if (pMoveModel)
    {
      if (IsAnimationStopTarget())
      {
        pMoveModel->OnStateBit(Field::MoveModel::FIELD_MOVE_MODEL_ANIM_STOP_ACTOR_UNREGIST);
      }
    }
  }
}

#if PM_DEBUG
void FieldMoveModelActor::SetDebugFureaiMoveAreaCollision(b32 flag)
{
	if (m_pDebugFureaiMoveAreaActor)
	{
		m_pDebugFureaiMoveAreaActor->SetActive( flag );
	}
}
#endif // #if PM_DEBUG


bool FieldMoveModelActor::IsActionButtonInterestClear() const
{
  // @fix GFMMcat[396]:ふれあいポケモンへの注視が不要に継続する、対処。スクリプトイベント中はふれあいの注視リクエストを無視するように修正した際のケア。イベント対象がふれあいポケモンの場合は注視を継続。
  if (IsContactPokemon())
  {
    return false;
  }
  else
  {
    return true;
  }
}

/**
*  @brief  処理負荷対策:アンレジスト時にアニメーションを停止する対象か？
*/
b32 FieldMoveModelActor::IsAnimationStopTarget(void) const
{
  switch (m_MoveModelHeaderWork.moveCodeId)
  {
  case MoveModel::FIELD_MOVE_CODE_FUREAI_A:
  case MoveModel::FIELD_MOVE_CODE_NPC_AZUKARIYA:
  {
    return false;
  }
  }

  return true;
}


GFL_NAMESPACE_END( Field );
