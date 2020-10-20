/**
 *  GAME FREAK inc.
 *
 *  @file   FieldBerryPointActor.cpp
 *  @brief  きのみアクター
 *  @author Masanori Kanamaru
 *  @date   2015.10.07
 */

#include "Field/FieldRo/include/PlacementData/actor/FieldBerryPointActor.h"

#include "GameSys/include/GameManager.h"

#include "Field/FieldStatic/include/Event/EventBerryPoint.h"

#include "Field/FieldRo/include/PlacementData/EventIDOffset.h"

GFL_NAMESPACE_BEGIN( Field )

/**
 *  @brief  コンストラクタ
 */
FieldBerryPointActor::FieldBerryPointActor()
  : FieldStaticModelActorBase()
{}

/**
 *  @brief  種別取得
 */
FieldActorType FieldBerryPointActor::GetType() const
{
  return FIELD_ACTOR_TYPE_BERRY_POINT;
}

/**
 *  @brief  起動
 *  @param  req パラメータ
 *  @param  p_outIsEventCreate  GameEventを生成したかどうか？ true :起動 false :起動していない
 *  @retval 何かしら処理を実行したか
 */
bool FieldBerryPointActor::Kick( const KickRequest &req, bool * p_outIsEventCreate )
{
  // イベント起動したかフラグをクリアする
  if( p_outIsEventCreate != NULL)
  {
    *p_outIsEventCreate = false;
  }

  if (this->IsInvalid())
  {
    return false;
  }

  EventBerryPoint::CallBerryPointEvent(GameSys::GameManager::GetInstance()->GetGameEventManager(), &this->lottedData, &this->GetPosision());

  if( p_outIsEventCreate != NULL)
  {
    *p_outIsEventCreate = true;
  }

  return true;
}

/**
 *  @brief  判定
 */
bool FieldBerryPointActor::IsInvalid()
{
  return false;
}

/**
 *  @brief  初期化
 */
void FieldBerryPointActor::Initialize(const FieldBerryPointAccessor::BerryPointData* pData, const FieldBerryManager* pBerryManager, SetupDesc& desc)
{
  // 抽選済みデータを取得
  bool isWealth = pBerryManager->GetLottedData(pData, &(this->lottedData));

  // 配置モデル用データを作成
  this->staticModelData.position[0] = pData->position[0];
  this->staticModelData.position[1] = pData->position[1];
  this->staticModelData.position[2] = pData->position[2];

  this->staticModelData.quaternion[0] = pData->quaternion[0];
  this->staticModelData.quaternion[1] = pData->quaternion[1];
  this->staticModelData.quaternion[2] = pData->quaternion[2];
  this->staticModelData.quaternion[3] = pData->quaternion[3];

  // きのみのなる木本体と区別をつけるためにオフセット加算
  this->staticModelData.eventID = pData->eventID + Field::BERRY_POINT_START;

  FieldBerryManager::VisualDesc vDesc;
  pBerryManager->GetVisualDesc(isWealth, &vDesc);

  this->staticModelData.staticModelID = vDesc.modelID;
  this->focusID                       = vDesc.focusID;

  gfl2::math::Vector3 pos(pData->position[0], pData->position[1], pData->position[2]);

  this->m_vPos = pos;

  // コリジョン準備
  {
    CollisionSetupData talkDesc;
    CollisionData colData;

    this->GetFixedCollisionData(vDesc.pTalkColData, &colData);

    talkDesc.pScene         = desc.pTalkCollisionScene;    
    talkDesc.pCollisionData = static_cast<void*>(&colData);

    IFieldActorBase::CreateCollisionInstance(&(this->talkArea), talkDesc, pos, desc.pAllocator, desc.pHeap, this);
  }
  {
    CollisionSetupData forbiddenDesc;
    CollisionData colData;

    this->GetFixedCollisionData(vDesc.pForbiddenColData, &colData);

    forbiddenDesc.pScene         = desc.pForbiddenCollisionScene;
    forbiddenDesc.pCollisionData = static_cast<void*>(&colData);

    IFieldActorBase::CreateCollisionInstance(&(this->forbiddenArea), forbiddenDesc, pos, desc.pAllocator, desc.pHeap, this);
  }
}

/**
 *  @brief  破棄
 */
void FieldBerryPointActor::Terminate()
{
  this->terminate();

  this->UnregistActor();

  GFL_SAFE_DELETE(this->talkArea.pActor);
  GFL_SAFE_DELETE(this->forbiddenArea.pActor);
}

/**
 *  @brief  シーンへの登録
 */
void FieldBerryPointActor::registActor()
{
  // ユーザー指定のRegist:true　Culling:falseのときのみ本当に登録
  if( isNeedRegistActor() )
  {
    // コリジョンシーンへの登録
    this->registCollisionScene();

    IFieldActorBase::RegisterCollision(&(this->talkArea));
    IFieldActorBase::RegisterCollision(&(this->forbiddenArea));

    // 配置モデルの表示ON
    this->visibleStaticModel();

  }
}

/**
 *  @brief  シーンから除去
 */
void FieldBerryPointActor::unregistActor()
{
  // ユーザー指定のRegist:false　か　Culling:trueのときは、unregist
  if( isNeedUnRegistActor() )
  {
    // コリジョンシーンからの削除
    this->unregistCollisionScene();

    IFieldActorBase::UnregisterCollision(&(this->talkArea));
    IFieldActorBase::UnregisterCollision(&(this->forbiddenArea));

    // 配置モデルの表示OFF
    this->invisibleStaticModel();
  }
}




/**
 *  @brief  配置モデル用を作成する
 */
void FieldBerryPointActor::SetupStaticModel(FieldStaticModelActor::StaticModelSetupData& setupData)
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
 *  @brief  きのみポイントIDを返す
 */
u32 FieldBerryPointActor::GetBerryPointID() const
{
  return this->lottedData.berryPointID;
}

/**
  *  @brief  イベントIDを返す
  */
u32 FieldBerryPointActor::GetEventID() const
{
  return this->staticModelData.eventID;
}

/**
  *  @brief  フォーカスIDを返す
  */
u32 FieldBerryPointActor::GetFocusID() const
{
  return this->focusID;
}

/**
  *  @brief  姿勢を返す
  */
void FieldBerryPointActor::GetRotation(gfl2::math::Quaternion* pOut) const
{
  pOut->Set(
    this->staticModelData.quaternion[0],
    this->staticModelData.quaternion[1],
    this->staticModelData.quaternion[2],
    this->staticModelData.quaternion[3]
  );
}

void FieldBerryPointActor::StartSwing()
{
  StaticModel::StaticModel::Handle_StartEventAnimation handle(0, 1.0f, true);
  this->m_pStaticModel->Set(&handle);
}

void FieldBerryPointActor::StopSwing()
{
  StaticModel::StaticModel::Handle_StopEventAnimation handle(0);
  this->m_pStaticModel->Set(&handle);
}

/**
 *  @brief  オフセット計算済みのコリジョンデータを生成する
 */
void FieldBerryPointActor::GetFixedCollisionData(const void* src, CollisionData* pOut) const
{
  FieldCollisionAccessor acc;
  acc.Initialize(src);
  FieldCollisionAccessor::CollisionData data;
  acc.GetData(0, &data);
  GFL_ASSERT(data.type == FieldCollisionAccessor::CYLINDER);
  pOut->amount = 1;
  pOut->type   = FieldCollisionAccessor::CYLINDER;
  pOut->data.center[0] = data.cylinder.center[0] + this->staticModelData.position[0];
  pOut->data.center[1] = data.cylinder.center[1] + this->staticModelData.position[1];
  pOut->data.center[2] = data.cylinder.center[2] + this->staticModelData.position[2];
  pOut->data.radius = data.cylinder.radius;
  pOut->data.height = data.cylinder.height;
}

GFL_NAMESPACE_END( Field )

