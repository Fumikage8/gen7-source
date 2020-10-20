 /**
 *  GAME FREAK inc.
 *
 *  @file   FieldFishingSpotActor.cpp
 *  @brief  釣りスポットアクター
 *  @author Masanori Kanamaru
 *  @date   2015.10.23
 */

#include "Field/FieldRo/include/PlacementData/actor/FieldfishingSpotActor.h"

#include "Field/FieldStatic/include/Event/EventFishingAction.h"

#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"

#include "Sound/include/Sound.h"

#include "GameSys/include/GameData.h"

GFL_NAMESPACE_BEGIN( Field )

/**
 *  @brief  コンストラクタ
 */
FieldFishingSpotActor::FieldFishingSpotActor()
  : FieldStaticModelActorBase()
{}

/**
 *  @brief  種別取得
 */
FieldActorType FieldFishingSpotActor::GetType() const
{
  return FIELD_ACTOR_TYPE_FISHING_SPOT;
}

/**
 *  @brief  初期化
 */
void FieldFishingSpotActor::Initialize(
  u32 actorIndex,
  const FieldFishingSpotAccessor::Data* pData,
  const CollisionSetupData& watchingColSetupData,
  const CollisionSetupData& actionColSetupData,
  System::nijiAllocator* pAllocator,
  gfl2::heap::HeapBase* pHeap
  )
{
  gfl2::math::Vector3 pos(pData->position[0], pData->position[1], pData->position[2]);

  this->m_vPos = pos;

  IFieldActorBase::CreateCollisionInstance(&(this->watchingArea), watchingColSetupData, pos, pAllocator, pHeap, this);
  IFieldActorBase::CreateCollisionInstance(&(this->actionArea), actionColSetupData, pos, pAllocator, pHeap, this);

  // 配置モデル用データを作成
  this->staticModelData.position[0] = pData->position[0];
  this->staticModelData.position[1] = pData->position[1];
  this->staticModelData.position[2] = pData->position[2];

  this->staticModelData.quaternion[0] = pData->quaternion[0];
  this->staticModelData.quaternion[1] = pData->quaternion[1];
  this->staticModelData.quaternion[2] = pData->quaternion[2];
  this->staticModelData.quaternion[3] = pData->quaternion[3];

  this->staticModelData.staticModelID = pData->staticModelID;

  this->pData = pData;
  this->actorIndex = actorIndex;
}

/**
 *  @brief  破棄
 */
void FieldFishingSpotActor::Terminate()
{
  this->terminate();

  this->UnregistActor();

  GFL_SAFE_DELETE(this->watchingArea.pActor);
  GFL_SAFE_DELETE(this->actionArea.pActor);
}
  
/**
 *  @brief  シーンへの登録
 */
void FieldFishingSpotActor::registActor()
{
  // ユーザー指定のRegist:true　Culling:falseのときのみ本当に登録
  if( isNeedRegistActor() )
  {
    // コリジョンシーンへの登録
    this->registCollisionScene();

    // 配置モデルの表示ON
    this->mist.Initialize(this, this->ExistRare());

    IFieldActorBase::RegisterCollision(&(this->watchingArea));
    IFieldActorBase::RegisterCollision(&(this->actionArea));
  }
}

/**
 *  @brief  シーンから除去
 */
void FieldFishingSpotActor::unregistActor()
{
  // ユーザー指定のRegist:false　か　Culling:trueのときは、unregist
  if( isNeedUnRegistActor() )
  {
    // コリジョンシーンからの削除
    this->unregistCollisionScene();

    // 配置モデルの表示OFF
    this->invisibleStaticModel();

    IFieldActorBase::UnregisterCollision(&(this->watchingArea));
    IFieldActorBase::UnregisterCollision(&(this->actionArea));
  }
}

/**
 *  @breif  レア判定を消す
 */
void FieldFishingSpotActor::EscapeRare()
{
  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFishingSpotData()->EscapeRare(this->actorIndex);
}

FieldFishingSpotActor::Mist* FieldFishingSpotActor::GetMist()
{
  return &(this->mist);
}

/**
 *  @brief  起動
 *  @param  req パラメータ
 *  @param  p_outIsEventCreate  GameEventを生成したかどうか？ true :起動 false :起動していない
 *  @retval 何かしら処理を実行したか
 */
bool FieldFishingSpotActor::Kick( const KickRequest &req, bool * p_outIsEventCreate )
{
  // イベント起動したかフラグをクリアする
  if( p_outIsEventCreate != NULL)
  {
    *p_outIsEventCreate = false;
  }

  bool res = false;

  if(req.pActor == this->watchingArea.pActor)
  {
    res = this->KickEscape(req);
  }
  else if (req.pActor == this->actionArea.pActor)
  {
    res = this->KickAction(req);
  }
  else
  {
    GFL_ASSERT_MSG(false, "Fish Spot : Why?\n");
    res = false;
  }

  if( p_outIsEventCreate != NULL)
  {
    *p_outIsEventCreate = res;
  }

  return res;
}

/**
 *  @brief  レア逃走判定と実行
 */
bool FieldFishingSpotActor::KickEscape(const KickRequest &req)
{
  KANAMARU_PRINT("FISH SPOT : START WATCHING\n");

  if (this->ExistRare() == false)
  {
    return false;
  }

  MoveModel::FieldMoveModelPlayer* pPlayer = static_cast<MoveModel::FieldMoveModelPlayer*>(req.pPlayer);

  if (this->CheckTooClose(pPlayer) || this->IsDetected(pPlayer))
  {
    KANAMARU_PRINT("FISH SPOT : ESCAPE\n");

    Sound::PlaySE(SEQ_SE_FLD_218);

    StaticModel::StaticModel::Handle_StartEventAnimation handle( 0, 1.0f, false);
    this->m_pStaticModel->Set(&handle);
    this->EscapeRare();
    return false;
  }

  return false;
}

/**
 *  @brief  釣りイベント開始判定と実行
 */
bool FieldFishingSpotActor::KickAction(const KickRequest &req)
{
  if (this->IsInvalid())
  {
    return false;
  }

  KANAMARU_PRINT("FISH SPOT : ACTION\n");
  EventFishingAction::Call(GameSys::GameManager::GetInstance()->GetGameEventManager(), this);

  return true;
}

/**
 *  @brief  判定
 */
bool FieldFishingSpotActor::IsInvalid()
{
  // 「つりざお」をもっているか判定
  const Savedata::MyItem* pMyItem = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMyItem();
  if( pMyItem->Check( ITEM_TURIZAO, 1) == false)
  {
    return true;
  }

  MoveModel::FieldMoveModelManager* pMoveModelManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldCharaModelManager();
  MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>( pMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

  // ライド状態による判定
  u32 code = pPlayer->GetMoveCodeId();
  if (code != MoveModel::FIELD_MOVE_CODE_PLAYER && code != MoveModel::FIELD_MOVE_CODE_RIDE_LAPLACE)
  {
    return true;
  }

  // 近すぎないか判定
  return this->CheckTooClose(pPlayer);
}

/**
 *  @brief  配置モデルを作成する
 */
void FieldFishingSpotActor::SetupStaticModel(FieldStaticModelActor::StaticModelSetupData& setupData)
{
  setupData.pData = &(this->staticModelData);
  setupData.vPos.Set(this->staticModelData.position[0], this->staticModelData.position[1], this->staticModelData.position[2]);
  setupData.Quaternion.Set(this->staticModelData.quaternion[0], this->staticModelData.quaternion[1], this->staticModelData.quaternion[2], this->staticModelData.quaternion[3]);

  // 通知リスナーの登録削除のために保持する
  this->m_pTerrainManager = setupData.pTerrainManager;
  this->m_pRenderingPipeLine = setupData.pRenderingPipeLine;

  // ☆配置モデルの生成
  // コリジョンは別で生成するので、配置モデルで生成する必要は無い。
  // その代わり、配置モデルの有効無効は、FieldStaticModelActorに対して行なう必要がある。
  this->m_pStaticModel = setupData.pStaticModelManager->CreateModel(
    setupData.pData->staticModelID,
    setupData.pData->eventID,
    setupData.vPos,
    setupData.Quaternion
    );
  this->m_pStaticModel->SetVisibleAll(false);  // 初期状態は全て非表示
  this->m_pStaticModel->SetStaticActor(NULL);
}

/**
 *  @brief  スポットデータを取得する
 */
const FieldFishingSpotAccessor::Data* FieldFishingSpotActor::GetData() const
{
  return this->pData;
}

/**
 *  @brief  抽選用データを取得する
 */
const FieldFishingSpotAccessor::LotData* FieldFishingSpotActor::GetLotData() const
{
  return this->ExistRare() ? &(this->pData->rareTable) : &(this->pData->normalTable);
}


/**
 *  @brief  レアがいるか
 */
bool FieldFishingSpotActor::ExistRare() const
{
  return GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFishingSpotData()->ExistRare(this->actorIndex);
}

/**
 *  @brief  スポットに近づきすぎか
 */
bool FieldFishingSpotActor::CheckTooClose(MoveModel::FieldMoveModelPlayer* const pPlayer) const
{  
  f32 distance = (pPlayer->GetLocalSRT().translate - this->GetPosision()).Length();

  return distance < this->pData->invalidAreaRadius;
}

/**
 *  @brief  レアに発見されるか
 */
bool FieldFishingSpotActor::IsDetected(const MoveModel::FieldMoveModelPlayer* pPlayer) const
{
  switch(pPlayer->GetMoveCodeId())
  {
  case MoveModel::FIELD_MOVE_CODE_PLAYER:
    return pPlayer->GetCharaDrawInstance()->GetAnimationId() == P2_BASE_FI012_RUN01;
  case MoveModel::FIELD_MOVE_CODE_RIDE_LAPLACE:
    return pPlayer->GetCharaDrawInstance()->GetAnimationId() == P2_BASE_RI108_SPEEDUP01;
  default:
    return true;
  }
}

//-------------------------------------------
// FieldFishingSpotActor::Mist
//-------------------------------------------

inline void FieldFishingSpotActor::Mist::Initialize(FieldFishingSpotActor* pActor, bool visible)
{
  this->pActor = pActor;
  if (visible)
  {
    this->pActor->visibleStaticModel();
  }

  this->state = Mist::Stay;
}

FieldFishingSpotActor::Mist::State FieldFishingSpotActor::Mist::GetState() const
{
  return this->state;
}

void FieldFishingSpotActor::Mist::StartDilute()
{
  if (this->pActor->m_pStaticModel->GetVisible())
  {
    this->state = Mist::StartDiluting;
  }
  else
  {
    this->state = Mist::Ditutied;
  }
}

void FieldFishingSpotActor::Mist::Update()
{
  switch (this->state)
  {
  case Stay:
    break;
  case StartDiluting:
    {
      StaticModel::StaticModel::Handle_StartEventAnimation handle( 0, 1.0f, false);
      this->pActor->m_pStaticModel->Set(&handle);
      this->state = Mist::Diluting;
    }
    break;
  case Diluting:
    {
      StaticModel::StaticModel::Handle_IsEndEventAnimation handle(0);
      if (this->pActor->m_pStaticModel->IsEnd(handle))
      {
        this->state = Mist::Ditutied;
      }
    }
    break;
  case Ditutied:
    this->pActor->invisibleStaticModel();
    break;
  }
}

GFL_NAMESPACE_END( Field )