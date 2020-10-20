/**
 *  GAME FREAK inc.
 *
 *  @file   FieldTrainerActor.cpp
 *  @brief  トレーナー用アクター
 *  @author saita_kazuki
 *  @date   2015.06.04
 */

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldTrainerActor.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldRo/include/MotionSignal/FieldMotionSignal.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficPath.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/source/EventCheck/FieldEventCheckTrainerEye.h"

#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeTrainerEye.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeTrainerPath.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"

// Script
#include "FieldScript/include/EventScriptCall.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptInfo.h"

// conv_header
#include <niji_conv_header/field/flagwork/FlagDefine.h>
#include <niji_conv_header/field/script/ScrOffsetId.h>

// gamesys
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

GFL_NAMESPACE_BEGIN( Field );

#define USE_DUMMY_TRAFFIC_PATH 0
#if USE_DUMMY_TRAFFIC_PATH
static const Field::Traffic::PathCoreData testPathCoreData = 
{
//  No.0                          No.1                          No.2
  3,
  false,
  true,
  false
  { 39730.0f, -917.0f, 22480.0f,  39786.0f, -919.0f, 22080.0f,  40040.0f, -913.0f, 22120.0f, },
};
static const u32 testPathPointNum = 2;
static const Field::Traffic::PathPointData testPathPointData[] =
{
  { 0.355f, 40,  0.2f, 1,  0.0f },
  { 0.58f,  40, -0.3f, 2, 90.0f },
  { 0.0f,   40,  0.0f, 1,  0.0f },
  { 1.0f,   40,  0.0f, 1, 90.0f },
};
#endif

/**
 *  @brief  コンストラクタ
 */
FieldTrainerActor::FieldTrainerActor( void )
  : FieldMoveModelActorBase()
  , m_pData( NULL )
  , m_pMoveModel( NULL)
  , m_MotionSignal( NULL )
  , m_TrafficPath( NULL )
  , m_pPathPointData( NULL )
  , m_pCollisionSceneForTerrain( NULL )
  , m_pCollisionSceneForStaticModel( NULL )
  , m_pCollisionSceneForEventTalk( NULL )
  , m_pCollisionSceneForTrainerEye( NULL )
  , m_pActorForTerrain( NULL )
  , m_pActorForStaticModel( NULL )
  , m_pActorForEventTalk( NULL )
  , m_pActorForTrainerEye( NULL )
  , m_pActorForTrainerEyeNew( NULL )
  , m_cpEventWork( NULL)
#ifdef ENABLE_TEST_EYE_DRAW
  , m_pGLAllocator( NULL)
  , m_pEyeModelBuff( NULL)
  , m_pEyeModel( NULL)
  , m_pEyeModelResourceRootNode( NULL)
#endif
  , m_DataZoneId(0)
{
}

/**
 *  @brief  デストラクタ
 */
FieldTrainerActor::~FieldTrainerActor( void )
{
  // PlacementDataManager側でワークを残すか完全破棄かを選択するので、
  // デストラクタでTerminate()は呼ばない
}

/**
 *  @brief  初期化
 */
void FieldTrainerActor::Initialize( TrainerSetupData &setupData )
{
  m_pMoveModelManager             = setupData.pMoveModelManager;
  m_pData                         = setupData.pTrainerData;
  m_pCollisionSceneForTerrain     = setupData.pCollisionSceneForTerrain;
  m_pCollisionSceneForStaticModel = setupData.pCollisionSceneForStaticModel;
  m_pCollisionSceneForEventTalk   = setupData.pCollisionSceneForEventTalk;
  m_pCollisionSceneForTrainerEye  = setupData.pCollisionSceneForTrainerEye;

  gfl2::math::Vector3     pos( m_pData->position[ 0 ], m_pData->position[ 1 ], m_pData->position[ 2 ] );
  gfl2::math::Quaternion  qua( m_pData->quaternion[ 0 ], m_pData->quaternion[ 1 ], m_pData->quaternion[ 2 ], m_pData->quaternion[ 3 ] );
  m_vPos = pos;

  m_pMoveData = setupData.pAccessor->GetMoveData( *m_pData);
  MoveModel::FIELD_MOVE_CODE_ID moveCode = static_cast<MoveModel::FIELD_MOVE_CODE_ID>( m_pMoveData->typeID );
  //MoveModel::FIELD_MOVE_CODE_ID moveCode = FieldMoveModelActorBase::GetPlacementMoveCodeToMoveCode( m_pMoveData->typeID );

  // 復帰処理かチェック
  m_ModelIndex = m_pMoveModelManager->GetFieldMoveModelIndexFromEventId( m_pData->eventID );  // Indexが帰ってきたら復帰処理

  m_MoveModelHeaderWork.moveCodeId                        = moveCode;
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
  m_MoveModelHeaderResource.pCollisionSceneForTrainerEye  = setupData.pCollisionSceneForTrainerEye;

  initializeMoveModelWork();

  m_DataZoneId = setupData.dataZoneId;
}

/**
 *  @brief  リソースとコリジョンの設定
 */
void FieldTrainerActor::CreateResource( TrainerCreateResourceData& createResourceData )
{
  FieldCollisionAccessor collisionAccessor;
  FieldCollisionAccessor::CollisionData colData;

  m_pTerrainManager = createResourceData.pTerrainManager;
  m_pRenderingPipeLine = createResourceData.pRenderingPipeLine;
  m_cpEventWork = createResourceData.cpEventWork;

  // 動作モデル取得
  Field::MoveModel::FieldMoveModel *pCharaModel = m_pMoveModelManager->GetFieldMoveModel( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );

  // 視線起動チェックで使用する
  m_pMoveModel = pCharaModel;

  // MoveCodeWork設定
  setupMoveCodeWork();

  // Path生成
  CreateTrafficPath( createResourceData.pHeap, createResourceData.pAccessor );

  // 接地対応
  m_pMoveModel->SetCollisionSceneContainerForGround( m_pTerrainManager->GetCollsionScene() );

  // モデル作成
  initializeMoveModelResource();

  // @fix GFNMCat[3962] MoveModel作成時にLandingフラグが再初期化されるためここで設定しなおす
  if( m_TrafficPath )
  {
    if( m_TrafficPath->CheckFlags( TrafficPath::FLAG_LANDING ) )
    {
      m_pMoveModel->SetForceLanding( true );
    }
    else
    {
      m_pMoveModel->SetForceLanding( false );
    }
  }

  // 非表示
  pCharaModel->SetVisible( false );

  gfl2::math::Vector3     pos( m_pData->position[ 0 ], m_pData->position[ 1 ], m_pData->position[ 2 ] );
  gfl2::math::Vector3 modelPos = pCharaModel->GetCharaDrawInstance()->GetPosition();
  // 衝突用コリジョンの設定
  {
    const void *p_col = createResourceData.pAccessor->GetCollisionData( *( m_pData ) );
    collisionAccessor.Initialize( p_col );
    if( getCollisionData( &colData, collisionAccessor))
    {
      // 自身を押し返す処理用
      m_pActorForTerrain = pCharaModel->GetDynamicActor( Field::MoveModel::COLLISION_TYPE_TERRAIN );
      setupCollision( colData, pos, m_pActorForTerrain, NULL);
      m_pActorForTerrain->SetPosition( modelPos );
      // 主人公との判定用
      m_pActorForStaticModel = pCharaModel->GetStaticActor( Field::MoveModel::COLLISION_STATIC_TYPE_STATIC_MODEL );
      setupCollision( colData, pos, m_pActorForStaticModel, NULL);
      m_pActorForStaticModel->SetPosition( modelPos );
    }
  }

  // 話しかけコリジョンの設定
  {
    const void *p_col = createResourceData.pAccessor->GetTalkCollisionData( *( m_pData ) );
    collisionAccessor.Initialize( p_col );
    if( getCollisionData( &colData, collisionAccessor))
    {
      // 話しかけコリジョン
      m_pActorForEventTalk = pCharaModel->GetStaticActor( Field::MoveModel::COLLISION_STATIC_TYPE_EVENT_TALK );
      setupCollision( colData, pos, m_pActorForEventTalk, this);
      m_pActorForEventTalk->SetPosition( modelPos );
    }
  }

  // 視線コリジョンの設定
  {
    // シェイプの情報はMoveCodeの中で書き換えるので初期化のみ
    colData.type = FieldCollisionAccessor::BOX;
    colData.box.center[0] = 0.0f;
    colData.box.center[1] = 0.0f;
    colData.box.center[2] = 0.0f;
    colData.box.quaternion[0] = 0.0f;
    colData.box.quaternion[1] = 0.0f;
    colData.box.quaternion[2] = 0.0f;
    colData.box.quaternion[3] = 1.0f;
    colData.box.scale[0] = 1.0f;
    colData.box.scale[1] = 1.0f;
    colData.box.scale[2] = 1.0f;

    m_pActorForTrainerEye = pCharaModel->GetStaticActor( Field::MoveModel::COLLISION_STATIC_TYPE_TRAINER_EYE );
    setupCollision( colData, pos, m_pActorForTrainerEye, this);
    m_pActorForTrainerEye->SetPosition( modelPos );

    // 旧視線は無効
    m_pActorForTrainerEye->SetActive(false);

    // シェイプの情報はMoveCodeの中で書き換えるので初期化のみ
    colData.type = FieldCollisionAccessor::TRIANGLE;
    colData.triangle.pos0[0] = 0.0f;
    colData.triangle.pos0[1] = 0.0f;
    colData.triangle.pos0[2] = 0.0f;
    colData.triangle.pos1[0] = 0.0f;
    colData.triangle.pos1[1] = 0.0f;
    colData.triangle.pos1[2] = 0.0f;
    colData.triangle.pos2[0] = 0.0f;
    colData.triangle.pos2[1] = 0.0f;
    colData.triangle.pos2[2] = 0.0f;

    m_pActorForTrainerEyeNew = pCharaModel->GetStaticActor( Field::MoveModel::COLLISION_STATIC_TYPE_TRAINER_EYE_NEW );
    setupCollision( colData, pos, m_pActorForTrainerEyeNew, this);
    m_pActorForTrainerEyeNew->SetPosition( modelPos );

#ifdef ENABLE_TEST_EYE_DRAW
    this->CreateEyeModel();
#endif
  }

  // MotionSignal生成
  bool bSignalCreate = false;
  const void *pSignalData = createResourceData.pAccessor->GetSignalData( *m_pData );
  if( pSignalData )
  {
    bSignalCreate = true;
  }
#if PM_DEBUG
  // デバッグでは調整機能を有効にするので常に作成する
  bSignalCreate = true;
#endif
  if( bSignalCreate )
  {
    m_MotionSignal = GFL_NEW( createResourceData.pHeap ) MotionSignal();
    m_MotionSignal->Initialize( pSignalData );
    pCharaModel->SetMotionSignal( m_MotionSignal );
  }

  // ☆カリングリスナーを有効にする
  // registCullingListener();
}

/**
 *  @brief  起動
 */
bool FieldTrainerActor::Kick( const KickRequest &req, bool * p_outIsEventCreate )
{
  if( p_outIsEventCreate ) *p_outIsEventCreate = false; 

  if( IsInvalid() ){ return false; }

  EventCheckTrainerEye::CallTrainerScript( GameSys::GameManager::GetInstance(), this, false  );

  if( p_outIsEventCreate ) *p_outIsEventCreate = true;
  return true;
}

/**
 *  @brief  判定
 */
bool FieldTrainerActor::IsInvalid( void )
{
  if( !m_pData ){ return true; }
  if( m_pData->scriptID == FieldScript::SCRID_NULL ){ return true; }

  return false;
}

/**
 *  @brief  トレーナーID取得
 */
trainer::TrainerID FieldTrainerActor::GetTrainerID() const
{
  Field::FieldScript::FieldScriptSystem *pSys = Field::FieldScript::FieldScriptSystem::GetInstance();
  Field::FieldScript::SCRIPT_ARC_TABLE  *pScriptArcTable = pSys->GetScriptArcTable();
  trainer::TrainerID trID;

//  GFL_ASSERT( pScriptArcTable[ID_TRAINER_2VS2_INDEX].scrid_start == ID_TRAINER_2VS2_OFFSET ); ///< 一致を確認した形跡を残す.
//  GFL_ASSERT( pScriptArcTable[ID_TRAINER_INDEX].scrid_start == ID_TRAINER_OFFSET );

  if( m_pData->scriptID < pScriptArcTable[ID_TRAINER_2VS2_INDEX].scrid_start )
  {
    trID = static_cast<trainer::TrainerID>( m_pData->scriptID - pScriptArcTable[ID_TRAINER_INDEX].scrid_start );
  }
  else
  {
    trID = static_cast<trainer::TrainerID>( m_pData->scriptID - pScriptArcTable[ID_TRAINER_2VS2_INDEX].scrid_start );
  }
  
  // トレーナー対戦フラグの範囲内かチェック
  if( (trID >= 0) && (trID <= (TR_FLAG_AREA_END - TR_FLAG_AREA_START)) )
  {
    return trID;
  }
  else
  {
    GFL_ASSERT(0);
    return static_cast<trainer::TrainerID>(1); // 短パン小僧と戦わせる。
  }
}

/**
 *  @brief  トレーナー対戦フラグ取得
 */
u16 FieldTrainerActor::GetTrainerFlag() const
{
  trainer::TrainerID trID = this->GetTrainerID();
  return TR_FLAG_AREA_START + trID;
}

/**
 *  @brief  対戦済みかどうか
 *  @retval true 対戦済み
 *  @retval false まだ対戦していない
 */
bool FieldTrainerActor::IsBattleFinished() const
{
  if( m_cpEventWork->CheckEventFlag( this->GetTrainerFlag()))
  {
    return true;
  }
  return false;
}

/**
 *  @brief  パスの生成
 */
void FieldTrainerActor::CreateTrafficPath( gfl2::heap::HeapBase *pHeap, FieldTrainerAccessor *pAccessor )
{
  const Field::Traffic::PathCoreData *pCoreData = pAccessor->GetPathCoreData( *m_pData );
  m_pPathPointData = pAccessor->GetPathPointData( *m_pData );
  // リソースからパスデータの取得を行う
  if( pCoreData )
  {
    m_TrafficPath = GFL_NEW( pHeap ) Field::TrafficPath();
    m_TrafficPath->Initialize( pHeap );
    m_TrafficPath->CreatePath( pCoreData );

    if( m_TrafficPath->CheckFlags( TrafficPath::FLAG_LANDING ) )
    {
      m_pMoveModel->SetForceLanding( true );
    }
    else
    {
      m_pMoveModel->SetForceLanding( false );
      m_pMoveModel->SetCollisionSceneContainerForGround( NULL );
    }
  }
}

/**
 *  @brief  演出点の数を取得する
 */
u32 FieldTrainerActor::GetPathPointDataNum( void )
{
  u32 num = 0;
  // リソースから演出点の取得を行う
  if( m_pPathPointData )
  {
    num = m_pPathPointData->num;
  }
  return num;
}

/**
 *  @brief  演出点の取得
 */
const Field::Traffic::PathPointData *FieldTrainerActor::GetPathPointData( u32 idx )
{
  // 範囲外アクセス
  if( GetPathPointDataNum() <= idx ){ return NULL; }

  // リソースから演出点の取得を行う
  const Field::Traffic::PathPointData *pResult = NULL;
  if( m_pPathPointData )
  {
    pResult = &( m_pPathPointData->data[ idx ] );
  }

  return pResult;
}

/**
 *  @brief  歩けるかチェック
 */
bool FieldTrainerActor::CheckEnableWalk( void )
{
  bool bResult = true;      //!< 返却値
  Field::MoveModel::FieldMoveModel *pChara = m_pMoveModel;
  Field::MoveModel::FieldMoveModelPlayer *pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>( m_pMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ) );
  poke_3d::model::CharaModel *pCharaDrawInstance = pChara->GetCharaDrawInstance();

  f32 size   = 1.0f;
  f32 length = 1.0f;

  u32 player_id = pPlayer->GetCharacterId();
  u32 chara_id = pChara->GetCharacterId();
  if( pPlayer->GetOnMoveModel() )
  {
    player_id = pPlayer->GetOnMoveModel()->GetCharacterId();
  }
  // キャラクタサイズを取得する
  Field::MoveModel::ApplicationDataForPlayer *pPlayerData = (Field::MoveModel::ApplicationDataForPlayer*)( m_pMoveModelManager->GetApplicationData( player_id ) );
  if( pPlayerData )
  {
    size   += pPlayerData->collisionR;
    length += pPlayerData->collisionR;
  }
  Field::MoveModel::ApplicationDataForNPC *pCharaData = (Field::MoveModel::ApplicationDataForNPC*)( m_pMoveModelManager->GetApplicationData( chara_id ) );
  if( pCharaData )
  {
    size   += pCharaData->collisionOshikaeshiCircleR;
    length += pCharaData->collisionOshikaeshiCircleR * 2.0f;
  }

  gfl2::math::Vector3 vPos = pCharaDrawInstance->GetPosition();
  gfl2::math::Vector3 vRot = pCharaDrawInstance->GetRotation();
  gfl2::math::Vector3 vCheckerPos = pPlayer->GetLocalSRT().translate;

  // ローカル空間に変更する
  vCheckerPos -= vPos;
  gfl2::math::Matrix44 mtx = gfl2::math::Matrix44::GetRotationZYX( vRot.z, vRot.y, vRot.x );
  vCheckerPos = mtx.Inverse() * vCheckerPos;

  // AABB判定
  if( ( -1 * size < vCheckerPos.x && vCheckerPos.x < size ) &&
    ( 0.0f < vCheckerPos.z && vCheckerPos.z < length ) )
  {
    bResult = false;
  }

  return bResult;
}

/**
 *  @brief  追加
 */
void FieldTrainerActor::registActor( void )
{
  if( isRegistActor() == false)
  {
    return;
  }

  if( m_pActorForTerrain && m_pActorForStaticModel )
  {
    m_pCollisionSceneForTerrain->RegistDynamicActor( m_pActorForTerrain );
    m_pCollisionSceneForStaticModel->RegistStaticActor( m_pActorForStaticModel );
  }
  if( m_pActorForEventTalk )
  {
    m_pCollisionSceneForEventTalk->RegistStaticActor( m_pActorForEventTalk );
  }
  if( m_pActorForTrainerEye )
  {
    if( this->IsBattleFinished() == false)
    {
      m_pCollisionSceneForTrainerEye->RegistStaticActor( m_pActorForTrainerEye );
    }
  }
  if( m_pActorForTrainerEyeNew )
  {
    if( this->IsBattleFinished() == false)
    {
      m_pCollisionSceneForTrainerEye->RegistStaticActor( m_pActorForTrainerEyeNew );
    }
  }

  registMoveModel();

#ifdef ENABLE_TEST_EYE_DRAW
  if( m_pEyeModel)
  {
    m_pEyeModel->SetVisible( true);
  }
#endif
}

/**
 *  @brief  削除
 */
void FieldTrainerActor::unregistActor( void )
{
  if( isUnRegistActor() == false)
  {
    return;
  }

  if( m_pActorForTerrain && m_pActorForStaticModel )
  {
    m_pCollisionSceneForTerrain->UnRegistDynamicActor( m_pActorForTerrain );
    m_pCollisionSceneForStaticModel->UnRegistStaticActor( m_pActorForStaticModel );
  }
  if( m_pActorForEventTalk )
  {
    m_pCollisionSceneForEventTalk->UnRegistStaticActor( m_pActorForEventTalk );
  }
  if( m_pActorForTrainerEye )
  {
    if( this->IsBattleFinished() == false)
    {
      m_pCollisionSceneForTrainerEye->UnRegistStaticActor( m_pActorForTrainerEye );
    }
  }
  if( m_pActorForTrainerEyeNew )
  {
    if( this->IsBattleFinished() == false)
    {
      m_pCollisionSceneForTrainerEye->UnRegistStaticActor( m_pActorForTrainerEyeNew );
    }
  }

  unregistMoveModel();

#ifdef ENABLE_TEST_EYE_DRAW
  if( m_pEyeModel)
  {
    m_pEyeModel->SetVisible( false);
  }
#endif
}

/**
 *  @brief  破棄共通処理
 *  @retval true 破棄処理を行った
 *  @retval false 破棄処理を行っていない
 */
bool FieldTrainerActor::terminate()
{
  Field::MoveModel::FieldMoveModel *pCharaModel = m_pMoveModelManager->GetFieldMoveModel( static_cast<Field::MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );

  GFL_SAFE_DELETE( m_TrafficPath );
  if( pCharaModel && pCharaModel->IsSetupWork())
  {
    pCharaModel->SetMotionSignal( NULL );
  }
  GFL_SAFE_DELETE( m_MotionSignal );

#ifdef ENABLE_TEST_EYE_DRAW
  this->DeleteEyeModel();
#endif

  bool isTerminate = FieldMoveModelActorBase::terminate();

  m_pActorForTerrain = NULL;
  m_pActorForStaticModel = NULL;
  m_pCollisionSceneForTerrain = NULL;
  m_pCollisionSceneForStaticModel = NULL;

  m_pActorForEventTalk = NULL;
  m_pCollisionSceneForEventTalk = NULL;

  m_pActorForTrainerEye = NULL;
  m_pActorForTrainerEyeNew = NULL;
  m_pCollisionSceneForTrainerEye = NULL;

  return isTerminate;
}

/**
 *  @brief  MoveCodeWorkに対してこのクラスでの設定を行う
 */
void FieldTrainerActor::setupMoveCodeWork()
{
  Field::MoveModel::FieldMoveModel* pChara = m_pMoveModelManager->GetFieldMoveModel( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );

  switch( m_MoveModelHeaderWork.moveCodeId)
  {
  case MoveModel::FIELD_MOVE_CODE_TRAINER_EYE:
    {
      u32 size = sizeof( Field::MoveModel::FieldMoveCodeTrainerEye::Work);
      void* work = MoveModel::FieldMoveCodeManager::GetMoveCodeWork( pChara->GetMoveCodeWork(), size);
      Field::MoveModel::FieldMoveCodeTrainerEye::Work *pWork;
      pWork = static_cast<Field::MoveModel::FieldMoveCodeTrainerEye::Work*>( work);
      pWork->pTrainerActor = this;
    }
    break;

  case MoveModel::FIELD_MOVE_CODE_TRAINER_PATH:
    {
      u32 size = sizeof( Field::MoveModel::FieldMoveCodeTrainerPath::Work);
      void* work = MoveModel::FieldMoveCodeManager::GetMoveCodeWork( pChara->GetMoveCodeWork(), size);
      Field::MoveModel::FieldMoveCodeTrainerPath::Work *pWork;
      pWork = static_cast<Field::MoveModel::FieldMoveCodeTrainerPath::Work*>( work);
      pWork->pTrainerActor = this;
    }
    break;

  default:
    GFL_ASSERT( 0);
    break;
  }
}



#ifdef ENABLE_TEST_EYE_DRAW
void FieldTrainerActor::CreateEyeModel()
{
  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG);
  m_pEyeModelBuff = NULL;

  m_pGLAllocator = GFL_NEW( pHeap) System::nijiAllocator( pHeap);

  // @fix ここは通らなくなった
  // リソースも削除
  gfl2::fs::AsyncFileManager::FileEasyReadReq req;
#if defined(GF_PLATFORM_WIN32)
  req.fileName = "data/test/field_test/trainer_eye/trainer_eye.wingl.gfbmdlp";
#elif defined(GF_PLATFORM_CTR)
  req.fileName = "data/test/field_test/trainer_eye/trainer_eye.ctr.gfbmdlp";
#endif
  req.align = 128;
  req.heapForBuf = pHeap->GetLowerHandle();
  req.heapForReq = pHeap->GetLowerHandle();
  req.ppBuf = &m_pEyeModelBuff;
  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->SyncFileEasyRead(req);

  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData( static_cast<c8*>( m_pEyeModelBuff));

  m_pEyeModelResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pGLAllocator, &resourceData);

  m_pEyeModel = GFL_NEW( pHeap) poke_3d::model::BaseModel;
  m_pEyeModel->Create( m_pGLAllocator, pHeap, m_pEyeModelResourceRootNode);

  m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pEyeModel->GetModelInstanceNode());
}

void FieldTrainerActor::DeleteEyeModel()
{
  if( m_pEyeModel)
  {
    if( m_pEyeModel->IsCreated() && m_pRenderingPipeLine)
    {
      m_pRenderingPipeLine->RemoveEdgeRenderingTarget( m_pEyeModel->GetModelInstanceNode());
      m_pEyeModel->Destroy();
    }
    GFL_DELETE( m_pEyeModel);
    m_pEyeModel = NULL;
  }
  GFL_SAFE_DELETE( m_pEyeModelResourceRootNode);
  if( m_pEyeModelBuff)
  {
    GflHeapFreeMemory( m_pEyeModelBuff);
    m_pEyeModelBuff = NULL;
  }
  GFL_SAFE_DELETE( m_pGLAllocator);
}
#endif

GFL_NAMESPACE_END( Field );
