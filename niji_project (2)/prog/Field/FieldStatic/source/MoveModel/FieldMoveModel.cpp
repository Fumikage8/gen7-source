//=================================================================================================
/**
*  @file   FieldMoveModel.cpp
*  @brief  フィールド用 動作モデル
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <debug/include/gfl2_DebugPrint.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h" 
#include "Field/FieldStatic/include/FieldNode.h"
#include "../../include/MoveModel/FieldMoveModel.h"
#include "../../include/MoveModel/FieldSubobject.h"
#include "../../include/MoveModel/FieldFacialAnime.h"
#include "../../include/MoveCode/FieldMoveCode_define.h"
#include "../../include/MoveCode/FieldMoveCodeManager.h"
#include "../../include/MoveCode/FieldMoveCodeBase.h"
#include "../../include/ActionCommand/FieldActionCommandManager.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommand_define.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandBase.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandStepMove.h"
#include "Field/FieldRo/include/Collision/BaseShape.h"
#include "Field/FieldRo/include/Collision/BaseCollisionScene.h"
#include "Field/FieldRo/include/Collision/RaycastCustomCallback.h"
#include "Field/FieldRo/include/MotionSignal/FieldMotionSignal.h"
#include "../../../../../../poke_3d_lib/model/include/gfl2_CharaModelFactory.h"
#include "niji_conv_header/field/chara/p2_base_fi.h"
#include "niji_conv_header/field/chara/tr0082_00.h"
#include "niji_conv_header/field/attribute/attribute_id.h"
#include "niji_conv_header/field/chara/pc0001_00_spmot_ev0500.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFootLand.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFootSand.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFootWater.h"
#include "Field/FieldStatic/include/MoveModel/Effect/FieldMoveModelEffectCreaterPlayer.h"
#include "Field/FieldStatic/include/MoveModel/Effect/FieldMoveModelEffectCreaterNpc.h"
#include "Field/FieldStatic/include/FieldAttribute.h"
// Effect
#include "Field/FieldRo/include/Effect/content/FieldEffectCharaRipple.h"
// RTTIデバッグシステム
#include "Field/FieldRo/include/RttiDebug/RttiEditor.h"
#include "Field/FieldRo/include/RttiDebug/RttiManager.h"
// debug
#if PM_DEBUG
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Field/FieldRo/include/Debug/FieldDebugMapJump.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#endif

// ゾーン
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"

namespace Field{ namespace MoveModel {

const f32 FieldMoveModel::GROUND_OFFSET( 4.0f );  // 床コリジョンの結果に足しこむ高さのオフセット

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveModel::FieldMoveModel()
: m_isSetupResource(false) // ClearWork関数を正しく走らせるためにリソースがセットアップされているかどうかだけコンストラクタで値をいれる
, m_isFreeMoveForEventScript         ( false )
, m_isPointMoveForEventScript        ( false )
{
  ClearWork();
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveModel::~FieldMoveModel()
{
  ClearWork();
  ClearResource();
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化(ワーク)
*
*  @param  pFieldMoveModelHeader  作成リクエスト
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetupWork(const FieldMoveModelHeaderWork* pFieldMoveModelHeaderWork)
{
  // ワークが既に初期化されているかどうか
  if( m_isSetupWork )
  {
    return;
  }
  m_headerWork  = *pFieldMoveModelHeaderWork;
  m_isSetupWork = true;

  // ムーブコードワークの初期化  
  Field::MoveModel::FieldMoveCodeManager::InitializeMoveCodeWork( &m_moveCodeWork, this, m_moveCodeExtendWorkSize, m_pMoveCodeExtendWork );
  m_pMoveCode = Field::MoveModel::FieldMoveCodeManager::GetMoveCode( m_headerWork.moveCodeId );
  m_pMoveCode->InitProcess( &m_moveCodeWork );

  // ムーブコードワークはセーブデータによる復元をしていない状態する
  m_moveCodeWork.isSetupWorkFromSaveData = false;

  // アクションコマンドワークの初期化
  Field::MoveModel::FieldActionCommandManager::InitializeActionCommandWork( &m_actionCommandWork, this );

  // 描画/座標/姿勢を復元用
  m_visibleForReturn       = true;
  m_enableVisibleForReturn = true;
  m_positionForReturn      = pFieldMoveModelHeaderWork->posForReturn;
  m_quaternionForReturn    = pFieldMoveModelHeaderWork->quaForReturn;
  m_srtForReturn.translate = m_positionForReturn;
  m_srtForReturn.rotation  = m_quaternionForReturn;
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化(リソース)
*
*  @param  pHeap                  ヒープ
*  @param  pMyRenderingPipeLine   レンダリングパイプライン
*  @param  pCharaModelFactory     キャラリソースファクトリー
*  @param  pFieldMoveModelHeader  作成リクエスト
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetupResource(gfl2::heap::HeapBase*                        pHeap,
                                   Field::MyRenderingPipeLine*                  pMyRenderingPipeLine,
                                   poke_3d::model::CharaModelFactory*           pCharaModelFactory,
                                   poke_3d::model::DressUpModelResourceManager* pDressUpModelResourceManager,
                                   const FieldMoveModelHeaderResource*          pFieldMoveModelHeaderResource)
{
  // リソースが既に作成されているかどうか
  if( m_isSetupResource )
  {
    return;
  }
  m_headerResource         = *pFieldMoveModelHeaderResource;
  m_isSetupResource        = true;
  m_animationId            = -1;
  m_prevAnimationFrame     = 0.0f;
  m_prevAnimationStepFrame = 1.0f;
  m_prevWalkSpeedY         = 0.0f;
  m_pLocalAllocator        = GFL_NEW( pHeap ) System::nijiAllocator( pHeap );
  m_pMyRenderingPipeLine   = pMyRenderingPipeLine;
  m_interestInitSpeedRequest = false; // CreateModel時に注目速度は初期化されるため

  // 着せ替えなし 描画インスタンスを作成
  if( !pFieldMoveModelHeaderResource->pDressUpParam )
  {
    m_pCharaDrawInstance  = GFL_NEW( pHeap ) poke_3d::model::CharaModel();
    bool result = pCharaModelFactory->CreateModel( m_pLocalAllocator, pHeap, m_pCharaDrawInstance , pFieldMoveModelHeaderResource->characterId );
    GFL_ASSERT(result);
  }
  // 着せ替えあり 描画インスタンスを作成
  else
  {
    m_pCharaDrawInstance  = GFL_NEW( pHeap ) poke_3d::model::DressUpModel();
    poke_3d::model::DressUpModel* pDressUpModel = static_cast<poke_3d::model::DressUpModel*>(m_pCharaDrawInstance);
    bool result = pCharaModelFactory->CreateDressUpModel( m_pLocalAllocator, pHeap, pDressUpModel, pDressUpModelResourceManager, pFieldMoveModelHeaderResource->characterId );
    GFL_ASSERT(result);
  }

  // レンダリングパイプランに登録
  AddEdgeRenderingTarget();

#if defined( GF_PLATFORM_CTR )
  // エッジIDの設定
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode = m_pCharaDrawInstance->GetModelInstanceNode();
  if( pModelInstanceNode )
  {
    pModelInstanceNode->CopyMaterialDepthStencilStateToStateObject();
    for( u32 i=0 ; i<pModelInstanceNode->GetMaterialNum() ; i++ )
    {
      u32 id = (u32)pModelInstanceNode->GetMaterialInstanceNode(i)->GetDepthStencilStateObject()->GetStencilReference();
      id += m_edgeId;
      if     ( id >= 255 ){ id = 255; }  // エッジをださない場合の特例
      else if( id >= 150 ){ id = 150; }  // キャラに割り当てられるのは150まで、それ以降は背景用
      pModelInstanceNode->GetMaterialInstanceNode(i)->GetDepthStencilStateObject()->SetStencilReference(static_cast<u8>(id));
      pModelInstanceNode->GetMaterialInstanceNode(i)->GetDepthStencilStateObject()->UpdateState();
    }
  }
#endif

  // 初期座標の設定
  m_pCharaDrawInstance->SetPosition    ( m_headerResource.pos );
  m_pCharaDrawInstance->SetRotationQuat( m_headerResource.qua );

  // Tスタンスにならないように待機モーションを流す
  m_pCharaDrawInstance->ChangeAnimation( m_defaultIdleAnimationId );

  // note: アクターのワークは事前に確保せず都度動的に確保するため、断片化のことも考えワークですがリソースと同タイミング(アプリ遷移時)に作成します。

  // コリジョンの作成
  if( m_headerResource.pCollisionSceneForTerrain )
  {
    m_pDynamicActor[COLLISION_TYPE_TERRAIN] = m_headerResource.pCollisionSceneForTerrain->CreateDynamicActor(m_pLocalAllocator,pHeap);
  }
  if( m_headerResource.pCollisionSceneForStaticModel )
  {
    m_pDynamicActor[COLLISION_TYPE_STATIC_MODEL]        = m_headerResource.pCollisionSceneForStaticModel->CreateDynamicActor(m_pLocalAllocator,pHeap);
    m_pStaticActor [COLLISION_STATIC_TYPE_STATIC_MODEL] = m_headerResource.pCollisionSceneForStaticModel->CreateStaticActor(m_pLocalAllocator,pHeap);
  }
  if( m_headerResource.pCollisionSceneForTrafficModel )
  {
    m_pDynamicActor[COLLISION_TYPE_TRAFFIC_MODEL]        = m_headerResource.pCollisionSceneForTrafficModel->CreateDynamicActor(m_pLocalAllocator,pHeap);
    m_pStaticActor [COLLISION_STATIC_TYPE_TRAFFIC_MODEL] = m_headerResource.pCollisionSceneForTrafficModel->CreateStaticActor(m_pLocalAllocator,pHeap);
  }
  if( m_headerResource.pCollisionSceneForEventPos )
  {
    m_pDynamicActor[COLLISION_TYPE_EVENT_POS] = m_headerResource.pCollisionSceneForEventPos->CreateDynamicActor(m_pLocalAllocator,pHeap);
  }
  if( m_headerResource.pCollisionSceneForFishingPos )
  {
    m_pDynamicActor[COLLISION_TYPE_FISHING_POS] = m_headerResource.pCollisionSceneForFishingPos->CreateDynamicActor(m_pLocalAllocator,pHeap);
  }

  if( m_headerResource.pCollisionSceneForEventIntrPos )
  {
    m_pDynamicActor[COLLISION_TYPE_EVENT_INTRPOS] = m_headerResource.pCollisionSceneForEventIntrPos->CreateDynamicActor(m_pLocalAllocator,pHeap);
  }
  if( m_headerResource.pCollisionSceneForEventTalk )
  {
    m_pDynamicActor[COLLISION_TYPE_EVENT_TALK]        = m_headerResource.pCollisionSceneForEventTalk->CreateDynamicActor(m_pLocalAllocator,pHeap);
    m_pStaticActor [COLLISION_STATIC_TYPE_EVENT_TALK] = m_headerResource.pCollisionSceneForEventTalk->CreateStaticActor(m_pLocalAllocator,pHeap);
    m_pStaticActor [COLLISION_STATIC_TYPE_EVENT_FUREAI_TALK] = m_headerResource.pCollisionSceneForEventTalk->CreateStaticActor(m_pLocalAllocator,pHeap);
  }
  if( m_headerResource.pCollisionSceneForEventPush )
  {
    m_pDynamicActor[COLLISION_TYPE_EVENT_PUSH] = m_headerResource.pCollisionSceneForEventPush->CreateDynamicActor(m_pLocalAllocator,pHeap);
  }
  if( m_headerResource.pCollisionSceneForEncount )
  {
    m_pDynamicActor[COLLISION_TYPE_ENCOUNT] = m_headerResource.pCollisionSceneForEncount->CreateDynamicActor(m_pLocalAllocator,pHeap);
  }
  if( m_headerResource.pCollisionSceneForTrainerEye )
  {
    m_pDynamicActor[COLLISION_TYPE_TRAINER_EYE]           = m_headerResource.pCollisionSceneForTrainerEye->CreateDynamicActor(m_pLocalAllocator,pHeap);
    m_pStaticActor[COLLISION_STATIC_TYPE_TRAINER_EYE]     = m_headerResource.pCollisionSceneForTrainerEye->CreateStaticActor(m_pLocalAllocator,pHeap);
    m_pStaticActor[COLLISION_STATIC_TYPE_TRAINER_EYE_NEW] = m_headerResource.pCollisionSceneForTrainerEye->CreateStaticActor(m_pLocalAllocator,pHeap);
  }
  if( m_headerResource.pCollisionSceneForStaticModelBBAnimation )
  {
    m_pDynamicActor[COLLISION_TYPE_STATIC_MODEL_BB_ANIMATION] = m_headerResource.pCollisionSceneForStaticModelBBAnimation->CreateDynamicActor(m_pLocalAllocator,pHeap);
  }
  if( m_headerResource.pCollisionSceneForOshikaeshiNaminori)
  {
    m_pDynamicActor[COLLISION_TYPE_OSHIKAESHI_NAMINORI] = m_headerResource.pCollisionSceneForOshikaeshiNaminori->CreateDynamicActor(m_pLocalAllocator,pHeap);
  }
  if( m_headerResource.pCollisionSceneForEventNaminori)
  {
    m_pDynamicActor[COLLISION_TYPE_EVENT_NAMINORI] = m_headerResource.pCollisionSceneForEventNaminori->CreateDynamicActor(m_pLocalAllocator,pHeap);
  }
  if( m_headerResource.pCollisionSceneForGimmickEncount)
  {
    m_pDynamicActor[COLLISION_TYPE_GIMMICK_ENCOUNT]       = m_headerResource.pCollisionSceneForGimmickEncount->CreateDynamicActor(m_pLocalAllocator,pHeap);
    m_pStaticActor[COLLISION_STATIC_TYPE_GIMMICK_ENCOUNT] = m_headerResource.pCollisionSceneForGimmickEncount->CreateStaticActor(m_pLocalAllocator,pHeap);
  }
  if( m_headerResource.pCollisionSceneForGimmickEncountAction)
  {
    m_pDynamicActor[COLLISION_TYPE_GIMMICK_ENCOUNT_ACTION]       = m_headerResource.pCollisionSceneForGimmickEncountAction->CreateDynamicActor(m_pLocalAllocator,pHeap);
    m_pStaticActor[COLLISION_STATIC_TYPE_GIMMICK_ENCOUNT_ACTION] = m_headerResource.pCollisionSceneForGimmickEncountAction->CreateStaticActor(m_pLocalAllocator,pHeap);
  }
  if( m_headerResource.pCollisionSceneForEventFinder )
  {
    m_pDynamicActor[COLLISION_TYPE_EVENT_FINDER] = m_headerResource.pCollisionSceneForEventFinder->CreateDynamicActor(m_pLocalAllocator,pHeap);
  }
  if( m_headerResource.pCollisionSceneForOshikaeshiRoba2)
  {
    m_pDynamicActor[COLLISION_TYPE_OSHIKAESHI_ROBA2] = m_headerResource.pCollisionSceneForOshikaeshiRoba2->CreateDynamicActor(m_pLocalAllocator,pHeap);
  }
  if( m_headerResource.pCollisionSceneForOshikaeshiRide)
  {
    m_pDynamicActor[COLLISION_TYPE_OSHIKAESHI_RIDE] = m_headerResource.pCollisionSceneForOshikaeshiRide->CreateDynamicActor(m_pLocalAllocator,pHeap);
  }

  // コリジョンの座標を初期化
  {
    gfl2::math::Vector3    pos  ( m_pCharaDrawInstance->GetPosition()     );
    gfl2::math::Quaternion quat ( m_pCharaDrawInstance->GetRotationQuat() );

    for( s32 i=0 ; i<COLLISION_TYPE_MAX ; i++ )
    {
      if( m_pDynamicActor[i] )
      {
        m_pDynamicActor[i]->SetPosition  ( pos  );
        m_pDynamicActor[i]->SetQuaternion( quat );
        m_pDynamicActor[i]->InitCollisionResult();
      }
    }

    for( s32 i=0 ; i<COLLISION_STATIC_TYPE_MAX ; i++ )
    {
      if( m_pStaticActor[i] )
      {
        m_pStaticActor[i]->SetPosition  ( pos  );
        m_pStaticActor[i]->InitCollisionResult();
      }
    }
  }

  // アプリケーションデータをコンバートリストから取得
  const Field::MoveModel::ApplicationDataForCommon* pAppData = (const ApplicationDataForCommon*)pCharaModelFactory->GetApplicationData( GetCharacterId() );
  m_appDataType = pAppData->type;

  // 接地情報の初期化
  if( !m_isNeedReturnFromWork )
  {
    // player用にカスタマイズ
    if     ( pAppData->type == Field::MoveModel::TYPE_PLAYER  ){ m_isForceLanding = true; }
    // npc用にカスタマイズ
    else if( pAppData->type == Field::MoveModel::TYPE_NPC     ){ m_isForceLanding = ((ApplicationDataForNPC*    )pAppData)->isLanding; }
    // pokemon用にカスタマイズ
    else if( pAppData->type == Field::MoveModel::TYPE_POKEMON ){ m_isForceLanding = ((ApplicationDataForPokemon*)pAppData)->isLanding; }
  }

  // 目アニメ初期化。目パチのスタートもこの中
  //m_pCharaDrawInstance->SetAutoBlinkMode( poke_3d::model::CharaModel::AUTO_BLINK_MODE_FIELD );
  m_pFacialAnimeEye = GFL_NEW( pHeap ) FieldFacialAnimeEye();
  m_pFacialAnimeEye->Initialize(m_pCharaDrawInstance);
  // 口アニメ初期化
  m_pFacialAnimeMouth = GFL_NEW( pHeap ) FieldFacialAnimeMouth();
  m_pFacialAnimeMouth->Initialize(m_pCharaDrawInstance);

  // エンカウント草エフェクト初期化
  if     ( m_appDataType == Field::MoveModel::TYPE_PLAYER && ( GetCharacterId() == CHARA_MODEL_ID_PC0001_00 || GetCharacterId() == CHARA_MODEL_ID_PC0002_00 ) )
  {
    FieldMoveModelEffectCreaterPlayer* pEncountGrassCreater = GFL_NEW( pHeap ) FieldMoveModelEffectCreaterPlayer();
    m_pEncountGrassCreater = pEncountGrassCreater;

    MoveModel::FieldMoveModelEffectCreaterPlayer::Description desc;
    desc.pEffectManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager();
    desc.pTerrainManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetTerrainManager();
    desc.pPlayer         = this;
    pEncountGrassCreater->Initialize(desc);
  }
  else if( m_appDataType == Field::MoveModel::TYPE_NPC
        || m_appDataType == Field::MoveModel::TYPE_POKEMON )
  {
    FieldMoveModelEffectCreaterNpc* pEncountGrassCreater = GFL_NEW( pHeap ) FieldMoveModelEffectCreaterNpc();
    m_pEncountGrassCreater = pEncountGrassCreater;

    MoveModel::FieldMoveModelEffectCreaterNpc::Description desc;
    desc.pEffectManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager();
    desc.pTerrainManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetTerrainManager();
    desc.pNpc            = this;
    pEncountGrassCreater->Initialize(desc);
  }
  else
  {
    m_pEncountGrassCreater = NULL;
  }

  // 異世界人特殊処理。ゴーグルのON/OFF
  if (m_appDataType == Field::MoveModel::TYPE_NPC)
  {
    u16 zoneId = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetZoneID();
    void* pZoneData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( zoneId );
    if (pZoneData)
    {
      // ゴーグル無効ゾーンなら非表示に
      ZoneDataAccessor zoneDataAccessor(pZoneData);
      if (zoneDataAccessor.IsChrGogglesOn() )
      {
        switch (pFieldMoveModelHeaderResource->characterId)
        {
        case CHARA_MODEL_ID_TR1001_00:
        case CHARA_MODEL_ID_TR1002_00:
        case CHARA_MODEL_ID_TR1003_00:
        case CHARA_MODEL_ID_TR1004_00:
        {
          for (s32 i = 0; i < m_pCharaDrawInstance->GetMeshCount(); ++i)
          {
            gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMeshLinkData = m_pCharaDrawInstance->GetModelInstanceNode()->GetMeshLinkData(i);

            GFL_PRINT("%d:%s\n", i, pMeshLinkData->GetName() );

            if (::strstr(pMeshLinkData->GetName(), "_SwpartsSkin") != NULL)
            {
              m_pCharaDrawInstance->SetMeshVisible(i, false);
            }
          }
          break;
        }
        } // 
      }
    }
  } // 異世界人特殊処理

  // ワークからリソースの状態を元に戻す
  ReturnResourceStateFromWork();
}

//----------------------------------------------------------------------------
/**
*  @brief  解放(ワーク)
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ClearWork(void)
{
  // ワークがありリソースがない状態はあるが、ワークがなくリソースがある状態は許さない
  if( m_isSetupResource )
  {
    ClearResource();
  }

  m_headerWork.Init();
  m_headerResource.Init();
  m_pLocalAllocator                   = NULL;
  m_isSetupWork                       = false;
  m_isSetupResource                   = false;
  m_edgeId                            = 0;
  m_stateBit                          = 0;
  m_stateBitPrev                      = m_stateBit;
  m_stopPreUpdateFrame                = -1;
  m_eventRequestCnt                   = 0;
  m_eventRequestCntPrev               = 0;
  m_isExecuteActionCommand            = false;
  m_isExecuteActionCommandPrev        = false;
  m_isCulling                         = false;
  m_isCullingPrev                     = false;
  m_isRegistToPipeline                = false;
  m_pParentNode                       = NULL;
  m_pCharaDrawInstance                = NULL;
  m_defaultIdleAnimationId            = 0;
  m_animationId                       = -1;
  m_isUseCustomValueForOrigin         = false;
  m_customScaleForOrigin              = 1.0f;
  m_customQuaternionForOrigin         = gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f);
  m_lastEnableOriginMoveVector        = gfl2::math::Vector4(0.0f,0.0f,0.0f,1.0f);
  m_prevAnimationFrame                = 0.0f;
  m_prevAnimationStepFrame            = 0.0f;
  m_prevWalkSpeedY                    = 0.0f;
  m_interestEnable                    = false;
  m_interestPosition                  = gfl2::math::Vector3(0.0f,0.0f,0.0f);
  m_interestResetFrame                = 0;
  m_interestResetRequest              = false;
  m_interestInitSpeedRequest          = false;
  m_pInterestTarget = NULL;
  m_interestTargetJointIndex = -1;
  m_isAutoKutipaku                    = false;
  m_AutoKutipakuSequence              = 0;
  m_AutoKutipackFrame                 = 0;
  m_pFacialAnimeEye                   = NULL;
  m_pFacialAnimeMouth                 = NULL;
  m_pMoveCode                         = NULL;
  m_moveCodeExtendWorkSize            = 0;
  m_pMoveCodeExtendWork               = NULL;
  m_isNeedForceResume                 = false;
  m_isNeedForceSuspend                = false;
  m_isFailedRaycast                   = false;
  m_isCreateGround                    = false;
  m_isForceLanding                    = true;
  m_isOnGround                        = false;
  m_groundAttribute                   = ATTRIBUTE_ERROR;
  m_groundAttributePrev               = ATTRIBUTE_ERROR;
  m_groundAttributeLastSuccessful     = ATTR_TSUCHI;     // 差しさわりの無い属性を入れる
  m_groundHeight                      = 0.0f;
  m_groundHeightPrev                  = 0.0f;
  m_groundNormal                      = gfl2::math::Vector3(0.0f,1.0f,0.0f);
  m_groundNormalPrev                  = gfl2::math::Vector3(0.0f,1.0f,0.0f);
  m_pCollisionSceneContainerForGround = NULL;
  m_pCollisionSceneContainerForWall   = NULL;
  m_positionPrevUpdateStart           = gfl2::math::Vector3(0.0f,0.0f,0.0f);
  m_positionPrevUpdateEnd             = gfl2::math::Vector3(0.0f,0.0f,0.0f);
  m_positionPrevAdjustGround          = gfl2::math::Vector3(99999.9f,99999.9f,99999.9f);
  m_isForceReturnFromWork             = false;
  m_isNeedReturnFromWork              = false;
  m_visibleForReturn                  = true;
  m_enableVisibleForReturn            = true;
  m_positionForReturn                 = gfl2::math::Vector3(0.0f,0.0f,0.0f);
  m_quaternionForReturn               = gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f);
  m_srtForReturn.translate            = gfl2::math::Vector3(0.0f,0.0f,0.0f);
  m_srtForReturn.rotation             = gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f);
  m_srtForReturn.scale                = gfl2::math::Vector3(1.0f,1.0f,1.0f);
  m_isTurnFrame                       = false;
  m_isOneStepFrame                    = false;
  m_oneStepCnt                        = 0.0f;
  m_oneStepMaxCnt                     = 50;
  m_pSignal                           = NULL;
  m_pEncountGrassCreater              = NULL;
  m_isNeedMonsterBall                 = false;
  m_isNeedRideGear                    = false;
  m_pCharaRipple                      = NULL;
  m_onTriangleNormal                  = gfl2::math::Vector4(0.0f,1.0f,0.0f,0.0f);
  m_pActorUserWork = NULL;

  for( s32 i=0; i<3 ; i++ )
  {
    m_onTrianglePosition[i] = gfl2::math::Vector4(100000.0f,100000.0f,100000.0f,0.0f);
  }
  for( s32 i=0 ; i<COLLISION_TYPE_MAX ; i++ )
  {
    m_pDynamicActor[i] = NULL;
  }
  for( s32 i=0 ; i<COLLISION_STATIC_TYPE_MAX ; i++ )
  {
    m_pStaticActor[i] = NULL;
  }
  for( s32 i=0 ; i<FIELD_MOVE_MODEL_SUB_OBJECT_CNT ; i++ )
  {
    m_pSubobject[i] = NULL;
    m_pSubobjectJoint[i] = 0;
    m_subobjectHeaderPrevClearResource[i].Init();
    m_isSubobjectPrevClearResource[i] = false;
  }

  m_isFreeMoveForEventScript         = false;
  m_isPointMoveForEventScript        = false;
}

//----------------------------------------------------------------------------
/**
*  @brief  解放(リソース)
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ClearResource(void)
{
  if( !m_isSetupResource )
  {
    return;
  }

  // ワークが残っている状態でリソースが破棄されたなら、リソースの再セットアップ時に復元を試みる
  if( IsSetupWork() )
  {
    m_isNeedReturnFromWork = true;
  }

  // note: アクターのワークは事前に確保せず都度動的に確保するため、断片化のことも考えワークですがリソースと同タイミング(アプリ遷移時)に破棄します。

  // アクターの登録解除
  if( m_headerResource.pCollisionSceneForTerrain && m_pDynamicActor[COLLISION_TYPE_TERRAIN] )
  {
    m_headerResource.pCollisionSceneForTerrain->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_TERRAIN] ); 
  }
  if( m_headerResource.pCollisionSceneForStaticModel && m_pDynamicActor[COLLISION_TYPE_STATIC_MODEL] )
  {
    m_headerResource.pCollisionSceneForStaticModel->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_STATIC_MODEL] ); 
  }
  if( m_headerResource.pCollisionSceneForTrafficModel && m_pDynamicActor[COLLISION_TYPE_TRAFFIC_MODEL] )
  {
    m_headerResource.pCollisionSceneForTrafficModel->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_TRAFFIC_MODEL] ); 
  }
  if( m_headerResource.pCollisionSceneForEventPos && m_pDynamicActor[COLLISION_TYPE_EVENT_POS] )
  {
    m_headerResource.pCollisionSceneForEventPos->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_EVENT_POS] ); 
  }
  if( m_headerResource.pCollisionSceneForFishingPos && m_pDynamicActor[COLLISION_TYPE_FISHING_POS] )
  {
    m_headerResource.pCollisionSceneForFishingPos->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_FISHING_POS] ); 
  }
  if( m_headerResource.pCollisionSceneForEventIntrPos && m_pDynamicActor[COLLISION_TYPE_EVENT_INTRPOS] )
  {
    m_headerResource.pCollisionSceneForEventIntrPos->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_EVENT_INTRPOS] ); 
  }
  if( m_headerResource.pCollisionSceneForEventTalk && m_pDynamicActor[COLLISION_TYPE_EVENT_TALK] )
  {
    m_headerResource.pCollisionSceneForEventTalk->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_EVENT_TALK] ); 
  }
  if( m_headerResource.pCollisionSceneForEventPush && m_pDynamicActor[COLLISION_TYPE_EVENT_PUSH] )
  {
    m_headerResource.pCollisionSceneForEventPush->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_EVENT_PUSH] ); 
  }
  if( m_headerResource.pCollisionSceneForEncount && m_pDynamicActor[COLLISION_TYPE_ENCOUNT] )
  {
    m_headerResource.pCollisionSceneForEncount->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_ENCOUNT] ); 
  }
  if( m_headerResource.pCollisionSceneForTrainerEye && m_pDynamicActor[COLLISION_TYPE_TRAINER_EYE] )
  {
    m_headerResource.pCollisionSceneForTrainerEye->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_TRAINER_EYE] ); 
  }
  if( m_headerResource.pCollisionSceneForStaticModelBBAnimation && m_pDynamicActor[COLLISION_TYPE_STATIC_MODEL_BB_ANIMATION] )
  {
    m_headerResource.pCollisionSceneForStaticModelBBAnimation->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_STATIC_MODEL_BB_ANIMATION] ); 
  }
  if( m_headerResource.pCollisionSceneForOshikaeshiNaminori && m_pDynamicActor[COLLISION_TYPE_OSHIKAESHI_NAMINORI] )
  {
    m_headerResource.pCollisionSceneForOshikaeshiNaminori->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_OSHIKAESHI_NAMINORI] );
  }
  if( m_headerResource.pCollisionSceneForEventNaminori && m_pDynamicActor[COLLISION_TYPE_EVENT_NAMINORI] )
  {
    m_headerResource.pCollisionSceneForEventNaminori->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_EVENT_NAMINORI] );
  }
  if( m_headerResource.pCollisionSceneForGimmickEncount && m_pDynamicActor[COLLISION_TYPE_GIMMICK_ENCOUNT] )
  {
    m_headerResource.pCollisionSceneForGimmickEncount->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_GIMMICK_ENCOUNT] );
  }
  if( m_headerResource.pCollisionSceneForGimmickEncountAction && m_pDynamicActor[COLLISION_TYPE_GIMMICK_ENCOUNT_ACTION] )
  {
    m_headerResource.pCollisionSceneForGimmickEncountAction->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_GIMMICK_ENCOUNT_ACTION] );
  }
  if( m_headerResource.pCollisionSceneForStaticModel && m_pStaticActor[COLLISION_STATIC_TYPE_STATIC_MODEL] )
  {
    m_headerResource.pCollisionSceneForStaticModel->UnRegistStaticActor( m_pStaticActor[COLLISION_STATIC_TYPE_STATIC_MODEL] ); 
  }
  if( m_headerResource.pCollisionSceneForTrafficModel && m_pStaticActor[COLLISION_STATIC_TYPE_TRAFFIC_MODEL] )
  {
    m_headerResource.pCollisionSceneForTrafficModel->UnRegistStaticActor( m_pStaticActor[COLLISION_STATIC_TYPE_TRAFFIC_MODEL] ); 
  }
  if( m_headerResource.pCollisionSceneForEventTalk && m_pStaticActor[COLLISION_STATIC_TYPE_EVENT_TALK] )
  {
    m_headerResource.pCollisionSceneForEventTalk->UnRegistStaticActor( m_pStaticActor[COLLISION_STATIC_TYPE_EVENT_TALK] ); 
  }
  if( m_headerResource.pCollisionSceneForEventTalk && m_pStaticActor[COLLISION_STATIC_TYPE_EVENT_FUREAI_TALK] )
  {
    m_headerResource.pCollisionSceneForEventTalk->UnRegistStaticActor( m_pStaticActor[COLLISION_STATIC_TYPE_EVENT_FUREAI_TALK] ); 
  }
  if( m_headerResource.pCollisionSceneForTrainerEye && m_pStaticActor[COLLISION_STATIC_TYPE_TRAINER_EYE] )
  {
    m_headerResource.pCollisionSceneForTrainerEye->UnRegistStaticActor( m_pStaticActor[COLLISION_STATIC_TYPE_TRAINER_EYE] ); 
    m_headerResource.pCollisionSceneForTrainerEye->UnRegistStaticActor( m_pStaticActor[COLLISION_STATIC_TYPE_TRAINER_EYE_NEW] ); 
  }
  if( m_headerResource.pCollisionSceneForGimmickEncount && m_pStaticActor[COLLISION_STATIC_TYPE_GIMMICK_ENCOUNT] )
  {
    m_headerResource.pCollisionSceneForGimmickEncount->UnRegistStaticActor( m_pStaticActor[COLLISION_STATIC_TYPE_GIMMICK_ENCOUNT] ); 
  }
  if( m_headerResource.pCollisionSceneForGimmickEncountAction && m_pStaticActor[COLLISION_STATIC_TYPE_GIMMICK_ENCOUNT_ACTION] )
  {
    m_headerResource.pCollisionSceneForGimmickEncountAction->UnRegistStaticActor( m_pStaticActor[COLLISION_STATIC_TYPE_GIMMICK_ENCOUNT_ACTION] ); 
  }
  if( m_headerResource.pCollisionSceneForEventFinder && m_pDynamicActor[COLLISION_TYPE_EVENT_FINDER] )
  {
    m_headerResource.pCollisionSceneForEventFinder->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_EVENT_FINDER] ); 
  }
  if( m_headerResource.pCollisionSceneForOshikaeshiRoba2 && m_pDynamicActor[COLLISION_TYPE_OSHIKAESHI_ROBA2] )
  {
    m_headerResource.pCollisionSceneForOshikaeshiRoba2->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_OSHIKAESHI_ROBA2] );
  }
  if( m_headerResource.pCollisionSceneForOshikaeshiRide && m_pDynamicActor[COLLISION_TYPE_OSHIKAESHI_RIDE] )
  {
    m_headerResource.pCollisionSceneForOshikaeshiRide->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_OSHIKAESHI_RIDE] );
  }

  m_pCollisionSceneContainerForGround = NULL;
  m_pCollisionSceneContainerForWall   = NULL;

  // アクターの破棄
  for( s32 i=0 ; i<COLLISION_TYPE_MAX ; i++ )
  {
    GFL_SAFE_DELETE( m_pDynamicActor[i] );
  }
  for( s32 i=0 ; i<COLLISION_STATIC_TYPE_MAX ; i++ )
  {
    GFL_SAFE_DELETE(m_pStaticActor[i]);
  }
  
  // 復帰情報を消さないようにサブオブジェクトを削除
  for( s32 i=0 ; i<FIELD_MOVE_MODEL_SUB_OBJECT_CNT ; i++ )
  {
    if( m_pSubobject[i] )
    {
      m_pSubobject[i]->GetCharaDrawInstance()->GetModelInstanceNode()->RemoveFromParent();
      m_pSubobject[i]->EnableDeleteRequest();
      m_pSubobject[i]      = NULL;
      m_pSubobjectJoint[i] = 0;
    }
  }

  // エンカウント草エフェクト削除
  if( m_pEncountGrassCreater )
  {
    m_pEncountGrassCreater->Terminate();
    GFL_SAFE_DELETE(m_pEncountGrassCreater);
  }

  // NPC泳ぎ波紋を削除
  if( m_pCharaRipple )
  {
    Field::Effect::EffectManager *pEffectManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager();
    if( pEffectManager )
    {
      pEffectManager->DeleteEffect( m_pCharaRipple );
    }
    m_pCharaRipple = NULL;
  }

  // 強制的に注目を解除( ↓で削除している描画インスタンスと絡んでいるのでクリアする必要あり )
  DisableInterestForce();

  // 描画パイプラインから削除( ↓で削除している描画インスタンスと絡んでいるのでクリアする必要あり )
  RemoveEdgeRenderingTarget();

  // ツリー情報を変更しないようにカリング状態を解除( ↓で削除している描画インスタンスと絡んでいるのでクリアする必要あり )
  m_isCulling          = false;
  m_isCullingPrev      = false;
  m_isRegistToPipeline = false;
  m_pParentNode        = NULL;

  // ムーブコードワークの終了( 描画インスタンスと絡んでいるので終了する必要あり )
  m_pMoveCode->Retirement( &m_moveCodeWork,false );

  // 目・口アニメ用インスタンス破棄
  GFL_SAFE_DELETE( m_pFacialAnimeEye );
  GFL_SAFE_DELETE( m_pFacialAnimeMouth );

  // 描画インスタンスの破棄
  GFL_SAFE_DELETE( m_pCharaDrawInstance );

  // アロケータ削除
  GFL_SAFE_DELETE( m_pLocalAllocator );

  m_isSetupResource = false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  前更新(コリジョン以外の座標/姿勢の変更を行う)
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::PrevUpdate(void)
{
  if( m_stopPreUpdateFrame > 0 )
  {
    m_stopPreUpdateFrame--;
    return;
  }

  // アニメーションやコリジョンによる移動前の値を確保
  m_positionPrevUpdateStart = m_pCharaDrawInstance->GetPosition();
  m_groundAttributePrev     = m_groundAttribute;
  m_groundHeightPrev        = m_groundHeight;
  m_groundNormalPrev        = m_groundNormal;

  //--------------------------------------------------------------------------
  // 動作コード or アクションコマンド
  //--------------------------------------------------------------------------

  // 動作コードを再開
  {
    // イベント状態からあけた直後
    if( !m_eventRequestCnt 
      && m_eventRequestCntPrev )
    {
      m_pMoveCode->Resume( &m_moveCodeWork );
    }
    // @fix GFNMCat[887] 1フレーム内でV字回復をしているので強制的にResumeを通す
    else if( m_isNeedForceResume )
    {
      GFL_ASSERT(0);
    }
    // イベント状態でない場合でアクションコマンドが終了直後
    //note: イベント状態時にスクリプト等からアクションコマンドが呼ばれるケースがあるため
    else if( !m_eventRequestCnt
          && !m_isExecuteActionCommand
          &&  m_isExecuteActionCommandPrev )
    {
      m_pMoveCode->Resume( &m_moveCodeWork );
    }
  }

  // アクションコマンド・イベントが終わっていたら、動作コードを実行 
  if( !m_eventRequestCnt
   && !m_isExecuteActionCommand )
  {
    m_pMoveCode->MainProcess( &m_moveCodeWork );
  }

  // 動作コードを停止
  {
    // イベント状態に入った直後
    if( m_eventRequestCnt 
    && !m_eventRequestCntPrev )
    {
      m_pMoveCode->Suspend( &m_moveCodeWork );
    }
    // @fix GFNMCat[887] 1フレーム内でV字回復をしているので強制的にSuspendを通す
    else if( m_isNeedForceSuspend )
    {
      GFL_ASSERT(0);
    }
    // イベント状態でない場合でアクションコマンドが開始直後
    //note: イベント状態時にスクリプト等からアクションコマンドが呼ばれるケースがあるため
    else if( !m_eventRequestCnt
          &&  m_isExecuteActionCommand
          && !m_isExecuteActionCommandPrev )
    {
      m_pMoveCode->Suspend( &m_moveCodeWork );
    }
  }

  // note: UpdateActionCommand関数でm_isExecuteActionCommandの中身が書き換えられる時があるので此処でする
  //       此処でしなければ"イベント状態ではない場合でアクションコマンドが終了直後"にMoveCodeのResume処理が走らない
  m_isExecuteActionCommandPrev = m_isExecuteActionCommand;

  // アクションコマンドを実行
  if( m_isExecuteActionCommand )
  {
    Field::MoveModel::FieldActionCommandManager::UpdateActionCommand( &m_actionCommandWork );
  }

  // フレーム可変速
  if( m_pSignal )
  {
    m_pSignal->Update( this );
  }
  //--------------------------------------------------------------------------
  // アニメーション
  //--------------------------------------------------------------------------

  // 目パチ口パクの更新
  if( !m_isCulling )
  {
  #if 0
    m_pCharaDrawInstance->UpdateEye();
    m_pCharaDrawInstance->UpdateMouth();
  #else
    m_pFacialAnimeEye->PrevUpdate();
    m_pFacialAnimeMouth->PrevUpdate();
  #endif
  }

  // note: 動作コード・アクションコマンドにてモーションが変えられるケースがあるので、
  //       アニメーションの更新は動作コード・アクションコマンドの後に行う必要あり

  // @fix GFNMCat[845] 自機やライドポケモンの足音のSEが鳴らない
  m_prevAnimationFrame     = m_pCharaDrawInstance->GetAnimationFrame();

  // アニメーション更新
  b32 isUpdateAnimation( true );
  
  // 処理負荷対策：イベントデータのUnRegistによるアニメーション停止
  {
    if (IsAnimationStopByUnRegist())
    {
      isUpdateAnimation = false;
    }
  }
  
  b32 isLastFramePrev = m_pCharaDrawInstance->IsAnimationLastFrame();
  {
    if (isUpdateAnimation)
    {
      m_pCharaDrawInstance->UpdateAnimation();
    }
  }
  b32 isLastFrameAfter = m_pCharaDrawInstance->IsAnimationLastFrame();

  gfl2::math::Vector3 walkSpeed( m_pCharaDrawInstance->GetWalkSpeed( -1.0f ) );
  m_prevAnimationStepFrame = m_pCharaDrawInstance->GetAnimationStepFrame();

  // @fix GFNMCat[1058] 最終フレームの移動量が連続で反映されるのを防ぐ
  if( isLastFramePrev && isLastFrameAfter )
  {
    walkSpeed        = gfl2::math::Vector3(0.0f,0.0f,0.0f);
    m_prevWalkSpeedY = 0.0f;
  }

  // 処理負荷対策:アニメーション停止中の移動量が反映されるのを防ぐ
  if (!isUpdateAnimation)
  {
    walkSpeed = gfl2::math::Vector3(0.0f, 0.0f, 0.0f);
    m_prevWalkSpeedY = 0.0f;
  }

  // アニメーションが切り替わっているかどうか
  s32 animeId = m_pCharaDrawInstance->GetAnimationId();
  if( m_animationId != animeId )
  {
    m_prevWalkSpeedY = 0.0f;
  }
  m_animationId = animeId;

  // ジョイント"Origin"の移動要素を取得(ジョイント"Origin"はスキニング対象外です)
  gfl2::math::Vector4 originMoveVector( 0.0f,0.0f,0.0f,0.0f );
  if( !CheckStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE) )
  {
    // ジョイント"Origin"による移動ベクトルを取得
    originMoveVector = gfl2::math::Vector4( 0.0f,                             // X軸データは入っていない
                                            walkSpeed.y - m_prevWalkSpeedY,   // Y軸データは絶対値で入っているので1フレーム前の値を引いて、1フレーム前の姿勢からのオフセット値にする
                                            walkSpeed.z,                      // Z軸データは1フレーム前の姿勢からのオフセットで入っている
                                            0.0f );
  }
  m_prevWalkSpeedY = walkSpeed.y;

#if 0
  // ボタン入力状況を取得
  GameSys::GameManager*    game_manager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* device_manager = game_manager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = device_manager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD); 
  b32                      isButton       = pButton->IsHold( gfl2::ui::BUTTON_R );
  if( isButton )
  {
    originMoveVector.x = 0.0f;
    originMoveVector.y = 0.0f;
    originMoveVector.z = 0.0f;
  }
#endif

  //--------------------------------------------------------------------------
  // ジョイント"Origin"による動作モデルの移動
  //--------------------------------------------------------------------------

  // note: ジョイント"Origin"による動作モデルの移動はアニメーション更新後でないと1フレーム遅れてしまう

  // ジョイント"Origin"による移動ベクトルを適応する
  if( originMoveVector.Length() > 0.0f )
  {
    // 1フレームの移動ベクトルを自身のローカルにする
    gfl2::math::Matrix34 mat;
    if( m_isUseCustomValueForOrigin )
    {
      m_customQuaternionForOrigin.QuaternionToMatrix(mat);
      originMoveVector *= m_customScaleForOrigin;
    }
    else
    {
      m_pCharaDrawInstance->GetRotationQuat().QuaternionToMatrix(mat);
    }

    originMoveVector = mat.TransformNormal( originMoveVector );

    // デバッグによる移動量増加処理
    f32 debugMoveSpeed = 1.0f;
#if PM_DEBUG
    if( !m_eventRequestCnt
     && !m_isExecuteActionCommand
     && GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD )->IsHold( gfl2::ui::BUTTON_L ) )
    {
      if( GetMoveCodeId() == FIELD_MOVE_CODE_PLAYER
       || GetMoveCodeId() == FIELD_MOVE_CODE_RIDE_KENTAROS
       || GetMoveCodeId() == FIELD_MOVE_CODE_RIDE_SAMEHADER 
       || GetMoveCodeId() == FIELD_MOVE_CODE_RIDE_MUURAND 
       || GetMoveCodeId() == FIELD_MOVE_CODE_RIDE_ROBA2
       || GetMoveCodeId() == FIELD_MOVE_CODE_RIDE_KAIRIKY
       || GetMoveCodeId() == FIELD_MOVE_CODE_RIDE_LAPLACE )
      {
        FieldMoveModelPlayer* pPlayer = reinterpret_cast<FieldMoveModelPlayer*>(this);
        if( pPlayer && pPlayer->IsRunLoopState() )
        {
          debugMoveSpeed = 5.0f;
        }
      }
    }
#endif

    // フリームーブ中の移動速度が上限を超えているか
    if( !m_eventRequestCnt
     && m_pDynamicActor[COLLISION_TYPE_TERRAIN]
     && m_pDynamicActor[COLLISION_TYPE_TERRAIN]->GetShapeType() == BaseActor::SHAPE_SPHERE )
    {
      SphereShape* pSphere = static_cast<SphereShape*>(m_pDynamicActor[COLLISION_TYPE_TERRAIN]->GetShape());

      // デフォルトの最大スピードはコリジョンの半径
      f32 maxSpeed =  pSphere->m_r;

      // プレイヤーのみは別途最大スピードの制限がある
      if( m_appDataType == Field::MoveModel::TYPE_PLAYER )
      {
        // 未ライド時は最大スピードは20cm(1フレーム)
        if( GetMoveCodeId() == FIELD_MOVE_CODE_PLAYER )
        {
          maxSpeed = 20.0f;
        }
        // ライド時は最大スピードは40cm(1フレーム)
        else if( GetMoveCodeId() == FIELD_MOVE_CODE_RIDE_KENTAROS
              || GetMoveCodeId() == FIELD_MOVE_CODE_RIDE_SAMEHADER 
              || GetMoveCodeId() == FIELD_MOVE_CODE_RIDE_MUURAND 
              || GetMoveCodeId() == FIELD_MOVE_CODE_RIDE_ROBA2
              || GetMoveCodeId() == FIELD_MOVE_CODE_RIDE_KAIRIKY
              || GetMoveCodeId() == FIELD_MOVE_CODE_RIDE_LAPLACE )
        {
          maxSpeed = 40.0f;
        }
        else
        {
          GFL_ASSERT_MSG(0,"Disable MoveCodeId");
          maxSpeed = 20.0f;
        }
      }

      gfl2::math::Vector3 originMoveVectorXZ(originMoveVector.x,0.0f,originMoveVector.z);
      if( originMoveVectorXZ.Length() > (maxSpeed - 1.0f) )
      {
        f32 prevW = originMoveVector.w;
        originMoveVector   = originMoveVector.Normalize3();
        originMoveVector   = originMoveVector * (maxSpeed - 1.0f);
        originMoveVector.w = prevW;
      }
    }

    s32 dstAnimID = m_pCharaDrawInstance->GetAnimationId();
    f32 dstSpeed  = originMoveVector.Length();
    f32 dstAnimSp = m_pCharaDrawInstance->GetWalkSpeed( -1.0f ).z;
    if( dstAnimID == P2_BASE_FI120_SEARIDEJUMP_START )
    {
      MIYACHI_PRINT(" SEARIDEJUMP_START ,%f,%f,\n", dstSpeed, dstAnimSp );
    }
    if( dstAnimID == P2_BASE_FI121_SEARIDEJUMP_END )
    {
      MIYACHI_PRINT(" SEARIDEJUMP_END   ,%f,%f,\n", dstSpeed, dstAnimSp );
    }

    m_pCharaDrawInstance->SetPosition( m_pCharaDrawInstance->GetPosition() + gfl2::math::Vector3( originMoveVector.x*debugMoveSpeed,originMoveVector.y*debugMoveSpeed,originMoveVector.z*debugMoveSpeed ) );
  }

  //--------------------------------------------------------------------------
  // その他
  //--------------------------------------------------------------------------

  // サブオブジェクト生成処理
  UpdateMonsterBallRequest();
  UpdateRideGearRequest();

  // 1歩分進んだか調査
  // @fix GFNMCat[1545]   段差ジャンプ開始終了間に野生エンカウントが発生しているように感じる
  m_isOneStepFrame = false;
  if( !GetEventRequestCnt() && !IsExecuteActionCommand() )
  {
    if( !CheckStateBit(FIELD_MOVE_MODEL_DISABLE_STEP_CNT) )
    {
      m_oneStepCnt += originMoveVector.Length3();
    }

    if( (u32)m_oneStepCnt / m_oneStepMaxCnt > 0 )
    {
      m_isOneStepFrame = true;
      m_oneStepCnt     = (u32)m_oneStepCnt % m_oneStepMaxCnt;
    }
  }

  // ターンしたか調査
  // @fix GFNMCat[1545]   段差ジャンプ開始終了間に野生エンカウントが発生しているように感じる
  m_isTurnFrame = false;
  if( !GetEventRequestCnt() && !IsExecuteActionCommand() )
  {
    gfl2::math::Vector3 temp0(             originMoveVector.x,            originMoveVector.y,            originMoveVector.z ); temp0 = temp0.Normalize();
    gfl2::math::Vector3 temp1( m_lastEnableOriginMoveVector.x,m_lastEnableOriginMoveVector.y,m_lastEnableOriginMoveVector.z ); temp1 = temp1.Normalize();
    if( temp0.Dot( temp1 ) < 0.0f )
    {
      m_isTurnFrame = true;
    }
  }

  // ジョイント"Origin"による移動情報を保存と初期化
  if( originMoveVector.Length3() > 0.0f )
  {
    m_lastEnableOriginMoveVector = originMoveVector;
  }
  m_isUseCustomValueForOrigin = false;

  // コリジョン前の値を確保(アニメーションによる移動は終了済み)
  m_positionPrevUpdateEnd = m_pCharaDrawInstance->GetPosition();

  // 前回の動作状況を確保
  m_stateBitPrev               = m_stateBit;
  m_eventRequestCntPrev        = m_eventRequestCnt;

  // プレイヤー以外でアクションコマンド・イベントが終わっていたら壁との衝突をオフにする
  if( !m_eventRequestCnt
   && !m_isExecuteActionCommand
   &&  m_appDataType != Field::MoveModel::TYPE_PLAYER
   &&  GetDynamicActor( Field::MoveModel::COLLISION_TYPE_TERRAIN ) )
  {
    // イベントで必要なときのみ、スクリプトから任意でtrueに切り替える
    GetDynamicActor( Field::MoveModel::COLLISION_TYPE_TERRAIN )->SetActive( false );
  }

  m_isCullingPrev = m_isCulling;
}

//-----------------------------------------------------------------------------
/**
*  @brief  後更新(座標の決定後に必要な処理を行う)
*
*  @param  pCharaModelFactory  キャラリソースファクトリー
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::PostUpdate(poke_3d::model::CharaModelFactory* pCharaModelFactory)
{
  // 首振り速度をリセットするフラグを立てる
  if( !m_eventRequestCnt && !m_isExecuteActionCommand )
  {
    if( m_interestInitSpeedRequest )
    {
      u32 jointCnt = pCharaModelFactory->GetModelData_InterestJointCount(GetCharacterId());
      for( u32 i=0 ; i<jointCnt ; i++ )
      {
        f32 defaultSpeed = pCharaModelFactory->GetModelData_InterestSpeed(GetCharacterId(),i);
        m_pCharaDrawInstance->SetInterestSpeed( i,defaultSpeed );
      }
      m_interestInitSpeedRequest = false;
    }
  }
  // アクションコマンドまたはイベント中なら、フリームーブになった瞬間に首振り速度をリセットするフラグを立てる
  else
  {
    m_interestInitSpeedRequest = true;
  }

  // 注目座標の変更
  if( m_interestEnable && !m_interestResetRequest )
  {
    if (m_pInterestTarget && m_pInterestTarget->IsSetup())
    {
      m_interestPosition = m_pInterestTarget->GetCharaDrawInstance()->GetJointWorldPosition(m_interestTargetJointIndex);
    }
    m_pCharaDrawInstance->SetInterestPosition( &m_interestPosition );

#if PM_DEBUG
    if (FIELD_DEBUG_SWITCH(Field::Debug::DebugTypes::Switch::ENABLE_MOVEMODEL_INTEREST) == 0 )
    {
      m_pCharaDrawInstance->SetInterestPosition( NULL );
    }
#endif // PM_DEBUG
  }

  // 自動口パク更新
//  UpdateAutoKutipaku();

  // 注目更新
  if( m_interestEnable )
  {
    m_pCharaDrawInstance->UpdateInterest();
  }

  // 首制御更新
  m_pCharaDrawInstance->UpdateNeck();

  // 注目の終了リクエスト処理
  if( m_interestResetRequest )
  {
    m_interestResetFrame = gfl2::math::Max( (s32)0, --m_interestResetFrame );
    if( m_interestResetFrame <= 0 )
    {
      m_interestEnable       = false;
      m_interestResetRequest = false;
    }
  }

  // アクションコマンド・イベントが終わっていたら、動作コードを実行(ポスト)
  if( !m_eventRequestCnt
   && !m_isExecuteActionCommand )
  {
    m_pMoveCode->PostProcess( &m_moveCodeWork );
  }

  // エフェクト生成
  // 処理負荷対策:アニメーション停止中はエフェクトを出さない @note フレームが進まないため、毎フレーム生成される恐れがある
  if (!m_isCulling && !IsAnimationStopByUnRegist() )
  {
    UpdateEffect();
  }

  // 別アプリから復帰用の値を更新
  m_visibleForReturn       = m_pCharaDrawInstance->IsVisible();
  m_positionForReturn      = m_pCharaDrawInstance->GetPosition();
  m_quaternionForReturn    = m_pCharaDrawInstance->GetRotationQuat();
  m_srtForReturn.translate = m_positionForReturn;
  m_srtForReturn.rotation  = m_quaternionForReturn;
  m_srtForReturn.scale     = gfl2::math::Vector3(1.0f,1.0f,1.0f);
}

//-----------------------------------------------------------------------------
/**
*  @brief  サブオブジェクト更新
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::UpdateSubobject(void)
{
  for( s32 i=0 ; i<FIELD_MOVE_MODEL_SUB_OBJECT_CNT ; i++ )
  {
    if( m_pSubobject[i] )
    {
      m_pSubobject[i]->Attach(m_pCharaDrawInstance->IsVisible(),
                              m_animationId,
                              m_prevAnimationFrame,
                              GetCharacterId() );
    }
  }
}

void FieldMoveModel::PostUpdateSubobject(void)
{
  // @note momiji対応:ハウのリング対応のためFieldMoveModelManager::PostUpdateで行っていた内容を関数化
  for (s32 i = 0; i<FIELD_MOVE_MODEL_SUB_OBJECT_CNT; i++)
  {
    if (m_pSubobject[i] && m_pSubobject[i]->IsSetup())
    {
      m_pSubobject[i]->PostUpdate(m_pCharaDrawInstance->IsVisible(),
        GetCharacterId());
    }
  }
}


//-----------------------------------------------------------------------------
/**
*  @brief  エフェクト生成
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::UpdateEffect(void)
{
  // プレイヤーかNPCかどうか
  if( m_appDataType != TYPE_PLAYER && m_appDataType != TYPE_NPC )
  {
    return;
  }

  // エンカウント草エフェクトを出す
  if( m_pEncountGrassCreater )
  {
    m_pEncountGrassCreater->CreateTimingMove();
  }
  
  // NPC用泳ぎ波紋：海アトリビュート+NPCであること
  if( Field::Attribute::IsSea( m_groundAttribute ) && m_appDataType == TYPE_NPC )
  {
    // NPCかトレーナーを保証する
    FIELD_MOVE_CODE_ID eMoveCode = GetMoveCodeId();
    if( FieldMoveCodeManager::IsNpcMoveCode( eMoveCode ) || eMoveCode == FIELD_MOVE_CODE_NONE || eMoveCode == FIELD_MOVE_CODE_TRAINER_EYE || eMoveCode == FIELD_MOVE_CODE_TRAINER_PATH )
    {
      if( !m_pCharaRipple )
      {
        m_pCharaRipple = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_CHARA_RIPPLE, this );

        if( ((m_eventRequestCnt>0) || (m_isExecuteActionCommand ))
         && ( m_pCharaRipple )
         && (eMoveCode == FIELD_MOVE_CODE_TRAINER_EYE || eMoveCode == FIELD_MOVE_CODE_TRAINER_PATH) )
        {
          Field::Effect::EffectCharaRipple *pRipple = reinterpret_cast<Field::Effect::EffectCharaRipple*>( m_pCharaRipple );
          pRipple->Suspend();
        }
      }
    }
  }

  // かいパンのサーチモーションの専用処理
  if( GetCharacterId() ==  CHARA_MODEL_ID_TR0082_00 && m_animationId == TR0082_00_FI121_DASH )
  {
    // 足跡エフェクトを出すフレームかどうか
    s32 animationFrame                               = (s32)m_pCharaDrawInstance->GetAnimationFrame();
    const u32 EFFECT_CREATE_CNT                      = 13;
    s32 targetAnimationFrameLeft[EFFECT_CREATE_CNT]  = {41,51,71, 87, 96,142,174,204,234,264,294,324,345};
    s32 targetAnimationFrameRight[EFFECT_CREATE_CNT] = {61,81,89,118,160,191,219,249,279,309,336, -1,-1};

    for(u32 i=0; i<EFFECT_CREATE_CNT ; i++)
    {
      if( animationFrame != targetAnimationFrameRight[i] && animationFrame != targetAnimationFrameLeft[i] )
      {
        continue;
      }

      // 足跡エフェクトの対象足を調べる
      b32 isLeftFoot = ( animationFrame == targetAnimationFrameRight[i] ) ? false:true;
      s32 footIndex  = ( isLeftFoot ) ? m_pCharaDrawInstance->GetJointIndex("LFoot") : m_pCharaDrawInstance->GetJointIndex("RFoot");
      if( footIndex == -1)
      {
        continue;
      }

      // 特定地面属性の上にいるなら足跡エフェクトを作成する
      if( m_groundAttribute == ATTR_SUNAHAMA )
      {
        Field::Effect::EffectFootLand* pEffect = static_cast<Field::Effect::EffectFootLand*>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_FOOT_LAND , this ) );
        if( pEffect )
        {
          // note:モーションに回転値が入っておりOriginと向きの違いがあることから、つま先のワールドマトリクスから影の座標・姿勢を作らなければいけない

          // つま先の座標から影の座標を取得
          gfl2::math::Vector3 footPos( m_pCharaDrawInstance->GetJointWorldMatrix(footIndex).GetElemPosition()  );
          footPos.y = m_pCharaDrawInstance->GetWorldMatrix().GetElemPosition().y;

          // つま先の向きを角度で取得
          gfl2::math::Vector3 UnitX    (1.0f,0.0f,0.0f);
          gfl2::math::Vector3 UnitZ    (0.0f,0.0f,1.0f);
          gfl2::math::Vector3 FootFront( m_pCharaDrawInstance->GetJointWorldMatrix(footIndex).GetElemAxisX() );
          FootFront.y = 0.0f;
          FootFront   = FootFront.Normalize();
          f32 defRad  = gfl2::math::AcosRad( UnitZ.Dot( FootFront ) );
          if( UnitX.Dot( FootFront ) < 0.0f ){ defRad *= -1.0f; }

          // つま先の角度から姿勢を取得
          gfl2::math::Quaternion  footQua      (0.0f,0.0f,0.0f,1.0f);
          footQua.RadianYToQuaternion( defRad );

          pEffect->SetPosQuaternion( isLeftFoot ? false:true , footPos, footQua);
        }
      }
    }
  }

  // 足跡エフェクトの対象アニメーションかどうか
  if( m_animationId != P2_BASE_FI010_WALK01
   && m_animationId != P2_BASE_FI012_RUN01
   && m_animationId != P2_BASE_FI073_SLOPERUN01_UP
   && m_animationId != P2_BASE_FI071_SLOPEWALK01_UP )
  {
    return;
  }

  // 足跡エフェクトを出すフレームかどうか
  u32 animationFrame            = m_pCharaDrawInstance->GetAnimationFrame();
  u32 animationEndFrame         = m_pCharaDrawInstance->GetAnimationEndFrame();
  u32 targetAnimationFrameRight = animationEndFrame-1;
  u32 targetAnimationFrameLeft  = animationEndFrame/2;
  if( animationFrame != targetAnimationFrameRight
   && animationFrame != targetAnimationFrameLeft )
  {
    return;
  }

  // 足跡エフェクトの対象足を調べる
  b32 isLeftFoot = ( animationFrame == targetAnimationFrameRight ) ? false:true;
  s32 footIndex  = ( isLeftFoot ) ? m_pCharaDrawInstance->GetJointIndex("LFoot") : m_pCharaDrawInstance->GetJointIndex("RFoot");
  if( footIndex == -1)
  {
    return;
  }

  // モデルの原点から足への相対座標を取得
  gfl2::math::Vector3 offsetPos( m_pCharaDrawInstance->GetJointWorldMatrix(footIndex).GetElemPosition() - m_pCharaDrawInstance->GetWorldMatrix().GetElemPosition() );
  offsetPos.y = 0.0f;

  // 特定地面属性の上にいるなら足跡エフェクトを作成する
  if( m_groundAttribute == ATTR_SUNAHAMA
   || m_groundAttribute == ATTR_SABAKU_SUNA
   || m_groundAttribute == ATTR_YUKI )
  {
    Field::Effect::EffectFootLand* pEffect = static_cast<Field::Effect::EffectFootLand*>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_FOOT_LAND , this ) );
    if( pEffect )
    {
      pEffect->SetOffset( isLeftFoot ? false:true , offsetPos );
    }
  }

  // 地面属性にあったパーティクルエフェクトを作成する
  switch( m_groundAttribute )
  {
    case ATTR_ASASE:
    {
      Field::Effect::EffectFootWater* pEffect = static_cast<Field::Effect::EffectFootWater*>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_FOOT_WATER , m_pCharaDrawInstance->GetPosition() ) );
      if( pEffect )
      {
        // 水面の高さにあわせる
        const gfl2::math::Vector3 ADD_OFFSET(0.0f,20.0f,0.0f);
        pEffect->SetOffset( offsetPos + ADD_OFFSET ,gfl2::math::Vector3(0.0f,0.0f,0.0f) );
      }
      break;
    }
    case ATTR_YUKI:
    {
      break;
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  地面にキャラを引っ付ける
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::AdjustGround(void)
{
  // 自身の真下にあるマップブロックが有効になっていないなら中断
  if( !m_isCreateGround )
  {
    return;
  }

  // 描画オフになっているものは中断
  if( !m_pCharaDrawInstance->IsVisible() )
  {
    return;
  }

  // 前回から移動しておらず地面情報を既に持っているなら中断
  gfl2::math::Vector3 def( m_positionPrevAdjustGround - m_pCharaDrawInstance->GetPosition() );
  if( m_isOnGround && def.Length() <= FLT_EPSILON )
  {
    return;
  }

  // 前回から移動しておらず前回のレイキャストが失敗していたら中断
  if( m_isFailedRaycast && def.Length() <= FLT_EPSILON  )
  {
    return;
  }

  // 遥か上空から地下深くまでレイキャストをするように
  f32 startEndOffset = 1000000.0f;

  // 地面の高さを取得
  gfl2::math::Vector3             startVec3(m_pCharaDrawInstance->GetPosition() + gfl2::math::Vector3(0.0f,startEndOffset,0.0f));
  gfl2::math::Vector3             endVec3  (m_pCharaDrawInstance->GetPosition() - gfl2::math::Vector3(0.0f,startEndOffset,0.0f));
  gfl2::math::Vector4             startVec (startVec3.x,startVec3.y,startVec3.z,0.0f);
  gfl2::math::Vector4             endVec   (  endVec3.x,  endVec3.y,  endVec3.z,0.0f);

  // 前回ヒットしたポリゴンに今もいるようなら処理を中断
  // note:立体交差がない前提のコードです
  if( m_isOnGround && !m_isForceLanding )
  {
    gfl2::renderingengine::scenegraph::resource::CollisionTriangle triangle;
    gfl2::math::Vector4 newIntersection;
    triangle.m_Positions[0] = m_onTrianglePosition[0];
    triangle.m_Positions[1] = m_onTrianglePosition[1];
    triangle.m_Positions[2] = m_onTrianglePosition[2];
    triangle.m_Normal       = m_onTriangleNormal;
    if( triangle.IntersectsRay(&newIntersection,startVec,endVec) )
    {
      // 地面の高さを更新
      m_groundHeight = newIntersection.y;
      // 地面接地処理をしたときのキャラ座標を取得
      m_positionPrevAdjustGround = m_pCharaDrawInstance->GetPosition();
      return;
    }
  }

  // 地面情報の初期化
  m_isOnGround       = false;
  m_groundAttribute  = ATTRIBUTE_ERROR;
  m_groundHeight     = 0.0f;
  m_groundNormal     = gfl2::math::Vector3( 0.0f,1.0f,0.0f );
  m_isFailedRaycast  = false;
  m_onTriangleNormal = gfl2::math::Vector3(0.0f,1.0f,0.0f);
  for( s32 i=0; i<3 ; i++ )
  {
    m_onTrianglePosition[i] = gfl2::math::Vector3(100000.0f,100000.0f,100000.0f);
  }

  if( m_pCollisionSceneContainerForGround )
  {
    // レイが地面に当たったかどうか
    RaycastCustomCallback::HIT_DATA hitData;
    if( m_pCollisionSceneContainerForGround->RaycastFromStaticActorsMesh( startVec,endVec,&hitData ) )
    {
      if( m_isForceLanding )
      {
        gfl2::math::Vector3 newPos = m_pCharaDrawInstance->GetPosition();
        newPos.y = hitData.intersection.y + GROUND_OFFSET;
        m_pCharaDrawInstance->SetPosition(newPos);
      }

      // 地面情報の確保
      m_isOnGround      = true;
      m_groundHeight    = hitData.intersection.y;

      // 処理負荷軽減のため前回のポリゴン情報を覚えておく
      m_onTrianglePosition[0] = hitData.pTriangle->m_Positions[0];
      m_onTrianglePosition[1] = hitData.pTriangle->m_Positions[1];
      m_onTrianglePosition[2] = hitData.pTriangle->m_Positions[2];
      m_onTriangleNormal      = hitData.pTriangle->m_Normal;

      // 地面アトリビュートの確保
      // リソース側のエラー値は-1なのでフィールド側のエラー値に合わせる
      s32 hitAttribute = hitData.pTriangle->m_GroundAttribute;
      if( hitAttribute == -1 )
      {
        hitAttribute = ATTRIBUTE_ERROR;
      }
      else
      {
        m_groundAttributeLastSuccessful = hitAttribute;
      }
      m_groundAttribute = hitAttribute;

      // 法線
      gfl2::math::Vector4 v0 = hitData.pTriangle->m_Positions[1] - hitData.pTriangle->m_Positions[0];
      gfl2::math::Vector4 v1 = hitData.pTriangle->m_Positions[2] - hitData.pTriangle->m_Positions[0];
      gfl2::math::Vector4 v2 = v0.Cross(v1).Normalize3();
      m_groundNormal = gfl2::math::Vector3( v2.x,v2.y,v2.z );
      if( m_groundNormal.y < 0.0f )
      {
        m_groundNormal *= -1.0f;
      }
    }
    else
    {
      m_isFailedRaycast = true;
    }
  }

  // 地面接地処理をしたときのキャラ座標を取得
  m_positionPrevAdjustGround = m_pCharaDrawInstance->GetPosition();
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作モデルの座標/姿勢をダイナミックアクターの座標/姿勢に適用する
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ApplyMoveModelToActor(FieldMoveModelCollisionType colType)
{
  if( m_pDynamicActor[colType] )
  {
    // コリジョンシステムに伝える
    gfl2::math::Vector3    pos  ( m_pCharaDrawInstance->GetPosition()     );
    gfl2::math::Quaternion quat ( m_pCharaDrawInstance->GetRotationQuat() );

    m_pDynamicActor[colType]->SetPosition  ( pos  );
    m_pDynamicActor[colType]->SetQuaternion( quat );
    m_pDynamicActor[colType]->InitCollisionResult();
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作モデルの座標/姿勢をスタティックアクターの座標/姿勢に適用する
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ApplyMoveModelToActor(FieldMoveModelStaticCollisionType colType)
{
  if( m_pStaticActor[colType] )
  {
    // コリジョンシステムに伝える
    gfl2::math::Vector3    pos  ( m_pCharaDrawInstance->GetPosition()     );
    gfl2::math::Quaternion quat ( m_pCharaDrawInstance->GetRotationQuat() );

    m_pStaticActor[colType]->SetPosition  ( pos  );
    m_pStaticActor[colType]->InitCollisionResult();
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  ダイナミックアクターの座標/姿勢を動作モデルの座標/姿勢に適用する
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ApplyActorToMoveModel(FieldMoveModelCollisionType colType)
{
  // 当たった相手がいるかどうか
  if( !m_pDynamicActor[colType] || !m_pDynamicActor[colType]->GetCollisionResult(0)->pTargetActor )
  {
    return;
  }

  // コリジョンによる押し返しを受けない
  if( !CheckStateBit( FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE ) )
  {
    gfl2::math::Vector3    pos ( m_pDynamicActor[colType]->GetPosition()   );
    gfl2::math::Quaternion quat( m_pDynamicActor[colType]->GetQuaternion() );
    m_pCharaDrawInstance->SetPosition    ( gfl2::math::Vector3( pos.x , m_pCharaDrawInstance->GetPosition().y , pos.z ) );
    m_pCharaDrawInstance->SetRotationQuat( quat );
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  全てのコリジョンアクターが衝突を解決しているかどうか
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::CheckCollisionSolved(void)
{
  if( !CheckStateBit( FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE ) )
  {
    // 自身が地面・配置モデル・往来モデルに当たっていたなら基の位置に戻す
    for( s32 i=COLLISION_TYPE_TERRAIN ; i<=COLLISION_TYPE_OSHIKAESHI_RIDE ; i++ )
    {
      if( m_pDynamicActor[i] && m_pDynamicActor[i]->GetCollisionResult(0)->pTargetActor )
      {
        m_pCharaDrawInstance->SetPosition(m_positionPrevUpdateStart);
        m_groundAttribute = m_groundAttributePrev;
        m_groundHeight    = m_groundHeightPrev;
        m_groundNormal    = m_groundNormalPrev;
        if( m_groundAttributePrev != ATTRIBUTE_ERROR )
        {
          m_groundAttributeLastSuccessful = m_groundAttributePrev;
        }
        return;
      }
    }
    // 自身(恐らく往来モデル)が他の動作モデルに当たっていたなら基の位置に戻す
    for( s32 i=COLLISION_STATIC_TYPE_STATIC_MODEL ; i<=COLLISION_STATIC_TYPE_TRAFFIC_MODEL ; i++ )
    {
      if( m_pStaticActor[i] && m_pStaticActor[i]->GetCollisionResult(0)->pTargetActor )
      {
        m_pCharaDrawInstance->SetPosition(m_positionPrevUpdateStart);
        m_groundAttribute = m_groundAttributePrev;
        m_groundHeight    = m_groundHeightPrev;
        m_groundNormal    = m_groundNormalPrev;
        if( m_groundAttributePrev != ATTRIBUTE_ERROR )
        {
          m_groundAttributeLastSuccessful = m_groundAttributePrev;
        }
        return;
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  表世界にいるかどうか
*
*  @return 表世界にいればtrue、裏世界にいればfalseを返す
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModel::CheckValidGround(b32 isDisableAssert)
{
  // Planモードマップかチェック
  // プレイヤーが表世界にいるかどうかチェック
  World* pWorld = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldWorld();
  if( pWorld->GetWorldInfo().m_TerrainMode != World::TerrainMode::FIX )
  {
#if PM_DEBUG
    return true;
#else
    // 製品版でありえないマップにいる。
    GFL_ASSERT_STOP(0);
    return false;
#endif
  }

  // 壁用シーンが無ければ常に表世界にいるとみなす
  if( !m_pCollisionSceneContainerForWall )
  {
    return true;
  }


#if PM_DEBUG
  // デバッグスウィッチでのON/OFF
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::CHECK_VALID_GROUND) == false )
  {
    return true;
  }
#endif // PM_DEBUG

  const u32 MAX_CHECK_CNT = 10;
  for( u32 i=0; i<MAX_CHECK_CNT; i++ )
  {
    // レイキャストの方向を取得
    gfl2::math::Vector3    frontZ(0.0f,0.0f,100000.0f);
    gfl2::math::Quaternion qua(0.0f,0.0f,0.0f,1.0f);
    qua.RadianYToQuaternion( (PI2/MAX_CHECK_CNT) * i );
    frontZ = qua.Transform(frontZ);

    // レイキャスト
    RaycastCustomCallback::HIT_DATA hitData;
    gfl2::math::Vector3             startVec3(GetLocalSRT().translate);
    gfl2::math::Vector3             endVec3  (GetLocalSRT().translate + frontZ);
    gfl2::math::Vector4             startVec (startVec3.x,startVec3.y,startVec3.z,0.0f);
    gfl2::math::Vector4             endVec   (  endVec3.x,  endVec3.y,  endVec3.z,0.0f);
    s32 polyCnt = m_pCollisionSceneContainerForWall->RaycastFromStaticActorsMesh( startVec,endVec );

    // 頂点を貫通した場合は再トライ
    if( polyCnt == -1 )
    {
      GFL_PRINT( "retry CheckValidGround" );
      continue;
    }

    // 貫通した面が奇数なら裏世界にいる
    if( polyCnt%2 != 0 )
    {
      // @fix GFNMCat[1032] 外部からCheckValidGroundを使うタイミングが増えたのでアサートの出し方を変更
      GFL_ASSERT_MSG( isDisableAssert , "error CheckValidGround" );
      return false;
    }
    // 貫通した面が偶数なら表世界にいる
    else
    {
      return true;
    }
  }

  // 常に頂点上を貫通した場合は念の為裏世界にいるとする
  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief  動作ビット  ON
*
*  @param  fieldMoveModelStateBit  ビット
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::OnStateBit(FieldMoveModelStateBit fieldMoveModelStateBit)
{
  m_stateBit |= fieldMoveModelStateBit;
}

//----------------------------------------------------------------------------
/**
*  @brief  動作ビットOFF
*  
*  @param  fieldMoveModelStateBit  ビット
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::OffStateBit(FieldMoveModelStateBit fieldMoveModelStateBit)
{
  m_stateBit &= ~fieldMoveModelStateBit;
}

//----------------------------------------------------------------------------
/**
*  @brief  動作ビットのチェック
*
*  @param  fieldMoveModelStateBit  チェックするビット
*  @return trueはbitOn、falseはbitOff
*/
//-----------------------------------------------------------------------------
bool FieldMoveModel::CheckStateBit(FieldMoveModelStateBit fieldMoveModelStateBit) const
{
  if( (m_stateBit & (u32)fieldMoveModelStateBit) == (u32)fieldMoveModelStateBit )
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief  1フレーム前の動作ビットのチェック
*
*  @param  fieldMoveModelStateBit  チェックするビット
*  @return trueはbitOn、falseはbitOff
*/
//-----------------------------------------------------------------------------
bool FieldMoveModel::CheckStateBitPrev(FieldMoveModelStateBit fieldMoveModelStateBit) const
{
  if( (m_stateBitPrev & (u32)fieldMoveModelStateBit) == (u32)fieldMoveModelStateBit )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの設定
*
*  @param  actionCommand  アクションコマンド
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetActionCommand(FieldActionCommandId actionCommand)
{
  // アクションコマンドを開始する際、別のアクションコマンドが不正に打ち切られていないか調べる
  CheckActionCommand();
  FieldActionCommandManager::SetActionCommand( &m_actionCommandWork, actionCommand );
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(回転)の設定
*
*  @param  actionCommand  アクションコマンド
*  @param  dirY             回転Y
*  @param  dirX             回転X
*  @param  dirZ             回転Z
*  @param  timeToUse        回転にかかるフレーム
*  @param  isEnableIdleInit 待機モーションに切り替えるか
*  @param  isCompressMotion 旋回モーションの再生をtimeToUseで指定したフレーム内で収めるかどうか
*  @param  isEnableTurnMotionInit 旋回開始時にモーションを切り替えるか
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetActionCommandRotation(FieldActionCommandId actionCommand,f32 dirY,f32 dirX,f32 dirZ, u32 timeToUse, b32 isEnableIdleInit, b32 isCompressMotion, b32 isEnableTurnMotionInit, b32 isEnableIdleWait )
{
  // アクションコマンドを開始する際、別のアクションコマンドが不正に打ち切られていないか調べる
  CheckActionCommand();
  FieldActionCommandManager::SetActionCommandRotation( &m_actionCommandWork, actionCommand, dirY, dirX, dirZ, timeToUse, isEnableIdleInit, isCompressMotion, isEnableTurnMotionInit, isEnableIdleWait );
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(指定地点移動)の設定
*
*  @param  actionCommand      アクションコマンド
*  @param  isWalk             歩きかどうか
*  @param  pos                目標座標
*  @param  qua                目標姿勢
*  @param  isAdjustQua        目標姿勢を利用するかどうか
*  @param  isEnableActionStartMove 歩き始めを再生するか
*  @param  isEnableActionEndMove   歩き終わりを再生するか
*  @param  thresholdAngleForMotion 旋回モーションを再生する角度差
*  @param  isSlow              低速歩きかどうか
*  @param  isCustomGoalCheck   ゴール判定を最適化するか(ワープがなくなる、momiji追加)
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetActionCommandTargetMove(FieldActionCommandId actionCommand, b32 isWalk, gfl2::math::Vector3 pos, gfl2::math::Quaternion qua, b32 isAdjustQua, b32 isEnableActionStartMove, b32 isEnableActionEndMove, f32 thresholdAngleForMotion, b32 isSlow, b32 isCustomGoalCheck, b32 isMoveEndIdleWait, b32 isRunToWalk, f32 runToWalkDistance, s32 mositonChangeFrame )
{
  // アクションコマンドを開始する際、別のアクションコマンドが不正に打ち切られていないか調べる
  CheckActionCommand();
	FieldActionCommandManager::SetActionCommandTargetMove(&m_actionCommandWork, actionCommand, isWalk, pos, qua, isAdjustQua, isEnableActionStartMove, isEnableActionEndMove, thresholdAngleForMotion, isSlow, isCustomGoalCheck, isMoveEndIdleWait, isRunToWalk, runToWalkDistance, mositonChangeFrame );
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(任意モーションによる指定地点移動)の設定
*
*  @param  actionCommand  アクションコマンド
*  @param  motionId       モーションID
*  @param  pos            目標座標
*  @param  qua            目標姿勢
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetActionCommandTargetMoveWithOptionalMotion(FieldActionCommandId actionCommand, u32 motionId,gfl2::math::Vector3 pos, gfl2::math::Quaternion qua)
{
  // アクションコマンドを開始する際、別のアクションコマンドが不正に打ち切られていないか調べる
  CheckActionCommand();
  FieldActionCommandManager::SetActionCommandTargetMoveWithOptionalMotion( &m_actionCommandWork, actionCommand, motionId, pos, qua );
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(ステップ移動)の設定
*
*  @param  actionCommand   アクションコマンド
*  @param  isWalk          歩きかどうか
*  @param  stepCnt         ステップ数
*  @param  enableCollision コリジョンON or OFF
*  @param  moveRate        モーションによる移動ベクトルを適用する割合
*  @param  isEnableWalkEnd     歩き終わりをおこなうか 
*  @param  isEnableIdleInit    待機に設定して終了するか
*  @param  isSlow              低速歩きかどうか
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetActionCommandStepMove(FieldActionCommandId actionCommand, b32 isWalk, u32 stepCnt, b32 enableCollision, f32 moveRate, b32 isEnableWalkEnd, b32 isEnableIdleInit, b32 isSlow )
{
  // アクションコマンドを開始する際、別のアクションコマンドが不正に打ち切られていないか調べる
  CheckActionCommand();
  FieldActionCommandManager::SetActionCommandStepMove( &m_actionCommandWork, actionCommand, isWalk, stepCnt, enableCollision, moveRate, isEnableWalkEnd, isEnableIdleInit, isSlow );
}

//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンド(ステップ移動(開始・終了無し))の設定
*
*  @param  actionCommand   アクションコマンド
*  @param  stepCnt         ステップ数
*  @param  isOneFrame       1フレーム実行するかどうか
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetActionCommandStepMoveWithoutStartEnd(FieldActionCommandId actionCommand, u32 stepCnt, b32 isOneFrame)
{
  // アクションコマンドを開始する際、別のアクションコマンドが不正に打ち切られていないか調べる
  CheckActionCommand();
  FieldActionCommandManager::SetActionCommandStepMoveWithoutStartEnd( &m_actionCommandWork, actionCommand, stepCnt, isOneFrame );
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドワークにモーション連続再生アクションコマンドの設定
*
*  @param  actionCommand       アクションコマンド
*  @param  pAnimationIdArray   アニメーションID配列(ActionCommandContinuousPlayback::REGIST_MAX_CNT長の配列を送ってください)
*  @param  pFrameStartArray    フレーム開始    配列(ActionCommandContinuousPlayback::REGIST_MAX_CNT長の配列を送ってください)
*  @param  pLoopMaxArray       ループ回数      配列(ActionCommandContinuousPlayback::REGIST_MAX_CNT長の配列を送ってください)
*  @param  pBlendArray         ブレンドフレーム配列(ActionCommandContinuousPlayback::REGIST_MAX_CNT長の配列を送ってください)
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetActionCommandContinuousPlayBack(FieldActionCommandId actionCommand,
                                                        u32*                 pAnimationIdArray,
                                                        s16*                 pFrameStartArray,
                                                        s16*                 pLoopMaxArray,
                                                        s16*                 pBlendArray)
{
  // アクションコマンドを開始する際、別のアクションコマンドが不正に打ち切られていないか調べる
  CheckActionCommand();
  FieldActionCommandManager::SetActionCommandContinuousPlayBack( &m_actionCommandWork, actionCommand, pAnimationIdArray, pFrameStartArray, pLoopMaxArray , pBlendArray );
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの強制停止
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ClearActionCommand(b32 isDispAssert)
{
  // アクションコマンドを開始する際、別のアクションコマンドが不正に打ち切られていないか調べる
  if(isDispAssert)
  {
    CheckActionCommand();
  }
  FieldActionCommandManager::ClearActionCommandWork( &m_actionCommandWork );
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドを開始する際、別のアクションコマンドが不正に打ち切られていないか調べる
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::CheckActionCommand(void)
{
#if PM_DEBUG
  if( !IsExecuteActionCommand() )
  {
    return;
  }

  if( !Field::Debug::DebugTypes::m_isUseActionCommandCancelAssert )
  {
    return;
  }

  // 指定場所まで動くアクションコマンドが中断されたかどうか
  if( GetActionCommandId() == FIELD_ACTION_COMMAND_TARGET_MOVE
   || GetActionCommandId() == FIELD_ACTION_COMMAND_TARGET_MOVE_WITH_OPTIONAL_MOTION )
  {
    u16 zoneId = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetZoneID();
    GFL_ASSERT_MSG( 0,
      "eventId[%d] zoneId[%s] position[%f,%f,%f] バミリ移動が中断されました",
      GetEventId(),
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu()->GetDebugMapJump()->GetZoneName( zoneId ),
      GetLocalSRT().translate.x,
      GetLocalSRT().translate.y,
      GetLocalSRT().translate.z);
  }

  // 歩数移動アクションコマンドが中断されたかどうか
  if( GetActionCommandId() == FIELD_ACTION_COMMAND_STEP_MOVE
   || GetActionCommandId() == FIELD_ACTION_COMMAND_STEP_MOVE_WITHOUT_START_END )
  {
    u16 zoneId = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetZoneID();
    GFL_ASSERT_MSG( 0,
      "eventId[%d] zoneId[%s] position[%f,%f,%f] 歩数移動が中断されました",
      GetEventId(),
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu()->GetDebugMapJump()->GetZoneName( zoneId ),
      GetLocalSRT().translate.x,
      GetLocalSRT().translate.y,
      GetLocalSRT().translate.z);
  }
#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  自動口パクの更処理
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::UpdateAutoKutipaku(void)
{
  if( !m_isAutoKutipaku )
  {
    return;
  }

  const u32 ONE_STATE_FRAME = 2;
  m_AutoKutipackFrame++;

  switch (m_AutoKutipakuSequence)
  {
  // 閉じる
  case 0:

    m_pCharaDrawInstance->SetMouthIndex( poke_3d::model::CharaModel::MOUTH_INDEX_CLOSE );

    // 次へ
    if ( m_pCharaDrawInstance->isSetupRandomFunc() && m_pCharaDrawInstance->GetRandomValue(6) == 0 )
    {
      ++m_AutoKutipakuSequence;
      m_AutoKutipackFrame = 0;
    }
    break;

  // 半開け
  case 1:

    m_pCharaDrawInstance->SetMouthIndex( poke_3d::model::CharaModel::MOUTH_INDEX_HALF_OPEN );

    // 次へ
    if( m_AutoKutipackFrame > ONE_STATE_FRAME )
    {
      ++m_AutoKutipakuSequence;
      m_AutoKutipackFrame = 0;
    }
    break;

  // 開ける
  case 2:

    m_pCharaDrawInstance->SetMouthIndex( poke_3d::model::CharaModel::MOUTH_INDEX_OPEN );

    // 次へ
    if( m_AutoKutipackFrame > ONE_STATE_FRAME )
    {
      ++m_AutoKutipakuSequence;
      m_AutoKutipackFrame = 0;
    }
    break;

  // 半開け
  case 3:

    m_pCharaDrawInstance->SetMouthIndex( poke_3d::model::CharaModel::MOUTH_INDEX_HALF_OPEN );

    // 次へ
    if( m_AutoKutipackFrame > ONE_STATE_FRAME )
    {
      ++m_AutoKutipakuSequence;
      m_AutoKutipackFrame = 0;
    }
    break;

  // 閉じる
  case 4:

    m_pCharaDrawInstance->SetMouthIndex( poke_3d::model::CharaModel::MOUTH_INDEX_CLOSE );

    // 最初へ
    if( m_AutoKutipackFrame > ONE_STATE_FRAME )
    {
      m_AutoKutipakuSequence = 0;
      m_AutoKutipackFrame = 0;
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  ワークからリソースの状態を元に戻す
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ReturnResourceStateFromWork(void)
{
  if( (IsSetupWork() && m_isNeedReturnFromWork )
   || (IsSetupWork() && m_isForceReturnFromWork) )
  {
    // 描画・座標・姿勢を復元
    if( m_enableVisibleForReturn )
    {
      m_pCharaDrawInstance->SetVisible( m_visibleForReturn );
    }
    m_pCharaDrawInstance->SetPosition    ( m_positionForReturn );
    m_pCharaDrawInstance->SetRotationQuat( m_quaternionForReturn );

    // 動作コードの復帰処理
    m_pMoveCode->Comeback( &m_moveCodeWork,m_moveCodeWork.isSetupWorkFromSaveData );

    // ムーブコードワークはセーブデータによる復元をしていない状態にする
    m_moveCodeWork.isSetupWorkFromSaveData = false;

    // その他
    m_oneStepCnt     = 0.0f;
    m_isOneStepFrame = false;
    m_isTurnFrame    = false;

    // セーブ復帰時に動作コードのResume処理を走らせるために行う
    if( m_isForceReturnFromWork  )
    {
      m_eventRequestCntPrev = 1;  // NMCat[1824] JoinFestaClose時にセーブデータから復帰した際に、CntPrevも1にして、Suspendがコールされない状態にした。
                                  // 通常セーブ復帰時も同様に、CntPrev=1にして、SuspendすることなくResumeがよばるようにする。
    }
  }

  m_isNeedReturnFromWork  = false;
  m_isForceReturnFromWork = false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作コードを変更
*
*  @param  変更したい動作コード
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ChangeMoveCode(FIELD_MOVE_CODE_ID moveCodeId)
{
  m_headerWork.moveCodeId = moveCodeId;
  Field::MoveModel::FieldMoveCodeManager::InitializeMoveCodeWork( &m_moveCodeWork, this, m_moveCodeExtendWorkSize, m_pMoveCodeExtendWork );
  m_pMoveCode = Field::MoveModel::FieldMoveCodeManager::GetMoveCode( moveCodeId );
  m_pMoveCode->InitProcess( &m_moveCodeWork );
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作コード再開停止
*  @note   FieldMoveModelManager::AddEventRequestAll関数以外から呼び出さないでください
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::MoveCodeResumeSuspend(void)
{
  // 動作コードを再開
  {
    // イベント状態からあけた直後
    // note:AddEventRequest時にしか呼ばないルールなのでMoveCode::MainProcessは絶対に走らないはず
    if( !m_eventRequestCnt 
      && m_eventRequestCntPrev )
    {
      GFL_ASSERT(0);
    }
    // @fix GFNMCat[887] 1フレーム内でV字回復をしているので強制的にResumeを通す
    else if( m_isNeedForceResume )
    {
      m_pMoveCode->Resume( &m_moveCodeWork );
    }
    // イベント状態でない場合でアクションコマンドが終了直後
    // note:AddEventRequest時にしか呼ばないルールなのでMoveCode::MainProcessは絶対に走らないはず
    else if( !m_eventRequestCnt
      &&     !m_isExecuteActionCommand
      &&      m_isExecuteActionCommandPrev )
    {
      GFL_ASSERT(0);
    }
  }

  // アクションコマンド・イベントが終わっていたら、動作コードを実行 
  // note:AddEventRequest時にしか呼ばないルールなのでMoveCode::MainProcessは絶対に走らないはず
  if( !m_eventRequestCnt
   && !m_isExecuteActionCommand )
  {
    GFL_ASSERT(0);
  }

  // 動作コードを停止
  {
    // イベント状態に入った直後
    if( m_eventRequestCnt 
    && !m_eventRequestCntPrev )
    {
      m_pMoveCode->Suspend( &m_moveCodeWork );
    }
    // @fix GFNMCat[887] 1フレーム内でV字回復をしているので強制的にSuspendを通す
    else if( m_isNeedForceSuspend )
    {
      m_pMoveCode->Suspend( &m_moveCodeWork );
    }
    // イベント状態でない場合でアクションコマンドが開始直後
    // note:AddEventRequest時にしか呼ばないルールなのでMoveCode::MainProcessは絶対に走らないはず
    else if( !m_eventRequestCnt
          &&  m_isExecuteActionCommand
          && !m_isExecuteActionCommandPrev )
    {
      GFL_ASSERT(0);
    }
  }

  m_isNeedForceResume   = false;
  m_isNeedForceSuspend  = false;
  m_eventRequestCntPrev = m_eventRequestCnt;
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIマネージャーに自身のプロパティを登録
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::RegistRtti(void)
{
#if USE_RTTI_DEBUG

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_isSetupWork            , L"初期化済み(ワーク)"            , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_isSetupResource        , L"初期化済み(リソース)"          , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_isOnGround             , L"地面に立っているか"            , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_groundAttribute        , L"地面属性"                      , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_groundHeight           , L"地面の高さ"                    , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
    RTTI_PARAM_REGIST     ( FieldMoveModel , m_isForceLanding         , L"地面に吸着させるかどうか"      );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_oneStepCnt             , L"現在のステップカウント"        , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
    RTTI_PARAM_REGIST     ( FieldMoveModel , m_oneStepMaxCnt          , L"一歩とするステップカウント"    );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_isTurnFrame            , L"振り返ったタイミングかどうか"  , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_isOneStepFrame         , L"一歩進んだタイミングかどうか"  , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
    RTTI_PARAM_REGIST     ( FieldMoveModel , m_defaultIdleAnimationId , L"デフォルト待機アニメーション"  );
    RTTI_PARAM_REGIST     ( FieldMoveModel , m_isAutoKutipaku         , L"自動口パクをするかどうか"      );
  RTTI_END_REGIST( FieldMoveModel );

#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIエディタに自身のインスタンスを登録
*
*  @param  エディタに表示したい名前
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::CreateRttiEditorPage(c16* pDispName)
{
#if USE_RTTI_DEBUG

  // RTTIエディタに自身のプロパティを登録
  RegistRtti();
  for( s32 i=0 ; i<COLLISION_TYPE_MAX ; i++ )
  {
    if( m_pDynamicActor[i] )
    {
      m_pDynamicActor[i]->GetShape()->RegistRtti();
    }
  }

  // RTTIエディタに自身のインスタンス内容を表示する
  {
    c16 dispName[gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE];
    gfl2::rttidebug::wideCharaCopy( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , pDispName );
    gfl2::rttidebug::RttiEditor::getInstance()->CreateEditorFromInstace( dispName , &typeid( FieldMoveModel ) , this );
  }
  if( m_pDynamicActor[COLLISION_TYPE_TERRAIN] )
  {
    c16 dispName[gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE];
    gfl2::rttidebug::wideCharaCopy( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , pDispName );
    gfl2::rttidebug::wideCharaCat ( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , L"[衝突形状:地面用]" );
    gfl2::rttidebug::RttiEditor::getInstance()->CreateEditorFromInstace( dispName , &typeid( *m_pDynamicActor[COLLISION_TYPE_TERRAIN]->GetShape() )      , m_pDynamicActor[COLLISION_TYPE_TERRAIN]->GetShape() );
  }
  if( m_pDynamicActor[COLLISION_TYPE_STATIC_MODEL] )
  {
    c16 dispName[gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE];
    gfl2::rttidebug::wideCharaCopy( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , pDispName );
    gfl2::rttidebug::wideCharaCat ( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , L"[衝突形状:配置モデル用]" );
    gfl2::rttidebug::RttiEditor::getInstance()->CreateEditorFromInstace( dispName , &typeid( *m_pDynamicActor[COLLISION_TYPE_STATIC_MODEL]->GetShape() ) , m_pDynamicActor[COLLISION_TYPE_STATIC_MODEL]->GetShape() );
  }
  if( m_pDynamicActor[COLLISION_TYPE_EVENT_POS] )
  {
    c16 dispName[gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE];
    gfl2::rttidebug::wideCharaCopy( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , pDispName );
    gfl2::rttidebug::wideCharaCat ( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , L"[衝突形状:POSイベント用]" );
    gfl2::rttidebug::RttiEditor::getInstance()->CreateEditorFromInstace( dispName , &typeid( *m_pDynamicActor[COLLISION_TYPE_EVENT_POS]->GetShape() )        , m_pDynamicActor[COLLISION_TYPE_EVENT_POS]->GetShape() );
  }
#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIエディタから自身のインスタンスを削除
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::DeleteRttiEditorPage(void)
{
#if USE_RTTI_DEBUG

  gfl2::rttidebug::RttiEditor::getInstance()->DeleteEditorFromInstace( this );
  for( s32 i=0 ; i<COLLISION_TYPE_MAX ; i++ )
  {
    if( m_pDynamicActor[i] && m_pDynamicActor[i]->GetShape() )
    {
      gfl2::rttidebug::RttiEditor::getInstance()->DeleteEditorFromInstace( m_pDynamicActor[i]->GetShape() );
    }
  }

#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  モーションシグナルの設定
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetMotionSignal( Field::MotionSignal *pSignal )
{
  m_pSignal = pSignal;
}

//-----------------------------------------------------------------------------
/**
*  @brief  モーションシグナルの設定
*/
//-----------------------------------------------------------------------------
Field::MotionSignal *FieldMoveModel::GetMotionSignal( void )
{
  return m_pSignal;
}

//-----------------------------------------------------------------------------
/**
*  @brief  サブオブジェクトを設定
*
*  @param  index 何番目の持ち物にするか
*  @param  index 持たせたいサブオブジェクト
*  @note   外したい場合はNULLを入れる
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetSubobject(u32 index,FieldSubobject* pSubobject,c8* jointName)
{
  // 前にあったものを外す
  if( m_pSubobject[index] )
  {
    m_pSubobject[index]->GetCharaDrawInstance()->GetModelInstanceNode()->RemoveFromParent();
    m_pSubobject[index]->EnableDeleteRequest();
    m_pSubobject[index]                       = NULL;
    m_pSubobjectJoint[index]                  = 0;
    m_isSubobjectPrevClearResource[index]     = false;
    m_subobjectHeaderPrevClearResource[index].Init();
  }

  if( !pSubobject )
  {
    return;
  }

  if( m_pCharaDrawInstance->GetJointIndex(jointName) == -1 )
  {
    return;
  }
  
  // 新しいサブオブジェクトを設定する
  m_pSubobject[index]                       = pSubobject;
  m_pSubobjectJoint[index]                  = m_pCharaDrawInstance->GetJointIndex(jointName);
  m_isSubobjectPrevClearResource[index]     = true;
  m_subobjectHeaderPrevClearResource[index] = pSubobject->GetHeader();
  m_pCharaDrawInstance->GetModelInstanceNode()->GetJointInstanceNode( m_pSubobjectJoint[index] )->AddChild( pSubobject->GetCharaDrawInstance()->GetModelInstanceNode() );

#if defined( GF_PLATFORM_CTR )
  // サブオブジェクトのエッジIDを設定
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode = pSubobject->GetCharaDrawInstance()->GetModelInstanceNode();
  if( pModelInstanceNode )
  {
    pModelInstanceNode->CopyMaterialDepthStencilStateToStateObject();
    for( u32 i=0 ; i<pModelInstanceNode->GetMaterialNum() ; i++ )
    {
      u32 id = (u32)pModelInstanceNode->GetMaterialInstanceNode(i)->GetDepthStencilStateObject()->GetStencilReference();
      id += m_edgeId;                    // サブオブジェクトには親と同じオフセットを流し込む
      if     ( id >= 255 ){ id = 255; }  // エッジをださない場合の特例
      else if( id >= 150 ){ id = 150; }  // キャラに割り当てられるのは150まで、それ以降は背景用
      pModelInstanceNode->GetMaterialInstanceNode(i)->GetDepthStencilStateObject()->SetStencilReference(static_cast<u8>(id));
      pModelInstanceNode->GetMaterialInstanceNode(i)->GetDepthStencilStateObject()->UpdateState();
    }
  }
#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  サブオブジェクトのキャラクターIDを取得
*
*  @return  サブオブジェクトキャラクターID
*/
//-----------------------------------------------------------------------------
u32 FieldMoveModel::GetSubobjectCharacterId(u32 index)
{
  if( m_pSubobject[index] )
  {
    return m_pSubobject[index]->GetHeader().characterId;
  }
  return 0;
}

//-----------------------------------------------------------------------------
/**
*  @brief  カリングを有効
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::EnableCulling(void)
{
  // ツリーから外す
  // note:親がルートノードなのでどのタイミングで呼んでも問題ない
  RemoveEdgeRenderingTarget();
  RemoveParentNode();

  // スキニングを停止
  if( m_pCharaDrawInstance )
  {
    m_pCharaDrawInstance->SetAnimationCalculateEnable(false);
  }
  m_isCulling = true;
}

//-----------------------------------------------------------------------------
/**
*  @brief  カリングを無効
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::DisableCulling(void)
{
  // ツリーに再設置
  // note:親がルートノードなのでどのタイミングで呼んでも問題ない
  AddEdgeRenderingTarget();
  ReSetParentNode();

  if( m_pCharaDrawInstance )
  {
    // スキニングを再開
    b32 isPrevCalculateEnable = m_pCharaDrawInstance->IsAnimationCalculateEnable();
    m_pCharaDrawInstance->SetAnimationCalculateEnable(true);

    // 前回スキニングをしていなければジョイントを更新
    if( !isPrevCalculateEnable )
    {
      f32 prevStepFrame = m_pCharaDrawInstance->GetAnimationStepFrame();
      m_pCharaDrawInstance->SetAnimationStepFrame(0.0f);
      m_pCharaDrawInstance->UpdateAnimation();
      m_pCharaDrawInstance->SetAnimationStepFrame(prevStepFrame);
    }
  }
  m_isCulling = false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  描画パイプラインに自身のキャラモデルを追加
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::AddEdgeRenderingTarget(void)
{
  // 既に登録されている
  if( m_isRegistToPipeline )
  {
    return;
  }

  // 着せ替えなし
  if( !m_headerResource.pDressUpParam )
  {
    m_pMyRenderingPipeLine->AddEdgeRenderingTarget( m_pCharaDrawInstance->GetModelInstanceNode() );
  }
  // 着せ替えあり
  else
  {
    poke_3d::model::DressUpModel* pDressUpModel = static_cast<poke_3d::model::DressUpModel*>(m_pCharaDrawInstance);
    for( u32 i=0; i<poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; ++i )
    {
      m_pMyRenderingPipeLine->AddEdgeRenderingTarget( pDressUpModel->GetPartsModelInstanceNode( static_cast<poke_3d::model::DressUpModel::PartsModel>(i) ) );
    }
  }
  m_isRegistToPipeline = true;
}

//-----------------------------------------------------------------------------
/**
*  @brief  描画パイプラインから自身のキャラモデルを削除
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::RemoveEdgeRenderingTarget(void)
{
  if( !m_isRegistToPipeline )
  {
    return;
  }

  // 着せ替えあり
  if( m_headerResource.pDressUpParam )
  {
    poke_3d::model::DressUpModel* pDressUpModel = static_cast<poke_3d::model::DressUpModel*>(m_pCharaDrawInstance);
    for( u32 i=0; i<poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; ++i )
    {
      m_pMyRenderingPipeLine->RemoveEdgeRenderingTarget( pDressUpModel->GetPartsModelInstanceNode( static_cast<poke_3d::model::DressUpModel::PartsModel>(i) ) );
    }
  }
  // 着せ替えなし
  else
  {
    m_pMyRenderingPipeLine->RemoveEdgeRenderingTarget( m_pCharaDrawInstance->GetModelInstanceNode() );
  }
  m_isRegistToPipeline = false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  モデルインスタンスノードを親から登録を外す
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::RemoveParentNode(void)
{
  if( !m_pParentNode )
  {
    m_pParentNode = m_pCharaDrawInstance->GetModelInstanceNode()->GetParent();
    m_pCharaDrawInstance->GetModelInstanceNode()->RemoveFromParent();
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  モデルインスタンスノードを親に再設定する
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ReSetParentNode(void)
{
  if( m_pParentNode )
  {
    m_pParentNode->AddChild( m_pCharaDrawInstance->GetModelInstanceNode() );
    m_pParentNode = NULL;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  サブオブジェクトとしてモンスターボールを必要とするタイミングかどうか更新
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::UpdateMonsterBallRequest(void)
{
  m_isNeedMonsterBall = false;

  if( m_isCulling )
  {
    return;
  }

  // ボールポンポンモーションを再生中か
  if( GetCharaDrawInstance()->GetAnimationId() == P2_BASE_FI129_POSE01 )
  {
    m_isNeedMonsterBall = true;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  サブオブジェクトとしてライドギアを必要とするタイミングかどうか更新
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::UpdateRideGearRequest(void)
{
  m_isNeedRideGear = false;

  // ライドギアモーションを再生中か
  if( GetCharaDrawInstance()->GetAnimationId() == P1_BASE_EV0500_RIDECALL )
  {
    m_isNeedRideGear = true;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  現在の座標からジャンプ座標までの間に障害物がない、且つジャンプ座標周辺に障害物が無いかを調べる
*
*  @param  jumpPos        ジャンプ座標
*  @param  acceptWallCnt  レイチェック時に許容する壁の枚数
*  @param  isCheckNaminori チェックする壁に押し返しがある波乗り    フィルターも含める
*  @param  isCheckRoba2    チェックする壁に押し返しがあるロバ2     フィルターも含める
*  @param  isCheckRide     チェックする壁に押し返しがあるライド禁止フィルターも含める
*  @return trueなら障害物がなし、falseなら障害物があり
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModel::CheckEnableJump(const gfl2::math::Vector3& jumpPos,u32 acceptWallCnt,b32 isCheckNaminori,b32 isCheckRoba2,b32 isCheckRide)
{
  // ジャンプ座標にコリジョンを配置し、1箇所以上に触れていたら障害物がありとみなす
  gfl2::math::Vector3 adjustPos(0.0f,0.0f,0.0f);
  if( GetHitCntFromOshikaeshiScene(jumpPos,&adjustPos) >= 1 )
  {
    return false;
  }

  // 開始座標から終了座標までの間に指定枚数の壁があるか調べる、指定枚数以外なら障害物がありとみなす
  return GetHitCntFromOshikaeshiScene(GetLocalSRT().translate,jumpPos,acceptWallCnt,isCheckNaminori,isCheckRoba2,isCheckRide);
}

//-----------------------------------------------------------------------------
/**
*  @brief  指定座標にコリジョンを配置した際、押し返しがあるシーンで何箇所に触れているか調べる
*
*  @param  checkPos   チェック座標
*  @param  pAdjustPos チェック後の座標
*  @return 触れている箇所の数
*/
//-----------------------------------------------------------------------------
u32 FieldMoveModel::GetHitCntFromOshikaeshiScene(const gfl2::math::Vector3& checkPos,gfl2::math::Vector3* pAdjustPos)
{
  u32 hitCnt = 0;
  gfl2::math::Vector3 adjustPos( checkPos );
  for( u32 i=0 ; i<2 ; i++ )
  {
    // リセット
    GetDynamicActor( COLLISION_TYPE_TERRAIN             )->InitCollisionResult();
    GetDynamicActor( COLLISION_TYPE_STATIC_MODEL        )->InitCollisionResult();
    GetDynamicActor( COLLISION_TYPE_TRAFFIC_MODEL       )->InitCollisionResult();
    GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_NAMINORI )->InitCollisionResult();
    GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_ROBA2    )->InitCollisionResult();
    GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_RIDE     )->InitCollisionResult();

    //@caution UpdateCollisionのシーンアップデート順と同じになっているか最後の最後まで確認する

    // 往来モデル用コリジョンの更新
    if( GetDynamicActor( COLLISION_TYPE_TRAFFIC_MODEL ) && m_headerResource.pCollisionSceneForTrafficModel )
    {
      GetDynamicActor( COLLISION_TYPE_TRAFFIC_MODEL )->SetPosition(adjustPos);
      m_headerResource.pCollisionSceneForTrafficModel->Update();
      adjustPos = GetDynamicActor( COLLISION_TYPE_TRAFFIC_MODEL )->GetPosition();
      hitCnt   += GetDynamicActor( COLLISION_TYPE_TRAFFIC_MODEL )->GetCollisionResult(0)->pTargetActor ? 1 : 0;
    }

    // 配置モデル用コリジョンの更新
    if( GetDynamicActor( COLLISION_TYPE_STATIC_MODEL ) && m_headerResource.pCollisionSceneForStaticModel )
    {
      GetDynamicActor( COLLISION_TYPE_STATIC_MODEL )->SetPosition(adjustPos);
      m_headerResource.pCollisionSceneForStaticModel->Update();
      adjustPos = GetDynamicActor( COLLISION_TYPE_STATIC_MODEL )->GetPosition();
      hitCnt   += GetDynamicActor( COLLISION_TYPE_STATIC_MODEL )->GetCollisionResult(0)->pTargetActor ? 1 : 0;
    }

    // 波乗り用コリジョンの更新
    if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_NAMINORI ) && m_headerResource.pCollisionSceneForOshikaeshiNaminori )
    {
      GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_NAMINORI )->SetPosition(adjustPos);
      m_headerResource.pCollisionSceneForOshikaeshiNaminori->Update();
      adjustPos = GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_NAMINORI )->GetPosition();
      hitCnt   += GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_NAMINORI )->GetCollisionResult(0)->pTargetActor ? 1 : 0;
    }

    // 地面馬用コリジョンの更新
    if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_ROBA2 ) && m_headerResource.pCollisionSceneForOshikaeshiRoba2 )
    {
      GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_ROBA2 )->SetPosition(adjustPos);
      m_headerResource.pCollisionSceneForOshikaeshiRoba2->Update();
      adjustPos = GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_ROBA2 )->GetPosition();
      hitCnt   += GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_ROBA2 )->GetCollisionResult(0)->pTargetActor ? 1 : 0;
    }

    // ライド進入禁止用コリジョンの更新
    if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_RIDE ) && m_headerResource.pCollisionSceneForOshikaeshiRide )
    {
      GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_RIDE )->SetPosition(adjustPos);
      m_headerResource.pCollisionSceneForOshikaeshiRide->Update();
      adjustPos = GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_RIDE )->GetPosition();
      hitCnt   += GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_RIDE )->GetCollisionResult(0)->pTargetActor ? 1 : 0;
    }

    // 壁用コリジョンの更新
    if( GetDynamicActor( COLLISION_TYPE_TERRAIN ) && m_headerResource.pCollisionSceneForTerrain )
    {
      GetDynamicActor( COLLISION_TYPE_TERRAIN )->SetPosition(adjustPos);
      m_headerResource.pCollisionSceneForTerrain->Update();
      adjustPos = GetDynamicActor( COLLISION_TYPE_TERRAIN )->GetPosition();
      hitCnt   += GetDynamicActor( COLLISION_TYPE_TERRAIN )->GetCollisionResult(0)->pTargetActor ? 1 : 0;
    }
  }

  *pAdjustPos = adjustPos;
  return hitCnt;
}

//-----------------------------------------------------------------------------
/**
*  @brief  開始座標から終了座標までの間に指定枚数の壁があるか調べる
*
*  @param  startPos        開始座標
*  @param  endPos          終了座標
*  @param  acceptWallCnt   チェックする壁の枚数
*  @param  isCheckNaminori チェックする壁に押し返しがある波乗り    フィルターも含める
*  @param  isCheckRoba2    チェックする壁に押し返しがあるロバ2     フィルターも含める
*  @param  isCheckRide     チェックする壁に押し返しがあるライド禁止フィルターも含める
*  @return trueなら指定枚数分の壁がある、falseならない
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModel::GetHitCntFromOshikaeshiScene(const gfl2::math::Vector3& startPos,const gfl2::math::Vector3& endPos,u32 acceptWallCnt,b32 isCheckNaminori,b32 isCheckRoba2,b32 isCheckRide)
{
  // コリジョン幅を取得
  f32 colR( -1.0f );

  // 背景から優先度の高い順場で幅を取得する
  if     ( GetDynamicActor( COLLISION_TYPE_TERRAIN             )->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ colR = static_cast<SphereShape*  >(GetDynamicActor(COLLISION_TYPE_TERRAIN             )->GetShape())->m_r; }
  else if( GetDynamicActor( COLLISION_TYPE_TRAFFIC_MODEL       )->GetShapeType() == BaseActor::SHAPE_CYLINDER ){ colR = static_cast<CylinderShape*>(GetDynamicActor(COLLISION_TYPE_TRAFFIC_MODEL       )->GetShape())->m_r; }
  else if( GetDynamicActor( COLLISION_TYPE_STATIC_MODEL        )->GetShapeType() == BaseActor::SHAPE_CYLINDER ){ colR = static_cast<CylinderShape*>(GetDynamicActor(COLLISION_TYPE_STATIC_MODEL        )->GetShape())->m_r; }
  else if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_NAMINORI )->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ colR = static_cast<SphereShape*  >(GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_NAMINORI )->GetShape())->m_r; }
  else if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_ROBA2    )->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ colR = static_cast<SphereShape*  >(GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_ROBA2    )->GetShape())->m_r; }
  else if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_RIDE     )->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ colR = static_cast<SphereShape*  >(GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_RIDE     )->GetShape())->m_r; }
  else { return false; }

  // 現在の座標からジャンプ座標への方向を取得
  gfl2::math::Vector3 nowPos( startPos );
  gfl2::math::Vector3 wayVec( endPos - nowPos );
  gfl2::math::Vector3 wayX  ( wayVec.Cross( gfl2::math::Vector3(0.0f,1.0f,0.0f) ) );

  // Y方向に移動している。
  if( wayX.LengthSq() <= EPSILON )
  {
    GFL_ASSERT( wayX.LengthSq() > EPSILON );
    return false;
  }
  wayX = wayX.Normalize();
  gfl2::math::Vector3 loopStartPos( nowPos - (wayX * colR) );
  BaseCollisionScene *pCollision                   = GetCollisionSceneContainerForWall();
  BaseCollisionScene *pCollisionStaticModel        = m_headerResource.pCollisionSceneForStaticModel;
  BaseCollisionScene *pCollisionOshikaeshiNaminori = m_headerResource.pCollisionSceneForOshikaeshiNaminori;
  BaseCollisionScene *pCollisionOshikaeshiRoba2    = m_headerResource.pCollisionSceneForOshikaeshiRoba2;
  BaseCollisionScene *pCollisionOshikaeshiRide     = m_headerResource.pCollisionSceneForOshikaeshiRide;

  // 始点と終点を結ぶ
  // コリジョン幅を考慮した3つのレイを飛ばし、ヒットしなければジャンプしてOK
  gfl2::math::Vector4 checkStart;
  gfl2::math::Vector4 checkEnd;
  for( u32 i=0; i<5; ++i )
  {
    checkStart = loopStartPos + (wayX * (colR/2.0f * static_cast<f32>(i)));
    checkEnd   = checkStart + wayVec;
    checkStart.SetW(0.0);
    checkEnd.SetW(0.0);

    // 背景とのレイチェック
    s32 polyCnt = pCollision->RaycastFromStaticActorsMesh( checkStart, checkEnd );
    if( isCheckNaminori )
    {
      polyCnt += pCollisionOshikaeshiNaminori->RaycastFromStaticActorsMesh( checkStart, checkEnd );
    }
    if( isCheckRoba2 )
    {
      polyCnt += pCollisionOshikaeshiRoba2->RaycastFromStaticActorsMesh   ( checkStart, checkEnd );
    }
    if( isCheckRide )
    {
      polyCnt += pCollisionOshikaeshiRide->RaycastFromStaticActorsMesh    ( checkStart, checkEnd );
    }

    // 戻り値 -1 = 頂点に当たった  0=あたらない 1以上=面に
    if( polyCnt != (s32)acceptWallCnt )
    {
      return false;
    }

    //note:配置モデルとNPCはCylinderとBox形状しか利用していない

    // 配置モデル & NPCとのレイチェック
    if( pCollisionStaticModel->RaycastFromStaticActorsCylinder( checkStart, checkEnd  ) )
    {
      return false;
    }

    // 配置モデル & NPCとのレイチェック
    if( pCollisionStaticModel->RaycastFromStaticActorsBox( checkStart, checkEnd ) )
    {
      return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
*  @brief  開始座標から終了座標までの間に指定枚数以下の壁があるか調べる(配置モデルの判定は含めません)
*
*  @param  startPos        開始座標
*  @param  endPos          終了座標
*  @param  acceptWallCnt   チェックする壁の枚数
*  @param  isCheckNaminori チェックする壁に押し返しがある波乗り    フィルターも含める
*  @param  isCheckRoba2    チェックする壁に押し返しがあるロバ2     フィルターも含める
*  @param  isCheckRide     チェックする壁に押し返しがあるライド禁止フィルターも含める
*  @return trueなら指定枚数以下の壁がある、falseならない
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModel::GetHitCntFromOshikaeshiSceneEx(const gfl2::math::Vector3& startPos,const gfl2::math::Vector3& endPos,u32 acceptWallCnt,b32 isCheckNaminori,b32 isCheckRoba2,b32 isCheckRide)
{
  // コリジョン幅を取得
  f32 colR( -1.0f );

  // 背景から優先度の高い順場で幅を取得する
  if     ( GetDynamicActor( COLLISION_TYPE_TERRAIN             )->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ colR = static_cast<SphereShape*  >(GetDynamicActor(COLLISION_TYPE_TERRAIN             )->GetShape())->m_r; }
  else if( GetDynamicActor( COLLISION_TYPE_TRAFFIC_MODEL       )->GetShapeType() == BaseActor::SHAPE_CYLINDER ){ colR = static_cast<CylinderShape*>(GetDynamicActor(COLLISION_TYPE_TRAFFIC_MODEL       )->GetShape())->m_r; }
  else if( GetDynamicActor( COLLISION_TYPE_STATIC_MODEL        )->GetShapeType() == BaseActor::SHAPE_CYLINDER ){ colR = static_cast<CylinderShape*>(GetDynamicActor(COLLISION_TYPE_STATIC_MODEL        )->GetShape())->m_r; }
  else if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_NAMINORI )->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ colR = static_cast<SphereShape*  >(GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_NAMINORI )->GetShape())->m_r; }
  else if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_ROBA2    )->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ colR = static_cast<SphereShape*  >(GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_ROBA2    )->GetShape())->m_r; }
  else if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_RIDE     )->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ colR = static_cast<SphereShape*  >(GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_RIDE     )->GetShape())->m_r; }
  else { return false; }

  // 現在の座標からジャンプ座標への方向を取得
  gfl2::math::Vector3 nowPos( startPos );
  gfl2::math::Vector3 wayVec( endPos - nowPos );
  gfl2::math::Vector3 wayX  ( wayVec.Cross( gfl2::math::Vector3(0.0f,1.0f,0.0f) ) );

  // Y方向に移動している。
  if( wayX.LengthSq() <= EPSILON )
  {
    GFL_ASSERT( wayX.LengthSq() > EPSILON );
    return false;
  }
  wayX = wayX.Normalize();
  gfl2::math::Vector3 loopStartPos( nowPos - (wayX * colR) );
  BaseCollisionScene *pCollision                   = GetCollisionSceneContainerForWall();
  BaseCollisionScene *pCollisionStaticModel        = m_headerResource.pCollisionSceneForStaticModel;
  BaseCollisionScene *pCollisionOshikaeshiNaminori = m_headerResource.pCollisionSceneForOshikaeshiNaminori;
  BaseCollisionScene *pCollisionOshikaeshiRoba2    = m_headerResource.pCollisionSceneForOshikaeshiRoba2;
  BaseCollisionScene *pCollisionOshikaeshiRide     = m_headerResource.pCollisionSceneForOshikaeshiRide;

  // 始点と終点を結ぶ
  // コリジョン幅を考慮した3つのレイを飛ばし、１つでもヒットしなければジャンプしてOK
  gfl2::math::Vector4 checkStart;
  gfl2::math::Vector4 checkEnd;
  for( u32 i=0; i<3; ++i )
  {
    checkStart = loopStartPos + (wayX * (colR * static_cast<f32>(i)));
    checkEnd   = endPos;
    checkStart.SetW(0.0);
    checkEnd.SetW(0.0);

    // 背景とのレイチェック
    s32 polyCnt = pCollision->RaycastFromStaticActorsMesh( checkStart, checkEnd );
    if( isCheckNaminori )
    {
      polyCnt += pCollisionOshikaeshiNaminori->RaycastFromStaticActorsMesh( checkStart, checkEnd );
    }
    if( isCheckRoba2 )
    {
      polyCnt += pCollisionOshikaeshiRoba2->RaycastFromStaticActorsMesh   ( checkStart, checkEnd );
    }
    if( isCheckRide )
    {
      polyCnt += pCollisionOshikaeshiRide->RaycastFromStaticActorsMesh    ( checkStart, checkEnd );
    }

    // 戻り値 -1 = 頂点に当たった  0=あたらない 1以上=面に
    TOMOYA_PRINT( "polyCnt[%d] > acceptWallCnt[%d]\n", polyCnt, acceptWallCnt );
    if( polyCnt <= (s32)acceptWallCnt )
    {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  エンカウント草用のブロックとグリッド座標を更新
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::EncountGrassCreaterUpdateBlockAndGridPos(void)
{
  if( m_pEncountGrassCreater )
  {
    m_pEncountGrassCreater->UpdateBlockAndGridPos();
  }
}

//================================================================================================================
// 親のインターフェースの実装部
//================================================================================================================

//-----------------------------------------------------------------------------
/**
*  @brief 表示ON・OFFを設定
*
*  @param isVisible  表示ON・OFF
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetVisible(bool isVisible)
{
  if( m_pCharaDrawInstance && m_pCharaDrawInstance->GetModelInstanceNode() )
  {
    m_pCharaDrawInstance->SetVisible(isVisible);
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  表示ON・OFFを取得
*
*  @return 表示ON・OFF
*/
//-----------------------------------------------------------------------------
bool FieldMoveModel::GetVisible(void) const
{
  if( m_pCharaDrawInstance && m_pCharaDrawInstance->GetModelInstanceNode() )
  {
    return m_pCharaDrawInstance->IsVisible() ? true : false;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief SRTを設定
*
*  @param srt 設定したいSRT
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetLocalSRT(const gfl2::math::SRT & srt)
{
  if( m_pCharaDrawInstance && m_pCharaDrawInstance->GetModelInstanceNode() )
  {
    m_pCharaDrawInstance->SetScale( srt.scale );
    m_pCharaDrawInstance->SetRotationQuat( srt.rotation );
    m_pCharaDrawInstance->SetPosition( srt.translate );
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  SRTを取得
*
*  @return SRT
*/
//-----------------------------------------------------------------------------
const gfl2::math::SRT& FieldMoveModel::GetLocalSRT(void) const
{
  if( !m_pCharaDrawInstance )
  {
    return m_srtForReturn;
  }
  return m_pCharaDrawInstance->GetModelInstanceNode()->GetLocalSRT();
}

//-----------------------------------------------------------------------------
/**
*  @brief モデルインスタンス数を取得
*
*  @return モデルインスタンス数
*/
//-----------------------------------------------------------------------------
u32 FieldMoveModel::GetModelInstanceNum(void) const
{
  if( m_pCharaDrawInstance && m_pCharaDrawInstance->GetModelInstanceNode() )
  {
    return 1;
  }
  return 0;
}

//-----------------------------------------------------------------------------
/**
*  @brief モデルインスタンスを取得
*
*  @return モデルインスタンス
*/
//-----------------------------------------------------------------------------
const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * FieldMoveModel::GetModelInstance(int index) const
{
  if( m_pCharaDrawInstance && m_pCharaDrawInstance->GetModelInstanceNode() )
  {
    return m_pCharaDrawInstance->GetModelInstanceNode();
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
*  @brief モデルインスタンスの遅延解放は自由に実装
*
*  @note:
*   削除リクエストを受けたモデルインスタンスを解放
*   -実行タイミング  フィールドプロセスの先頭で実行します。
*   -制限            PipeLineの描画後にモデルインスタンスの遅延解放リクエストを出さないこと。
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ExecuteTerminateRequest(void)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  安全に破棄できる常態かチェック
*
*  @return 安全に解放できるかどうか
*/
//-----------------------------------------------------------------------------
bool FieldMoveModel::IsCanTerminate(void)
{
  return true;
}

//-----------------------------------------------------------------------------
/**
*  @brief  アンレジストに起因するアニメーション停止を行うか？
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModel::IsAnimationStopByUnRegist(void) const
{
#if PM_DEBUG
  if (FIELD_DEBUG_SWITCH(Field::Debug::DebugTypes::Switch::ENABLE_UNREGIST_ANIM_STOP))
#endif
  {
    if (CheckStateBit(Field::MoveModel::FIELD_MOVE_MODEL_ANIM_STOP_ACTOR_UNREGIST) && !GetVisible()) // @note イベント中にレジストせずに演出させている箇所があるため
    {
      // アクションコマンド実行中はアニメーションを更新する
      if (m_isExecuteActionCommand)
      {
        // GFL_ASSERT_MSG(0, "アンレジストされているキャラクターのアクションコマンドが実行されている(eventId=%d)", GetEventId());
        return false;
      }
      return true;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  イベント時にフリー移動をした状態にする
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::EnableFreeMoveForEventScript(void)
{ 
  m_isFreeMoveForEventScript  = true;
  m_isPointMoveForEventScript = false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  イベント時にバミリ移動をした状態にする
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::EnablePointMoveForEventScript(void)
{
  m_isFreeMoveForEventScript  = false;
  m_isPointMoveForEventScript = true;
}

//-----------------------------------------------------------------------------
/**
*  @brief  バミリチェックフラグをクリア
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ClearPositionCheckForEventScript(void)
{
  m_isFreeMoveForEventScript = false;
  m_isPointMoveForEventScript = false;
}

}; //end of namespace MoveModel
}; //end of namespace Field
