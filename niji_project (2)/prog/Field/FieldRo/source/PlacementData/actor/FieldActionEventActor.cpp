/**
 *  GAME FREAK inc.
 *
 *  @file   FieldActionEventActor.cpp
 *  @brief  配置イベント用アクター
 *  @author miyachi_soichi
 *  @date   2015.05.27
 */

// niji
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// Field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldActionEventActor.h"
#include "Field/FieldRo/include/DropItem/FieldDropItemCreator.h"
#include "Field/FieldRo/include/debug/FieldDebugMenu.h"

// Camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetTypeMoveModel.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeArea.h"

// キャラクターシステム
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeRideKentaros.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommand_define.h"

// コリジョンシステム
#include "Field/FieldRo/include/Collision/StaticActor.h"
#include "Field/FieldRo/include/Collision/BaseShape.h"

// エフェクト
#include "Field/FieldRo/include/Effect/content/FieldEffectRock.h"

// convHeader
#include "niji_conv_header/field/FieldActionEventTypes.h"
#include "niji_conv_header/field/chara/p2_base_fi.h"
#include <niji_conv_header/field/chara/chara_model_id.h>

// イベント
#include "FieldScript/include/EventScriptCall.h"
#include <niji_conv_header/field/script/inc/field_event.inc>

// サウンド
#include "Sound/include/Sound.h"

// savedata
#include "Savedata/include/EventWork.h"
#include  "Savedata/include/Record.h"

// アクションコマンド
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandStepJump.h"


#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "niji_reference_files/script/FieldPokemonFureaiScriptTypes.h"

GFL_NAMESPACE_BEGIN( Field )

/**
 *  @brief  コンストラクタ
 */
FieldActionEventActor::FieldActionEventActor( void )
  : IFieldActorBase()
  , m_pData( NULL )
  , m_Accessor()
  , m_pTalkActor(NULL)
{
}

/**
 *  @brief  デストラクタ
 */
FieldActionEventActor::~FieldActionEventActor( void )
{
}



/**
 *  @brief  初期化
 */
void FieldActionEventActor::Initialize( const Description& description )
{
  if( description.colAccessor->GetDataMax() <= 0 ){ return; }
  m_pCollisionScene = description.pCollisionScene;
  m_Size = description.colAccessor->GetDataMax();
  m_vPos = description.vPos;

  m_pData= description.pData;

  // アクター配列確保
  m_ppActor = GFL_NEW_ARRAY( description.pHeap ) StaticActor*[m_Size];

  // アクター生成
  for( u32 i = 0; i < m_Size; ++ i )
  {
    // データ取得
    FieldCollisionAccessor::CollisionData colData;
    description.colAccessor->GetData( i, &colData );

    // アクターの生成
    StaticActor *pActor = m_pCollisionScene->CreateStaticActor( description.pAllocator, description.pHeap );

    // コリジョン設定
    FieldCollisionAccessor::CreateActor( pActor, colData, description.vPos );
    // ユーザーデータ設定
    pActor->SetUserData( reinterpret_cast<void*>( this ) );

    m_ppActor[ i ] = pActor;
  }

  // 話しかけ範囲生成
  if( IsTalkEventMake( m_pData ) )
  {
    m_pTalkActor = GFL_NEW( description.pHeap ) FieldActionEventTalkActor();
    m_pTalkActor->Initialize( description );
  }
}

/**
 *  @brief  破棄
 */
void FieldActionEventActor::Terminate( void )
{
  if( m_pTalkActor )
  {
    m_pTalkActor->Terminate();
    GFL_SAFE_DELETE( m_pTalkActor );
  }
  IFieldActorBase::Terminate();
}


/**
 *  @brief  起動
 */
bool FieldActionEventActor::Kick( const KickRequest &req, bool * p_outIsEventCreate )
{
  if( p_outIsEventCreate ) *p_outIsEventCreate = false; 

  if( !m_pData ){ return false; }

  // 条件を満たしていない
  if( IsInvalid() ){ return false; }

  const void* pActionData = m_Accessor.GetActionData( *m_pData);

  // 戻り値
  bool ret = false;

  // 起動する
  switch( m_pData->actionID )
  {
  case ACTIONEVENT_OTTOTTO:
  case ACTIONEVENT_STEPJUMP:
  case ACTIONEVENT_OFFROADBORDER:
    {
      // 段差ジャンプはライン形状のシェイプのみで起動させる
      BaseActor::SHAPE shape = req.pActor->GetShapeType();
      if( shape==BaseActor::SHAPE_LINE )
      {
        // ラインシェイプと対象となる動作モデルを取得
        LineShape* pLineShape = static_cast<LineShape*>( req.pActor->GetShape() );
        Field::MoveModel::FieldMoveModelPlayer* pFieldModelPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(req.pPlayer);
        if( pLineShape 
        &&  pFieldModelPlayer
        && !pFieldModelPlayer->IsExecuteActionCommand() )
        {
          // プレイヤーの向きを取得
          gfl2::math::Vector4 vec0(0.0f,0.0f,1.0f,0.0f);
          {
            gfl2::math::Matrix34 mat;
            pFieldModelPlayer->GetCharaDrawInstance()->GetRotationQuat().QuaternionToMatrix(mat);
            vec0   = mat.TransformNormal(vec0);
            vec0.y = 0.0f;
            vec0   = vec0.Normalize();
          }

          // イベントの向き
          gfl2::math::Vector4 vec1(0.0f,0.0f,1.0f,0.0f);
          {
            gfl2::math::Vector3 pos = pLineShape->m_pos1 - pLineShape->m_pos0;
            gfl2::math::Matrix34  mat = gfl2::math::Matrix34::GetRotationY(PI/2.0f);
            vec1   = mat.TransformNormal(gfl2::math::Vector4(pos.x,pos.y,pos.z,0.0f));
            vec1.y = 0.0f;
            vec1   = vec1.Normalize();
          }

          f32 ANGLE_THRESHOLD = 45.0f;
#if PM_DEBUG
          // デバッグメニューから
          Debug::FieldDebugMenu* pFieldDebugMenu = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu();
          if( pFieldDebugMenu && pFieldDebugMenu->isUseJumpAngle() )
          {
            ANGLE_THRESHOLD = (f32)pFieldDebugMenu->GetJumpAngle();
          }
#endif

          // イベントに対しプレイヤーが進入角度をまもってに入ったかどうか
          f32 dot = vec0.Dot( vec1 );
          f32 deg = gfl2::math::ConvRadToDeg( acosf( dot ) );
          if( gfl2::math::FAbs(deg) < ANGLE_THRESHOLD )
          {
            // 段差ジャンプ
            if( m_pData->actionID == ACTIONEVENT_STEPJUMP )
            {
              // 段差ジャンプによる移動量を取得
              gfl2::math::Matrix  rotMat;
              gfl2::math::Vector4 moveVec(Field::MoveModel::ActionCommandStepJump::PLAYER_JUMP_MOVE.x,
                                          Field::MoveModel::ActionCommandStepJump::PLAYER_JUMP_MOVE.y,
                                          Field::MoveModel::ActionCommandStepJump::PLAYER_JUMP_MOVE.z);  
              if( pFieldModelPlayer->GetOnMoveModel() )
              {
                moveVec.x = Field::MoveModel::ActionCommandStepJump::RIDE_JUMP_MOVE.x;
                moveVec.y = Field::MoveModel::ActionCommandStepJump::RIDE_JUMP_MOVE.y;
                moveVec.z = Field::MoveModel::ActionCommandStepJump::RIDE_JUMP_MOVE.z;
              }
              pFieldModelPlayer->GetCharaDrawInstance()->GetRotationQuat().QuaternionToMatrix(rotMat);
              moveVec = rotMat.TransformNormal(moveVec);

              // 移動前と移動後に壁が1枚以下なら段差ジャンプができる
              gfl2::math::Vector3 endVec3(pFieldModelPlayer->GetCharaDrawInstance()->GetPosition() + gfl2::math::Vector3(moveVec.x,moveVec.y,moveVec.z) );

              // ライド&バンバドロにのっているかどうか調べる
              b32 isRide      = false;
              b32 isRideRoba2 = false;
              if( pFieldModelPlayer->GetOnMoveModel() )
              {
                isRide = true;
                if( pFieldModelPlayer->GetOnMoveModel()->GetCharacterId() == CHARA_MODEL_ID_PM0840_00 )
                {
                  isRideRoba2 = true;
                }
              }

              if( pFieldModelPlayer->IsEnableStepJump( endVec3, true, isRideRoba2?false:true, isRide) )
              {
                pFieldModelPlayer->SetActionCommand( Field::MoveModel::FIELD_ACTION_COMMAND_STEP_JUMP );
                ret = true;
              }
            }
            // 地面馬以外がおっとっと
            else if( m_pData->actionID == ACTIONEVENT_OFFROADBORDER )
            {
              if( ( !pFieldModelPlayer->GetOnMoveModel() ) 
               || (  pFieldModelPlayer->GetOnMoveModel() && pFieldModelPlayer->GetOnMoveModel()->GetCharacterId() != CHARA_MODEL_ID_PM0840_00 ) )
              {
                if( !pFieldModelPlayer->IsLockDirFromOutside() && pFieldModelPlayer->LockDirFromOutside() )
                {
                  pFieldModelPlayer->SetActionCommand( Field::MoveModel::FIELD_ACTION_COMMAND_REFUSE );
                  ret = true;
                }
              }
            }
            // おっとっと
            else
            {
              if( !pFieldModelPlayer->IsLockDirFromOutside() && pFieldModelPlayer->LockDirFromOutside() )
              {
                pFieldModelPlayer->SetActionCommand( Field::MoveModel::FIELD_ACTION_COMMAND_REFUSE );
                ret = true;
              }
            }
          }
        }
      }
    }
    break;
  case ACTIONEVENT_ROCKBREAKER:
    {
        StaticModel::StaticModel *pStaticModel = req.pStaticModelManager->GetEventIDStaticModel( GetData()->eventID );

        // ライド中なら
        Field::MoveModel::FieldMoveModelPlayer* pFieldModelPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(req.pPlayer);
        if( pStaticModel
        &&  pFieldModelPlayer 
        &&  pFieldModelPlayer->GetOnMoveModel() )
        {
          // ケンタロス or サメハダーに乗っていて対象となる岩なら
          if( (pFieldModelPlayer->GetMoveCodeId() == Field::MoveModel::FIELD_MOVE_CODE_RIDE_KENTAROS  &&  pFieldModelPlayer->IsRunLoopState()) 
           || (pFieldModelPlayer->GetMoveCodeId() == Field::MoveModel::FIELD_MOVE_CODE_RIDE_SAMEHADER &&  pFieldModelPlayer->IsRunLoopState()) )
          {

            // @fix GFNMCat[2465] ケンタロスの岩を背中向きでも破壊できる

            // プレイヤーの向きを取得
            gfl2::math::Vector4 vec0(0.0f,0.0f,1.0f,0.0f);
            {
              gfl2::math::Matrix34 mat;
              pFieldModelPlayer->GetCharaDrawInstance()->GetRotationQuat().QuaternionToMatrix(mat);
              vec0   = mat.TransformNormal(vec0);
              vec0.y = 0.0f;
              vec0   = vec0.Normalize();
            }

            // プレイヤーからみたイベントの方向を取得
            gfl2::math::Vector4 vec1(0.0f,0.0f,1.0f,0.0f);
            {
              vec1   = pStaticModel->GetLocalSRT().translate - pFieldModelPlayer->GetCharaDrawInstance()->GetPosition();
              vec1.y = 0.0f;
              vec1   = vec1.Normalize();
            }

             // イベントに対しプレイヤーが進入角度をまもって入ったかどうか
            f32 ANGLE_THRESHOLD = 80.0f;
            f32 dot = vec0.Dot( vec1 );
            f32 deg = gfl2::math::ConvRadToDeg( acosf( dot ) );
            if( gfl2::math::FAbs(deg) > ANGLE_THRESHOLD )
            {
              break;
            }

            // イベントコリジョンのオフ
            req.pActor->SetActive(false);
            // 押し返しコリジョンのオフ
            if( pStaticModel->GetStaticActor() )
            {
              pStaticModel->GetStaticActor()->SetActive(false);
            }

            // MCat[289]
            // Actorのunresist
            reinterpret_cast<IFieldActorBase*>(req.pActor->GetUserData())->UnregistActor();


            // 破壊アニメーションを再生
            StaticModel::StaticModel::Handle_StartEventAnimation handle(0);
            pStaticModel->Set( &handle );
            pStaticModel->Update();

            // ヒットストップ
            const Field::MoveModel::FieldMoveCodeRideKentaros* pMoveCodeKentaros = static_cast<const Field::MoveModel::FieldMoveCodeRideKentaros*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCode(Field::MoveModel::FIELD_MOVE_CODE_RIDE_KENTAROS) );
            pFieldModelPlayer->SetStopPreUpdateFrame(pMoveCodeKentaros->GetHitStopFrame());
            pFieldModelPlayer->GetOnMoveModel()->SetStopPreUpdateFrame(pMoveCodeKentaros->GetHitStopFrame());
            pStaticModel->SetStopUpdateFrame(pMoveCodeKentaros->GetHitStopFrame());

            // カメラシェイク
            GameSys::GameManager*         pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
            Fieldmap*                     pFieldMap      = pGameManager->GetFieldmap();
            Field::Camera::CameraManager* pCameraManager = pFieldMap->GetCameraManager();
            Field::Camera::CameraUnit*    pCameraUnit    = pCameraManager->GetMainViewCamera();
            if( pCameraUnit )
            {
              Field::Camera::Controller::ControllerBase* pController = pCameraUnit->GetControllerBase();
              if( pController )
              {
                pController->ShakeHorizonRequest( pMoveCodeKentaros->GetCameraShakeMass(),
                                                  pMoveCodeKentaros->GetCameraShakeK(),
                                                  pMoveCodeKentaros->GetCameraShakeFirstVelocity(),
                                                  pMoveCodeKentaros->GetCameraShakeDamping() );
              }
            }

            // エフェクト表示
            Field::Effect::EffectRock* pRock = static_cast<Field::Effect::EffectRock* >( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_ROCKBREAK_W , pStaticModel ) );
            if( pRock )
            {
              if( pFieldModelPlayer->GetMoveCodeId() == Field::MoveModel::FIELD_MOVE_CODE_RIDE_KENTAROS  ){ pRock->CreateRock(false); }
              if( pFieldModelPlayer->GetMoveCodeId() == Field::MoveModel::FIELD_MOVE_CODE_RIDE_SAMEHADER ){ pRock->CreateRock(true ); }
            }

            // ドロップアイテム生成
            const FieldActionEventAccessor::ActionRockBreakerData* pRockBreakerData;
            pRockBreakerData = reinterpret_cast<const FieldActionEventAccessor::ActionRockBreakerData*>( pActionData);
            DropItem::Creator* pCreator = pFieldMap->GetDropItemCreator();
            const Field::Location* pLocation = pGameManager->GetGameData()->GetStartLocation();

            bool isCreate = false;
            FieldItemManager::Type dropItemType = FieldItemManager::TYPE_NORMAL;
            if( pFieldModelPlayer->GetMoveCodeId() == Field::MoveModel::FIELD_MOVE_CODE_RIDE_SAMEHADER ){ dropItemType = FieldItemManager::TYPE_SEA; }
            isCreate = pCreator->CreateLotteryDropItem( pRockBreakerData->dropItemID, pStaticModel->GetLocalSRT().translate, dropItemType);

            if( isCreate)
            {
              // ドロップアイテムSE再生。岩が壊れる音と重ならないよう遅れて音がなるリソース
              Sound::PlaySE( SEQ_SE_FLD_DROPITEM);
              Savedata::IncRecord( Savedata::Record::RECID_BRAKE_ITEM );  // 岩を壊してドロップアイテムを出現させた数
            }

						// ふれあいリアクション通知
		        {
              GameSys::GameManager* pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
							Field::FieldResident* pFieldResident = pGameManager->GetFieldResident();
			        Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();
							pPlacementDataManager->RequestEventPlayerActionForFureaiActor(PokemonFureai::FPF_PLAYER_ACITON_IWA_KUDAKI);
		        }

            // 破壊状況が巻き戻らないようフラグを落とす
            Field::EventWork * pEventWork = pGameManager->GetGameData()->GetEventWork();
            if( pEventWork->CheckIDFlagRenge( m_pData->flagwork ) )
            {
              pEventWork->SetEventFlag( m_pData->flagwork );
            }
            else
            {
              GFL_ASSERT_MSG(0, "ActionEvent Support Is Flag Only [%d]", m_pData->flagwork);
            }

            ret = true; // 実行した。
          }
        }
      }
    break;
  default:
    break;
  }

  return ret;
}

/**
 *  @brief  判定の共通処理
 */
bool FieldActionEventActor::_IsInvalid( const FieldActionEventAccessor::ActionEventData *pData )
{
  bool bResult = true;
  // データ設定がおかしい
  if( !pData ){ return bResult; }

  switch( pData->actionID )
  {
  case ACTIONEVENT_STEPJUMP:
    bResult = false;
    break;
  case ACTIONEVENT_ROCKBREAKER:
    bResult = false;
    break;
  case ACTIONEVENT_OTTOTTO:
    bResult = false;
    break;
  case ACTIONEVENT_OFFROADBORDER:
    bResult = false;
    break;
  default:
    GFL_ASSERT_MSG(false, "Invalid ActionType !!!");
    break;
  }

  return bResult;
}

/**
 *  @brief  判定
 */
bool FieldActionEventActor::IsInvalid( void )
{
  return _IsInvalid(m_pData);
}

/**
 *  @brief  追加
 */
void FieldActionEventActor::RegistActor( void )
{
  IFieldActorBase::RegistActor();
  if(m_pTalkActor) m_pTalkActor->RegistActor();

  for( u32 i = 0; i < m_Size; ++i)
  {
    if( m_ppActor[i])
    {
      m_ppActor[i]->SetActive( true);
    }
  }
}

/**
 *  @brief  削除
 */
void FieldActionEventActor::UnregistActor( void )
{
  IFieldActorBase::UnregistActor();
  if(m_pTalkActor) m_pTalkActor->UnregistActor();

  for( u32 i = 0; i < m_Size; ++i)
  {
    if( m_ppActor[i])
    {
      m_ppActor[i]->SetActive( false);
    }
  }
}

/**
 *  @brief  専用アクセサー初期化
 */
void FieldActionEventActor::InitializeAccessor( const void* pData)
{
  m_Accessor.Initialize( pData);
}

/**
 *  @brief  話しかけイベントを生成するか？
 */
bool FieldActionEventActor::IsTalkEventMake( const FieldActionEventAccessor::ActionEventData *pData )
{
  bool bResult = false;
  // データ設定がおかしい
  if( !pData ){ return bResult; }

  switch( pData->actionID )
  {
  case ACTIONEVENT_STEPJUMP:
    bResult = false;
    break;
  case ACTIONEVENT_ROCKBREAKER:
    bResult = true;
    break;
  case ACTIONEVENT_OTTOTTO:
    bResult = false;
    break;
  case ACTIONEVENT_OFFROADBORDER:
    bResult = false;
    break;
  default:
    GFL_ASSERT_MSG(false, "Invalid ActionType !!!");
    break;
  }

  return bResult;
}

/**
 *  @brief  話しかけイベントのスクリプトID取得
 */
u32 FieldActionEventActor::GetTalkEventScrID( const FieldActionEventAccessor::ActionEventData *pData )
{
  u32 bResult = FieldScript::SCRID_NULL;
  // データ設定がおかしい
  if( !pData ){ return bResult; }

  switch( pData->actionID )
  {
  case ACTIONEVENT_STEPJUMP:
    bResult = FieldScript::SCRID_NULL;
    break;
  case ACTIONEVENT_ROCKBREAKER:
    bResult = SCRID_FLD_EV_KOWASERUROCK_01;
    break;
  case ACTIONEVENT_OTTOTTO:
    bResult = FieldScript::SCRID_NULL;
    break;
  default:
    GFL_ASSERT_MSG(false, "Invalid ActionType !!!");
    break;
  }

  return bResult;
}



/** 
 *  @brief コンストラクタ
 */
FieldActionEventTalkActor::FieldActionEventTalkActor(void)
  : IFieldActorBase()
  , m_pData( NULL )
{
}

/** 
 *  @brief デストラクタ
 */
FieldActionEventTalkActor::~FieldActionEventTalkActor(void)
{
}

/**
 *  @brief  初期化
 */
void FieldActionEventTalkActor::Initialize( const FieldActionEventActor::Description& description )
{
  if( description.colAccessor->GetDataMax() <= 0 ){ return; }
  m_pCollisionScene = description.pCollisionSceneTalk;
  m_Size = description.colAccessor->GetDataMax();
  m_vPos = description.vPos;

  m_pData= description.pData;

  // アクター配列確保
  m_ppActor = GFL_NEW_ARRAY( description.pHeap ) StaticActor*[m_Size];

  static const f32 sc_TalkPulsDistance = 20.0f;

  // アクター生成
  for( u32 i = 0; i < m_Size; ++ i )
  {
    // データ取得
    FieldCollisionAccessor::CollisionData colData;
    description.colAccessor->GetData( i, &colData );

    // アクターの生成
    StaticActor *pActor = m_pCollisionScene->CreateStaticActor( description.pAllocator, description.pHeap );

    // 膨らませる。
    switch( colData.type )
    {
    case FieldCollisionAccessor::CYLINDER:
      {
        colData.cylinder.radius += sc_TalkPulsDistance;
      }
      break;

    case FieldCollisionAccessor::BOX:
      {
        GFL_ASSERT_STOP(0);
      }
      break;

    case FieldCollisionAccessor::LINE:
      {
        GFL_ASSERT_STOP(0);
      }
      break;

    case FieldCollisionAccessor::TRIANGLE:
      {
        GFL_ASSERT_STOP(0);
      }
      break;
    }

    // コリジョン設定
    FieldCollisionAccessor::CreateActor( pActor, colData, description.vPos );

    // ユーザーデータ設定
    pActor->SetUserData( reinterpret_cast<void*>( this ) );

    m_ppActor[ i ] = pActor;
  }
}

/**
 *  @brief  起動
 */
bool FieldActionEventTalkActor::Kick( const KickRequest &req, bool* p_outIsEventCreate )
{
  // イベント起動したかフラグをクリア
  if( p_outIsEventCreate ) *p_outIsEventCreate = false;

  // 起動条件を満たしていない
  if( IsInvalid() ){ return false; }

  u32 script_id = FieldActionEventActor::GetTalkEventScrID( m_pData );

  // GameManagerがほしい
  EventScriptCall::CallScript( GameSys::GameManager::GetInstance(), script_id, this );
  if( p_outIsEventCreate ) *p_outIsEventCreate = true;

  return true;
}

/**
 *  @brief  判定
 */
bool FieldActionEventTalkActor::IsInvalid( void )
{
  return FieldActionEventActor::_IsInvalid(m_pData);
}

GFL_NAMESPACE_END( Field )
