//======================================================================
/**
 * @file FieldPokeFinderActor.cpp
 * @date 2015/10/01 16:43:39
 * @author miyachi_soichi
 * @brief 配置：ポケファインダースターター
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// gfl2
#include <debug/include/gfl2_DebugPrint.h>
// GameSys
#include "GameSys/include/GameManager.h"
// System
#include "System/include/nijiAllocator.h"
#include "System/include/HeapDefine.h"
// event
#include "Field/FieldStatic/include/Event/FieldEventPokeFinderForPlacement.h"
// util
#include "Field/FieldRo/include/TrialModel/FieldFinderUtil.h"
// Script
#include "FieldScript/include/EventScriptCall.h"
#include "niji_conv_header/field/script/inc/finder_event.inc"
// self
#include "Field/FieldRo/include/PlacementData/actor/FieldPokeFinderActor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldCollisionAccessor.h"
#include "NetStatic/NetEvent/include/FatalErrorEvent.h"

GFL_NAMESPACE_BEGIN(Field)

/**
 * @class  FieldPokeFinderActor
 * @brief  ポケファインダー
 */
/**
 *  @brief  コンストラクタ
 */
FieldPokeFinderActor::FieldPokeFinderActor( void )
: IFieldActorBase()
, m_pHeap( NULL )
, m_nSpotID( 0 )
, m_nSubjectID( -1 )
, m_pData( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
FieldPokeFinderActor::~FieldPokeFinderActor( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 *  @param  setupData   初期化データ
 */
void FieldPokeFinderActor::Initialize( FieldPokeFinderActor::PokeFinderSetupData &setupData )
{
  m_pCollisionScene = setupData.pCollisionSceneForEventTalk;
  m_pHeap           = setupData.pHeap;
  m_nSpotID         = setupData.pData->spotID;
  m_nSubjectID      = setupData.pData->subjectID;
  m_pData           = setupData.pData;
  m_pMoveAData      = setupData.pAccessor->GetMoveAData(  *m_pData );
  m_pMoveBData      = setupData.pAccessor->GetMoveBData(  *m_pData );
  m_pWaitData       = setupData.pAccessor->GetWaitData(   *m_pData );
  m_pOnewayData     = setupData.pAccessor->GetOnewayData( *m_pData );

  gfl2::math::Vector3     pos( setupData.pData->position[ 0 ], setupData.pData->position[ 1 ], setupData.pData->position[ 2 ] );
  gfl2::math::Quaternion  qua( setupData.pData->quaternion[ 0 ], setupData.pData->quaternion[ 1 ], setupData.pData->quaternion[ 2 ], setupData.pData->quaternion[ 3 ]);
  m_vPos = pos;

  // コリジョン
  const void *colData = setupData.pAccessor->GetCollisionData( *setupData.pData );
  FieldCollisionAccessor colAccessor; colAccessor.Initialize( colData );
  m_Size = colAccessor.GetDataMax();
  m_ppActor = GFL_NEW_ARRAY( m_pHeap ) StaticActor*[m_Size];
  for( u32 i = 0; i < m_Size; ++ i )
  {
    // データ取得
    FieldCollisionAccessor::CollisionData col_form;
    colAccessor.GetData( i, &col_form );

    // アクターの生成
    StaticActor *pActor = m_pCollisionScene->CreateStaticActor( setupData.pAllocator, setupData.pHeap );

    // コリジョン設定
    FieldCollisionAccessor::CreateActor( pActor, col_form, m_vPos );
    pActor->SetUserData( reinterpret_cast<void*>( this ) );
    m_ppActor[ i ] = pActor;
  }
}

/**
 *  @brief  破棄
 */
void FieldPokeFinderActor::Terminate( void )
{
  UnregistActor();
  for( u32 i = 0; i < m_Size; ++ i )
  {
    if( !m_ppActor[ i ] ){ continue; }
    GFL_SAFE_DELETE( m_ppActor[ i ] );
  }
  GFL_SAFE_DELETE_ARRAY( m_ppActor );
}

/**
 *  @brief  追加
 */
void FieldPokeFinderActor::RegistActor( void )
{
  IFieldActorBase::RegistActor();
}

/**
 *  @brief  削除
 */
void FieldPokeFinderActor::UnregistActor( void )
{
  IFieldActorBase::UnregistActor();
}

/**
 *  @brief  起動
 */
bool FieldPokeFinderActor::Kick( const KickRequest &req, bool *p_outIsEventCreate )
{
  // イベント起動したかフラグをクリア
  if( p_outIsEventCreate ) *p_outIsEventCreate = false;
  // 起動条件を満たしていない
  if( IsInvalid() ){ return false; }

  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  u32 state = 0;
#if defined(GF_PLATFORM_CTR)
  gfl2::imagedb::ImageDBAccessor::imgDbResult result;
  gfl2::heap::HeapBase *pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD );
  u32 num = Field::TrialModel::FinderUtil::CheckSDCard( &result, pHeap );
  switch( result )
  {
    // 問題ない
  case imgdb::ResultSuccess:
    state = 0;
    break;

    // エラーだが続行するもの
  case imgdb::ResultErrorInsufficientStorageSpace:
    state = 1;
    break;
  case imgdb::ResultErrorOverNumImages:
    state = 2;
    break;

    // 致命傷
  case imgdb::ResultErrorFileOperationFailure:
    state = 4;
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::FatalError::FatalErrorEvent>( pGameManager->GetGameEventManager() );
    break;
  case imgdb::ResultErrorReadOnlyDbFile:
    state = 4;
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::FatalError::FatalErrorEvent>( pGameManager->GetGameEventManager() );
    break;
  case imgdb::ResultErrorNotExistStorage:
    state = 4;
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::FatalError::FatalErrorEvent>( pGameManager->GetGameEventManager() );
    break;
  case imgdb::ResultErrorWriteProtect:
    state = 4;
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::FatalError::FatalErrorEvent>( pGameManager->GetGameEventManager() );
    break;
  case imgdb::ResultErrorNotAvailableStorage:
    state = 4;
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::FatalError::FatalErrorEvent>( pGameManager->GetGameEventManager() );
    break;

    // その他SDカードエラー
  default:
    state = 3;
    break;
  }
#endif
  if( state < 4 )
  {
    EventScriptCall::CallScript( pGameManager, SCRID_FINDER_EVENT, this, NULL, m_nSpotID, state );

    if( p_outIsEventCreate ) *p_outIsEventCreate = true;
    MIYACHI_PRINT( "FieldPokeFinderActor::Kick\n" );
  }

  return true;
}

/**
 *  @brief  判定
 */
bool FieldPokeFinderActor::IsInvalid( void )
{
  return false;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Debug Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#if PM_DEBUG
/**
 *  @brief  デバッグ用初期化
 */
void FieldPokeFinderActor::Debug_Initialize( Debug_PokeFinderSetupData &setupData )
{
  m_pCollisionScene = setupData.pCollisionSceneForEventTalk;
  m_pHeap           = setupData.pHeap;
  m_nSpotID         = setupData.spot_id;
  m_nSubjectID      = setupData.pData->subjectID;

  gfl2::math::Vector3     pos( setupData.pData->position[ 0 ], setupData.pData->position[ 1 ], setupData.pData->position[ 2 ] );
  gfl2::math::Quaternion  qua( setupData.pData->quaternion[ 0 ], setupData.pData->quaternion[ 1 ], setupData.pData->quaternion[ 2 ], setupData.pData->quaternion[ 3 ]);
  m_vPos = pos;

  // コリジョン
  m_Size = 1;
  m_ppActor = GFL_NEW_ARRAY( m_pHeap ) StaticActor*[m_Size];
  for( u32 i = 0; i < m_Size; ++ i )
  {
    // データ取得
    FieldCollisionAccessor::CollisionData &col_form = *setupData.pColData;

    // アクターの生成
    StaticActor *pActor = m_pCollisionScene->CreateStaticActor( setupData.pAllocator, setupData.pHeap );

    // コリジョン設定
    FieldCollisionAccessor::CreateActor( pActor, col_form, m_vPos );
    pActor->SetUserData( reinterpret_cast<void*>( this ) );
    m_ppActor[ i ] = pActor;
  }
}
#endif
GFL_NAMESPACE_END(mynamespace)
