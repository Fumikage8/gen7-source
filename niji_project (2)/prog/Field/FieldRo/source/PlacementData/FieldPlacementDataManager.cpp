//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldPlacementDataManager.cpp
 *	@brief  配置データマネージャ−
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"
#include "Field/FieldRo/include/PlacementData/EventIDOffset.h"

#include "Savedata/include/EventWork.h"
#include "Savedata/include/FishingSpot.h"

#include "Field/FieldRo/include/Collision/BaseCollisionScene.h"
#include "Field/FieldRo/include/EnvSound/EnvSoundManager.h"

#include "niji_conv_header/field/FieldActionEventTypes.h"
#include "niji_conv_header/field/mapjump/FieldMapJumpType.h"
#include "niji_conv_header/field/FieldPlacementPMVersion.h"
#include "niji_conv_header/field/FieldPlacementTimeZone.h"
#include "niji_conv_header/field/FieldPlacementMoveCode.h"

#include "niji_reference_files/script/FieldPawnTypes.h"

#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountDataAccessor.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountManager.h"

#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/TimeZone.h"
#include "GameSys/include/NijiTime.h"

#include "Field/FieldRo/include/PlacementData/accessor/FieldFishingSpotAccessor.h"

#include "Field/FieldRo/include/PlacementData/accessor/FieldContactPokemonAccessor.h"

// Actor
#include "Field/FieldRo/include/PlacementData/actor/FieldActionEventActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldPosEventActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldIntrPosEventActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldMapJumpActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldMoveModelActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldStaticModelActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldBgEventActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldEncountActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldTrainerActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountActorBase.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountStandbyActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountChaseActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountPatrolActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountEscapeActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldPokeFinderActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldBerryPointActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldFishingSpotActor.h"

#include "Field/FieldRo/include/PlacementData/actor/FieldItemActor.h"

#include "Field/FieldStatic/include/Item/FieldItemManager.h"
#include "Field/FieldStatic/include/Berry/FieldBerryManager.h"

// Debug
#include "Field/FieldStatic/include/Debug/DebugSymbolNameLoader.h"
#include "FieldScript/include/ScriptAssert.h"

GFL_NAMESPACE_BEGIN(Field);

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
static const s32 FIELD_ACTOR_HEAP_SIZE = 0x10000;

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------


/**
 *   @brief コンストラクタ
 */
 PlacementDataManager::PlacementDataManager() : 
  m_pWorld(NULL),
  m_pCollisionSceneForTerrain(NULL),
  m_pCollisionSceneForStaticModel(NULL),
  m_pCollisionSceneForEventPos(NULL),
  m_pCollisionSceneForEventIntrPos(NULL),
  m_pCollisionSceneForEventTalk(NULL),
  m_pCollisionSceneForEventPush(NULL),
  m_pCollisionSceneForFishingPos(NULL),
  m_pMoveModelManager(NULL),
  m_pStaticModelManager(NULL),
  m_pTerrainManager(NULL),
  m_pRenderingPipeLine(NULL),
  m_pEnvSoundManager(NULL),
  m_pGimmickEncountManager(NULL),
  m_pFieldResident(NULL),
  m_pItemManager(NULL),
  m_pBerryManager(NULL),
  m_pFishingSpot(NULL),
  m_MapJumpEventData(),
  m_pPlacementData(NULL),
  m_cpEventWork(NULL),
  m_pActorHeap(NULL),
  m_pAllocator(NULL)
{
  for( u32 i = 0; i < POS_EVENT_CONTANER_MAX; ++ i )
  {
    m_pPosActor[ i ] = NULL;
  }
  for( u32 i = 0; i < MAPJUMP_EVENT_ACTOR_MAX; ++ i )
  {
    m_pMapJumpActor[ i ] = NULL;
  }
  for( u32 i = 0; i < MOVEMODEL_EVENT_ACTOR_MAX; ++ i )
  {
    m_pMoveModelActor[ i ] = NULL;
  }
  m_MoveModelActorAliasNum = 0;
  for( u32 i = 0; i < ACTION_EVENT_MAX; ++ i )
  {
    m_pActionEventActor[ i ] = NULL;
  }
  for( u32 i = 0; i < STATICMODEL_ACTOR_MAX; ++ i )
  {
    m_pStaticModelActor[ i ] = NULL;
  }
  m_StaticModelActorAliasNum = 0;
  for( u32 i = 0; i < BG_EVENT_ACTOR_MAX; ++ i )
  {
    m_pBgEventActor[ i ] = NULL;
  }
  for( u32 i = 0; i < ENCOUNT_ACTOR_MAX; ++ i )
  {
    m_pEncountActor[ i ] = NULL;
  }
  for( u32 i = 0; i < TRAINER_ACTOR_MAX; ++ i )
  {
    m_pTrainerActor[ i ] = NULL;
  }
  m_TrainerActorAliasNum = 0;
  for( u32 i = 0; i < GIMMICK_ENCOUNT_ACTOR_MAX; ++ i )
  {
    m_pGimmickEncountActor[ i ] = NULL;
    m_pGimmickEncountActorBase[ i ] = NULL;
  }
  for( u32 i = 0; i < ITEM_ACTOR_MAX; ++ i )
  {
    m_pItemActor[ i ] = NULL;
  }
  for( u32 i = 0; i < POKEFINDER_MAX; ++ i )
  {
    m_pPokeFinderActor[ i ] = NULL;
  }
  for (u32 i = 0; i < BERRY_POINT_ACTOR_MAX; ++i)
  {
    m_pBerryPointActor[i] = NULL;
  }
  for (u32 i = 0; i < FISHING_SPOT_ACTOR_MAX; ++i)
  {
    m_pFishingSpotActor[i] = NULL;
  }
  for ( u32 i = 0; i < PUSHROCK_ACTOR_MAX; ++i )
  {
    m_pPushRockActor[i] =  NULL;
    m_pPushRockMoveModelAccessor[i] = NULL;
  }
  for ( u32 i = 0; i < PUSHROCK_HOLE_MAX; ++i )
  {
    m_pPushRockHoleStaticActor[i] =  NULL;
    m_pPushRockHoleData[i] = NULL;
  }


#if PM_DEBUG
  debugCheckPlacementSymbol();
#endif
}

/**
 *   @brief デストラクタ
 */
 PlacementDataManager::~PlacementDataManager()
{
  this->Terminate();
}

/**
 *   @brief 初期化
 */
 void PlacementDataManager::Initialize( const Desc& desc )
{
  m_pCollisionSceneForTerrain               = desc.pCollisionSceneForTerrain;
  m_pCollisionSceneForStaticModel           = desc.pCollisionSceneForStaticModel;
  m_pCollisionSceneForEventPos              = desc.pCollisionSceneForEventPos;
  m_pCollisionSceneForEventIntrPos          = desc.pCollisionSceneForEventIntrPos;
  m_pCollisionSceneForEventTalk             = desc.pCollisionSceneForEventTalk;
  m_pCollisionSceneForEventPush             = desc.pCollisionSceneForEventPush;
  m_pCollisionSceneForEncount               = desc.pCollisionSceneForEncount;
  m_pCollisionSceneForTrainerEye            = desc.pCollisionSceneForTrainerEye;
  m_pCollisionSceneForGimmickEncount        = desc.pCollisionSceneForGimmickEncount;
  m_pCollisionSceneForGimmickEncountAction  = desc.pCollisionSceneForGimmickEncountAction;
  m_pCollisionSceneForEventFinder           = desc.pCollisionSceneForEventFinder;
  m_pCollisionSceneForFishingPos            = desc.pCollisionSceneForFishingPos;

  m_pMoveModelManager             = desc.pMoveModelManager;
  m_pStaticModelManager           = desc.pStaticModelManager;
  m_pEnvSoundManager              = desc.pEnvSoundManager;
  m_pGimmickEncountManager        = desc.pGimmickEncountManager;
  m_pFieldResident                = desc.pFieldResident;
  m_pItemManager                  = desc.pItemManager;
  m_pBerryManager                 = desc.pBerryManager;
  m_pFishingSpot                  = desc.pFishingSpot;

  m_pWorld         = desc.pWorld;
  m_pArea          = desc.pArea;
  m_pPlacementData = desc.pPlacementData;

  m_cpEventWork = desc.cpEventWork;

  m_MapJumpEventData.Initialize( desc.cpMapJumpParam, desc.mapJumpParamSize );


  // アクター用ヒープ確保
  m_pActorHeap = GFL_CREATE_LOCAL_HEAP_NAME( desc.pHeap, FIELD_ACTOR_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FieldActor" );
  m_pAllocator = GFL_NEW( m_pActorHeap ) System::nijiAllocator( m_pActorHeap );

  GFL_ASSERT( m_pPlacementData->GetDataMax() >= PACK_IDX_MAX );

  // 種類ごとの配置データに分割
  for( u32 i=0; i<PACK_IDX_MAX; ++i )
  {
    void * p_data = m_pPlacementData->GetData(i);
    m_AreaPlacementData[i].Initialize(p_data);
  }

}

/**
 *   @brief 初期化
 */
 void PlacementDataManager::InitializeFieldmap( const DescFieldmap& desc )
{
  m_pTerrainManager               = desc.pTerrainManager;
  m_pRenderingPipeLine            = desc.pRenderingPipeLine;
}


/**
 *   @brief 破棄
 */
 void PlacementDataManager::Terminate()
{

  m_MapJumpEventData.Terminate();

  for( u32 i = 0; i < POS_EVENT_CONTANER_MAX; ++ i )
  {
    if( m_pPosActor[ i ] )
    {
      m_pPosActor[ i ]->Terminate();
      GFL_SAFE_DELETE(m_pPosActor[ i ]);
    }
  }
  for( u32 i = 0; i < MAPJUMP_EVENT_ACTOR_MAX; ++ i )
  {
    if( m_pMapJumpActor[ i ] )
    {
      m_pMapJumpActor[ i ]->Terminate();
      GFL_SAFE_DELETE(m_pMapJumpActor[ i ]);
    }
  }
  for( u32 i = 0; i < MOVEMODEL_EVENT_ACTOR_MAX; ++ i )
  {
    if(m_pMoveModelActor[ i ]){
      reinterpret_cast<FieldMoveModelActor *>(m_pMoveModelActor[ i ])->TerminateResource(); // アプリジャンプ用　動作モデルワークは残す。
      GFL_SAFE_DELETE(m_pMoveModelActor[ i ]);
    }
  }
  for( u32 i = 0; i < ACTION_EVENT_MAX; ++ i )
  {
    if( m_pActionEventActor[ i ] )
    {
      m_pActionEventActor[ i ]->Terminate();
      GFL_SAFE_DELETE(m_pActionEventActor[ i ]);
    }
  }
  for( u32 i = 0; i < STATICMODEL_ACTOR_MAX; ++ i )
  {
    if( m_pStaticModelActor[ i ] )
    {
      m_pStaticModelActor[ i ]->Terminate();
      GFL_SAFE_DELETE(m_pStaticModelActor[ i ]);
    }
  }
  for( u32 i = 0; i < BG_EVENT_ACTOR_MAX; ++ i )
  {
    if( m_pBgEventActor[ i ] )
    {
      m_pBgEventActor[ i ]->Terminate();
      GFL_SAFE_DELETE(m_pBgEventActor[ i ]);
    }
  }
  for( u32 i = 0; i < ENCOUNT_ACTOR_MAX; ++ i )
  {
    if( m_pEncountActor[ i ] )
    {
      m_pEncountActor[ i ]->Terminate();
      GFL_SAFE_DELETE(m_pEncountActor[ i ]);
    }
  }
  for( u32 i = 0; i < TRAINER_ACTOR_MAX; ++ i )
  {
    if(m_pTrainerActor[ i ]){
      reinterpret_cast<FieldTrainerActor *>(m_pTrainerActor[ i ])->TerminateResource(); // アプリジャンプ用　動作モデルワークは残す。
      GFL_SAFE_DELETE(m_pTrainerActor[ i ]);
    }
  }
  for( u32 i = 0; i < GIMMICK_ENCOUNT_ACTOR_MAX; ++ i )
  {
    if(m_pGimmickEncountActor[ i ]){
      if( m_pGimmickEncountActorBase[ i ]->GetLocalWork()->GetGimmickType() != GimmickEncount::GIMMICK_TYPE_STANDBY)
      {
        // 動作モデルの破棄処理。アプリジャンプ用　動作モデルワークは残す。
        reinterpret_cast<FieldMoveModelActorBase *>(m_pGimmickEncountActor[ i ])->TerminateResource();
      }
      else
      {
        m_pGimmickEncountActor[ i ]->Terminate();
      }
      GFL_SAFE_DELETE(m_pGimmickEncountActor[ i ]);
      m_pGimmickEncountActorBase[ i ] = NULL;
    }
  }
  for( u32 i = 0; i < ITEM_ACTOR_MAX; ++ i )
  {
    if( m_pItemActor[ i ] )
    {
      m_pItemActor[ i ]->Terminate();
      GFL_SAFE_DELETE(m_pItemActor[ i ]);
    }
  }
  for( u32 i = 0; i < POKEFINDER_MAX; ++ i )
  {
    if( m_pPokeFinderActor[ i ] )
    {
      m_pPokeFinderActor[ i ]->Terminate();
      GFL_SAFE_DELETE( m_pPokeFinderActor[ i ] );
    }
  }
  for (u32 i = 0; i < BERRY_POINT_ACTOR_MAX; ++i)
  {
    if( m_pBerryPointActor[i] )
    {
      m_pBerryPointActor[i]->Terminate();
      GFL_SAFE_DELETE(m_pBerryPointActor[i]);
    }
  }
  for (u32 i = 0; i < FISHING_SPOT_ACTOR_MAX; ++i) 
  {
    if( m_pFishingSpotActor[i] )
    {
      m_pFishingSpotActor[i]->Terminate();
      GFL_SAFE_DELETE(m_pFishingSpotActor[i]);
    }
  }

  for ( u32 i = 0; i < PUSHROCK_ACTOR_MAX; ++i )
  {
    if( m_pPushRockActor[i] )
    {
      reinterpret_cast<FieldMoveModelActor *>(m_pPushRockActor[i])->TerminateResource();
      GFL_SAFE_DELETE(m_pPushRockActor[i]);
      GFL_SAFE_DELETE(m_pPushRockMoveModelAccessor[i]);
    }
  }
  for ( u32 i = 0; i < PUSHROCK_HOLE_MAX; ++i )
  {
    if( m_pPushRockHoleStaticActor[i] )
    {
      m_pCollisionSceneForStaticModel->UnRegistStaticActor( reinterpret_cast<StaticActor*>(m_pPushRockHoleStaticActor[i]) );
      GFL_SAFE_DELETE(m_pPushRockHoleStaticActor[i]);
      m_pPushRockHoleData[i] = NULL;
    }
  }

  m_pFieldResident  = (NULL);
  m_pItemManager    = (NULL);
  m_pBerryManager   = (NULL);
  m_pFishingSpot    = (NULL);

  GFL_SAFE_DELETE( m_pAllocator );
  if( m_pActorHeap )
  {
    GFL_DELETE_HEAP( m_pActorHeap );
    m_pActorHeap = NULL;
  }
}

/**
 *   @brief 破棄
 */
 void PlacementDataManager::TerminateFieldmap()
{
  m_pTerrainManager = NULL;
  m_pRenderingPipeLine = NULL;
}

/**
 *  @brief flagworkに関連する　全イベントのRegist
 */
void PlacementDataManager::RegistAllEvent( u32 flagwork )
{
  RegistZonePos( flagwork );
  RegistZoneMapJump( flagwork );
  RegistZoneActionEvent( flagwork );
  RegistZoneModel(flagwork);
  RegistZoneStaticModel(flagwork);
  RegistZoneBgEvent( flagwork );
  RegistZoneTrainer( flagwork );
  RegistZoneItem( flagwork );
}

/**
 *  @brief flagworkに関連する 全イベントを対象にUnregist
 */
void PlacementDataManager::UnRegistAllEvent( u32 flagwork )
{
  UnRegistZonePos( flagwork );
  UnRegistZoneMapJump( flagwork );
  UnRegistZoneActionEvent( flagwork );
  UnRegistZoneModel(flagwork);
  UnRegistZoneStaticModel(flagwork);
  UnRegistZoneBgEvent( flagwork );
  UnRegistZoneTrainer( flagwork );
  UnRegistZoneItem( flagwork );
}

/**
 * @brief POSイベントの生成
 */
 void PlacementDataManager::CreateZonePos( ZoneID zoneID )
{
  void * p_data = GetZonePlacementData( zoneID, PACK_IDX_POS );
  if( p_data == NULL ){ return ; }

  // アクセサー生成
  FieldPosEventAccessor posAccessor;
  posAccessor.Initialize( p_data );

  // データサイズチェック
  u32 maxi = posAccessor.GetDataMax();
  if( maxi > POS_EVENT_CONTANER_MAX )
  {
    GFL_ASSERT_MSG( false, "PosEvent num Over (%d/%d)", maxi, POS_EVENT_CONTANER_MAX );
    return;
  }

  // IntrPosかPosか
  b32 isPos = false;

  // posイベントの生成
  for( u32 i = 0; i < maxi; ++ i )
  {
    const FieldPosEventAccessor::PosData *posData = posAccessor.GetData( i );
    gfl2::math::Vector3 pos( posData->position[ 0 ], posData->position[ 1 ], posData->position[ 2 ] );

    // IntrPosかPosか
    isPos = !posData->intr_pos_flag;

    FieldCollisionAccessor collisionAccessor;
    const void *p_col = posAccessor.GetCollisionData( *posData );
    collisionAccessor.Initialize( p_col );

    IFieldActorBase::SetupData setupData;
    setupData.pAllocator    = m_pAllocator;
    setupData.pHeap         = m_pActorHeap;
    setupData.pColAccessor  = &collisionAccessor;
    setupData.vPos          = pos;

    if( isPos )
    {
      setupData.pScene        = m_pCollisionSceneForEventPos;
    }
    else
    {
      setupData.pScene        = m_pCollisionSceneForEventIntrPos;
    }

    FieldPosEventActor *pActor = NULL;
    
    if(isPos)
    {
      pActor = GFL_NEW( m_pActorHeap ) FieldPosEventActor();
    }
    else
    {
      pActor = GFL_NEW( m_pActorHeap ) FieldIntrPosEventActor();
    }

    pActor->Initialize( setupData );
    if( isEventCreate( posData->romVersion, posData->flagwork, posData->flagwork_num, static_cast<ReverseTimeType>(posData->reversetime_type) ) )
    {
      // 起動条件を満たしているものだけ追加する
      pActor->RegistActor();
    }

    pActor->SetData( posData );

    m_pPosActor[ i ] = pActor;
  }
}

/**
 * @brief POSイベントの破棄
 */
 void PlacementDataManager::DeleteZonePos()
{
  // POSイベントの破棄
  for( u32 i = 0; i < POS_EVENT_CONTANER_MAX; ++ i )
  {
    if( m_pPosActor[ i ] )
    {
      m_pPosActor[ i ]->Terminate();
      GFL_SAFE_DELETE( m_pPosActor[ i ] );
    }
  }
}


/**
 *  @brief  POSイベントの追加
 *  @param  flagwork  生成条件「flagwork」
 */
 void PlacementDataManager::RegistZonePos( u32 flagwork )
{
  for( u32 i = 0; i < POS_EVENT_CONTANER_MAX; ++ i )
  {
    FieldPosEventActor* pActor = reinterpret_cast<FieldPosEventActor*>( m_pPosActor[ i ] );

    if( !pActor ) { continue; }
    // 既に登録済み
    if( pActor->IsRegist() ) { continue; }
    const FieldPosEventAccessor::PosData* pData = pActor->GetData();

    // 生成条件が一致するもののみを追加
    if( flagwork != pData->flagwork ) { continue; }
    // 条件を満たしていない
    if( !isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, static_cast<ReverseTimeType>(pData->reversetime_type) ) ){ continue; }

    // 追加する
    m_pPosActor[ i ]->RegistActor();
  }
}

// POSイベントの破棄
/**
 *  @brief  個別POSイベントの破棄
 *  @param  zoneID    ゾーンID
 *  @param  flagwork  生成条件「flagwork」        (破棄条件ではなく生成条件)
 */
 void PlacementDataManager::UnRegistZonePos(u32 flagwork )
{
  for( u32 i = 0; i < POS_EVENT_CONTANER_MAX; ++ i )
  {
    FieldPosEventActor* pActor = reinterpret_cast<FieldPosEventActor*>( m_pPosActor[ i ] );
    if( !pActor ) { continue; }
    // 既に削除済み
    if( !pActor->IsRegist() ) { continue; }
    const FieldPosEventAccessor::PosData* pData = pActor->GetData();
    // 生成条件が一致するもののみを追加
    if( flagwork != pData->flagwork ) { continue; }
    // 条件を満たしている
    if( isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, static_cast<ReverseTimeType>(pData->reversetime_type) ) ){ continue; }

    // 削除する
    m_pPosActor[ i ]->UnregistActor();
  }
}


/**
 *  @brief  MAPJUMPイベントの生成
 */
 void PlacementDataManager::CreateZoneMapJump( ZoneID zoneID )
{
  void * p_data = GetZonePlacementData( zoneID, PACK_IDX_MAPJUMP );
  if( p_data == NULL ){ return ; }

  // アクセサー生成
  FieldMapJumpAccessor mapJumpAccessor;
  mapJumpAccessor.Initialize( p_data );
  
  // データサイズチェック
  u32 maxi = mapJumpAccessor.GetDataMax();
  if( maxi > MAPJUMP_EVENT_ACTOR_MAX )
  {
    GFL_ASSERT_MSG( false, "MapJump num Over (%d/%d)", maxi, POS_EVENT_CONTANER_MAX );
    return;
  }

  // posイベントの生成
  for( u32 i = 0; i < maxi; ++ i )
  {
    const FieldMapJumpAccessor::MapJumpData *mapjumpData = mapJumpAccessor.GetData( i );
    gfl2::math::Vector3 pos( mapjumpData->position[ 0 ], mapjumpData->position[ 1 ], mapjumpData->position[ 2 ] );
    BaseCollisionScene *pScene = NULL;
    u32 kick_type = m_MapJumpEventData.GetMapJumpParam( mapjumpData->mapJumpType )->kick_type;
    // コリジョンシーンの選択
    switch( kick_type )
    {
    case MAPJUMP_KICK_POS:
      pScene = m_pCollisionSceneForEventPos;
      break;
    case MAPJUMP_KICK_TALK:
      pScene = m_pCollisionSceneForEventTalk;
      break;
    case MAPJUMP_KICK_PUSH:
      pScene = m_pCollisionSceneForEventPush;
      break;
    }
    FieldMapJumpActor *pActor = GFL_NEW( m_pActorHeap ) FieldMapJumpActor();
    
    FieldCollisionAccessor collisionAccessor;
    const void *p_col = mapJumpAccessor.GetCollisionData( *mapjumpData );
    collisionAccessor.Initialize( p_col );

    IFieldActorBase::SetupData setupData;
    setupData.pAllocator    = m_pAllocator;
    setupData.pHeap         = m_pActorHeap;
    setupData.pScene        = pScene;
    setupData.pColAccessor  = &collisionAccessor;
    setupData.vPos          = pos;

    pActor->Initialize( setupData );
    if( isEventCreate( mapjumpData->romVersion, mapjumpData->flagwork, mapjumpData->flagwork_num, RTIME_TYPE_ALL ) )
    {
      pActor->RegistActor();
    }
    pActor->SetData( mapjumpData );
    m_pMapJumpActor[ i ] = pActor;
  }
}

/**
 *  @brief  MAPJUMPイベントの破棄
 */
 void PlacementDataManager::DeleteZoneMapJump()
{
  // MapJumpイベントの破棄
  for( u32 i = 0; i < MAPJUMP_EVENT_ACTOR_MAX; ++ i )
  {
    if( m_pMapJumpActor[ i ] )
    {
      m_pMapJumpActor[ i ]->Terminate();
      GFL_SAFE_DELETE( m_pMapJumpActor[ i ] );
    }
  }
}

/**
 *  @brief  個別MAPJUMPイベントの生成
 */
 void PlacementDataManager::RegistZoneMapJump( u32 flagwork )
{
  for( u32 i = 0; i < MAPJUMP_EVENT_ACTOR_MAX; ++ i )
  {
    FieldMapJumpActor *pActor = reinterpret_cast<FieldMapJumpActor*>( m_pMapJumpActor[ i ] );
    if( !pActor ) { continue; }
    // 登録済み
    if( pActor->IsRegist() ) { continue; }
    const FieldMapJumpAccessor::MapJumpData *pData = pActor->GetData();
    // 生成条件が一致するもののみを捜査対象にする
    if( flagwork != pData->flagwork ) { continue; }
    // 条件を満たしている
    if( !isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }

    // 追加する
    pActor->RegistActor();

    // 連携するStaticModelが存在する場合がある。　そちらも設定
    RegistZoneEventIDStaticModel( pData->eventID );
  }
}

void PlacementDataManager::RegistZoneEventIDMapJump( EventID eventId )
{
  for( u32 i = 0; i < MAPJUMP_EVENT_ACTOR_MAX; ++ i )
  {
    FieldMapJumpActor *pActor = reinterpret_cast<FieldMapJumpActor*>( m_pMapJumpActor[ i ] );
    if( !pActor ) { continue; }
    // 登録済み
    if( pActor->IsRegist() ) { continue; }
    const FieldMapJumpAccessor::MapJumpData *pData = pActor->GetData();
    // 生成条件が一致するもののみを捜査対象にする
    if( eventId != pData->eventID ) { continue; }
    // 条件を満たしている
    if( !isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }

    // 追加する
    pActor->RegistActor();

    // 連携するStaticModelが存在する場合がある。　そちらも設定
    RegistZoneEventIDStaticModel( pData->eventID );
  }
}

/**
 *  @brief  個別MAPJUMPイベントの破棄
 */
 void PlacementDataManager::UnRegistZoneMapJump( u32 flagwork )
{
  for( u32 i = 0; i < MAPJUMP_EVENT_ACTOR_MAX; ++ i )
  {
    FieldMapJumpActor *pActor = reinterpret_cast<FieldMapJumpActor*>( m_pMapJumpActor[ i ] );
    if( !pActor ) { continue; }
    // 削除済み
    if( !pActor->IsRegist() ) { continue; }
    const FieldMapJumpAccessor::MapJumpData *pData = pActor->GetData();
    // 生成条件が一致するもののみを捜査対象にする
    if( flagwork != pData->flagwork ) { continue; }
    // 条件を満たしている
    if( isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }

    // 削除する
    pActor->UnregistActor();

    // 連携するStaticModelが存在する場合がある。　そちらも設定
    UnRegistZoneEventIDStaticModel( pData->eventID );
  }
}

/**
 *  @brief  個別MAPJUMPイベントの破棄
 */
void PlacementDataManager::UnRegistZoneEventIDMapJump( EventID eventId )
{
  for( u32 i = 0; i < MAPJUMP_EVENT_ACTOR_MAX; ++ i )
  {
    FieldMapJumpActor *pActor = reinterpret_cast<FieldMapJumpActor*>( m_pMapJumpActor[ i ] );
    if( !pActor ) { continue; }
    // 削除済み
    if( !pActor->IsRegist() ) { continue; }
    const FieldMapJumpAccessor::MapJumpData *pData = pActor->GetData();
    // 生成条件が一致するもののみを捜査対象にする
    if( eventId != pData->eventID ) { continue; }
    // 条件を満たしている
    if( isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }

    // 削除する
    pActor->UnregistActor();

    // 連携するStaticModelが存在する場合がある。　そちらも設定
    UnRegistZoneEventIDStaticModel( pData->eventID );
  }
}

 
/**
 *  @brief  個別MAPJUMPイベントのサーチ
 */
IFieldActorBase* PlacementDataManager::SearchZoneMapJump( EventID eventId )
{
  for( u32 i = 0; i < MAPJUMP_EVENT_ACTOR_MAX; ++ i )
  {
    FieldMapJumpActor *pActor = reinterpret_cast<FieldMapJumpActor*>( m_pMapJumpActor[ i ] );
    if( !pActor ) { continue; }
    // 削除済み
    if( !pActor->IsRegist() ) { continue; }
    const FieldMapJumpAccessor::MapJumpData *pData = pActor->GetData();

    if( pData->eventID == eventId )
    {
      return pActor; 
    }
  }
  return NULL;
}


/**
 *  @brief  アクションイベントの生成
 */
 void PlacementDataManager::CreateZoneActionEvent( ZoneID zoneID )
{
  void * pData = GetZonePlacementData( zoneID, PACK_IDX_ACTION );
  if( pData == NULL ){ return ; }

  // アクセッサ生成
  FieldActionEventAccessor accessor;
  accessor.Initialize( pData );

  // データサイズチェック
  u32 maxi = accessor.GetDataMax();
  if( maxi > ACTION_EVENT_MAX )
  {
    GFL_ASSERT_MSG( false, "ActionEvent num Over (%d/%d)", maxi, ACTION_EVENT_MAX );
    return;
  }

  // アクションイベントの生成
  for( u32 i = 0; i < maxi; ++ i )
  {
    const FieldActionEventAccessor::ActionEventData *data = accessor.GetData( i );
    BaseCollisionScene *pScene = getCollisionSceneFromActionType( data->actionID );
    if( !pScene ){ continue; }
    gfl2::math::Vector3 pos( data->position[ 0 ], data->position[ 1 ], data->position[ 2 ] );
    FieldActionEventActor *pActor = GFL_NEW( m_pActorHeap ) FieldActionEventActor();

    FieldCollisionAccessor collisionAccessor;
    const void *p_col = accessor.GetCollisionData( *data );
    collisionAccessor.Initialize( p_col );

    FieldActionEventActor::Description setupData;
    setupData.pAllocator    = m_pAllocator;
    setupData.pHeap         = m_pActorHeap;
    setupData.pCollisionScene = pScene;
    setupData.pCollisionSceneTalk = m_pCollisionSceneForEventTalk;
    setupData.colAccessor   = &collisionAccessor;
    setupData.vPos          = pos;
    setupData.pData         = data;

    pActor->Initialize( setupData );
    if( isEventCreate( data->romVersion, data->flagwork, data->flagwork_num, RTIME_TYPE_ALL ) )
    {
      pActor->RegistActor();
    }
    pActor->InitializeAccessor( pData );
    m_pActionEventActor[ i ] = pActor;
  }
}

/**
 *  @brief  アクションイベントの破棄
 */
 void PlacementDataManager::DeleteZoneActionEvent()
{
  // アクションイベントの破棄
  for( u32 i = 0; i < ACTION_EVENT_MAX; ++ i )
  {
    if( m_pActionEventActor[ i ] )
    {
      m_pActionEventActor[ i ]->Terminate();
      GFL_SAFE_DELETE( m_pActionEventActor[ i ] );
    }
  }
}

/**
 *  @brief  アクションイベントの追加
 */
 void PlacementDataManager::RegistZoneActionEvent( u32 flagwork )
{
  for( u32 i = 0; i < ACTION_EVENT_MAX; ++ i )
  {
    FieldActionEventActor *pActor = reinterpret_cast<FieldActionEventActor*>( m_pActionEventActor[ i ] );
    if( !pActor ) { continue; }
    // 登録済み
    if( pActor->IsRegist() ) { continue; }
    const FieldActionEventAccessor::ActionEventData *pData = pActor->GetData();
    // 生成条件が一致するもののみを捜査対象にする
    if( flagwork != pData->flagwork ) { continue; }
    // 条件を満たしている
    if( !isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }

    // 追加する
    m_pActionEventActor[ i ]->RegistActor();

    // 連携するStaticModelが存在する場合がある。　そちらも設定
    RegistZoneEventIDStaticModel( pData->eventID );
  }
}

 void PlacementDataManager::RegistZoneEventIDActionEvent( EventID eventID )
{
  for( u32 i = 0; i < ACTION_EVENT_MAX; ++ i )
  {
    FieldActionEventActor *pActor = reinterpret_cast<FieldActionEventActor*>( m_pActionEventActor[ i ] );
    if( !pActor ) { continue; }
    // 登録済み
    if( pActor->IsRegist() ) { continue; }
    const FieldActionEventAccessor::ActionEventData *pData = pActor->GetData();
    // 生成条件が一致するもののみを捜査対象にする
    if( eventID != pData->eventID ) { continue; }
    // 条件を満たしている
    if( !isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }

    // 追加する
    m_pActionEventActor[ i ]->RegistActor();

    // 連携するStaticModelが存在する場合がある。　そちらも設定
    RegistZoneEventIDStaticModel( pData->eventID );
  }
}

/**
 *  @brief  アクションイベントの削除
 */
 void PlacementDataManager::UnRegistZoneActionEvent( u32 flagwork )
{
  for( u32 i = 0; i < ACTION_EVENT_MAX; ++ i )
  {
    FieldActionEventActor *pActor = reinterpret_cast<FieldActionEventActor*>( m_pActionEventActor[ i ] );
    if( !pActor ) { continue; }
    // 削除済み
    if( !pActor->IsRegist() ) { continue; }
    const FieldActionEventAccessor::ActionEventData *pData = pActor->GetData();
    // 生成条件が一致するもののみを捜査対象にする
    if( flagwork != pData->flagwork ) { continue; }
    // 条件を満たしている
    if( isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }

    // 削除する
    m_pActionEventActor[ i ]->UnregistActor();

    // 連携するStaticModelが存在する場合がある。　そちらも設定
    UnRegistZoneEventIDStaticModel( pData->eventID );
  }
}

 void PlacementDataManager::UnRegistZoneEventIDActionEvent( EventID eventID )
{
  for( u32 i = 0; i < ACTION_EVENT_MAX; ++ i )
  {
    FieldActionEventActor *pActor = reinterpret_cast<FieldActionEventActor*>( m_pActionEventActor[ i ] );
    if( !pActor ) { continue; }
    // 削除済み
    if( !pActor->IsRegist() ) { continue; }
    const FieldActionEventAccessor::ActionEventData *pData = pActor->GetData();
    // 生成条件が一致するもののみを捜査対象にする
    if( eventID != pData->eventID ) { continue; }
    // 条件を満たしている
    if( isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }

    // 削除する
    m_pActionEventActor[ i ]->UnregistActor();

    // 連携するStaticModelが存在する場合がある。　そちらも設定
    UnRegistZoneEventIDStaticModel( pData->eventID );
  }
}

/**
 *  @brief  ジョインフェスタ用のアクションイベントの削除
 *  @note ジョインフェスタの岩を壊すアトラクションでのみ使用可能
 *  @note 上記アトラクションで配置した岩は全てUnRegistされているため、この関数内では連携するStaticModelのUnRegistを行う
 */
 void PlacementDataManager::UnRegistZoneActionEventForJoinFesta( u32 flagwork )
{
  for( u32 i = 0; i < ACTION_EVENT_MAX; ++ i )
  {
    FieldActionEventActor *pActor = reinterpret_cast<FieldActionEventActor*>( m_pActionEventActor[ i ] );

    if( pActor == NULL )
    {
      continue;
    }

    const FieldActionEventAccessor::ActionEventData *pData = pActor->GetData();

    // 生成条件が一致するもののみを捜査対象にする
    if( flagwork != pData->flagwork )
    {
      continue;
    }
    // 条件を満たしている
    if( isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) )
    {
      continue;
    }

    // ジョインフェスタアトラクション中にKick処理によって全てUnRegsitされているはず
    if( pActor->IsRegist() )
    {
      GFL_ASSERT( 0 );
      pActor->UnregistActor();
    }

    // 連携するStaticModelを削除する
    UnRegistZoneEventIDStaticModel( pData->eventID );

    break;
  }
}


/**
 *  @brief  アクションイベントのサーチ
 */
IFieldActorBase* PlacementDataManager::SearchZoneActionEvent( u32 flagWork )
{
  for( u32 i = 0; i < ACTION_EVENT_MAX; ++ i )
  {
    FieldActionEventActor *pActor = reinterpret_cast<FieldActionEventActor*>( m_pActionEventActor[ i ] );
    if( !pActor ) { continue; }
    // 削除済み
    if( !pActor->IsRegist() ) { continue; }
    const FieldActionEventAccessor::ActionEventData *pData = pActor->GetData();

    if( pData->flagwork == flagWork )
    {
      return pActor; 
    }
  }
  return NULL;
}

/**
 *   @brief 動作モデルの生成：ワークを作る
 */
 void PlacementDataManager::CreateZoneModelWork( ZoneID zoneID )
{
  // momiji にて『ふれあい表現』用の動作モデルが増えたので２つの動作モデルを同時に扱うように処理を変更
  void * p_data         = GetZonePlacementData(zoneID, PACK_IDX_MOVEMODEL);
  void * p_data_contact = GetZonePlacementData(zoneID, PACK_IDX_CONTACT_POKEMON);

  // 共にデータが存在しない場合は中断
  if (p_data == NULL && p_data_contact == NULL){ return; }

  FieldMoveModelAccessor moveModelAccessor;
  FieldContactPokemonAccessor contactPokeAccessor;

  // -----------------------------------------
  // アクセサーの生成とデータサイズのチェック
  // -----------------------------------------
  u32 maxi = 0;
  if (p_data)
  {
    // 通常の動作モデル用アクセサー生成
    moveModelAccessor.Initialize(p_data);
    maxi += moveModelAccessor.GetNotAliasDataMax();
  }
  if (p_data_contact)
  {
    // ふれあい用アクセサー生成
    contactPokeAccessor.Initialize(p_data_contact);
    maxi += contactPokeAccessor.GetNotAliasDataMax();
  }
  // データサイズチェック
  if ((maxi + m_MoveModelActorAliasNum) > MOVEMODEL_EVENT_ACTOR_MAX)
  {
    GFL_ASSERT_MSG(false, "MoveModel num Over (%d/%d) zone(%d) alias(%d)", maxi + m_MoveModelActorAliasNum, MOVEMODEL_EVENT_ACTOR_MAX, maxi, m_MoveModelActorAliasNum);
    return;
  }

  // -----------------------------------------
  // 動作モデル生成
  // -----------------------------------------
  u32 countup = 0;
  if (p_data)
  {  
    // 通常の動作モデル生成
    u32 roopNum = moveModelAccessor.GetDataMax();
    for (u32 i = 0; i < roopNum; ++i)
    {
      const FieldMoveModelAccessor::MoveModelData *moveModelData = moveModelAccessor.GetData(i);

      // エイリアスでない物だけ下に流す
      if (moveModelData->alias)
      {
        continue;
      }

      FieldMoveModelActor *pActor = createMoveModelActor(moveModelData, zoneID, false);
      m_pMoveModelActor[countup + m_MoveModelActorAliasNum] = pActor;
      countup++;
    }
  }
  // カウンタを引き継いでふれあい用の動作モデルを生成
  if (p_data_contact)
  {
    // ふれあい用の動作モデル生成
    u32 roopNum = contactPokeAccessor.GetDataMax();
    for (u32 i = 0; i < roopNum; ++i)
    {
      const FieldMoveModelAccessor::MoveModelData *moveModelData = contactPokeAccessor.GetData(i);

      // エイリアスでない物だけ下に流す
      if (moveModelData->alias)
      {
        continue;
      }

      const FieldContactPokemonAccessor::ContactPokemonData *contactPokemonData = contactPokeAccessor.GetDataForContact(i);

      FieldMoveModelActor *pActor = createMoveModelActor(moveModelData, zoneID, false, contactPokemonData);
      m_pMoveModelActor[countup + m_MoveModelActorAliasNum] = pActor;
      countup++;
    }
  }
  

}

/**
 *   @brief 動作モデルの生成：リソースとコリジョンを設定
 */
 void PlacementDataManager::CreateZoneModelResource( ZoneID zoneID )
{
  // momiji にて『ふれあい表現』用の動作モデルが増えたので２つの動作モデルを同時に扱うように処理を変更
  void * p_data         = GetZonePlacementData(zoneID, PACK_IDX_MOVEMODEL);
  void * p_data_contact = GetZonePlacementData(zoneID, PACK_IDX_CONTACT_POKEMON);
  
  // 共にデータが存在しない場合は中断
  if (p_data == NULL && p_data_contact == NULL){ return; }

  // アクセサー生成
  FieldMoveModelAccessor moveModelAccessor;
  if (p_data)
  {
    moveModelAccessor.Initialize(p_data);
  }
  FieldContactPokemonAccessor contactPokeAccessor;
  if (p_data_contact)
  {
    contactPokeAccessor.Initialize(p_data_contact);
  }
  
  // 通常の動作モデル
  if (p_data)
  {
    for (u32 i = m_MoveModelActorAliasNum; i < MOVEMODEL_EVENT_ACTOR_MAX; ++i)
    {
      FieldMoveModelActor *pActor = reinterpret_cast<FieldMoveModelActor *>(m_pMoveModelActor[i]);
      if (!pActor){ continue; }

      // 通常の動作モデルの場合のみ処理
      if (!pActor->IsContactPokemon())
      {
        createMoveModelActorResource(pActor, &moveModelAccessor);
      }
    }
  }

  // ふれあい用の動作モデル
  if (p_data_contact)
  {
    for (u32 i = m_MoveModelActorAliasNum; i < MOVEMODEL_EVENT_ACTOR_MAX; ++i)
    {
      FieldMoveModelActor *pActor = reinterpret_cast<FieldMoveModelActor *>(m_pMoveModelActor[i]);
      if (!pActor){ continue; }

      // ふれあい用の動作モデルの場合のみ処理
      if (pActor->IsContactPokemon())
      {
        createMoveModelActorResource(pActor, &contactPokeAccessor);
      }
    }
  }
  
}

void PlacementDataManager::RegistZoneModel( u32 flagwork )
{
  for( u32 i = 0; i < MOVEMODEL_EVENT_ACTOR_MAX; ++ i )
  {
    FieldMoveModelActor *pActor = reinterpret_cast<FieldMoveModelActor *>( m_pMoveModelActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に追加されている
    if( pActor->IsRegist() ){ continue; }
    const FieldMoveModelAccessor::MoveModelData* pData = pActor->GetData();
    if( pData->flagwork == flagwork )
    {
      // 条件を満たしていない
      if( !isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, static_cast<ReverseTimeType>(pData->reversetime_type) ) ){ continue; }

      pActor->RegistActor();
    }
  }
}

/**
 *  @brief  ゾーンの特定のイベントIDの動作モデルを追加
 */
 s32 PlacementDataManager::RegistZoneEventIDModel( EventID eventId, Field::EventWork* pEventWork, b32 isVanishUpdate )
{
  for( u32 i = 0; i < MOVEMODEL_EVENT_ACTOR_MAX; ++ i )
  {
    FieldMoveModelActor *pActor = reinterpret_cast<FieldMoveModelActor *>( m_pMoveModelActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に追加されている
    if( pActor->IsRegist() ){ continue; }
    const FieldMoveModelAccessor::MoveModelData* pData = pActor->GetData();
    if( pData->eventID == eventId )
    {
      pActor->RegistActor();
      
      // アクターの状態を元にバニッシュフラグを更新
      if( isVanishUpdate )
      {
        pActor->ApplyActorToVanishFlag( pEventWork );
      }
      return i;
    }
  }

  // そのeventIDのデータは無かった。
  return -1;
}

/**
 *  @brief  ゾーンの特定のイベントIDの動作モデルを削除
 */
 s32 PlacementDataManager::UnRegistZoneEventIDModel( EventID eventId, Field::EventWork* pEventWork, b32 isVanishUpdate )
{
  for( u32 i = 0; i < MOVEMODEL_EVENT_ACTOR_MAX; ++ i )
  {
    FieldMoveModelActor *pActor = reinterpret_cast<FieldMoveModelActor *>( m_pMoveModelActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に削除されている
    if( !pActor->IsRegist() ){ continue; }
    const FieldMoveModelAccessor::MoveModelData* pData = pActor->GetData();
    if( pData->eventID == eventId )
    {
      pActor->UnregistActor();

      // アクターの状態を元にバニッシュフラグを更新
      if( isVanishUpdate )
      {
        pActor->ApplyActorToVanishFlag( pEventWork );
      }
      return i;
    }
  }

  // そのeventIDのデータは無かった。
  return -1;
}

void PlacementDataManager::UnRegistZoneModel( u32 flagwork )
{
  for( u32 i = 0; i < MOVEMODEL_EVENT_ACTOR_MAX; ++ i )
  {
    FieldMoveModelActor *pActor = reinterpret_cast<FieldMoveModelActor *>( m_pMoveModelActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に削除されている
    if( !pActor->IsRegist() ){ continue; }
    const FieldMoveModelAccessor::MoveModelData* pData = pActor->GetData();
    if( pData->flagwork == flagwork )
    {
      pActor->UnregistActor();
    }
  }
}

/**
 *  @brief  ゾーンの特定のイベントIDの動作モデルをサーチ
 */
FieldMoveModelActor* PlacementDataManager::SearchZoneEventIDModel( EventID eventId )
{
  for( u32 i = 0; i < MOVEMODEL_EVENT_ACTOR_MAX; ++ i )
  {
    FieldMoveModelActor *pActor = reinterpret_cast<FieldMoveModelActor *>( m_pMoveModelActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に削除されている
    if( !pActor->IsRegist() ){ continue; }
    const FieldMoveModelAccessor::MoveModelData* pData = pActor->GetData();
    if( pData->eventID == eventId )
    {
      return pActor;
    }
  }

  // そのeventIDのデータは無かった。
  return NULL;
}

/**
 *  @brief  ゾーンの特定のイベントIDの動作モデルアクターの登録があるか？
 *  
 *  Regist/Unregistステータスはチェックしない
 */
bool PlacementDataManager::IsEntryZoneEventIDModel( EventID eventId )
{
  for( u32 i = 0; i < MOVEMODEL_EVENT_ACTOR_MAX; ++ i )
  {
    FieldMoveModelActor *pActor = reinterpret_cast<FieldMoveModelActor *>( m_pMoveModelActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    const FieldMoveModelAccessor::MoveModelData* pData = pActor->GetData();
    if( pData->eventID == eventId )
    {
      return true;
    }
  }

  // そのeventIDのデータは無かった。
  return false;
}

/**
 *  @brief  ゾーンで配置した動作モデルを破棄する
 */
 void PlacementDataManager::DeleteZoneModel()
{
  deleteMoveModels( m_MoveModelActorAliasNum, MOVEMODEL_EVENT_ACTOR_MAX );
 }


/**
 *  @brief 動作モデルの生成：リソースとコリジョンを破棄
 */
 void PlacementDataManager::DeleteZoneModelResource()
{
  deleteMoveModelResources( m_MoveModelActorAliasNum, MOVEMODEL_EVENT_ACTOR_MAX );
}


/**
 *  @brief  動作モデル　ゾーンの物を全てUnRegistする。
 */
 void PlacementDataManager::UnRegistZoneModel()
{
  for( u32 i = m_MoveModelActorAliasNum; i < MOVEMODEL_EVENT_ACTOR_MAX; ++ i )
  {
    FieldMoveModelActor *pActor = reinterpret_cast<FieldMoveModelActor *>( m_pMoveModelActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に削除されている
    if( !pActor->IsRegist() ){ continue; }
    pActor->UnregistActor();
  }
}

/**
 *  @brief  配置モデルの生成
 */
 void PlacementDataManager::CreateZoneStaticModel( ZoneID zoneID )
{
  void * p_data = GetZonePlacementData( zoneID, PACK_IDX_STATICMODEL );
  if( p_data == NULL ){ return ; }

  // アクセサー生成
  FieldStaticModelAccessor staticModelAccessor;
  staticModelAccessor.Initialize( p_data );
  
  u32 max = staticModelAccessor.GetDataMax();

  // データサイズチェック
  u32 maxi = staticModelAccessor.GetNotAliasDataMax();
  if( (maxi + m_StaticModelActorAliasNum) > STATICMODEL_ACTOR_MAX )
  {
    GFL_ASSERT_MSG( false, "StaticModel num Over (%d/%d) zone(%d) alias(%d)", maxi +m_StaticModelActorAliasNum, STATICMODEL_ACTOR_MAX, maxi, m_StaticModelActorAliasNum);
    return;
  }


  int countup = 0;
  u32 roopNum = staticModelAccessor.GetDataMax();
  for( u32 i = 0; i < roopNum; ++ i )
  {

    const FieldStaticModelAccessor::StaticModelData *staticModelData = staticModelAccessor.GetData( i);

    // エイリアスデータでないかチェック
    if( staticModelData->alias )
    {
      continue;
    }
    
    // StaticModelActorの生成
    FieldStaticModelActor *pActor = craeteStaticModelActor( staticModelData, &staticModelAccessor );

    m_pStaticModelActor[ m_StaticModelActorAliasNum + countup ] = pActor;
    countup ++;
  }

}


/**
 *  @brief  配置モデルの破棄リクエスト
 */
 void PlacementDataManager::RequestDeleteZoneStaticModel()
{
  requestDeleteStaticModels( m_StaticModelActorAliasNum, STATICMODEL_ACTOR_MAX );
}
/**
 *  @brief  配置モデルの破棄
 */
 void PlacementDataManager::DeleteZoneStaticModel()
{
  deleteStaticModels( m_StaticModelActorAliasNum, STATICMODEL_ACTOR_MAX );
}
/**
 *  @brief  配置モデルの追加
 */
 void PlacementDataManager::RegistZoneEventIDStaticModel( EventID eventId )
{
  for( u32 i = 0; i < STATICMODEL_ACTOR_MAX; ++ i )
  {
    FieldStaticModelActor *pActor = reinterpret_cast<FieldStaticModelActor *>( m_pStaticModelActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に追加されている
    if( pActor->IsRegist() ){ continue; }
    const FieldStaticModelAccessor::StaticModelData* pData = pActor->GetData();
    if( pData->eventID == eventId )
    {
      pActor->RegistActor();
    }
  }

}

 void PlacementDataManager::RegistZoneStaticModel( u32 flagwork )
{
  for( u32 i = 0; i < STATICMODEL_ACTOR_MAX; ++ i )
  {
    FieldStaticModelActor *pActor = reinterpret_cast<FieldStaticModelActor *>( m_pStaticModelActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に追加されている
    if( pActor->IsRegist() ){ continue; }
    const FieldStaticModelAccessor::StaticModelData* pData = pActor->GetData();
    if( pData->flagwork == flagwork )
    {
      // 条件を満たしていない。
      if( !isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }

      pActor->RegistActor();
    }
  }

}

/**
 *  @brief  配置モデルの削除
 */
 void PlacementDataManager::UnRegistZoneEventIDStaticModel( EventID eventId )
{
  for( u32 i = 0; i < STATICMODEL_ACTOR_MAX; ++ i )
  {
    FieldStaticModelActor *pActor = reinterpret_cast<FieldStaticModelActor *>( m_pStaticModelActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に削除されている
    if( !pActor->IsRegist() ){ continue; }
    const FieldStaticModelAccessor::StaticModelData* pData = pActor->GetData();
    if( pData->eventID == eventId )
    {
      pActor->UnregistActor();
    }
  }
}

 void PlacementDataManager::UnRegistZoneStaticModel( u32 flagwork  )
{
  for( u32 i = 0; i < STATICMODEL_ACTOR_MAX; ++ i )
  {
    FieldStaticModelActor *pActor = reinterpret_cast<FieldStaticModelActor *>( m_pStaticModelActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に削除されている
    if( !pActor->IsRegist() ){ continue; }
    const FieldStaticModelAccessor::StaticModelData* pData = pActor->GetData();
    if( pData->flagwork == flagwork )
    {
      // 条件を満たしていない。
      if( isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }

      pActor->UnregistActor();
    }
  }
}

/**
*  @brief  配置モデルのサーチ
*/
IFieldActorBase* PlacementDataManager::SearchZoneEventIDStaticModel( EventID eventId )
{
  for( u32 i = 0; i < STATICMODEL_ACTOR_MAX; ++ i )
  {
    FieldStaticModelActor *pActor = reinterpret_cast<FieldStaticModelActor *>( m_pStaticModelActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に削除されている
    if( !pActor->IsRegist() ){ continue; }
    const FieldStaticModelAccessor::StaticModelData* pData = pActor->GetData();
    if( pData->eventID == eventId )
    {
      return pActor;
    }
  }
  return NULL;
}

/**
 *  @brief  Bgイベントの生成
 */
 void PlacementDataManager::CreateZoneBgEvent( ZoneID zoneID )
{
  void * p_data = GetZonePlacementData( zoneID, PACK_IDX_BG );
  if( p_data == NULL ){ return ; }

  // アクセサー生成
  FieldBgEventAccessor bgAccessor;
  bgAccessor.Initialize( p_data );

  // データサイズチェック
  u32 maxi = bgAccessor.GetDataMax();
  if( maxi > BG_EVENT_ACTOR_MAX )
  {
    GFL_ASSERT_MSG( false, "BgEvent num Over (%d/%d)", maxi, BG_EVENT_ACTOR_MAX );
    return;
  }

  // posイベントの生成
  for( u32 i = 0; i < maxi; ++ i )
  {
    const FieldBgEventAccessor::BgEventData *bgData = bgAccessor.GetData( i );
    gfl2::math::Vector3 pos( bgData->position[ 0 ], bgData->position[ 1 ], bgData->position[ 2 ] );
    FieldBgEventActor *pActor = GFL_NEW( m_pActorHeap ) FieldBgEventActor();
    
    FieldCollisionAccessor collisionAccessor;
    const void *p_col = bgAccessor.GetCollisionData( *bgData );
    collisionAccessor.Initialize( p_col );

    IFieldActorBase::SetupData setupData;
    setupData.pAllocator    = m_pAllocator;
    setupData.pHeap         = m_pActorHeap;
    setupData.pScene        = m_pCollisionSceneForEventTalk;
    setupData.pColAccessor  = &collisionAccessor;
    setupData.vPos          = pos;

    pActor->Initialize( setupData );
    if( isEventCreate( bgData->romVersion, bgData->flagwork, bgData->flagwork_num, RTIME_TYPE_ALL ) )
    {
      // 起動条件を満たしているものだけ追加する
      pActor->RegistActor();
    }
    pActor->SetData( bgData );
    m_pBgEventActor[ i ] = pActor;
  }
}

/**
 *  @brief  Bgイベントの破棄
 */
 void PlacementDataManager::DeleteZoneBgEvent()
{
  // POSイベントの破棄
  for( u32 i = 0; i < BG_EVENT_ACTOR_MAX; ++ i )
  {
    if( m_pBgEventActor[ i ] )
    {
      m_pBgEventActor[ i ]->Terminate();
      GFL_SAFE_DELETE( m_pBgEventActor[ i ] );
    }
  }
}

/**
 *  @brief  Bgイベントの追加
 */
 void PlacementDataManager::RegistZoneBgEvent( u32 flagwork )
{
  for( u32 i = 0; i < BG_EVENT_ACTOR_MAX; ++ i )
  {
    FieldBgEventActor* pActor = reinterpret_cast<FieldBgEventActor*>( m_pBgEventActor[ i ] );
    if( !pActor ) { continue; }
    // 既に登録済み
    if( pActor->IsRegist() ) { continue; }
    const FieldBgEventAccessor::BgEventData* pData = pActor->GetData();
    // 生成条件が一致するもののみを追加
    if( flagwork != pData->flagwork ) { continue; }
    // 条件を満たしていない
    if( !isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }

    // 追加する
    m_pBgEventActor[ i ]->RegistActor();


    // 連携するStaticModelが存在する場合がある。　そちらも設定
    RegistZoneEventIDStaticModel( pData->eventID );
  }
}

 void PlacementDataManager::RegistZoneEventIDBgEvent( EventID eventID )
{
  for( u32 i = 0; i < BG_EVENT_ACTOR_MAX; ++ i )
  {
    FieldBgEventActor* pActor = reinterpret_cast<FieldBgEventActor*>( m_pBgEventActor[ i ] );
    if( !pActor ) { continue; }
    // 既に登録済み
    if( pActor->IsRegist() ) { continue; }
    const FieldBgEventAccessor::BgEventData* pData = pActor->GetData();
    // 生成条件が一致するもののみを追加
    if( eventID != pData->eventID ) { continue; }
    // 条件を満たしていない
    if( !isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }

    // 追加する
    m_pBgEventActor[ i ]->RegistActor();


    // 連携するStaticModelが存在する場合がある。　そちらも設定
    RegistZoneEventIDStaticModel( pData->eventID );
  }
}

/**
 *  @brief  Bgイベントの削除
 */
void PlacementDataManager::UnRegistZoneBgEvent( u32 flagwork )
{
  for( u32 i = 0; i < BG_EVENT_ACTOR_MAX; ++ i )
  {
    FieldBgEventActor* pActor = reinterpret_cast<FieldBgEventActor*>( m_pBgEventActor[ i ] );
    if( !pActor ) { continue; }
    // 既に削除済み
    if( !pActor->IsRegist() ) { continue; }
    const FieldBgEventAccessor::BgEventData* pData = pActor->GetData();
    // 生成条件が一致するもののみを追加
    if( flagwork != pData->flagwork ) { continue; }
    // 条件を満たしている
    if( isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }

    // 削除する
    m_pBgEventActor[ i ]->UnregistActor();

    // 連携するStaticModelが存在する場合がある。　そちらも設定
    UnRegistZoneEventIDStaticModel( pData->eventID );
  }
}

void PlacementDataManager::UnRegistZoneEventIDBgEvent( EventID eventID )
{
  for( u32 i = 0; i < BG_EVENT_ACTOR_MAX; ++ i )
  {
    FieldBgEventActor* pActor = reinterpret_cast<FieldBgEventActor*>( m_pBgEventActor[ i ] );
    if( !pActor ) { continue; }
    // 既に削除済み
    if( !pActor->IsRegist() ) { continue; }
    const FieldBgEventAccessor::BgEventData* pData = pActor->GetData();
    // 生成条件が一致するもののみを追加
    if( eventID != pData->eventID ) { continue; }
    // 条件を満たしている
    //if( isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }

    // 削除する
    m_pBgEventActor[ i ]->UnregistActor();

    // 連携するStaticModelが存在する場合がある。　そちらも設定
    UnRegistZoneEventIDStaticModel( pData->eventID );
  }
}

/**
 *  @brief  Bgイベントの削除
 */
FieldBgEventActor* PlacementDataManager::SearchZoneBgEvent( EventID eventId )
{
  for( u32 i = 0; i < BG_EVENT_ACTOR_MAX; ++ i )
  {
    FieldBgEventActor* pActor = reinterpret_cast<FieldBgEventActor*>( m_pBgEventActor[ i ] );
    if( !pActor ) { continue; }
    // 既に削除済み
    if( !pActor->IsRegist() ) { continue; }
    const FieldBgEventAccessor::BgEventData* pData = pActor->GetData();

    if( pData->eventID == eventId )
    {
      return pActor; 
    }
  }
  return NULL;

}


/**
 *  @brief  エンカウントの生成
 */
 void PlacementDataManager::CreateZoneEncount( ZoneID zoneID )
{
  void * p_data = GetZonePlacementData( zoneID, PACK_IDX_ENCOUNT );
  if( p_data == NULL ){ return ; }

  // アクセサー生成
  FieldEncountAccessor encountAccessor;
  encountAccessor.Initialize( p_data );
  
  u32 max = encountAccessor.GetDataMax();
  for( u32 i = 0; i < max; ++ i )
  {
    FieldEncountActor *pActor = GFL_NEW( m_pActorHeap) FieldEncountActor();

    const FieldEncountAccessor::EncountData *encountData = encountAccessor.GetData( i);

    FieldEncountActor::EncountSetupData setupData;
    setupData.pAllocator          = m_pAllocator;
    setupData.pHeap               = m_pActorHeap;
    setupData.pCollisionScene     = m_pCollisionSceneForEncount;
    setupData.pAccessor           = &encountAccessor;
    setupData.pEncountData        = encountData;

    pActor->Initialize( setupData );
    pActor->RegistActor();

    m_pEncountActor[ i ] = pActor;
  }

}
/**
 *  @brief  エンカウントの破棄
 */
 void PlacementDataManager::DeleteZoneEncount()
{
  for( u32 i = 0; i < ENCOUNT_ACTOR_MAX; ++ i )
  {
    if( m_pEncountActor[ i ] )
    {
      m_pEncountActor[ i ]->Terminate();
      GFL_SAFE_DELETE( m_pEncountActor[ i ] );
    }
  }
}
/**
 *  @brief エンカウントの追加
 */
 void PlacementDataManager::RegistZoneEncount()
{
  for( u32 i = 0; i < ENCOUNT_ACTOR_MAX; ++ i )
  {
    FieldEncountActor *pActor = reinterpret_cast<FieldEncountActor *>( m_pEncountActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に追加されている
    if( pActor->IsRegist() ){ continue; }
    pActor->RegistActor();
  }

}
/**
 *  @brief  エンカウントの削除
 */
 void PlacementDataManager::UnRegistZoneEncount()
{
  for( u32 i = 0; i < ENCOUNT_ACTOR_MAX; ++ i )
  {
    FieldEncountActor *pActor = reinterpret_cast<FieldEncountActor *>( m_pEncountActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に削除されている
    if( !pActor->IsRegist() ){ continue; }
    pActor->UnregistActor();
  }

}



/**
 *   @brief トレーナーの生成：ワークを作る
 */
 void PlacementDataManager::CreateZoneTrainerWork( ZoneID zoneID )
{
  void * p_data = GetZonePlacementData( zoneID, PACK_IDX_TRAINER );
  if( p_data == NULL ){ return ; }

  // アクセサー生成
  FieldTrainerAccessor trainerAccessor;
  trainerAccessor.Initialize( p_data );
  
  // データサイズチェック
  u32 maxi = trainerAccessor.GetNotAliasDataMax();
  if( maxi + m_TrainerActorAliasNum > TRAINER_ACTOR_MAX )
  {
    GFL_ASSERT_MSG( false, "Trainer num Over (%d/%d) zone(%d) alias(%d)", maxi + m_TrainerActorAliasNum, TRAINER_ACTOR_MAX, maxi, m_TrainerActorAliasNum );
    return;
  }

  u32 countup = 0;
  u32 roopNum = trainerAccessor.GetDataMax();
  for( u32 i = 0; i < roopNum; ++ i )
  {
    const FieldTrainerAccessor::TrainerData *trainerData = trainerAccessor.GetData( i );

    // エイリアスチェック　通常の物しか下記に通さない。
    if( trainerData->alias )
    {
      continue;
    }

    FieldTrainerActor* pActor = createTrainerActor( trainerData, &trainerAccessor, zoneID, false );
    m_pTrainerActor[ countup + m_TrainerActorAliasNum ] = pActor;
    countup ++;
  }
}

/**
 *   @brief トレーナーの生成：リソースとコリジョンを設定
 */
 void PlacementDataManager::CreateZoneTrainerResource( ZoneID zoneID )
{
  void * p_data = GetZonePlacementData( zoneID, PACK_IDX_TRAINER );
  if( p_data == NULL ){ return ; }

  // アクセサー生成
  FieldTrainerAccessor trainerAccessor;
  trainerAccessor.Initialize( p_data );
  
  for( u32 i = m_TrainerActorAliasNum; i < TRAINER_ACTOR_MAX; ++ i )
  {
    FieldTrainerActor *pActor = reinterpret_cast<FieldTrainerActor *>( m_pTrainerActor[ i ] );
    if( !pActor ){ continue; }

    createTrainerActorResource( pActor, &trainerAccessor );
  }
}

/**
 *  @brief  ゾーンで配置したトレーナーを破棄する
 */
 void PlacementDataManager::DeleteZoneTrainer()
{
  deleteTrainers( m_TrainerActorAliasNum, TRAINER_ACTOR_MAX );
}

// トレーナーの生成：リソースとコリジョンを破棄
 void PlacementDataManager::DeleteZoneTrainerResource()
{
  deleteTrainerResources( m_TrainerActorAliasNum, TRAINER_ACTOR_MAX );
}

/**
 *  @brief  ゾーンの特定のイベントIDのトレーナーを追加
 */
 s32 PlacementDataManager::RegistZoneEventIDTrainer( EventID eventId )
{
  for( u32 i = 0; i < TRAINER_ACTOR_MAX; ++ i )
  {
    FieldTrainerActor *pActor = reinterpret_cast<FieldTrainerActor *>( m_pTrainerActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に追加されている
    if( pActor->IsRegist() ){ continue; }
    const FieldTrainerAccessor::TrainerData* pData = pActor->GetData();
    if( pData->eventID == eventId )
    {
      pActor->RegistActor();
      return i;
    }
  }

  // そのeventIDのデータは無かった。
  return -1;
}

void PlacementDataManager::RegistZoneTrainer( u32 flagwork )
{
  for( u32 i = 0; i < TRAINER_ACTOR_MAX; ++ i )
  {
    FieldTrainerActor *pActor = reinterpret_cast<FieldTrainerActor *>( m_pTrainerActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に追加されている
    if( pActor->IsRegist() ){ continue; }
    const FieldTrainerAccessor::TrainerData* pData = pActor->GetData();
    if( pData->flagwork == flagwork )
    {
      // 有効でない
      if( !isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ) {continue;}

      pActor->RegistActor();
    }
  }
}

/**
 *  @brief  ゾーンの特定のイベントIDのトレーナーを削除
 */
 s32 PlacementDataManager::UnRegistZoneEventIDTrainer( EventID eventId )
{
  for( u32 i = 0; i < TRAINER_ACTOR_MAX; ++ i )
  {
    FieldTrainerActor *pActor = reinterpret_cast<FieldTrainerActor *>( m_pTrainerActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に削除されている
    if( !pActor->IsRegist() ){ continue; }
    const FieldTrainerAccessor::TrainerData* pData = pActor->GetData();
    if( pData->eventID == eventId )
    {
      pActor->UnregistActor();
      return i;
    }
  }

  // そのeventIDのデータは無かった。
  return -1;
}


void PlacementDataManager::UnRegistZoneTrainer( u32 flagwork )
{
  for( u32 i = 0; i < TRAINER_ACTOR_MAX; ++ i )
  {
    FieldTrainerActor *pActor = reinterpret_cast<FieldTrainerActor *>( m_pTrainerActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に削除されている
    if( !pActor->IsRegist() ){ continue; }
    const FieldTrainerAccessor::TrainerData* pData = pActor->GetData();
    if( pData->flagwork == flagwork )
    {
      // 有効
      if( isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ) {continue;}

      pActor->UnregistActor();
    }
  }
}

/**
 *  @brief  ゾーンの特定のイベントIDのトレーナーをサーチ
 */
FieldTrainerActor* PlacementDataManager::SearchZoneEventIDTrainer( EventID eventId )
{
  for( u32 i = 0; i < TRAINER_ACTOR_MAX; ++ i )
  {
    FieldTrainerActor *pActor = reinterpret_cast<FieldTrainerActor *>( m_pTrainerActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に削除されている
    if( !pActor->IsRegist() ){ continue; }
    const FieldTrainerAccessor::TrainerData* pData = pActor->GetData();

    if( pData->eventID == eventId )
    {
      return pActor; 
    }
  }
  return NULL;
}
/**
 *  @brief  ゾーンの特定のスクリプトIDのトレーナーをサーチ
 */
FieldTrainerActor* PlacementDataManager::SearchZoneScriptIDTrainer( u32 scriptId )
{
  for( u32 i = 0; i < TRAINER_ACTOR_MAX; ++ i )
  {
    FieldTrainerActor *pActor = reinterpret_cast<FieldTrainerActor *>( m_pTrainerActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に削除されている
    if( !pActor->IsRegist() ){ continue; }
    const FieldTrainerAccessor::TrainerData* pData = pActor->GetData();

    if( pData->scriptID == scriptId )
    {
      return pActor; 
    }
  }
  return NULL;
}

/**
 *  @brief トレーナー ゾーンの物を全てUnRegistする。
 */
 void PlacementDataManager::UnRegistZoneTrainer()
{
  for( u32 i = m_TrainerActorAliasNum; i < TRAINER_ACTOR_MAX; ++ i )
  {
    FieldTrainerActor *pActor = reinterpret_cast<FieldTrainerActor *>( m_pTrainerActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に削除されている
    if( !pActor->IsRegist() ){ continue; }
    pActor->UnregistActor();
  }
}


/**
 *  @brief  エリアの環境音データ登録
 */
void PlacementDataManager::CreateAreaEnvSound( gfl2::heap::HeapBase* pHeap )
{
  // サイズが０＝データなし
  if( m_AreaPlacementData[ PACK_IDX_ENVSOUND ].GetDataSize( 0 ) == 0 ){ return; }
  void * p_data = m_AreaPlacementData[ PACK_IDX_ENVSOUND ].GetData( 0 );
  
  m_pEnvSoundManager->Initialize(pHeap, p_data);
}

/**
 *  @brief  エリアの環境音データ破棄
 */
void PlacementDataManager::DeleteAreaEnvSound()
{
  if(m_pEnvSoundManager){
    m_pEnvSoundManager->Terminate();
  }
}


/**
 *   @brief ギミックエンカウントの生成：ワークを作る
 */
 void PlacementDataManager::CreateZoneGimmickEncount( ZoneID zoneID )
{
  // ギミックエンカウントが有効化チェック
  if( !m_pGimmickEncountManager ){ return ; }

  void * p_data = GetZonePlacementData( zoneID, PACK_IDX_GIMMICK_ENCOUNT );
  if( p_data == NULL ){ return ; }

  // アクセサー生成
  FieldGimmickEncountAccessor gimmickEncountAccessor;
  gimmickEncountAccessor.Initialize( p_data );
  
  // データサイズチェック
  u32 maxi = gimmickEncountAccessor.GetDataMax();
  if( maxi > GIMMICK_ENCOUNT_ACTOR_MAX )
  {
    GFL_ASSERT_MSG( false, "GimmickEncount num Over (%d/%d)", maxi, GIMMICK_ENCOUNT_ACTOR_MAX );
    return;
  }

  GimmickEncount::DataAccessor gimmickEncountDataAccessor;
  gimmickEncountDataAccessor.SetData( m_pFieldResident->GetResource( BL_IDX_FIELD_RESIDENT__GIMMICK_ENCOUNT_PACK));

  for( u32 i = 0; i < GIMMICK_ENCOUNT_ACTOR_MAX; ++ i )
  {
    // 念のため初期化
    m_pGimmickEncountActor[ i ] = NULL;
    m_pGimmickEncountActorBase[ i ] = NULL;

    if( maxi <= i ){ continue ; }

    const FieldGimmickEncountAccessor::Data *gimmickEncountData = gimmickEncountAccessor.GetData( i );

    // フラグワークIDが有効か
    if( gimmickEncountData->flagwork != WORK_NULL)
    {
      // フラグワークの値をチェック
      if( isEventCreate( 0, gimmickEncountData->flagwork, gimmickEncountData->flagwork_num, RTIME_TYPE_ALL) == false)
      {
        continue;
      }

      // フラグワークが設定されているシンボルは抽選確率100%想定なので、
      // この後、通常抽選に流せば必ず生成される 2015.09.07 saita_kazuki
    }

    // 抽選結果を元に生成するかを決める
    bool isCreate = false;
    u16 gimmickFlagNo = FZ_GIMMICK_ENCOUNT_00 + i;
    isCreate = isEventCreate( 0, gimmickFlagNo, 0, RTIME_TYPE_ALL );

    // 待機型のみ常時表示フラグをチェックする
    bool isDrawOnly = false;
    GimmickEncount::GimmickType type = gimmickEncountDataAccessor.GetGimmickType( gimmickEncountData->presetID);
    if( type == GimmickEncount::GIMMICK_TYPE_STANDBY)
    {
      void* pData = gimmickEncountDataAccessor.GetData( gimmickEncountData->presetID);
      bool isAllwaysDraw = static_cast<bool>( reinterpret_cast<GimmickEncount::StandbyData*>( pData)->isAllwaysDraw);
      // 生成しない かつ 常時表示フラグがON なら見た目だけ生成する
      if( isCreate == false && isAllwaysDraw)
      {
        isDrawOnly =  true;
      }
    }

    if( isCreate || isDrawOnly)
    {
      GimmickEncount::GimmickType gimmickType = gimmickEncountDataAccessor.GetGimmickType( gimmickEncountData->presetID);
      this->createGimmickEncountActor( i, gimmickType, gimmickFlagNo, &gimmickEncountAccessor, gimmickEncountData, isDrawOnly, zoneID);
    }
  }
}

/**
 *   @brief ギミックエンカウントの生成：リソースとコリジョンを設定
 */
 void PlacementDataManager::CreateZoneGimmickEncountResource( ZoneID zoneID )
{
  // ギミックエンカウントが有効化チェック
  if( !m_pGimmickEncountManager ){ return ; }

  void * p_data = GetZonePlacementData( zoneID, PACK_IDX_GIMMICK_ENCOUNT );
  if( p_data == NULL ){ return ; }

  // アクセサー生成
  FieldGimmickEncountAccessor gimmickEncountAccessor;
  gimmickEncountAccessor.Initialize( p_data );

  GimmickEncount::DataAccessor gimmickEncountDataAccessor;
  gimmickEncountDataAccessor.SetData( m_pFieldResident->GetResource( BL_IDX_FIELD_RESIDENT__GIMMICK_ENCOUNT_PACK));
  
  for( u32 i = 0; i < GIMMICK_ENCOUNT_ACTOR_MAX; ++ i )
  {
    if( !m_pGimmickEncountActor[ i ] ){ continue; }

    const FieldGimmickEncountAccessor::Data *gimmickEncountData = gimmickEncountAccessor.GetData( i );
    bool isRegist = this->createGimmickEncountActorResource( i, &gimmickEncountAccessor, gimmickEncountData);

    // @fix GFNMCat[1230] ポニのげんや：メニュー項目の開閉を行うと、逃走したコソクムシが終点に再配置される
    // createGimmickEncountActorResourceの結果でRegistするかを決める
    // 追加
    if( isRegist )
    {
      m_pGimmickEncountActor[ i ]->RegistActor();
    }
  }
}

/**
 *  @brief  ゾーンで配置したギミックエンカウントを破棄する
 */
 void PlacementDataManager::DeleteZoneGimmickEncount()
{
  // ギミックエンカウントが有効化チェック
  if( !m_pGimmickEncountManager ){ return ; }

  for( u32 i = 0; i < GIMMICK_ENCOUNT_ACTOR_MAX; ++ i )
  {
    if(m_pGimmickEncountActor[ i ]){
      m_pGimmickEncountActor[ i ]->Terminate();
      GFL_SAFE_DELETE( m_pGimmickEncountActor[ i ] );
    }
  }
}

/**
 *  @brief ギミックエンカウントの生成：リソースとコリジョンを破棄
 */
 void PlacementDataManager::DeleteZoneGimmickEncountResource()
{
  // ギミックエンカウントが有効化チェック
  if( !m_pGimmickEncountManager ){ return ; }

  for( u32 i = 0; i < GIMMICK_ENCOUNT_ACTOR_MAX; ++ i )
  {
    if(m_pGimmickEncountActor[ i ])
    {
      if( m_pGimmickEncountActorBase[ i ]->GetLocalWork()->GetGimmickType() != GimmickEncount::GIMMICK_TYPE_STANDBY)
      {
        // 動作モデルを使用したギミックエンカウントはリソースのみ破棄する
        reinterpret_cast<FieldMoveModelActorBase *>(m_pGimmickEncountActor[ i ])->TerminateResource();
      }
      else
      {
        // 配置モデルはここで削除。この処理の前にRequestDeleteZoneGimmickEncountが呼ばれている前提
        reinterpret_cast<FieldStaticModelActorBase *>(m_pGimmickEncountActor[ i ])->Terminate();
        GFL_SAFE_DELETE( m_pGimmickEncountActor[ i ] );
      }
    }
  }
}

/**
 *  @brief  ギミックエンカウントの破棄リクエスト
 */
 void PlacementDataManager::RequestDeleteZoneGimmickEncount()
{
  // ギミックエンカウントが有効化チェック
  if( !m_pGimmickEncountManager ){ return ; }

  for( u32 i = 0; i < GIMMICK_ENCOUNT_ACTOR_MAX; ++ i )
  {
    if(m_pGimmickEncountActor[ i ])
    {
      // 配置モデルを使用したギミックエンカウントのみ行う処理
      if( m_pGimmickEncountActorBase[ i ]->GetLocalWork()->GetGimmickType() == GimmickEncount::GIMMICK_TYPE_STANDBY)
      {
        reinterpret_cast<FieldStaticModelActorBase *>(m_pGimmickEncountActor[ i ])->RequestDelayTerminate();
      }
    }
  }
}

/**
 *  @brief ギミックエンカウント ゾーンの物を全てUnRegistする。
 */
 void PlacementDataManager::UnRegistZoneGimmickEncount()
{
  // ギミックエンカウントが有効化チェック
  if( !m_pGimmickEncountManager ){ return ; }

  for( u32 i = 0; i < GIMMICK_ENCOUNT_ACTOR_MAX; ++ i )
  {
    // 生成されていない
    if( !m_pGimmickEncountActor[ i ] ){ continue; }
    // 既に削除されている
    if( !m_pGimmickEncountActor[ i ]->IsRegist() ){ continue; }
    m_pGimmickEncountActor[ i ]->UnregistActor();
  }
}

/**
 *  @brief  ゾーンの特定のイベントIDのギミックエンカウントをRegist
 */
 s32 PlacementDataManager::RegistZoneEventIDGimmickEncount( EventID eventId )
{
  // ギミックエンカウントが有効化チェック
  if( !m_pGimmickEncountManager ){ return -1; }

  for( u32 i = 0; i < GIMMICK_ENCOUNT_ACTOR_MAX; ++ i )
  {
    // 生成されていない
    if( !m_pGimmickEncountActor[ i ] ){ continue; }

    const FieldGimmickEncountAccessor::Data* pData = m_pGimmickEncountActorBase[ i ]->GetGimmickEncountData();
    if( pData->eventID == eventId )
    {
      if( m_pGimmickEncountActor[ i ]->IsRegist() ){ continue; }
      m_pGimmickEncountActor[ i ]->RegistActor();
    }
  }

  // そのeventIDのデータは無かった。
  return -1;
}

/**
 *  @brief  ゾーンの特定のイベントIDのギミックエンカウントをUnRegist
 */
 s32 PlacementDataManager::UnRegistZoneEventIDGimmickEncount( EventID eventId )
{
  // ギミックエンカウントが有効化チェック
  if( !m_pGimmickEncountManager ){ return -1; }

  for( u32 i = 0; i < GIMMICK_ENCOUNT_ACTOR_MAX; ++ i )
  {
    // 生成されていない
    if( !m_pGimmickEncountActor[ i ] ){ continue; }

    const FieldGimmickEncountAccessor::Data* pData = m_pGimmickEncountActorBase[ i ]->GetGimmickEncountData();
    if( pData->eventID == eventId )
    {
      if( !m_pGimmickEncountActor[ i ]->IsRegist() ){ continue; }
      m_pGimmickEncountActor[ i ]->UnregistActor();
    }
  }

  // そのeventIDのデータは無かった。
  return -1;
}

/**
 *  @brief  ゾーンの特定のイベントIDのギミックエンカウントを削除
 */
 s32 PlacementDataManager::DeleteZoneEventIDGimmickEncount( EventID eventId )
{
  // ギミックエンカウントが有効化チェック
  if( !m_pGimmickEncountManager ){ return -1; }

  for( u32 i = 0; i < GIMMICK_ENCOUNT_ACTOR_MAX; ++ i )
  {
    // 生成されていない
    if( !m_pGimmickEncountActor[ i ] ){ continue; }

    const FieldGimmickEncountAccessor::Data* pData = m_pGimmickEncountActorBase[ i ]->GetGimmickEncountData();
    if( pData->eventID == eventId )
    {
      if( m_pGimmickEncountActor[ i ] )
      {
        m_pGimmickEncountActor[ i ]->Terminate();
        GFL_SAFE_DELETE( m_pGimmickEncountActor[ i ] );
      }
    }
  }

  // そのeventIDのデータは無かった。
  return -1;
}


/**
 *  @brief  配置アイテムの生成
 */
 void PlacementDataManager::CreateZoneItem( ZoneID zoneID )
{
  void * p_data = GetZonePlacementData( zoneID, PACK_IDX_PLACEMENT_ITEM );
  if( p_data == NULL ){ return ; }

  // アクセサー生成
  FieldItemAccessor Accessor;
  Accessor.Initialize( p_data );

  // データサイズチェック
  u32 maxi = Accessor.GetDataMax();
  if( maxi > ITEM_ACTOR_MAX )
  {
    GFL_ASSERT_MSG( false, "Field item num Over (%d/%d)", maxi, ITEM_ACTOR_MAX );
    return;
  }

  // posイベントの生成
  for( u32 i = 0; i < maxi; ++ i )
  {
    const FieldItemAccessor::FieldItemData *pData = Accessor.GetData( i );
    gfl2::math::Vector3 pos( pData->position[ 0 ], pData->position[ 1 ], pData->position[ 2 ] );
    FieldItemActor *pActor = GFL_NEW( m_pActorHeap ) FieldItemActor();
    
    FieldCollisionAccessor collisionAccessor;
    const void *p_col = Accessor.GetCollisionData( *pData );
    collisionAccessor.Initialize( p_col );

    IFieldActorBase::SetupData setupData;
    setupData.pAllocator    = m_pAllocator;
    setupData.pHeap         = m_pActorHeap;
    setupData.pScene        = m_pCollisionSceneForEventTalk;
    setupData.pColAccessor  = &collisionAccessor;
    setupData.vPos          = pos;

    pActor->Initialize( setupData );
    if( isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) )
    {
      // 起動条件を満たしているものだけ追加する
      pActor->RegistActor();
    }
    pActor->SetData( pData );
    m_pItemActor[ i ] = pActor;
  }
}

/**
 *  @brief  配置アイテムの破棄
 */
 void PlacementDataManager::DeleteZoneItem()
{
  // POSイベントの破棄
  for( u32 i = 0; i < ITEM_ACTOR_MAX; ++ i )
  {
    if( m_pItemActor[ i ] )
    {
      m_pItemActor[ i ]->Terminate();
      GFL_SAFE_DELETE( m_pItemActor[ i ] );
    }
  }
}

/**
 *  @brief  配置アイテムの追加
 */
 void PlacementDataManager::RegistZoneEventIDItem( EventID eventID )
{
  for( u32 i = 0; i < ITEM_ACTOR_MAX; ++ i )
  {
    FieldItemActor* pActor = reinterpret_cast<FieldItemActor*>( m_pItemActor[ i ] );
    if( !pActor ) { continue; }
    // 既に登録済み
    if( pActor->IsRegist() ) { continue; }
    const FieldItemAccessor::FieldItemData* pData = pActor->GetData();
    // 生成条件が一致するもののみを追加
    if( eventID != pData->eventID ) { continue; }
    // 条件を満たしていない
    if( !isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }

    // 追加する
    m_pItemActor[ i ]->RegistActor();

    // StaticModelも追加
    RegistZoneEventIDStaticModel( pData->eventID );
  }
}

void PlacementDataManager::RegistZoneItem( u32 flagwork )
{
  for( u32 i = 0; i < ITEM_ACTOR_MAX; ++ i )
  {
    FieldItemActor* pActor = reinterpret_cast<FieldItemActor*>( m_pItemActor[ i ] );
    if( !pActor ) { continue; }
    // 既に登録済み
    if( pActor->IsRegist() ) { continue; }
    const FieldItemAccessor::FieldItemData* pData = pActor->GetData();
    // 生成条件が一致するもののみを追加
    if( flagwork != pData->flagwork ) { continue; }
    // 条件を満たしていない
    if( !isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }

    // 追加する
    m_pItemActor[ i ]->RegistActor();

    // StaticModelも追加
    RegistZoneEventIDStaticModel( pData->eventID );
  }
}

/**
 *  @brief  配置アイテムの削除
 */
 void PlacementDataManager::UnRegistZoneEventIDItem( EventID eventID )
{
  for( u32 i = 0; i < ITEM_ACTOR_MAX; ++ i )
  {
    FieldItemActor* pActor = reinterpret_cast<FieldItemActor*>( m_pItemActor[ i ] );
    if( !pActor ) { continue; }
    // 既に削除済み
    if( !pActor->IsRegist() ) { continue; }
    const FieldItemAccessor::FieldItemData* pData = pActor->GetData();
    // 生成条件が一致するもののみを追加
    if( eventID != pData->eventID ) { continue; }
    // 条件を満たしていない。
    //if( !isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) )
    {
      // 削除する
      pActor->UnregistActor();
      // StaticModelも削除
      UnRegistZoneEventIDStaticModel( pData->eventID );
    }
  }
}

void PlacementDataManager::UnRegistZoneItem( u32 flagwork )
{
  for( u32 i = 0; i < ITEM_ACTOR_MAX; ++ i )
  {
    FieldItemActor* pActor = reinterpret_cast<FieldItemActor*>( m_pItemActor[ i ] );
    if( !pActor ) { continue; }
    // 既に削除済み
    if( !pActor->IsRegist() ) { continue; }
    const FieldItemAccessor::FieldItemData* pData = pActor->GetData();
    // 生成条件が一致するもののみを追加
    if( flagwork != pData->flagwork ) { continue; }
    // 条件を満たしていない。
    if( !isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) )
    {
      // 削除する
      pActor->UnregistActor();
      // StaticModelも削除
      UnRegistZoneEventIDStaticModel( pData->eventID );
    }
  }
}

/**
 *  @brief  配置アイテムのサーチ
 */
IFieldActorBase* PlacementDataManager::SearchZoneItem( EventID eventID )
{
  for( u32 i = 0; i < ITEM_ACTOR_MAX; ++ i )
  {
    FieldItemActor* pActor = reinterpret_cast<FieldItemActor*>( m_pItemActor[ i ] );
    if( !pActor ) { continue; }
    // 既に削除済み
    if( !pActor->IsRegist() ) { continue; }
    const FieldItemAccessor::FieldItemData* pData = pActor->GetData();
    if( pData->eventID == eventID )
    {
      return pActor; 
    }
  }
  return NULL;
}

/**
 *  @brief  ドロップアイテムの生成
 */
 void PlacementDataManager::CreateDropItem( void )
{
  FieldItemManager* pItemManager = m_pItemManager;
  if( pItemManager == NULL ){ return ; }

  void * p_data = pItemManager->GetDropItemData();
  if( p_data == NULL ){ return ; }

  // アクセサー生成
  FieldItemAccessor Accessor;
  // ドロップアイテムをゲームデータから生成
  Accessor.Initialize( p_data );

  for( u32 i = 0; i < FieldItemManager::MAX_DROP_ITEM; i++ )
  {
    const FieldItemAccessor::FieldItemData *pData = Accessor.GetData( i );
    if( pData == NULL ) continue;
    if( pData->eventID == 0 ) continue;

    gfl2::math::Vector3 pos( pData->position[ 0 ], pData->position[ 1 ], pData->position[ 2 ] );
    FieldItemActor *pActor = GFL_NEW( m_pActorHeap ) FieldItemActor();
    
    FieldCollisionAccessor collisionAccessor;
    const void *p_col = Accessor.GetCollisionData( *pData );
    collisionAccessor.Initialize( p_col );

    IFieldActorBase::SetupData setupData;
    setupData.pAllocator    = m_pAllocator;
    setupData.pHeap         = m_pActorHeap;
    setupData.pScene        = m_pCollisionSceneForEventTalk;
    setupData.pColAccessor  = &collisionAccessor;
    setupData.vPos          = pos;

    pActor->Initialize( setupData );
    if( isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) )
    {
      // 起動条件を満たしているものだけ追加する
      pActor->RegistActor();
    }
    pActor->SetData( pData );
    m_pDropItemActor[ i ] = pActor;


    // ドロップアイテムは見た目も追加する
    // @fix GFNMCat[957] ドロップアイテムのEVENTIDがスクリプト動的生成NPCのEVENTIDとかぶる
    if( (pData->eventID >= FieldItemManager::DROP_ITEM_EVENT_ID_OFFSET) && (pData->eventID < FieldItemManager::DROP_ITEM_EVENT_ID_OFFSET + FieldItemManager::MAX_DROP_ITEM) )
    {
      FieldStaticModelAccessor ModelAccessor;
      ModelAccessor.Initialize( pItemManager->GetDropItemModelData() );
      const void* p_model = ModelAccessor.GetData( i );

      {
        FieldStaticModelActor *pActor = GFL_NEW( m_pActorHeap) FieldStaticModelActor();

        const FieldStaticModelAccessor::StaticModelData *staticModelData = ModelAccessor.GetData( i );

        gfl2::math::Vector3 pos( staticModelData->position[ 0 ], staticModelData->position[ 1 ], staticModelData->position[ 2 ] );
        gfl2::math::Quaternion qt( staticModelData->quaternion[ 0 ], staticModelData->quaternion[ 1 ], staticModelData->quaternion[ 2 ], staticModelData->quaternion[ 3 ] );

        FieldStaticModelActor::StaticModelSetupData setupData;
        setupData.pStaticModelManager = m_pStaticModelManager;
        setupData.pAccessor           = &ModelAccessor;
        setupData.pCollisionScene     = m_pCollisionSceneForStaticModel;
        setupData.pData               = staticModelData;
        setupData.pHeap               = m_pActorHeap;
        setupData.vPos                = pos;
        setupData.Quaternion          = qt;
        setupData.pTerrainManager     = m_pTerrainManager;
        setupData.pRenderingPipeLine  = m_pRenderingPipeLine;

        // @fix NMCat[3869] ドロップアイテムは動的配置モデル用の初期化をする
        pActor->InitializeForDynamic( setupData );
        // 追加
        if( isEventCreate( staticModelData->romVersion, staticModelData->flagwork, staticModelData->flagwork_num, RTIME_TYPE_ALL ) )
        {
          pActor->RegistActor();
        }
        pItemManager->SetDropItemModelActor( pActor, i );
      }
    }
    else
    {
      GFL_ASSERT(0);  // ドロップアイテムの生成にドロップアイテム以外が来ている。
    }

  }
}

/**
 *  @brief  ドロップアイテムの破棄リクエスト
 */
 void PlacementDataManager::RequestDeleteDropItem()
{
  FieldItemManager* pItemManager = m_pItemManager;
  if( pItemManager == NULL )
  {
    return ;
  }

  for( u32 i = 0; i < FieldItemManager::MAX_DROP_ITEM; ++ i )
  {
    FieldStaticModelActor *pActor = pItemManager->GetDropItemModelActor( i );
    // 生成されていない
    if( !pActor ){ continue; }
    pActor->RequestDelayTerminate();
  }
}

/**
 *  @brief  ドロップアイテムの破棄リクエスト
 */
 void PlacementDataManager::RequestDeleteDropItem( EventID eventID )
{
  FieldItemManager* pItemManager = m_pItemManager;

  if( pItemManager == NULL )
  {
    return ;
  }

  for( u32 i = 0; i < FieldItemManager::MAX_DROP_ITEM; ++ i )
  {
    FieldStaticModelActor *pActor = pItemManager->GetDropItemModelActor( i );
    // 生成されていない
    if( !pActor ){ continue; }
    if( pActor->GetData()->eventID != eventID ){ continue; }
    pActor->RequestDelayTerminate();
    break;
  }
}

/**
 *  @brief  ドロップアイテムの破棄
 */
 void PlacementDataManager::DeleteDropItem()
{
  FieldItemManager* pItemManager = m_pItemManager;
  if( pItemManager == NULL )
  {
    return ;
  }

  for( u32 i = 0; i < FieldItemManager::MAX_DROP_ITEM; i++ )
  {
    if( m_pDropItemActor[ i ] ){ m_pDropItemActor[ i ]->Terminate(); }
    GFL_SAFE_DELETE( m_pDropItemActor[ i ] );
    FieldStaticModelActor *pActor = pItemManager->GetDropItemModelActor( i );
    if( pActor ){ pActor->Terminate(); }
    GFL_SAFE_DELETE( pActor );
    pItemManager->SetDropItemModelActor( NULL, i );
  }
}

/**
 *  @brief  配置アイテムの削除
 */
 void PlacementDataManager::DeleteDropItem( EventID eventID )
{
  FieldItemManager* pItemManager = m_pItemManager;

  if( pItemManager == NULL )
  {
    return ;
  }

  // アクセサー生成
  FieldItemAccessor Accessor;
  Accessor.Initialize( pItemManager->GetDropItemData() );

  for( u32 i = 0; i < FieldItemManager::MAX_DROP_ITEM; i++ )
  {
    const FieldItemAccessor::FieldItemData *pData = Accessor.GetData( i );
    if( pData == NULL ) continue;
    if( pData->eventID == eventID )
    {
      if( m_pDropItemActor[ i ] ){ m_pDropItemActor[ i ]->Terminate(); }
      GFL_SAFE_DELETE( m_pDropItemActor[ i ] );
      FieldStaticModelActor *pActor = pItemManager->GetDropItemModelActor( i );
      if( pActor ) { pActor->Terminate(); }
      GFL_SAFE_DELETE( pActor );
      pItemManager->SetDropItemModelActor( NULL, i );
    }
  }
}


/**
 *  @brief  ドロップアイテムの追加
 */
 bool PlacementDataManager::RegistDropItem( u32 itemID, u32 num, gfl2::math::Vector3 pos, FieldItemManager::Type type )
{
  FieldItemManager* pItemManager = m_pItemManager;

  if( pItemManager == NULL )
  {
    return false;
  }

  u32 eventID;
  u32 index = pItemManager->CreateDropItemData( pos, itemID, type, &eventID );

  if( index == FieldItemManager::MAX_DROP_ITEM )  // 登録できず
  {
    return false;
  }

  // アクセサー生成
  FieldItemAccessor Accessor;
  Accessor.Initialize( pItemManager->GetDropItemData() );

  const FieldItemAccessor::FieldItemData *pData = Accessor.GetData( index );
  if( pData->eventID == eventID )
  {

    FieldItemActor *pActor = GFL_NEW( m_pActorHeap ) FieldItemActor();
    
    FieldCollisionAccessor collisionAccessor;
    const void *p_col = Accessor.GetCollisionData( *pData );
    collisionAccessor.Initialize( p_col );

    IFieldActorBase::SetupData setupData;
    setupData.pAllocator    = m_pAllocator;
    setupData.pHeap         = m_pActorHeap;
    setupData.pScene        = m_pCollisionSceneForEventTalk;
    setupData.pColAccessor  = &collisionAccessor;
    setupData.vPos          = pos;

    pActor->Initialize( setupData );

    // 起動条件を満たしているものだけ追加する
    pActor->RegistActor();
    pActor->SetData( pData );
    m_pDropItemActor[ index ] = pActor;

    // ドロップアイテムは見た目も追加する
    // @fix GFNMCat[957] ドロップアイテムのEVENTIDがスクリプト動的生成NPCのEVENTIDとかぶる
    if( (pData->eventID >= FieldItemManager::DROP_ITEM_EVENT_ID_OFFSET) && (pData->eventID < FieldItemManager::DROP_ITEM_EVENT_ID_OFFSET + FieldItemManager::MAX_DROP_ITEM) )
    {
      FieldStaticModelAccessor ModelAccessor;
      ModelAccessor.Initialize( pItemManager->GetDropItemModelData() );
      const void* p_model = ModelAccessor.GetData( index );

      {
        FieldStaticModelActor *pActor = GFL_NEW( m_pActorHeap) FieldStaticModelActor();

        const FieldStaticModelAccessor::StaticModelData *staticModelData = ModelAccessor.GetData( index );

        gfl2::math::Vector3 pos( staticModelData->position[ 0 ], staticModelData->position[ 1 ], staticModelData->position[ 2 ] );
        gfl2::math::Quaternion qt( staticModelData->quaternion[ 0 ], staticModelData->quaternion[ 1 ], staticModelData->quaternion[ 2 ], staticModelData->quaternion[ 3 ] );

        FieldStaticModelActor::StaticModelSetupData setupData;
        setupData.pStaticModelManager = m_pStaticModelManager;
        setupData.pAccessor           = &ModelAccessor;
        setupData.pCollisionScene     = m_pCollisionSceneForStaticModel;
        setupData.pData               = staticModelData;
        setupData.pHeap               = m_pActorHeap;
        setupData.vPos                = pos;
        setupData.Quaternion          = qt;
        setupData.pTerrainManager     = m_pTerrainManager;
        setupData.pRenderingPipeLine  = m_pRenderingPipeLine;

        pActor->InitializeForDynamic( setupData );
        pActor->RegistActor();
        pItemManager->SetDropItemModelActor( pActor, index );
      }
    }

    return true;
  }

  return false;
}


/**
 *  @brief  配置アイテムの非表示化
 */
 void PlacementDataManager::UnRegistDropItem( EventID eventID )
{
  FieldItemManager* pItemManager = m_pItemManager;

  if( pItemManager == NULL )
  {
    return ;
  }

  // アクセサー生成
  FieldItemAccessor Accessor;
  Accessor.Initialize( pItemManager->GetDropItemData() );

  for( u32 i = 0; i < FieldItemManager::MAX_DROP_ITEM; i++ )
  {
    const FieldItemAccessor::FieldItemData *pData = Accessor.GetData( i );
    if( pData == NULL ) continue;
    if( pData->eventID == eventID )
    { 
      pItemManager->UnregistDropItemModelActor( i );
      m_pDropItemActor[ i ]->UnregistActor();
    }
  }
}

/**
 *  @brief  ファインダーの生成
 */
 void PlacementDataManager::CreateZonePokeFinder( ZoneID zoneID )
{
  void *p_data = GetZonePlacementData( zoneID, PACK_IDX_FINDER_SPOT );
  if( !p_data ){ return; }

  FieldPokeFinderAccessor accessor;
  accessor.Initialize( p_data );
  // データサイズチェック
  u32 maxi = accessor.GetDataMax();
  if( maxi > POKEFINDER_MAX )
  {
    GFL_ASSERT_MSG( false, "PokeFinder num over %d/%d", maxi, POKEFINDER_MAX );
    return;
  }

  for( u32 i = 0; i < maxi ; ++ i )
  {
    const FieldPokeFinderAccessor::PokeFinderData *data = accessor.GetData( i );
    FieldPokeFinderActor::PokeFinderSetupData setupData;
    setupData.pCollisionSceneForEventTalk   = m_pCollisionSceneForEventFinder;
    setupData.pAllocator                    = m_pAllocator;
    setupData.pHeap                         = m_pActorHeap;
    setupData.pAccessor                     = &accessor;
    setupData.pData                         = data;

    FieldPokeFinderActor *pActor = GFL_NEW( m_pActorHeap ) FieldPokeFinderActor();
    pActor->Initialize( setupData );
    // 起動条件を満たしているものだけ追加する
    if( isEventCreate( 0, data->flagwork, data->flagwork_num, RTIME_TYPE_ALL ) )
    {
      pActor->RegistActor();
    }
    m_pPokeFinderActor[ i ] = pActor;
  }
}

/**
 *  @brief  ファインダーの破棄
 */
 void PlacementDataManager::DeleteZonePokeFinder( void )
{
  for( u32 i = 0; i < POKEFINDER_MAX; ++ i )
  {
    if( !m_pPokeFinderActor[ i ] ){ continue; }

    m_pPokeFinderActor[ i ]->Terminate();
    GFL_SAFE_DELETE( m_pPokeFinderActor[ i ] );
  }
}
/**
 *  @brief  ファインダーの追加
 */
 void PlacementDataManager::RegistZonePokeFinder( u32 flagwork )
{
  for( u32 i = 0; i < POKEFINDER_MAX; ++ i )
  {
    if( !m_pPokeFinderActor[ i ] ){ continue; }
    if( m_pPokeFinderActor[ i ]->IsRegist() ){ continue; }

    const FieldPokeFinderAccessor::PokeFinderData *pData = m_pPokeFinderActor[ i ]->GetData();
    // 生成条件が一致するもののみ追加
    if( flagwork != pData->flagwork ){ continue; }
    // 条件を満たしていない
    if( !isEventCreate( 0, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }
    // 追加する
    m_pPokeFinderActor[ i ]->RegistActor();
  }
}

/**
 *  @brief  ファインダーの削除
 */
 void PlacementDataManager::UnRegistZonePokeFinder( u32 flagwork )
{
  for( u32 i = 0; i < POKEFINDER_MAX; ++ i )
  {
    if( !m_pPokeFinderActor[ i ] ){ continue; }
    if( !m_pPokeFinderActor[ i ]->IsRegist() ){ continue; }

    const FieldPokeFinderAccessor::PokeFinderData *pData = m_pPokeFinderActor[ i ]->GetData();
    // 生成条件が一致するもののみ追加
    if( flagwork != pData->flagwork ){ continue; }
    // 条件を満たしている
    if( isEventCreate( 0, pData->flagwork, pData->flagwork_num, RTIME_TYPE_ALL ) ){ continue; }
    // 削除する
    m_pPokeFinderActor[ i ]->UnregistActor();
  }
}

/**
 *  @brief  ファインダー：サーチ
 */
FieldPokeFinderActor *PlacementDataManager::SearchZonePokeFinder( u32 spotID )
{
  for( u32 i = 0; i < POKEFINDER_MAX; ++ i )
  {
    FieldPokeFinderActor *pActor = this->m_pPokeFinderActor[i];
    if( !pActor ){ continue; }
    if( pActor->GetSpotID() == spotID )
    {
      return pActor;
    }
  }

  return NULL;
}
FieldPokeFinderActor *PlacementDataManager::SearchZonePokeFinderFromIndex( u32 idx )
{
  if( idx < POKEFINDER_MAX )
  {
    return m_pPokeFinderActor[ idx ];
  }

  return NULL;
}
FieldPokeFinderActor *PlacementDataManager::SearchZonePokeFinderFromSubjectID( u32 idx )
{
  for( u32 i = 0; i < POKEFINDER_MAX; ++ i )
  {
    FieldPokeFinderActor *pActor = this->m_pPokeFinderActor[i];
    if( !pActor ){ continue; }
    if( pActor->GetSubjectID() == idx )
    {
      return pActor;
    }
  }

  return NULL;
}

/**
 * @brief きのみ : ワーク生成
 */
 void PlacementDataManager::CreateZoneBerryPointWork(ZoneID zoneID)
{
  void* p_data = this->GetZonePlacementData( zoneID, PACK_IDX_BERRY_POINT );
  if( p_data == NULL )
  {
    return;
  }

  // アクセサー生成
  FieldBerryPointAccessor accessor;
  accessor.Initialize( p_data );

  // データサイズチェック
  u32 dataAmountMax = accessor.GetDataMax();
  if( dataAmountMax > BERRY_POINT_ACTOR_MAX )
  {
    GFL_ASSERT_MSG( false, "Field BerryPoint Amount Over (%d/%d)\n", dataAmountMax, BERRY_POINT_ACTOR_MAX );
    return;
  }

  FieldBerryManager* pBerryManager = m_pBerryManager;

  if( pBerryManager == NULL )
  {
    return ;
  }

  // きのみアクターの生成
  for( u32 i = 0; i < dataAmountMax; ++ i )
  {
    const FieldBerryPointAccessor::BerryPointData* pData = accessor.GetData(i);

    // 配置するかチェック
    if (pBerryManager->ExistBerry(pData) == false)
    {
      continue;
    }

    FieldBerryPointActor* pActor = GFL_NEW( this->m_pActorHeap ) FieldBerryPointActor();

    FieldBerryPointActor::SetupDesc desc;
    desc.pAllocator               = this->m_pAllocator;
    desc.pHeap                    = this->m_pActorHeap;
    desc.pTalkCollisionScene      = this->m_pCollisionSceneForEventTalk;
    desc.pForbiddenCollisionScene = this->m_pCollisionSceneForStaticModel;

    pActor->Initialize(pData, pBerryManager, desc);

    this->m_pBerryPointActor[i] = pActor;
  }
}

/**
 * @brief きのみ : ワーク破棄
 */
 void PlacementDataManager::DeleteZoneBerryPointWork()
{
  for (u32 i = 0; i < BERRY_POINT_ACTOR_MAX; ++i)
  {
    if( m_pBerryPointActor[i] == NULL)
    {
      continue;
    }

    m_pBerryPointActor[i]->Terminate();
    GFL_SAFE_DELETE(m_pBerryPointActor[i]);
  }
}

/**
 * @brief きのみ : リソース生成
 *
 * 1) 配置モデルアクターを生成し、きのみアクターに紐付ける 
 * 2) きのみアクターをシーンに追加
 */
 void PlacementDataManager::CreateZoneBerryPointResource(ZoneID zoneID)
{
  for( u32 i = 0; i < BERRY_POINT_ACTOR_MAX; ++ i )
  {
    FieldBerryPointActor* pActor = this->m_pBerryPointActor[i];
    if (pActor == NULL)
    {
      continue;
    }

    // 1) 配置モデルの生成
    {
      FieldStaticModelActor::StaticModelSetupData setupData;
      setupData.pStaticModelManager = this->m_pStaticModelManager;
      setupData.pAccessor           = NULL;
      setupData.pCollisionScene     = NULL;
      setupData.pHeap               = this->m_pActorHeap;
      setupData.pTerrainManager     = this->m_pTerrainManager;
      setupData.pRenderingPipeLine  = this->m_pRenderingPipeLine;
      setupData.pAllocator          = this->m_pAllocator;

      pActor->SetupStaticModel(setupData);
    }

    // 2) きのみアクターをシーンに追加
    pActor->RegistActor(); 
  }
}

/**
 * @brief きのみ : 破棄リクエスト
 */
 void PlacementDataManager::RequestDeleteZoneBerryPoint()
{
  for( u32 i = 0; i < BERRY_POINT_ACTOR_MAX; ++ i )
  {
    FieldBerryPointActor* pActor = this->m_pBerryPointActor[i];
    if (pActor == NULL)
    {
      continue;
    }
    pActor->RequestDelayTerminate();
  }
}

/**
 * @brief きのみ : リソース破棄
 */
 void PlacementDataManager::DeleteZoneBerryPointResource()
{
  this->DeleteZoneBerryPointWork();
}

/**
 * @brief きのみ : アクターの除去(破棄はしない)
 */
 void PlacementDataManager::UnRegisterZoneBerryPoint(u32 beryyPointID)
{
  for( u32 i = 0; i < BERRY_POINT_ACTOR_MAX; ++ i )
  {
    FieldBerryPointActor* pActor = this->m_pBerryPointActor[i];
    if (pActor == NULL)
    {
      continue;
    }

    if( beryyPointID != pActor->GetBerryPointID())
    {
      continue;
    }

    pActor->UnregistActor();
    break;
  }
}

void PlacementDataManager::UnRegisterZoneBerryPoint()
{
  for( u32 i = 0; i < BERRY_POINT_ACTOR_MAX; ++ i )
  {
    FieldBerryPointActor* pActor = this->m_pBerryPointActor[i];
    if (pActor == NULL)
    {
      continue;
    }

    // 既に削除されている
    if( pActor->IsRegist() == false )
    {
      continue;
    }

    pActor->UnregistActor();
    break;
  }
}



/**
 * @brief 釣りスポット : ワーク生成
 */
void PlacementDataManager::CreateZoneFishingSpotWork(ZoneID zoneID)
{
  void* p_data = this->GetZonePlacementData( zoneID, PACK_IDX_FISHING_SPOT );
  if( p_data == NULL )
  {
    return;
  }

  FieldFishingSpotAccessor accessor;
  accessor.Initialize(p_data);

  Savedata::FishingSpot* pSaveData = m_pFishingSpot;

  // セーブデータがなければ生成しない。
  if( pSaveData == NULL )
  {
    return;
  }

  u32 dataAmount = accessor.GetDataMax();

  if( dataAmount > FISHING_SPOT_ACTOR_MAX )
  {
    GFL_ASSERT_MSG( false, "Field FishingSpot Amount Over (%d/%d)\n", dataAmount, FISHING_SPOT_ACTOR_MAX );
    return;
  }

  u32 rarePercentages[FISHING_SPOT_ACTOR_MAX] ={};

  // アクターの生成
  for( u32 i = 0; i < dataAmount; ++ i )
  {
    const FieldFishingSpotAccessor::Data* pData = accessor.GetData(i);

    FieldFishingSpotActor* pActor = GFL_NEW( this->m_pActorHeap ) FieldFishingSpotActor();

    FieldFishingSpotActor::CollisionSetupData watchingArea;
    watchingArea.pCollisionData = accessor.GetCollisionData(pData, FieldFishingSpotAccessor::Watching);
    watchingArea.pScene = this->m_pCollisionSceneForFishingPos;

    FieldFishingSpotActor::CollisionSetupData actionArea;
    actionArea.pCollisionData = accessor.GetCollisionData(pData, FieldFishingSpotAccessor::Action);
    actionArea.pScene = this->m_pCollisionSceneForEventTalk;

    pActor->Initialize(i, pData, watchingArea, actionArea, this->m_pAllocator, this->m_pActorHeap);

    this->m_pFishingSpotActor[i] = pActor;

    rarePercentages[i] = pData->rarePercentage;
  }

  // レア抽選
  if (pSaveData->NeedLottery())
  {
    pSaveData->LotRare(rarePercentages, dataAmount);
  }

}

/**
 * @brief 釣りスポット : リソース生成
 *
 * 1) 配置モデルアクターの生成
 * 2) 釣りスポットアクターをシーンに登録
 */
void PlacementDataManager::CreateZoneFishingSpotResource(ZoneID zoneID)
{
  for (u32 i = 0; i < FISHING_SPOT_ACTOR_MAX; ++i)
  {
    FieldFishingSpotActor* pActor = this->m_pFishingSpotActor[i];

    if (pActor == NULL)    
    {
      continue;
    }
    // 1) 配置モデルの生成
    {
      FieldStaticModelActor::StaticModelSetupData setupData;
      setupData.pStaticModelManager = this->m_pStaticModelManager;
      setupData.pAccessor           = NULL;
      setupData.pCollisionScene     = NULL;
      setupData.pHeap               = this->m_pActorHeap;
      setupData.pTerrainManager     = this->m_pTerrainManager;
      setupData.pRenderingPipeLine  = this->m_pRenderingPipeLine;
      setupData.pAllocator          = this->m_pAllocator;

      pActor->SetupStaticModel(setupData);
    }

    // 「つりざお」を持っていなければ登録をしない
    const Savedata::MyItem* pMyItem = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMyItem();
    if( pMyItem->Check( ITEM_TURIZAO, 1) == false)
    {
      continue;
    }

    // フラグワークチェック
    if( isEventCreate(PLACEMENT_ROM_VERSION_NONE, pActor->GetData()->flagwork, pActor->GetData()->flagWorkValue, RTIME_TYPE_ALL ))
    {
      // 2) 釣りスポットアクターをシーンに登録
      pActor->RegistActor();
    }
  }
}

/**
 * @brief 釣りスポット : ワーク破棄
 */
void PlacementDataManager::DeleteZoneFishingSpotWork()
{
  for (u32 i = 0; i < FISHING_SPOT_ACTOR_MAX; ++i)
  {
    if (this->m_pFishingSpotActor[i] == NULL)
    {
      continue;
    }
    this->m_pFishingSpotActor[i]->Terminate();
    GFL_SAFE_DELETE(this->m_pFishingSpotActor[i]);
  }
}

/**
 * @brief 釣りスポット : リソース破棄
 */
void PlacementDataManager::DeleteZoneFishingSpotResource()
{
  this->DeleteZoneFishingSpotWork();
}

/**
 * @brief 釣りスポット : 破棄リクエスト
 */
void PlacementDataManager::RequestDeleteZoneFishingSpot()
{
  for( u32 i = 0; i < FISHING_SPOT_ACTOR_MAX; ++ i )
  {
    FieldFishingSpotActor* pActor = this->m_pFishingSpotActor[i];
    if (pActor == NULL)
    {
      continue;
    }
    pActor->RequestDelayTerminate();
  }
}

// 釣りスポット : シーンへの登録
void PlacementDataManager::RegisterZoneFishingSpot(u32 eventID)
{
  for( u32 i = 0; i < FISHING_SPOT_ACTOR_MAX; ++ i )
  {
    FieldFishingSpotActor* pActor = this->m_pFishingSpotActor[i];

    // 生成されていない
    if(pActor == NULL)
    {
      continue;
    }

    if(pActor->GetData()->eventID == eventID)
    {
      GFL_ASSERT_MSG(pActor->IsRegist() == false, "Already Registered");

      pActor->RegistActor();
    }

  }
}

// 釣りスポット : シーンからの登録解除
void PlacementDataManager::UnregisterZoneFishingSpot(u32 eventID)
{
  for( u32 i = 0; i < FISHING_SPOT_ACTOR_MAX; ++ i )
  {
    FieldFishingSpotActor* pActor = this->m_pFishingSpotActor[i];

    // 生成されていない
    if(pActor == NULL)
    {
      continue;
    }

    if(pActor->GetData()->eventID == eventID)
    {
      GFL_ASSERT_MSG(pActor->IsRegist(), "Already Unregistered");

      pActor->UnregistActor();
    }

  }  
}

// 釣りスポット : シーンからの登録解除
void PlacementDataManager::UnregisterZoneFishingSpot()
{
  for( u32 i = 0; i < FISHING_SPOT_ACTOR_MAX; ++ i )
  {
    FieldFishingSpotActor* pActor = this->m_pFishingSpotActor[i];

    // 生成されていない
    if(pActor == NULL)
    {
      continue;
    }

    // 既に削除されている
    if( pActor->IsRegist() == false )
    {
      continue;
    }

    pActor->UnregistActor();
  }
}


/**
 * @brief きのみ : アクターのサーチ
 */
FieldBerryPointActor* PlacementDataManager::SearchZoneBerryPoint( u32 beryyPointID )
{
  for( u32 i = 0; i < BERRY_POINT_ACTOR_MAX; ++ i )
  {
    FieldBerryPointActor* pActor = this->m_pBerryPointActor[i];
    if( !pActor ) { continue; }
    // 登録がない
    if( !pActor->IsRegist() ) { continue; }

    if( beryyPointID == pActor->GetBerryPointID())
    {
      return pActor;
    }
  }
  return NULL;
}
FieldBerryPointActor* PlacementDataManager::SearchZoneBerryPointFromEventID( EventID eventId )
{
  for( u32 i = 0; i < BERRY_POINT_ACTOR_MAX; ++ i )
  {
    FieldBerryPointActor* pActor = this->m_pBerryPointActor[i];
    if( !pActor ) { continue; }
    // 登録がない
    if( !pActor->IsRegist() ) { continue; }

    if( eventId == pActor->GetEventID())
    {
      return pActor;
    }
  }
  return NULL;
}

// 押す岩を配置
void PlacementDataManager::CreateZonePushRockWork( ZoneID zoneID )
{
  void * p_data = GetZonePlacementData( zoneID, PACK_IDX_PUSHROCK );
  if( p_data == NULL ){ return ; }

  // アクセサー生成
  FieldPushRockAccessor pushRockAccessor;
  pushRockAccessor.Initialize( p_data );

  // 岩のマトリクス数
  u32 pr_maxi = pushRockAccessor.GetDataMax();

  // 登録岩の総数
  u32 rock_index = 0;

  // 登録穴の総数
  u32 hole_index = 0;

  // PushRockの領域分ループ
  for( u32 pr_i = 0; pr_i<pr_maxi; ++pr_i )
  {
    const FieldPushRockAccessor::PushRockData* cpPushRock = pushRockAccessor.GetData( pr_i );

    // 岩の生成
    {
      // データサイズチェック
      u32 maxi = cpPushRock->rock_num;
      if( (maxi + rock_index) > PUSHROCK_ACTOR_MAX )
      {
        GFL_ASSERT_MSG( false, "PushRock Rock num Over (%d/%d)\n", maxi + rock_index, PUSHROCK_ACTOR_MAX );
        return;
      }

      u32 countup = 0;
      u32 roopNum = maxi;
      for( u32 i = 0; i < roopNum; ++ i )
      {
        FieldPushRockMoveModelAccessor* pushRockMoveModelAccessor = GFL_NEW(m_pActorHeap) FieldPushRockMoveModelAccessor();
        // セットアプ
        pushRockMoveModelAccessor->Setup( *cpPushRock );

        const FieldPushRockAccessor::RockData& rockData = pushRockAccessor.GetRockData( *cpPushRock, i );

        pushRockMoveModelAccessor->Initialize( &rockData );

        const FieldMoveModelAccessor::MoveModelData *moveModelData = pushRockMoveModelAccessor->GetData( 0 );

        FieldMoveModelActor *pActor = createMoveModelActor( moveModelData, zoneID, false );
        m_pPushRockActor[ rock_index ] = pActor;
        m_pPushRockMoveModelAccessor[ rock_index ] = pushRockMoveModelAccessor;
        rock_index ++;
      }
    }
  }
}

// 押す岩を配置
void PlacementDataManager::CreateZonePushRockResource( ZoneID zoneID )
{
  void * p_data = GetZonePlacementData( zoneID, PACK_IDX_PUSHROCK );
  if( p_data == NULL ){ return ; }

  // アクセサー生成
  FieldPushRockAccessor pushRockAccessor;
  pushRockAccessor.Initialize( p_data );

  // 岩のマトリクス数
  u32 pr_maxi = pushRockAccessor.GetDataMax();

  // 登録穴の総数
  u32 hole_index = 0;

  u32 rock_index = 0;
 


  // PushRockの領域分ループ
  for( u32 pr_i = 0; pr_i<pr_maxi; ++pr_i )
  {
    const FieldPushRockAccessor::PushRockData* cpPushRock = pushRockAccessor.GetData( pr_i );

    // 岩の生成
    {
      // データサイズチェック
      u32 maxi = cpPushRock->rock_num;
      u32 roopNum = maxi;
      for( u32 i = 0; i < roopNum; ++ i )
      {
        createMoveModelActorResource( reinterpret_cast<FieldMoveModelActor*>(m_pPushRockActor[ rock_index ]), m_pPushRockMoveModelAccessor[rock_index] );
        rock_index ++;
      }
    }


    // 穴の生成
    {
      u32 maxi = cpPushRock->hole_num;
      if( (maxi + hole_index) > PUSHROCK_HOLE_MAX )
      {
        GFL_ASSERT_MSG( false, "PushRock Hole num Over (%d/%d)\n", maxi + hole_index, PUSHROCK_ACTOR_MAX );
        return;
      }

      u32 countup = 0;
      u32 roopNum = maxi;
      const FieldPushRockAccessor::HoleData* cpHole = NULL;
      for( u32 i = 0; i < roopNum; ++ i )
      {
        BaseActor * pActor = m_pCollisionSceneForStaticModel->CreateStaticActor( m_pAllocator, m_pActorHeap );
        cpHole = &pushRockAccessor.GetHoleData( *cpPushRock, i );
        pushRockAccessor.CreateHoleCollisionData( *cpPushRock, i, pActor );
        m_pPushRockHoleStaticActor[ hole_index ] = pActor;
        m_pPushRockHoleData[hole_index] = cpHole;
        hole_index ++;

        // HOLEの情報が取得できるかチェック
        TOMOYA_PRINT( "hole %d\n", this->GetPushRockGridType( zoneID, 0, cpHole->x, cpHole->z ) );

        // 岩が落ちていなければ、コリジョンをレジスト
        u16 work_no = *m_cpEventWork->GetEventWorkAdrs( WK_PUSH_ROCK_HOLE_01 + cpHole->uniq_id );
        if( work_no == 0 )
        {
          m_pCollisionSceneForStaticModel->RegistStaticActor( reinterpret_cast<StaticActor*>(pActor) );
        }
        else
        {
          // 岩が落ちていればレジストしない＆岩を移動
          gfl2::math::Matrix34 matrix;
          gfl2::math::Vector3  position;

          FieldPushRockAccessor::GetPushRockMatrix( *cpPushRock , &matrix );
          FieldPushRockAccessor::GetPushRockLocalPosition( *cpPushRock, cpHole->x, cpHole->z, &position );
          position = matrix * position;

          u32 movemodel_idx = m_pMoveModelManager->GetFieldMoveModelIndexFromEventId( PR_ROCK_ACTOR_START + work_no );
          Field::MoveModel::FieldMoveModel* pMoveModel = NULL;
          if( movemodel_idx < MoveModel::FIELD_MOVE_MODEL_MAX )
          {
            pMoveModel = m_pMoveModelManager->GetFieldMoveModel( movemodel_idx ) ;
          }

          GFL_ASSERT_STOP( ((movemodel_idx < MoveModel::FIELD_MOVE_MODEL_MAX) && (pMoveModel != NULL)) );

          // 岩が落ちていればその分Y座標をずらす
          const f32 BLOCK_SIZE = 300.0f;
          gfl2::math::SRT localSRT = pMoveModel->GetLocalSRT();
          localSRT.translate = gfl2::math::Vector3(position.x,position.y-BLOCK_SIZE,position.z);
          pMoveModel->SetLocalSRT(localSRT);

          // 地面からの押し返しをやめる
          pMoveModel->SetCollisionSceneContainerForGround(NULL);
        }
      }
    }
  }
}

// 押す岩を削除
void PlacementDataManager::DeleteZonePushRockWork( void )
{
  for( u32 i = 0; i < PUSHROCK_ACTOR_MAX; ++ i )
  {
    if(m_pPushRockActor[ i ]){
      m_pPushRockActor[ i ]->Terminate();
      GFL_SAFE_DELETE( m_pPushRockActor[ i ] );
      GFL_SAFE_DELETE( m_pPushRockMoveModelAccessor[ i ] );
    }
  }

}

// 押す岩を削除
void PlacementDataManager::DeleteZonePushRockResource( void )
{
  for( u32 i = 0; i < PUSHROCK_ACTOR_MAX; ++ i )
  {
    if(m_pPushRockActor[ i ])
    {
      reinterpret_cast<FieldMoveModelActor*>(m_pPushRockActor[ i ])->TerminateResource();
    }
  }

  for( u32 i=0; i< PUSHROCK_HOLE_MAX; ++i )
  {
    if(m_pPushRockHoleStaticActor[i])
    {
      m_pCollisionSceneForStaticModel->UnRegistStaticActor( reinterpret_cast<StaticActor*>(m_pPushRockHoleStaticActor[i]) );
      GFL_SAFE_DELETE(m_pPushRockHoleStaticActor[i]);
      m_pPushRockHoleData[i] = NULL;
    }
  }
}

// 押す岩のシーンからの登録解除
void PlacementDataManager::UnRegistZonePushRock(void)
{
  for( u32 i = 0; i < PUSHROCK_ACTOR_MAX; ++ i )
  {
    FieldMoveModelActor *pActor = reinterpret_cast<FieldMoveModelActor *>( m_pPushRockActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に削除されている
    if( !pActor->IsRegist() ){ continue; }
    pActor->UnregistActor();
  }

  //HOLE
  for( u32 i=0; i<PUSHROCK_HOLE_MAX; ++i )
  {
    StaticActor * pActor = reinterpret_cast<StaticActor*>(m_pPushRockHoleStaticActor[i]);
    if( !pActor ){ continue; }
    m_pCollisionSceneForStaticModel->UnRegistStaticActor( pActor );
  }
}

// 押す岩のシーンからの登録解除
void PlacementDataManager::UnRegistZonePushRockHole(u32 holeID)
{
  //HOLE
  for( u32 i=0; i<PUSHROCK_HOLE_MAX; ++i )
  {
    StaticActor * pActor = reinterpret_cast<StaticActor*>(m_pPushRockHoleStaticActor[i]);
    if( !pActor ){ continue; }
    if( m_pPushRockHoleData[i]->uniq_id != holeID ){ continue; }
    m_pCollisionSceneForStaticModel->UnRegistStaticActor( pActor );
  }
}

void PlacementDataManager::UnRegistZonePushRockHoleAndSetSaveWork(GameSys::GameData * pGameData, u32 holeID, u32 rock_event_id )
{
  EventWork* pEventWork = pGameData->GetEventWork();

  //HOLE
  for( u32 i=0; i<PUSHROCK_HOLE_MAX; ++i )
  {
    StaticActor * pActor = reinterpret_cast<StaticActor*>(m_pPushRockHoleStaticActor[i]);
    if( !pActor ){ continue; }
    if( m_pPushRockHoleData[i]->uniq_id != holeID ){ continue; }

    // 岩を落とす。
    u16 * work = pEventWork->GetEventWorkAdrs( holeID + WK_PUSH_ROCK_HOLE_01 );
    *work = rock_event_id - PR_ROCK_ACTOR_START;
    m_pCollisionSceneForStaticModel->UnRegistStaticActor( pActor );
  }
}


/**
 *  @brief 押す岩の格子座標を返す。
 *  @param  world_position  ワールド座標
 *  @param  out_index       格子のインデックス(ゾーン内に複数の格子がある可能性がある）
 *  @param  out_x           X座標
 *  @param  out_z           Z座標
 *
 *  @retval true  格子の領域内
 *  @retval false 格子の領域外
 */
bool PlacementDataManager::GetPushRockMatrix( ZoneID zoneID, const gfl2::math::Vector3& world_position, u32 * out_index, u32 * out_x, u32* out_z )
{
  void * p_data = GetZonePlacementData( zoneID, PACK_IDX_PUSHROCK );
  if( p_data == NULL ){ return false; }

  // アクセサー生成
  FieldPushRockAccessor pushRockAccessor;
  pushRockAccessor.Initialize( p_data );

  // 岩のマトリクス数
  u32 pr_maxi = pushRockAccessor.GetDataMax();

  // PushRockの領域分ループ
  for( u32 pr_i = 0; pr_i<pr_maxi; ++pr_i )
  {
    const FieldPushRockAccessor::PushRockData* cpPushRock = pushRockAccessor.GetData( pr_i );

    // @fix GFMMCat[156]:一部のフィールドでカイリキ岩が動かなくなっている 対処→momijiで追加された領域外チェック処理をフェローチェマップのゾーンだけで動くように修正 ↓以下の処理ではゾーンIDを渡すように変更
    if (FieldPushRockAccessor::GetPushRockLocalGrid(*cpPushRock, world_position, out_x, out_z, zoneID))
    {
      *out_index = pr_i;
      return true;
    }
  }

  return false;
}


/**
 *  @brief 押す岩の格子グリッドのワールド座標を取得
 *  @param  index       格子のインデックス(ゾーン内に複数の格子がある可能性がある）
 *  @param  x           X座標
 *  @param  z           Z座標
 *  @param  out_position ワールド座標格納先
 *
 *  @retval 格子の範囲内かどうか
 */
b32 PlacementDataManager::GetPushRockGridWorldPosition( ZoneID zoneID, u32 index, u32 x, u32 z, gfl2::math::Vector3 * out_position )
{
  void * p_data = GetZonePlacementData( zoneID, PACK_IDX_PUSHROCK );
  if( p_data == NULL ){ return false; }

  // アクセサー生成
  FieldPushRockAccessor pushRockAccessor;
  pushRockAccessor.Initialize( p_data );

  // PushRockの領域分ループ
  const FieldPushRockAccessor::PushRockData* cpPushRock = pushRockAccessor.GetData( index );

  gfl2::math::Matrix34 matrix;
  gfl2::math::Vector3 position;

  FieldPushRockAccessor::GetPushRockMatrix(*cpPushRock, &matrix );
  b32 result = FieldPushRockAccessor::GetPushRockLocalPosition( *cpPushRock, x, z, &position );

  *out_position = matrix * position;

  return result;
}

/**
 *  @brief 押す岩の格子のタイプを取得
 *  @param  index       格子のインデックス(ゾーン内に複数の格子がある可能性がある）
 *  @param  x           X座標
 *  @param  z           Z座標
 *
 *  @retval 格子のタイプ
 */
FieldPushRockAccessor::PushRockDef PlacementDataManager::GetPushRockGridType( ZoneID zoneID, u32 index, u32 x, u32 z )
{
  void * p_data = GetZonePlacementData( zoneID, PACK_IDX_PUSHROCK );
  if( p_data == NULL ){ return FieldPushRockAccessor::DEF_MAX; }

  // アクセサー生成
  FieldPushRockAccessor pushRockAccessor;
  pushRockAccessor.Initialize( p_data );

  // PushRockの領域分ループ
  const FieldPushRockAccessor::PushRockData* cpPushRock = pushRockAccessor.GetData( index );

  FieldPushRockAccessor::PushRockDef def = pushRockAccessor.GetGridType( *cpPushRock, x, z );
  if( def == FieldPushRockAccessor::DEF_ROCK )  // 岩の初期位置でなく、今いる位置を返す。
  {
    def = FieldPushRockAccessor::DEF_FLOOR;
  }

  // 現状岩のいる位置じゃないかチェック
  gfl2::math::SRT local_srt;
  u32 rock_x, rock_z;
  for( u32 i = 0; i < PUSHROCK_ACTOR_MAX; ++ i )
  {
    if(m_pPushRockActor[ i ])
    {
      FieldMoveModelActor * pActor = reinterpret_cast<FieldMoveModelActor*>(m_pPushRockActor[ i ]);

      MoveModel::FieldMoveModel * pMoveModel = pActor->GetMoveModel();
      local_srt = pMoveModel->GetLocalSRT();

      // @fix GFMMCat[156]:一部のフィールドでカイリキ岩が動かなくなっている 対処→momijiで追加された領域外チェック処理をフェローチェマップのゾーンだけで動くように修正 ↓以下の処理ではゾーンIDを渡すように変更
      if (FieldPushRockAccessor::GetPushRockLocalGrid(*cpPushRock, local_srt.translate, &rock_x, &rock_z, zoneID))
      {
        if( (rock_x == x) && (rock_z == z) )
        {
          def = FieldPushRockAccessor::DEF_ROCK;
          break;
        }
      }
    }
  }

  return def;
}


/**
 *  @brief 押す岩の格子の穴IDを取得
 *  @param  index       格子のインデックス(ゾーン内に複数の格子がある可能性がある）
 *  @param  x           X座標
 *  @param  z           Z座標
 *
 *  @retval true  穴
 *  @retval false 穴以外
 */
bool PlacementDataManager::GetPushRockHoleID( ZoneID zoneID, u32 index, u32 x, u32 z, u32 * out_holeid )
{
  void * p_data = GetZonePlacementData( zoneID, PACK_IDX_PUSHROCK );
  if( p_data == NULL ){ return false; }

  // アクセサー生成
  FieldPushRockAccessor pushRockAccessor;
  pushRockAccessor.Initialize( p_data );

  // PushRockの領域分ループ
  for( u32 i=0; i<PUSHROCK_HOLE_MAX; ++i )
  {
    if( m_pPushRockHoleData[i] )
    {
      if( (m_pPushRockHoleData[i]->x == x) && (m_pPushRockHoleData[i]->z == z) )
      {
        *out_holeid = m_pPushRockHoleData[i]->uniq_id;
        return true;
      }
    }
  }

  return false;
}




/**
 *  @brief エイリアス　StaticModelの生成
 */
void PlacementDataManager::CreateAreaStaticModel(void)
{
  GFL_ASSERT( m_StaticModelActorAliasNum == 0 );

  // エリアに存在するゾーン全ての配置情報からエイリアス設定がされているものを作成
  for( u32 index = 0; index < m_pWorld->GetZoneNum(); index++ )
  {
    s32 zoneId = m_pWorld->GetZoneInfo(index).m_ZoneID;
    s32 areaId = m_pWorld->GetZoneInfo(index).m_AreaID;

    if (areaId != m_pArea->GetAreaID())  // @note momiji修正、違うエリアを弾いてなかったので弾くように修正
    {
      continue;
    }

    u32 zoneIndexInArea = m_pWorld->GetZoneIndexInArea(zoneId);
    void * p_data = getIndexPlacementData( zoneIndexInArea, PACK_IDX_STATICMODEL );   // @note momiji修正、getIndexPlacementDataに渡すのはエリアローカルでのゾーンデータのindex
    if (p_data == NULL)
    {
      continue;
    }

    // アクセサー生成
    FieldStaticModelAccessor staticModelAccessor;
    staticModelAccessor.Initialize( p_data );
    
    // データサイズチェック
    u32 maxi = staticModelAccessor.GetAliasDataMax();
    if( (maxi + m_StaticModelActorAliasNum) > STATICMODEL_ACTOR_MAX )
    {
      GFL_ASSERT_MSG( false, "StaticModel num Over (%d/%d)\n", maxi +m_StaticModelActorAliasNum, STATICMODEL_ACTOR_MAX);
      return;
    }

    u32 roopNum = staticModelAccessor.GetDataMax();
    for( u32 i = 0; i < roopNum; ++ i )
    {
      const FieldStaticModelAccessor::StaticModelData *staticModelData = staticModelAccessor.GetData( i);
      
      if(!staticModelData->alias) continue;

      // StaticModelActorの生成
      FieldStaticModelActor *pActor = craeteStaticModelActor( staticModelData, &staticModelAccessor );

      m_pStaticModelActor[ m_StaticModelActorAliasNum ] = pActor;
      m_StaticModelActorAliasNum ++;
    }
  }
}

/**
 *  @brief エイリアス  StaticModelの破棄リクエスト
 */
void PlacementDataManager::RequestDeleteAreaStaticModel()
{
  requestDeleteStaticModels( 0, m_StaticModelActorAliasNum );
}

/**
 *  @brief エイリアス　StaticModelの破棄
 */
void PlacementDataManager::DeleteAreaStaticModel(void)
{
  deleteStaticModels( 0, m_StaticModelActorAliasNum );

  m_StaticModelActorAliasNum = 0;

}

/**
 *  @brief エイリアス　動作モデルの生成
 */
void PlacementDataManager::CreateAreaMoveModelWork(void)
{
#define DEBUG_PRINT_USE_161102  ( 0 )

#if DEBUG_PRINT_USE_161102
  gfl2::heap::HeapBase* pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);
  Field::Debug::SymbolNameLoader* pSymbolNameLoader = GFL_NEW(pDebugHeap) Field::Debug::SymbolNameLoader();
  pSymbolNameLoader->Load(pDebugHeap, Field::Debug::SymbolNameLoader::SymbolType::ZONE_ID);
  Field::Debug::SymbolNameLoader* pAreaNameLoader = GFL_NEW(pDebugHeap) Field::Debug::SymbolNameLoader();
  pAreaNameLoader->Load(pDebugHeap, Field::Debug::SymbolNameLoader::SymbolType::AREA_ID);
#endif

  GFL_ASSERT( m_MoveModelActorAliasNum == 0 );

  // @note nijiでデータindexの扱いに問題があった為、momijiで大規模に修正を行っています
  for( int index = 0; index<m_pWorld->GetZoneNum(); index++ )
  {
    s32 zoneId = m_pWorld->GetZoneInfo(index).m_ZoneID;
    s32 areaId = m_pWorld->GetZoneInfo(index).m_AreaID;
  
    if( areaId != m_pArea->GetAreaID() )  // @note momiji修正、違うエリアを弾いてなかったので弾くように修正
    {
      continue;
    }

    u32 zoneIndexInArea = m_pWorld->GetZoneIndexInArea( zoneId );

    // momiji にて『ふれあい表現』用の動作モデルが増えたので２つの動作モデルを同時に扱うように処理を変更
    void * p_data         = getIndexPlacementData(zoneIndexInArea, PACK_IDX_MOVEMODEL);   // @note momiji修正、getIndexPlacementDataに渡すのはエリアローカルでのゾーンデータのindex
    void * p_data_contact = getIndexPlacementData(zoneIndexInArea, PACK_IDX_CONTACT_POKEMON);

    // 共にデータが存在しない場合は中断
    if (p_data == NULL && p_data_contact == NULL){ continue; }

    FieldMoveModelAccessor moveModelAccessor;
    FieldContactPokemonAccessor contactPokeAccessor;

    // アクセサー生成
    u32 maxi = 0;
    if (p_data)
    {
      moveModelAccessor.Initialize(p_data);
      maxi += moveModelAccessor.GetAliasDataMax();
    }
    if (p_data_contact)
    {
      contactPokeAccessor.Initialize(p_data_contact);
      maxi += contactPokeAccessor.GetAliasDataMax();
    }
    
    // データサイズチェック
    if( (maxi + m_MoveModelActorAliasNum) > MOVEMODEL_EVENT_ACTOR_MAX )
    {
      GFL_ASSERT_MSG( false, "MoveModel num Over (%d/%d)", maxi + m_MoveModelActorAliasNum, MOVEMODEL_EVENT_ACTOR_MAX );
      return;
    }

    // 動作モデル生成
    if (p_data)
    {
      // 通常の動作モデル生成
      u32 roopNum = moveModelAccessor.GetDataMax();
      for (u32 i = 0; i < roopNum; ++i)
      {
        const FieldMoveModelAccessor::MoveModelData *moveModelData = moveModelAccessor.GetData(i);

        // エイリアスかチェック
        if (!moveModelData->alias) continue;

        FieldMoveModelActor *pActor = createMoveModelActor(moveModelData, zoneId, true);  // @note momiji修正、ゾーンIDを渡す必要がある関数にデータIndexを渡していたので修正

        m_pMoveModelActor[m_MoveModelActorAliasNum] = pActor;
        m_MoveModelActorAliasNum++;

#if DEBUG_PRINT_USE_161102
        c8* work;
        zoneId = m_pWorld->GetIDZoneInfo(index).m_ZoneID;
        pSymbolNameLoader->GetName(zoneId, &work);
        GFL_PRINT("GetIDZoneInfo %d(%s) inArea %d\n", zoneId, work, m_pWorld->GetZoneIndexInArea(zoneId));
        zoneId = m_pWorld->GetZoneInfo(index).m_ZoneID;
        pSymbolNameLoader->GetName(zoneId, &work);
        GFL_PRINT("GetZoneInfo %d(%s) inArea %d\n", zoneId, work, m_pWorld->GetZoneIndexInArea(zoneId));
#endif
      }
    }
    if (p_data_contact)
    {
      // ふれあい用の動作モデル生成
      u32 roopNum = contactPokeAccessor.GetDataMax();
      for (u32 i = 0; i < roopNum; ++i)
      {
        const FieldMoveModelAccessor::MoveModelData *moveModelData = contactPokeAccessor.GetData(i);

        // エイリアスかチェック
        if (!moveModelData->alias) continue;

        const FieldContactPokemonAccessor::ContactPokemonData *contactPokemonData = contactPokeAccessor.GetDataForContact(i);

        FieldMoveModelActor *pActor = createMoveModelActor(moveModelData, zoneId, true, contactPokemonData);  // @note momiji修正、ゾーンIDを渡す必要がある関数にデータIndexを渡していたので修正

        m_pMoveModelActor[m_MoveModelActorAliasNum] = pActor;
        m_MoveModelActorAliasNum++;

#if DEBUG_PRINT_USE_161102
        c8* work;
        zoneId = m_pWorld->GetIDZoneInfo(index).m_ZoneID;
        pSymbolNameLoader->GetName(zoneId, &work);
        GFL_PRINT("GetIDZoneInfo %d(%s) inArea %d\n", zoneId, work, m_pWorld->GetZoneIndexInArea(zoneId));
        zoneId = m_pWorld->GetZoneInfo(index).m_ZoneID;
        pSymbolNameLoader->GetName(zoneId, &work);
        GFL_PRINT("GetZoneInfo %d(%s) inArea %d\n", zoneId, work, m_pWorld->GetZoneIndexInArea(zoneId));
#endif
      }
    }
    
  }
#if DEBUG_PRINT_USE_161102
  GFL_SAFE_DELETE(pSymbolNameLoader);
  GFL_SAFE_DELETE(pAreaNameLoader);
#endif
}


/**
 *  @brief エイリアス　動作モデルの生成
 */
void PlacementDataManager::CreateAreaMoveModelResource(void)
{
  // momiji にて『ふれあい表現』用の動作モデルが増えたので２つの動作モデルを同時に扱うように処理を変更

  // 通常の動作モデルを処理
  for( u32 i = 0; i < m_MoveModelActorAliasNum; ++ i )
  {
    FieldMoveModelActor *pActor = reinterpret_cast<FieldMoveModelActor *>( m_pMoveModelActor[ i ] );
    if( !pActor ){ continue; }

    if (pActor->IsContactPokemon()){ continue; }    // ふれあい用の場合は無視

    // アクターからゾーンIDを取得
    ZoneID zoneID = pActor->GetDataZoneID();

    // アクセサー生成
    void * p_data = GetZonePlacementData( zoneID, PACK_IDX_MOVEMODEL );
    if( p_data == NULL ){ break ; }
    FieldMoveModelAccessor moveModelAccessor;
    moveModelAccessor.Initialize( p_data );

    createMoveModelActorResource( pActor, &moveModelAccessor );
  }

  // ふれあい用の動作モデルを処理
  for (u32 i = 0; i < m_MoveModelActorAliasNum; ++i)
  {
    FieldMoveModelActor *pActor = reinterpret_cast<FieldMoveModelActor *>(m_pMoveModelActor[i]);
    if (!pActor){ continue; }

    if (!pActor->IsContactPokemon()){ continue; }    // 通常の動作モデルの場合は無視

    // アクターからゾーンIDを取得
    ZoneID zoneID = pActor->GetDataZoneID();

    // アクセサー生成
    void * p_data = GetZonePlacementData(zoneID, PACK_IDX_CONTACT_POKEMON);
    if (p_data == NULL){ break; }
    FieldContactPokemonAccessor contactPokemonAccessor;
    contactPokemonAccessor.Initialize(p_data);

    createMoveModelActorResource(pActor, &contactPokemonAccessor);
  }
}


/**
 *  @brief エイリアス　動作モデルの破棄
 */
void PlacementDataManager::DeleteAreaMoveModelWork(void)
{
  deleteMoveModels(0, m_MoveModelActorAliasNum);
  m_MoveModelActorAliasNum = 0;
}

/**
 *  @brief エイリアス　動作モデルの破棄
 */
void PlacementDataManager::DeleteAreaMoveModelResource(void)
{
  deleteMoveModelResources(0, m_MoveModelActorAliasNum);
}

/**
 *  @brief エイリアス　トレーナーの生成
 */
void PlacementDataManager::CreateAreaTrainerWork(void)
{
  GFL_ASSERT( m_TrainerActorAliasNum == 0 );

  // @note nijiでデータindexの扱いに問題があった為、momijiで大規模に修正を行っています
  for( int index = 0; index<m_pWorld->GetZoneNum(); index++ )
  {
    s32 zoneId = m_pWorld->GetZoneInfo(index).m_ZoneID;
    s32 areaId = m_pWorld->GetZoneInfo(index).m_AreaID;

    if (areaId != m_pArea->GetAreaID())  // @note momiji修正、違うエリアを弾いてなかったので弾くように修正
    {
      continue;
    }

    u32 zoneIndexInArea = m_pWorld->GetZoneIndexInArea(zoneId);
    void * p_data = getIndexPlacementData(zoneIndexInArea, PACK_IDX_TRAINER);   // @note momiji修正、getIndexPlacementDataに渡すのはエリアローカルでのゾーンデータのindex
    if (p_data == NULL)
    {
      continue;
    }

    // アクセサー生成
    FieldTrainerAccessor trainerAccessor;
    trainerAccessor.Initialize( p_data );
    
    // データサイズチェック
    u32 maxi = trainerAccessor.GetAliasDataMax();
    if( maxi + m_TrainerActorAliasNum > TRAINER_ACTOR_MAX )
    {
      GFL_ASSERT_MSG( false, "Trainer num Over (%d/%d)", maxi + m_TrainerActorAliasNum, TRAINER_ACTOR_MAX );
      return;
    }

    u32 roopNum = trainerAccessor.GetDataMax();
    for( u32 i = 0; i < roopNum; ++ i )
    {
      const FieldTrainerAccessor::TrainerData *trainerData = trainerAccessor.GetData( i );

      // エイリアスチェック
      if(!trainerData->alias){ continue; }

      FieldTrainerActor* pActor = createTrainerActor( trainerData, &trainerAccessor, zoneId, true ); // @note momiji修正、ゾーンIDを渡す必要がある関数にデータIndexを渡していたので修正
      m_pTrainerActor[ m_TrainerActorAliasNum ] = pActor;
      m_TrainerActorAliasNum ++;
    }
  }

}

/**
 *  @brief エイリアス　トレーナーの生成
 */
void PlacementDataManager::CreateAreaTrainerResource(void)
{
  for( u32 i = 0; i < m_TrainerActorAliasNum; ++ i )
  {
    FieldTrainerActor *pActor = reinterpret_cast<FieldTrainerActor *>( m_pTrainerActor[ i ] );
    if( !pActor ){ continue; }

    // アクターからゾーンIDを取得
    ZoneID zoneID = pActor->GetDataZoneID();

    // アクセサー生成
    void * p_data = GetZonePlacementData( zoneID, PACK_IDX_TRAINER );
    if( p_data == NULL ){ return ; }
    FieldTrainerAccessor trainerAccessor;
    trainerAccessor.Initialize( p_data );

    createTrainerActorResource( pActor, &trainerAccessor );
  }

}

/**
 *  @brief エイリアス　トレーナーの破棄
 */
void PlacementDataManager::DeleteAreaTrainerWork(void)
{
  deleteTrainers( 0, m_TrainerActorAliasNum );
  m_TrainerActorAliasNum = 0;
}

/**
 *  @brief エイリアス　トレーナーの破棄
 */
void PlacementDataManager::DeleteAreaTrainerResource(void)
{
  deleteTrainerResources( 0, m_TrainerActorAliasNum );
}



/**
 *  @brief  アクターが持っているユーザーデータのイベントタイプを返す
 *  @retval FIELD_PLACEMENT_DATA_TYPE_POSEVENT,
            FIELD_PLACEMENT_DATA_TYPE_MVMODEL,
            FIELD_PLACEMENT_DATA_TYPE_BGEVENT,
 */
 u32 PlacementDataManager::GetActorUserDataEventType( BaseActor* actor )
{
  u32* type = static_cast<u32*>(actor->GetUserData());

  GFL_ASSERT_STOP( (*type) < FIELD_PLACEMENT_DATA_TYPE_MAX );

  return (*type);

}

/**
 *  @brief  アクターが持っているイベントデータのスクリプトIDを取得
 *  @retval FieldScript::SCRID_NUL  なし
 *  @retval その他　スクリプトID
 */
 u32 PlacementDataManager::GetScriptID( BaseActor* actor )
{
  u32 type = GetActorUserDataEventType(actor);
  switch( type ){
  case FIELD_PLACEMENT_DATA_TYPE_POSEVENT:
    {
      const FieldPosEventAccessor::PosData* pData = reinterpret_cast<const FieldPosEventAccessor::PosData*>(actor->GetUserData());
      return pData->scriptID;
    }

  case FIELD_PLACEMENT_DATA_TYPE_MVMODEL:
    {
      const FieldMoveModelAccessor::MoveModelData* pData = reinterpret_cast<const FieldMoveModelAccessor::MoveModelData*>(actor->GetUserData());
      return pData->scriptID;
    }
  }
  return FieldScript::SCRID_NULL;
}


/** 
 *  @brief  特定ゾーンのイベントポインタを取得する。
 *
 *  @param  ZoneID zoneID ゾーンID
 *  @param  pack_idx     何の情報を取得するか 
    PACK_IDX_POS,       ///< ポジション配置情報
    PACK_IDX_MOVEMODEL, ///< 動作配置情報
    PACK_IDX_MAPJUMP,   ///< マップジャンプ配置情報
    PACK_IDX_BG,        ///< BG配置情報
    PACK_IDX_STATICMODEL, ///< 配置モデル配置情報
    PACK_IDX_ACTION,    ///< アクションイベント配置情報
    PACK_IDX_ENCOUNT,   ///< エンカウント配置情報

    @retval NULL  　このゾーンにpack_idxの配置情報がない
    @retval その他　配置データ
 */
 void * PlacementDataManager::GetZonePlacementData( ZoneID zoneID, u32 pack_idx )
{
  u32 index = m_pWorld->GetZoneIndexInArea( zoneID );
  return getIndexPlacementData( index, pack_idx );
}

// イベント生成チェック
bool PlacementDataManager::IsEventCreate( const Field::EventWork* cpEventWork, u32 rom_version, u32 flagwork, u32 num )
{
  // ROM Versionのチェック
  if ((rom_version != Field::PLACEMENT_ROM_VERSION_NONE) && (GET_VERSION() != rom_version)){ return false; }

  // フラグワークのチェック
  if( EventWork::CheckIDFlagRenge( flagwork ) ){
    if( flagwork != 0 ){
      if( cpEventWork->CheckEventFlag( flagwork ) ){ return false; }
    }
  }else{
    const u16 * pwork = cpEventWork->GetEventWorkAdrs( flagwork );
    if(pwork){
      if( (*pwork) != num ){ return false; }
    }
  }
  return true;

}

/**
*  @brief  動作モデルアクターのレジスト状態に、イベントデータとの差異がないかチェックする
*  @note   開発時のチェック用、スクリプト終了時に実行される
*
**/
void PlacementDataManager::CheckMoveModelActorResistState( void )
{
  for( u32 i = 0; i < MOVEMODEL_EVENT_ACTOR_MAX; ++ i )
  {
    FieldMoveModelActor* pActor = reinterpret_cast<FieldMoveModelActor*>( m_pMoveModelActor[i] );
    if( !pActor ){ continue; }
    const FieldMoveModelAccessor::MoveModelData* pData = pActor->GetData();

    // @fix MMcat[607]:特異なケースにおいて、レジスト状態チェックを明示的に切る機能を追加
    if (!pActor->GetMoveModel() || pActor->GetMoveModel()->CheckStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_RESIST_CHECK_INVALID))
    {
      continue;
    }

    if( isEventCreate( pData->romVersion, pData->flagwork, pData->flagwork_num, static_cast<ReverseTimeType>(pData->reversetime_type) ) )
    {
      // 条件を満たしているのに、レジストされていない
      GFL_ASSERT_MSG(pActor->IsRegist(), "フラグワークが生成条件を満たしているのに生成されていない動作モデルがあります。　eventID=%d\n", pActor->GetData()->eventID);
      continue; 
    }
    else
    {
      // 条件を満たしていないのに、レジストされている
      GFL_ASSERT_MSG(!pActor->IsRegist(), "フラグワークが生成条件を満たしていないのに生成されている動作モデルがあります。　eventID=%d\n", pActor->GetData()->eventID);
    }
  }
}

//
// データ情報取得
// 

/**
*  @brief 動作モデルアクターのデータ数、データ最大制限数、エイリアスデータ数
*/
s32 PlacementDataManager::GetMoveModelActorCount( void ) const
{
  s32 count = 0;
  for( u32 i = 0; i < MOVEMODEL_EVENT_ACTOR_MAX; ++ i )
  {
    if( !m_pMoveModelActor[ i ] ){ continue; }
    count++;
  }

  return count;
}

/**
*  @brief 視線トレーナーアクターのデータ数、データ最大制限数、エイリアスデータ数
*/
s32 PlacementDataManager::GetTrainerActorCount( void ) const
{
  s32 count = 0;
  for( u32 i = 0; i < TRAINER_ACTOR_MAX; ++ i )
  {
    if( !m_pTrainerActor[i] ){ continue; }
    count++;
  }
  return count;
}

/**
*  @brief ギミックエンカウントアクターのデータ数、データ最大制限数
*/
s32 PlacementDataManager::GetGimmickEncountActorCount( void ) const
{
  s32 count = 0;
  for( u32 i = 0; i < TRAINER_ACTOR_MAX; ++ i )
  {
    if( !m_pGimmickEncountActor[i] ){ continue; }
    count++;
  }
  return count;
}

/**
*  @brief 主人公がアクションを行った事をふれあいアクターに通知
*  @note イベント中にふれあいポケモンが喜ぶ動作を行った場合、コール
*/
void PlacementDataManager::RequestEventPlayerActionForFureaiActor( s32 type )
{
  for (u32 i = 0; i < PlacementDataManager::MOVEMODEL_EVENT_ACTOR_MAX; i++)
  {
    FieldMoveModelActor* pWorkActor(
      reinterpret_cast<Field::FieldMoveModelActor*>(GetMoveModelActor(i)));

    if (pWorkActor == NULL || !pWorkActor->IsSetupMoveModel()) // @note IsRegistでは、非表示状態の動作モデルが省かれてしまう
    {
      continue;
    }

    if (pWorkActor->IsContactPokemon())
    {
      pWorkActor->EventPlayerActionReq( type );
    }
  }
}

//=========================================================================================================
// private
//=========================================================================================================

void * PlacementDataManager::getIndexPlacementData( u32 index, u32 pack_idx )
{
  // ゾーンがワールドに登録されていない。
  if( index >= m_pWorld->GetZoneNum() ){ return NULL; }

  // サイズが０＝データなし
  if( m_AreaPlacementData[ pack_idx ].GetDataSize( index ) == 0 ){ return NULL; }

  void * p_data = m_AreaPlacementData[ pack_idx ].GetData( index );
 
  return p_data;
}

/**
 *  @brief  イベント生成チェック
 */
 bool PlacementDataManager::isEventCreate( u32 rom_version, u32 flagwork, u32 num, ReverseTimeType reversetime_type )
{
  bool ret = IsEventCreate( m_cpEventWork, rom_version, flagwork, num );
  if( ret == false )
  {
  	return ret;
  }
  
  
  // trueのときはさらに別世界時間チェック
  switch( reversetime_type )
  {
  // 常に有効
  case RTIME_TYPE_ALL:
    break;

  // 別世界時間のみ有効
  case RTIME_TYPE_REVERSE:
    if( !GameSys::AlolaTime::CheckAlolaOtherWorld( m_cpEventWork ) )
    {
      return false;
    }
    break;

  default:
    GFL_ASSERT( reversetime_type < RTIME_TYPE_MAX );
    break;
  }

  return true;

}


/**
 *  @brief  アクションタイプからコリジョンシーンを選択する
 */
 BaseCollisionScene *PlacementDataManager::getCollisionSceneFromActionType( u32 actionType )
{
  BaseCollisionScene *pResult = NULL;

  // @fix GFNMCat[519] ROCKBREAKER以外をPushイベントに変えました
  switch( actionType )
  {
  case ACTIONEVENT_STEPJUMP:
    pResult = m_pCollisionSceneForEventPush;
    break;
  case ACTIONEVENT_ROCKBREAKER:
    pResult = m_pCollisionSceneForEventPos;
    break;
  case ACTIONEVENT_OTTOTTO:
    pResult = m_pCollisionSceneForEventPush;
    break;
  case ACTIONEVENT_OFFROADBORDER:
    pResult = m_pCollisionSceneForEventPush;
    break;
  default:
    GFL_ASSERT_MSG(false, "Invalid ActionType !!!");
  }
  return pResult;
}

/**
 *  @brief ギミックエンカウントのアクターを生成する
 */
 void PlacementDataManager::createGimmickEncountActor(
  u32 idx,
  GimmickEncount::GimmickType gimmickType,
  u16 gimmickFlagNo,
  FieldGimmickEncountAccessor* pGimmickEncountAccessor,
  const FieldGimmickEncountAccessor::Data* pData,
  bool isDrawOnly,
  ZoneID zoneId
  )
{
  switch( gimmickType)
  {
  // 待機型
  case GimmickEncount::GIMMICK_TYPE_STANDBY:
    {
      FieldGimmickEncountStandbyActor *pActor = GFL_NEW( m_pActorHeap ) FieldGimmickEncountStandbyActor();

      FieldGimmickEncountStandbyActor::SetupWorkData setupData;
      setupData.pGimmickEncountData                     = pData;
      setupData.pGimmickEncountManager                  = m_pGimmickEncountManager;
      setupData.gimmickFlagNo                           = gimmickFlagNo;
      setupData.isDrawOnly                              = isDrawOnly;

      pActor->InitializeWork( setupData );
      m_pGimmickEncountActor[ idx ] = pActor;
      m_pGimmickEncountActorBase[ idx] = pActor;
    }
    break;

  // 接近型
  case GimmickEncount::GIMMICK_TYPE_CHASE:
    {
      FieldGimmickEncountChaseActor *pActor = GFL_NEW( m_pActorHeap ) FieldGimmickEncountChaseActor();

      FieldGimmickEncountChaseActor::SetupData setupData;
      setupData.pMoveModelManager                       = m_pMoveModelManager;
      setupData.pCollisionSceneForGimmickEncount        = m_pCollisionSceneForGimmickEncount;
      setupData.pCollisionSceneForGimmickEncountAction  = m_pCollisionSceneForGimmickEncountAction;
      setupData.pGimmickEncountData                     = pData;
      setupData.pGimmickEncountManager                  = m_pGimmickEncountManager;
      setupData.gimmickFlagNo                           = gimmickFlagNo;
      setupData.zoneId                                  = zoneId;

      pActor->Initialize( setupData );
      m_pGimmickEncountActor[ idx ] = pActor;
      m_pGimmickEncountActorBase[ idx] = pActor;
    }
    break;

  // 巡回型
  case GimmickEncount::GIMMICK_TYPE_PATROL:
    {
      FieldGimmickEncountPatrolActor *pActor = GFL_NEW( m_pActorHeap ) FieldGimmickEncountPatrolActor();

      FieldGimmickEncountPatrolActor::SetupData setupData;
      setupData.pMoveModelManager                       = m_pMoveModelManager;
      setupData.pCollisionSceneForGimmickEncount        = m_pCollisionSceneForGimmickEncount;
      setupData.pAccessor                               = pGimmickEncountAccessor;
      setupData.pGimmickEncountData                     = pData;
      setupData.pHeap                                   = m_pActorHeap;
      setupData.pGimmickEncountManager                  = m_pGimmickEncountManager;
      setupData.gimmickFlagNo                           = gimmickFlagNo;
      setupData.zoneId                                  = zoneId;

      pActor->Initialize( setupData );
      m_pGimmickEncountActor[ idx ] = pActor;
      m_pGimmickEncountActorBase[ idx] = pActor;
    }
    break;

  // 逃走型
  case GimmickEncount::GIMMICK_TYPE_ESCAPE:
    {
      FieldGimmickEncountEscapeActor *pActor = GFL_NEW( m_pActorHeap ) FieldGimmickEncountEscapeActor();

      FieldGimmickEncountEscapeActor::SetupData setupData;
      setupData.pMoveModelManager                       = m_pMoveModelManager;
      setupData.pCollisionSceneForGimmickEncount        = m_pCollisionSceneForGimmickEncount;
      setupData.pCollisionSceneForGimmickEncountAction  = m_pCollisionSceneForGimmickEncountAction;
      setupData.pAccessor                               = pGimmickEncountAccessor;
      setupData.pGimmickEncountData                     = pData;
      setupData.pHeap                                   = m_pActorHeap;
      setupData.pGimmickEncountManager                  = m_pGimmickEncountManager;
      setupData.gimmickFlagNo                           = gimmickFlagNo;
      setupData.zoneId                                  = zoneId;

      pActor->Initialize( setupData );
      m_pGimmickEncountActor[ idx ] = pActor;
      m_pGimmickEncountActorBase[ idx] = pActor;
    }
    break;

  // 不正な型
  default:
    GFL_ASSERT( 0);
    break; // ここを通ってしまったら、このデータは配置しない
  }
}

/**
 *  @brief ギミックエンカウントのアクターのリソース、コリジョンを生成する
 */
 bool PlacementDataManager::createGimmickEncountActorResource(
  u32 idx,
  FieldGimmickEncountAccessor* pGimmickEncountAccessor,
  const FieldGimmickEncountAccessor::Data* pData
  )
{
  bool isRegist = true;

  switch( m_pGimmickEncountActorBase[ idx ]->GetLocalWork()->GetGimmickType())
  {
  // 待機型
  case GimmickEncount::GIMMICK_TYPE_STANDBY:
    {
      FieldGimmickEncountStandbyActor* pActor = reinterpret_cast<FieldGimmickEncountStandbyActor*>( m_pGimmickEncountActor[idx]);

      FieldGimmickEncountStandbyActor::SetupData setupData;
      setupData.pHeap                                   = m_pActorHeap;
      setupData.pStaticModelManager                     = m_pStaticModelManager;
      setupData.pCollisionSceneForGimmickEncount        = m_pCollisionSceneForGimmickEncount;
      setupData.pCollisionSceneForGimmickEncountAction  = m_pCollisionSceneForGimmickEncountAction;
      setupData.pTerrainManager                         = m_pTerrainManager;
      setupData.pRenderingPipeLine                      = m_pRenderingPipeLine;
      setupData.pAccessor                               = pGimmickEncountAccessor;
      setupData.pGimmickEncountData                     = pData;
      setupData.pAllocator                              = m_pAllocator;

      pActor->Initialize( setupData);
    }
    break;

  // 接近型
  case GimmickEncount::GIMMICK_TYPE_CHASE:
    {
      FieldGimmickEncountChaseActor* pActor = reinterpret_cast<FieldGimmickEncountChaseActor*>( m_pGimmickEncountActor[idx]);

      FieldGimmickEncountChaseActor::CreateResourceData setupData;
      setupData.pAccessor                       = pGimmickEncountAccessor;
      setupData.pTerrainManager                 = m_pTerrainManager;
      setupData.pRenderingPipeLine              = m_pRenderingPipeLine;
      setupData.pHeap                           = m_pActorHeap;
      setupData.pAllocator                      = m_pAllocator;

      pActor->CreateResource( setupData );
    }
    break;

  // 巡回型
  case GimmickEncount::GIMMICK_TYPE_PATROL:
    {
      FieldGimmickEncountPatrolActor* pActor = reinterpret_cast<FieldGimmickEncountPatrolActor*>( m_pGimmickEncountActor[idx]);

      FieldGimmickEncountPatrolActor::CreateResourceData setupData;
      setupData.pAccessor                       = pGimmickEncountAccessor;
      setupData.pTerrainManager                 = m_pTerrainManager;
      setupData.pRenderingPipeLine              = m_pRenderingPipeLine;

      pActor->CreateResource( setupData );
    }
    break;

  // 逃走型
  case GimmickEncount::GIMMICK_TYPE_ESCAPE:
    {
      FieldGimmickEncountEscapeActor* pActor = reinterpret_cast<FieldGimmickEncountEscapeActor*>( m_pGimmickEncountActor[idx]);

      FieldGimmickEncountEscapeActor::CreateResourceData setupData;
      setupData.pAccessor                       = pGimmickEncountAccessor;
      setupData.pTerrainManager                 = m_pTerrainManager;
      setupData.pRenderingPipeLine              = m_pRenderingPipeLine;
      setupData.pHeap                           = m_pActorHeap;
      setupData.pAllocator                      = m_pAllocator;

      pActor->CreateResource( setupData );

      // @fix GFNMCat[1230] ポニのげんや：メニュー項目の開閉を行うと、逃走したコソクムシが終点に再配置される
      // リソースが生成後に、バニッシュ状態かを見てRegistするかを決める
      // アプリ復帰後は、CreateResource内でリソース生成後にMoveCodeのComeback処理が走るが、そこではUnRegistできないため
      if( pActor->IsVanish() )
      {
        isRegist = false;
      }
    }
    break;

  // 不正な型
  default:
    GFL_ASSERT( 0); // 正しい運用ならここは通らない
    break;
  }

  return isRegist;
}

// ギミックエンカウントのアクターを取得
 IFieldActorBase* PlacementDataManager::GetGimmickEncountActor( u32 idx)
{
  if( m_pGimmickEncountActor[idx] == NULL || m_pGimmickEncountActorBase[idx] == NULL)
  {
    return NULL;
  }

  switch( m_pGimmickEncountActorBase[idx]->GetLocalWork()->GetGimmickType())
  {
  case GimmickEncount::GIMMICK_TYPE_STANDBY:
    return reinterpret_cast<FieldGimmickEncountStandbyActor*>( m_pGimmickEncountActor[idx]);
  case GimmickEncount::GIMMICK_TYPE_CHASE:
    return reinterpret_cast<FieldGimmickEncountChaseActor*>( m_pGimmickEncountActor[idx]);
  case GimmickEncount::GIMMICK_TYPE_PATROL:
    return reinterpret_cast<FieldGimmickEncountPatrolActor*>( m_pGimmickEncountActor[idx]);
  case GimmickEncount::GIMMICK_TYPE_ESCAPE:
    return reinterpret_cast<FieldGimmickEncountEscapeActor*>( m_pGimmickEncountActor[idx]);
  }
  GFL_ASSERT( 0);
  return NULL;
}

#if PM_DEBUG
/**
 *  @brief  デバッグ用ファインダー作成
 */
 void PlacementDataManager::Debug_CreateZonePokeFinder( FieldPokeFinderAccessor::PokeFinderData *pData, FieldCollisionAccessor::CollisionData *pColData )
{
  for( u32 i = 0; i < POKEFINDER_MAX; ++ i )
  {
    if( m_pPokeFinderActor[ i ] ){ continue; }

    FieldPokeFinderActor::Debug_PokeFinderSetupData data;
    data.pCollisionSceneForEventTalk   = m_pCollisionSceneForEventTalk;
    data.pHeap                         = m_pActorHeap;
    data.pData                         = pData;
    data.pColData                      = pColData;

    FieldPokeFinderActor *pActor = GFL_NEW( m_pActorHeap ) FieldPokeFinderActor();
    pActor->Debug_Initialize( data );
    pActor->RegistActor();
    m_pPokeFinderActor[ i ] = pActor;

    break;
  }
}
#endif


#if PM_DEBUG
/**
 *  @brief 配置用シンボルの整合性チェック
 */
void PlacementDataManager::debugCheckPlacementSymbol()
{
  GFL_ASSERT( Field::PLACEMENT_ROM_VERSION_SUN2 == VERSION_SUN2 );
  GFL_ASSERT( Field::PLACEMENT_ROM_VERSION_MOON2 == VERSION_MOON2 );

  GFL_ASSERT( Field::PLACEMENT_TIMEZONE_MORNING  == GameSys::TimeZone::MORNING );
  GFL_ASSERT( Field::PLACEMENT_TIMEZONE_NOON     == GameSys::TimeZone::NOON );
  GFL_ASSERT( Field::PLACEMENT_TIMEZONE_EVENING  == GameSys::TimeZone::EVENING );
  GFL_ASSERT( Field::PLACEMENT_TIMEZONE_NIGHT    == GameSys::TimeZone::NIGHT );
  GFL_ASSERT( Field::PLACEMENT_TIMEZONE_MIDNIGHT == GameSys::TimeZone::MIDNIGHT );
}
#endif



/**
 *  @brief StaticModelActorの生成
 */
FieldStaticModelActor * PlacementDataManager::craeteStaticModelActor( const FieldStaticModelAccessor::StaticModelData* staticModelData, FieldStaticModelAccessor* accessor )
{
  FieldStaticModelActor *pActor = GFL_NEW( m_pActorHeap) FieldStaticModelActor();

  gfl2::math::Vector3 pos( staticModelData->position[ 0 ], staticModelData->position[ 1 ], staticModelData->position[ 2 ] );
  gfl2::math::Quaternion qt( staticModelData->quaternion[ 0 ], staticModelData->quaternion[ 1 ], staticModelData->quaternion[ 2 ], staticModelData->quaternion[ 3 ] );

  FieldStaticModelActor::StaticModelSetupData setupData;
  setupData.pStaticModelManager = m_pStaticModelManager;
  setupData.pAccessor           = accessor;
  setupData.pCollisionScene     = m_pCollisionSceneForStaticModel;
  setupData.pData               = staticModelData;
  setupData.pHeap               = m_pActorHeap;
  setupData.vPos                = pos;
  setupData.Quaternion          = qt;
  setupData.pTerrainManager     = m_pTerrainManager;
  setupData.pRenderingPipeLine  = m_pRenderingPipeLine;
  setupData.pAllocator          = m_pAllocator;

  pActor->Initialize( setupData );
  // 追加
  if( isEventCreate( staticModelData->romVersion, staticModelData->flagwork, staticModelData->flagwork_num, RTIME_TYPE_ALL ) )
  {
    pActor->RegistActor();
  }
  return pActor;
}

/**
 *  @brief  StaticModelの破棄リクエスト
 */
void PlacementDataManager::requestDeleteStaticModels( u32 start_idx, u32 end_idx )
{
  for( u32 i = start_idx; i < end_idx; ++ i )
  {
    FieldStaticModelActor *pActor = reinterpret_cast<FieldStaticModelActor *>( m_pStaticModelActor[ i ] );
    // 生成されていない
    if( !pActor ){ continue; }
    // 既に削除されている
    if( !pActor->IsRegist() ){ continue; }
    pActor->RequestDelayTerminate();
  }
}

/**
 *  @brief  StaticModelの破棄処理
 */
void PlacementDataManager::deleteStaticModels( u32 start_idx, u32 end_idx )
{
  for( u32 i = start_idx; i < end_idx; ++ i )
  {
    if( m_pStaticModelActor[ i ] )
    {
      m_pStaticModelActor[ i ]->Terminate();
      GFL_SAFE_DELETE( m_pStaticModelActor[ i ] );
    }
  }
}

/** 
 *  @brief MoveModelActorの生成
 */
FieldMoveModelActor* PlacementDataManager::createMoveModelActor( const FieldMoveModelAccessor::MoveModelData* moveModelData, ZoneID zoneId, bool alias, const FieldContactPokemonAccessor::ContactPokemonData* contactPokemonData )
{
  FieldMoveModelActor::MoveModelSetupData setupData;
  setupData.pMoveModelManager               = m_pMoveModelManager;
  setupData.pCollisionSceneForTerrain       = m_pCollisionSceneForTerrain;
  setupData.pCollisionSceneForStaticModel   = m_pCollisionSceneForStaticModel;
  setupData.pCollisionSceneForEventPos      = m_pCollisionSceneForEventPos;
  setupData.pCollisionSceneForEventTalk     = m_pCollisionSceneForEventTalk;
  setupData.pCollisionSceneForEventPush     = m_pCollisionSceneForEventPush;
  setupData.pMoveModelData                  = moveModelData;
  setupData.pContactPokemonData             = contactPokemonData;                 // ふれあい用
  setupData.zoneId                          = zoneId;
  setupData.dataZoneId                      = zoneId;
  if(alias)
  {
    setupData.zoneId = static_cast<ZoneID>(ALIAS_CODE);
  }

  FieldMoveModelActor *pActor = GFL_NEW( m_pActorHeap ) FieldMoveModelActor();
  pActor->Initialize( setupData );

  return pActor;
}

/**
 *  @brief MoveModel CreateResource
 */
void PlacementDataManager::createMoveModelActorResource( FieldMoveModelActor* pActor, FieldMoveModelAccessor* moveModelAccessor )
{
  FieldMoveModelActor::MoveModelCreateResourceData setupData;
  setupData.pTerrainManager                 = m_pTerrainManager;
  setupData.pRenderingPipeLine              = m_pRenderingPipeLine;
  setupData.pAccessor                       = moveModelAccessor;
  setupData.cpEventWork                     = m_cpEventWork;
  pActor->CreateResource( setupData );
  // 追加
  const FieldMoveModelAccessor::MoveModelData *moveModelData = pActor->GetData();
  if( isEventCreate( moveModelData->romVersion, moveModelData->flagwork, moveModelData->flagwork_num, static_cast<ReverseTimeType>(moveModelData->reversetime_type) ) )
  {
    pActor->RegistActor();
  }
}

/**
 *  @brief MoveModelWorkの破棄
 */
void PlacementDataManager::deleteMoveModels( u32 start_idx, u32 end_idx )
{
  for( u32 i = start_idx; i < end_idx; ++ i )
  {
    if(m_pMoveModelActor[ i ]){
      m_pMoveModelActor[ i ]->Terminate();
      GFL_SAFE_DELETE( m_pMoveModelActor[ i ] );
    }
  }
}

/**
 *  @brief MoveModelResourceの破棄
 */
void PlacementDataManager::deleteMoveModelResources( u32 start_idx, u32 end_idx )
{
  for( u32 i = start_idx; i < end_idx; ++ i )
  {
    if(m_pMoveModelActor[ i ])
    {
      reinterpret_cast<FieldMoveModelActor*>(m_pMoveModelActor[ i ])->TerminateResource();
    }
  }
}


/** 
 *  @brief TranerActorの生成
 */
FieldTrainerActor* PlacementDataManager::createTrainerActor( const FieldTrainerAccessor::TrainerData* trainerData, FieldTrainerAccessor* trainerAccessor, ZoneID zoneId, bool alias )
{
  FieldTrainerActor::TrainerSetupData setupData;
  setupData.pMoveModelManager               = m_pMoveModelManager;
  setupData.pCollisionSceneForTerrain       = m_pCollisionSceneForTerrain;
  setupData.pCollisionSceneForStaticModel   = m_pCollisionSceneForStaticModel;
  setupData.pCollisionSceneForEventPos      = m_pCollisionSceneForEventPos;
  setupData.pCollisionSceneForEventTalk     = m_pCollisionSceneForEventTalk;
  setupData.pCollisionSceneForEventPush     = m_pCollisionSceneForEventPush;
  setupData.pCollisionSceneForTrainerEye    = m_pCollisionSceneForTrainerEye;
  setupData.pAccessor                       = trainerAccessor;
  setupData.pTrainerData                    = trainerData;
  setupData.zoneId                          = zoneId;
  setupData.dataZoneId                      = zoneId;
  if(alias)
  {
    setupData.zoneId = static_cast<ZoneID>(ALIAS_CODE);
  }

  FieldTrainerActor *pActor = GFL_NEW( m_pActorHeap ) FieldTrainerActor();
  pActor->Initialize( setupData );

  return pActor;
}

/**
 *  @brief Traner CreateResource
 */
void PlacementDataManager::createTrainerActorResource( FieldTrainerActor* pActor, FieldTrainerAccessor* trainerAccessor )
{
  FieldTrainerActor::TrainerCreateResourceData setupData;
  setupData.pTerrainManager                 = m_pTerrainManager;
  setupData.pRenderingPipeLine              = m_pRenderingPipeLine;
  setupData.pAccessor                       = trainerAccessor;
  setupData.cpEventWork                     = m_cpEventWork;
  setupData.pHeap                           = m_pActorHeap;
  pActor->CreateResource( setupData );
  // 追加
  const FieldTrainerAccessor::TrainerData *trainerData = pActor->GetData();
  if( isEventCreate( trainerData->romVersion, trainerData->flagwork, trainerData->flagwork_num, RTIME_TYPE_ALL ) )
  {
    pActor->RegistActor();
  }
}

/**
 *  @brief TranerWorkの破棄
 */
void PlacementDataManager::deleteTrainers( u32 start_idx, u32 end_idx )
{
  for( u32 i = start_idx; i < end_idx; ++ i )
  {
    if(m_pTrainerActor[ i ]){
      m_pTrainerActor[ i ]->Terminate();
      GFL_SAFE_DELETE( m_pTrainerActor[ i ] );
    }
  }
}

/**
 *  @brief TranerResourceの破棄
 */
void PlacementDataManager::deleteTrainerResources( u32 start_idx, u32 end_idx )
{
  for( u32 i = start_idx; i < end_idx; ++ i )
  {
    if(m_pTrainerActor[ i ])
    {
      FieldTrainerActor *pActor = reinterpret_cast<FieldTrainerActor *>( m_pTrainerActor[ i ] );
      pActor->TerminateResource();
    }
  }
}

#if PM_DEBUG
/**
 *  @brief  デバッグ用：StaticModelVisible
 */
void PlacementDataManager::DebugFunc_ChangeVisibleStaticModel( void )
{
  for( u32 i = 0; i < STATICMODEL_ACTOR_MAX; ++ i )
  {
    FieldStaticModelActorBase *pActor = reinterpret_cast<FieldStaticModelActorBase*>( m_pStaticModelActor[i] );
    if( !pActor ){ continue; }

    pActor->DebugFunc_ChangeVisible();
  }
  for( u32 i = 0; i < BERRY_POINT_ACTOR_MAX; ++ i )
  {
    FieldStaticModelActorBase *pActor = reinterpret_cast<FieldStaticModelActorBase*>( m_pBerryPointActor[i] );
    if( !pActor ){ continue; }

    pActor->DebugFunc_ChangeVisible();
  }
  for( u32 i = 0; i < FISHING_SPOT_ACTOR_MAX; ++ i )
  {
    FieldStaticModelActorBase *pActor = reinterpret_cast<FieldStaticModelActorBase*>( m_pFishingSpotActor[i] );
    if( !pActor ){ continue; }

    pActor->DebugFunc_ChangeVisible();
  }
}

/**
 *  @brief  デバッグ用：MoveModelVisible
 */
void PlacementDataManager::DebugFunc_ChangeVisibleMoveModel( void )
{
  for( u32 i = 0; i < MOVEMODEL_EVENT_ACTOR_MAX; ++ i )
  {
    FieldMoveModelActorBase *pActor = reinterpret_cast<FieldMoveModelActorBase*>( m_pMoveModelActor[i] );
    if( !pActor ){ continue; }

    pActor->DebugFunc_ChangeVisible();
  }
  for( u32 i = 0; i < TRAINER_ACTOR_MAX; ++ i )
  {
    FieldMoveModelActorBase *pActor = reinterpret_cast<FieldMoveModelActorBase*>( m_pTrainerActor[i] );
    if( !pActor ){ continue; }

    pActor->DebugFunc_ChangeVisible();
  }
}

#endif


GFL_NAMESPACE_END(Field);
