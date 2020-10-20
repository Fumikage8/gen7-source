/**
 *  GAME FREAK inc.
 *
 *  @file   FieldTrafficActor.cpp
 *  @brief  往来NPC用アクター
 *  @author miyachi_soichi
 *  @date   2015/06/29
 */

// Field
#include "Field/FieldRo/include/Traffic/FieldTrafficActor.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficPath.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeTraffic.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldRo/include/PlacementData/EventIDOffset.h"

// Script
#include "FieldScript/include/EventScriptCall.h"
#include "GameSys/include/GameManager.h"

GFL_NAMESPACE_BEGIN(Field)

/**
 *  @brief  コンストラクタ
 */
TrafficActor::TrafficActor( void )
: IFieldActorBase()
, m_pMoveModelManager( NULL )
, m_pTrafficManager( NULL )
, m_pCollisionSceneForTerrain( NULL )
, m_pCollisionSceneForStaticModel( NULL )
, m_pCollisionSceneForTrafficModel( NULL )
, m_pCollisionSceneForEventPos( NULL )
, m_pCollisionSceneForEventTalk( NULL )
, m_pCollisionSceneForEventPush( NULL )
, m_pCollisionSceneForGround( NULL )
, m_pActorForTrafficModel( NULL )
, m_pActorForEventTalk( NULL )
, m_pTalker( NULL )
, m_MoveModelHeaderWork()
, m_MoveModelHeaderResource()
, m_ModelIndex( Field::MoveModel::FIELD_MOVE_MODEL_MAX )
, m_ScriptID( FieldScript::SCRID_NULL )
, m_nCharacterID( 0 )
, m_PathID( Field::Traffic::FIELD_TRAFFIC_MAX )
, m_nStayID( 0 )
, m_nWalkID( 0 )
, m_nTurnLID( 0 )
, m_nTurnRID( 0 )
, m_nEmoteID( 0 )
, m_fProgress( 0.0f )
, m_bEmote( false )
, m_bEndTraffic( false )
, m_bResume( false )
, m_bEscape( false )
{
}

/**
 *  @brief  デストラクタ
 */
TrafficActor::~TrafficActor( void )
{
}

/**
 *  @brief  初期化
 *  @param  pMoveModelManager 動作モデル管理
 */
void TrafficActor::Initialize( Field::MoveModel::FieldMoveModelManager *pMoveModelManager, Field::TrafficManager *pTrafficManager )
{
  m_pMoveModelManager = pMoveModelManager;
  m_pTrafficManager = pTrafficManager;
}

/**
 *  @brief  コリジョンシーンの設定
 *  @param  setup      初期化パラメータ
 */
void TrafficActor::SetupCollisionScene( const Traffic::SetupCollision &setup )
{
  m_pCollisionSceneForTerrain = setup.pCollisionSceneForTerrain;
  m_pCollisionSceneForStaticModel = setup.pCollisionSceneForStaticModel;
  m_pCollisionSceneForTrafficModel = setup.pCollisionSceneForTrafficModel;
  m_pCollisionSceneForEventPos = setup.pCollisionSceneForEventPos;
  m_pCollisionSceneForEventTalk = setup.pCollisionSceneForEventTalk;
  m_pCollisionSceneForEventPush = setup.pCollisionSceneForEventPush;
  m_pCollisionSceneForGround = setup.pCollisionSceneForGround;
}

/**
 *  @brief  破棄
 */
void TrafficActor::Terminate( void )
{
  DeleteActor();
}

/**
 *  @brief  演者の作成：ワーク側
 *  @param  pPath     移動用パス
 *  @param  coreData  NPC用データ
 */
void TrafficActor::CreateActorWork( u32 path_idx, const Traffic::NpcCoreData &coreData )
{
  // 固定値
  m_nStayID = 0;
  m_nTurnRID = 0x001E;
  m_nTurnLID = 0x001F;
  m_fProgress = 0.0f;
  m_bEmote = false;
  m_bEndTraffic = false;
  // 変動値
  m_nWalkID = coreData.walk_id;
  m_nEmoteID = coreData.emote_id;
  m_ScriptID = coreData.script_id;
  m_nCharacterID = coreData.model_id;
  m_PathID = path_idx;
  
  TrafficPath *pPath = GetPath();
  // MoveModel作る
  m_ModelIndex = m_pMoveModelManager->GetFieldMoveModelIndexFromFreeSpace();
  if( m_ModelIndex >= MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    return;
  }

  m_MoveModelHeaderWork.eventId                            = Field::TRAFFIC_ACTOR_START + path_idx;
  m_MoveModelHeaderWork.moveCodeId                         = Field::MoveModel::FIELD_MOVE_CODE_TRAFFIC;
  m_MoveModelHeaderWork.zoneId                             = m_pTrafficManager->GetZoneID();
  m_MoveModelHeaderResource.characterId                    = coreData.model_id;
  m_MoveModelHeaderResource.pDressUpParam                  = NULL;
  m_MoveModelHeaderResource.pCollisionSceneForTerrain      = m_pCollisionSceneForTerrain;
  m_MoveModelHeaderResource.pCollisionSceneForStaticModel  = m_pCollisionSceneForStaticModel;
  m_MoveModelHeaderResource.pCollisionSceneForTrafficModel = m_pCollisionSceneForTrafficModel;
  m_MoveModelHeaderResource.pCollisionSceneForEventPos     = m_pCollisionSceneForEventPos;
  m_MoveModelHeaderResource.pCollisionSceneForEventTalk    = m_pCollisionSceneForEventTalk;
  m_MoveModelHeaderResource.pCollisionSceneForEventPush    = m_pCollisionSceneForEventPush;

  m_pMoveModelManager->InitializeMoveModelWork(
    static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ),
    &m_MoveModelHeaderWork
    );

  Field::MoveModel::FieldMoveModel *pChara = m_pMoveModelManager->GetFieldMoveModel( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );
  // 往来専用設定
  {
    Field::MoveModel::FieldMoveCodeWork *work = pChara->GetMoveCodeWork();
    Field::MoveModel::FieldMoveCodeTraffic::Work *pWork;
    pWork = static_cast<Field::MoveModel::FieldMoveCodeTraffic::Work*>(
      MoveModel::FieldMoveCodeManager::GetMoveCodeWork( work, sizeof( Field::MoveModel::FieldMoveCodeTraffic::Work ) )
      );
    pWork->actor = this;
    pWork->path = pPath;
  }
  // 往来はセーブ対象にならない
  pChara->OnStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_NOSERIALIZE );

  // パスを使用済みにする
  pPath->StartUse();
}

/**
 *  @brief  演者の作成：リソース
 */
void TrafficActor::CreateActorResource()
{
  // 設定されていないリソースは作れない
  if( m_ModelIndex >= MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    return;
  }

  TrafficPath *pPath = GetPath();
  if( !pPath )
  {
    GFL_ASSERT_MSG( false, "TrafficPath is nothing!!\n" );
    return;
  }
  // パスを使用済みにする
  pPath->StartUse();
  pPath->SetProgress( m_fProgress );

  // アプリ復帰用にコリジョンを再設定する
  m_MoveModelHeaderResource.pCollisionSceneForTerrain      = m_pCollisionSceneForTerrain;
  m_MoveModelHeaderResource.pCollisionSceneForStaticModel  = m_pCollisionSceneForStaticModel;
  m_MoveModelHeaderResource.pCollisionSceneForTrafficModel = m_pCollisionSceneForTrafficModel;
  m_MoveModelHeaderResource.pCollisionSceneForEventPos     = m_pCollisionSceneForEventPos;
  m_MoveModelHeaderResource.pCollisionSceneForEventTalk    = m_pCollisionSceneForEventTalk;
  m_MoveModelHeaderResource.pCollisionSceneForEventPush    = m_pCollisionSceneForEventPush;

  m_pMoveModelManager->InitializeMoveModelResource(
    static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ),
    &m_MoveModelHeaderResource
    );

  Field::MoveModel::FieldMoveModel *pChara = m_pMoveModelManager->GetFieldMoveModel( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );

  // 衝突用コリジョンの設定
  Field::MoveModel::ApplicationDataForCommon* pCommonData = (Field::MoveModel::ApplicationDataForCommon*)m_pMoveModelManager->GetApplicationData(m_nCharacterID);
  f32                 collisionOshikaeshiR     ( 37.5f  );
  f32                 collisionOshikaeshiHeight( 150.0f );
  gfl2::math::Vector3 collisionOshikaeshiOffset( 0.0f,100.0f,0.0f );
  f32                 collisionTalkR           ( 150.0f  );
  f32                 collisionTalkHeight      ( 150.0f );
  gfl2::math::Vector3 collisionTalkOffset      ( 0.0f,100.0f,0.0f );
  if( pCommonData->type == Field::MoveModel::TYPE_NPC )
  {
    Field::MoveModel::ApplicationDataForNPC* pNPCData = (Field::MoveModel::ApplicationDataForNPC*)pCommonData;
    collisionOshikaeshiR        = pNPCData->collisionOshikaeshiCircleR;
    collisionOshikaeshiHeight   = pNPCData->collisionOshikaeshiCircleHeight;
    collisionOshikaeshiOffset.x = pNPCData->collisionOshikaeshiOffsetX;
    collisionOshikaeshiOffset.y = pNPCData->collisionOshikaeshiOffsetY;
    collisionOshikaeshiOffset.z = pNPCData->collisionOshikaeshiOffsetZ;
    collisionTalkR              = pNPCData->collisionTalkCircleR;
    collisionTalkHeight         = pNPCData->collisionTalkCircleHeight;
    collisionTalkOffset.x       = pNPCData->collisionTalkOffsetX;
    collisionTalkOffset.y       = pNPCData->collisionTalkOffsetY;
    collisionTalkOffset.z       = pNPCData->collisionTalkOffsetZ;
  }

  {
    // 主人公
    m_pActorForTrafficModel = pChara->GetStaticActor( Field::MoveModel::COLLISION_STATIC_TYPE_TRAFFIC_MODEL );
    m_pActorForTrafficModel->CreateCylinder( collisionOshikaeshiOffset,collisionOshikaeshiR,collisionOshikaeshiHeight );
    m_pCollisionSceneForTrafficModel->RegistStaticActor( m_pActorForTrafficModel );
  }
  // 話しかけコリジョン
  {
    m_pActorForEventTalk = pChara->GetStaticActor( Field::MoveModel::COLLISION_STATIC_TYPE_EVENT_TALK );
    m_pActorForEventTalk->CreateCylinder( collisionTalkOffset,collisionTalkR,collisionTalkHeight );
    m_pCollisionSceneForEventTalk->RegistStaticActor( m_pActorForEventTalk );
    m_pActorForEventTalk->SetUserData( reinterpret_cast<void*>( this ) );
  }
  // 接地対応
  if( !pPath->CheckFlags( TrafficPath::FLAG_LANDING ) )
  {
    pChara->SetCollisionSceneContainerForGround( NULL );
  }
  else
  {
    // #923 MoveModelActorを継承していないので地形登録を自前で行う
    pChara->SetCollisionSceneContainerForGround( m_pCollisionSceneForGround );
  }

  // @fix NMCat[1319]対応：座標設定を手前で行う
  {
    poke_3d::model::BaseModel *pBaseModel = pChara->GetCharaDrawInstance();
    gfl2::math::Vector3 vPos = pPath->GetCurrentPosition();
    gfl2::math::Vector3 vRot = pPath->GetCurrentRotation();
    pBaseModel->SetPosition( vPos );
    pBaseModel->SetRotation( vRot );

    u32 animID = pChara->GetDefaultIdleAnimationId();
    pBaseModel->ChangeAnimationSmooth( animID, 0 ); // @fix NMCat[1769]ブレンドしない設定に切り替え
    // MoveModelManagerの更新はすでに終了しているのでこちらでTスタンスを回避する
    pBaseModel->UpdateAnimation();
  }
}

/**
 *  @brief  演者の破棄
 */
void TrafficActor::DeleteActor( void )
{
  if( m_ModelIndex >= Field::MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    return;
  }

  m_pMoveModelManager->TerminateMoveModelWorkResource( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );

  // メンバ変数をきれいにする
  m_ModelIndex = Field::MoveModel::FIELD_MOVE_MODEL_MAX;
  m_bEmote = false;
  m_bEndTraffic = false;
  m_bEscape = false;
  m_fProgress = 0.0f;
}

/**
 *  @brief  リソースの解放
 */
void TrafficActor::DeleteActorResource( void )
{
  if( m_ModelIndex >= Field::MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    return;
  }

  m_MoveModelHeaderResource.pCollisionSceneForEventTalk->UnRegistStaticActor( m_pActorForEventTalk );
  m_MoveModelHeaderResource.pCollisionSceneForTrafficModel->UnRegistStaticActor( m_pActorForTrafficModel );
}

void TrafficActor::UnRegistActor( void )
{
  if( m_ModelIndex >= Field::MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    return;
  }
  m_MoveModelHeaderResource.pCollisionSceneForEventTalk->UnRegistStaticActor( m_pActorForEventTalk );
  m_MoveModelHeaderResource.pCollisionSceneForTrafficModel->UnRegistStaticActor( m_pActorForTrafficModel );
  Field::MoveModel::FieldMoveModel *pChara = m_pMoveModelManager->GetFieldMoveModel( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );
  if( pChara )
  {
    pChara->SetVisible( false );
  }
}

/**
 *  @brief  起動
 */
bool TrafficActor::Kick( const KickRequest &req, bool * p_outIsEventCreate )
{
  if( p_outIsEventCreate ) *p_outIsEventCreate = false;
  if( IsInvalid() ){ return false; }

  // 実行
  if( m_ScriptID == FieldScript::SCRID_NULL )
  {
    // 挨拶
    m_bEmote = true;
    m_pTalker = req.pPlayer;
  }
  else
  {
    EventScriptCall::CallScript( GameSys::GameManager::GetInstance(), m_ScriptID, this, this->GetMoveModel() );
    if( p_outIsEventCreate ) *p_outIsEventCreate = true; 
    m_bResume = true;
  }

  return true;
}

/**
 *  @brief  判定
 */
bool TrafficActor::IsInvalid( void )
{
  return false;
}

/**
 *  @brief  終了
 */
void TrafficActor::EndTraffic( void )
{
  // パスを不使用にする
  GetPath()->EndUse();
  // 動作モデルを片付ける
  DeleteActor();
}

/**
 *  @brief  歩けるか判定
 */
bool TrafficActor::CheckEnableWalk( f32 size, f32 length )
{
  bool bResult = true;      //!< 返却値
  Field::MoveModel::FieldMoveModel *pChara = m_pMoveModelManager->GetFieldMoveModel( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );
  Field::MoveModel::FieldMoveModel *pPlayer = m_pMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );
  poke_3d::model::CharaModel *pCharaDrawInstance = pChara->GetCharaDrawInstance();

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
 *  @brief  パスの取得
 */
TrafficPath *TrafficActor::GetPath( void )
{
  if( 0 <= m_PathID && m_PathID < Traffic::FIELD_TRAFFIC_MAX )
  {
    return m_pTrafficManager->GetTrafficPath( m_PathID );
  }

  return NULL;
}

/**
 *  @brief  往来キャラクタを取得する
 */
MoveModel::FieldMoveModel *TrafficActor::GetMoveModel( void )
{
  if( !m_pMoveModelManager ){ return NULL; }
  return m_pMoveModelManager->GetFieldMoveModel( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );
}

/**
 *  @brief  退避命令
 */
void TrafficActor::RequestEscape( void )
{
  m_bEscape = true;
  MoveModel::FieldMoveModel *pModel = GetMoveModel();
  if( pModel )
  {
    pModel->SetVisible( false );
    // @fix NMCat[570]:消えたキャラクタに話しかけられないようにする
    StaticActor* pStaticActor = pModel->GetStaticActor( Field::MoveModel::COLLISION_STATIC_TYPE_EVENT_TALK );
    if( pStaticActor )
    {
      pStaticActor->SetActive( false );
    }
  }
}

/**
 *  @brief  スクリプトからの表示操作
 */
void TrafficActor::RequestScriptDisp( bool bDisp )
{
  MoveModel::FieldMoveModel *pModel = GetMoveModel();
  if( !pModel ){ return; }

  Field::MoveModel::FieldMoveCodeWork *pMoveCodeWork = pModel->GetMoveCodeWork();
  if( !pMoveCodeWork ){ return; }

  u32 size = sizeof(Field::MoveModel::FieldMoveCodeTraffic::Work);
  void *work = Field::MoveModel::FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, size );
  Field::MoveModel::FieldMoveCodeTraffic::Work *pWork = static_cast<Field::MoveModel::FieldMoveCodeTraffic::Work*>( work );
  if( !pWork ){ return; }

  // 表示状態を操作しないステートが存在する
  bool bCtrl = true;
  switch( pWork->state )
  {
  case Field::MoveModel::FieldMoveCodeTraffic::STATE_SUICIDE:   bCtrl = false; break;
  }

  if( bCtrl )
  {
    pModel->SetVisible( bDisp );
  }
}

GFL_NAMESPACE_END(Field)
