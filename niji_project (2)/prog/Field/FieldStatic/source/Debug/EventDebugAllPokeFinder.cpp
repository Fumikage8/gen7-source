//======================================================================
/**
 * @file EventDebugAllPokeFinder.cpp
 * @date 2016/02/04 16:01:59
 * @author miyachi_soichi
 * @brief デバッグ：全ファインダー起動
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG
// self
#include "Field/FieldStatic/include/Debug/EventDebugAllPokeFinder.h"
// gamesys
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
// field
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
// placement
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldPokeFinderActor.h"
// trialmodel
#include "Field/FieldRo/include/TrialModel/FieldFinderModel_define.h"
// script
#include "FieldScript/include/FieldScriptSystem.h"
#include "Field/FieldStatic/include/Event/EventMapJump.h"
#include "FieldScript/include/EventScriptCall.h"
#include "niji_conv_header/field/script/inc/finder_event.inc"
#include "Field/FieldStatic/include/Event/FieldEventPokeFinderForPlacement.h"
// data
#include <niji_conv_header/field/zone/zone_id.h>
#include <niji_conv_header/field/world/world_id.h>
// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 *  @class  EventDebugAllPokeFinder
 *  @brief デバッグ：全ファインダー起動
 */
/**
 *  @brief  コンストラクタ
 *  @param  pHeap   workようヒープ
 */
EventDebugAllPokeFinder::EventDebugAllPokeFinder( gfl2::heap::HeapBase *pHeap )
: GameSys::GameEvent( pHeap )
, m_bLottery( false )
{
}

/**
 *  @brief  イベントを動かしてよいかどうかの判定関数
 *  @param  pGameManager  ゲームマネージャ
 *  @retval true  起動して良い
 *  @retval false 起動してはいけない
 */
bool EventDebugAllPokeFinder::BootChk( GameSys::GameManager *pGameManager )
{
  return true;
}

/**
 *  @brief  初期化関数
 *  @param  pGameManager  ゲームマネージャ
 */
void EventDebugAllPokeFinder::InitFunc( GameSys::GameManager *pGameManager )
{
  m_zoneID = 0;
  m_actorIdx = 0;
  Field::Debug::DebugTypes::s_DebugFinderParam.nTutorial = 6;
  Field::Debug::DebugTypes::s_DebugFinderParam.bLoop = 0;
  Field::Debug::DebugTypes::s_DebugFinderParam.bLotteryLock = m_bLottery? 0: 1;
  Field::Debug::DebugTypes::s_DebugFinderParam.nPokeIdx = 0;
  Field::Debug::DebugTypes::s_DebugFinderParam.nForceStop = 1;
}

/**
 *  @brief  メイン関数
 *  @param  pGameManager    ゲームマネージャ
 *  @return GMEVENT_RESUTLT イベント制御関数の戻り値
 */
GameSys::GMEVENT_RESULT EventDebugAllPokeFinder::MainFunc( GameSys::GameManager *pGameManager )
{
  switch( GetSeqNo() )
  {
  case SEQ_CheckZone:
    {
      Field::ZoneDataLoader *pZdl = pGameManager->GetGameData()->GetFieldZoneDataLoader();
      if( pZdl->GetZoneNum() > m_zoneID )
      {
        if( checkZone( pGameManager, m_zoneID ) )
        {
          SetSeqNo( SEQ_MapJump );
        }
        else
        {
          SetSeqNo( SEQ_AddZone );
        }
      }
      else
      {
        SetSeqNo( SEQ_End );
      }
    }
    break;
  case SEQ_MapJump:
    {
      GFL_PRINT( "AllPokeFinderTest::ZoneID[%d]\n", m_zoneID );
      EventMapJump::CallMapJumpDefaultPos( pGameManager, m_zoneID );
      AddSeqNo();
    }
    break;
  case SEQ_CallFinder:
    {
      Field::FieldResident *pResident = pGameManager->GetFieldResident();
      Field::PlacementDataManager *pPlacementDataManager = pResident->GetPlacementDataManager();
      Field::FieldPokeFinderActor *pActor = pPlacementDataManager->SearchZonePokeFinderFromIndex( m_actorIdx );
      if( pActor )
      {
        m_spotID = pActor->GetSpotID();
        Event::EventPokeFinderForPlacement::SetupData data;
        data.spot_id = pActor->GetSpotID();
        Event::EventPokeFinderForPlacement *pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Event::EventPokeFinderForPlacement>( pGameManager->GetGameEventManager() );
        pEvent->Initialize( data );

        GFL_PRINT( "AllPokeFinderTest::SpotID[%d]::PokeIdx[%d]\n", data.spot_id, Field::Debug::DebugTypes::s_DebugFinderParam.nPokeIdx );
        AddSeqNo();
      }
      else
      {
        SetSeqNo( SEQ_AddZone );
      }
    }
    break;
  case SEQ_AddIdx:
    {
      if( checkAddIdx( pGameManager ) )
      {
        // 次のアクターへ
        AddSeqNo();
      }
      else
      {
        // もう一回
        Field::Debug::DebugTypes::s_DebugFinderParam.nPokeIdx += 1;
        SetSeqNo( SEQ_CallFinder );
      }
    }
    break;
  case SEQ_AddActor:
    {
      Field::Debug::DebugTypes::s_DebugFinderParam.nPokeIdx = 0;
      Field::Debug::DebugTypes::s_DebugFinderParam.nMaxPokeIdx = 0;
      ++ m_actorIdx;
      for( u32 i = 0; i < 20; ++ i )
      {
        m_bAdvent[i] = false;
      }
      SetSeqNo( SEQ_CallFinder );
    }
    break;
  case SEQ_AddZone:
    {
      //予約スクリプトを破棄しないと、イベント状態が解除されないので多重登録アサートに引っかかる
      pGameManager->GetFieldScriptSystem()->SetReserveScript( FieldScript::SCRID_DUMMY );
      m_actorIdx = 0;
      ++ m_zoneID;
      SetSeqNo( SEQ_CheckZone );
    }
    break;
  case SEQ_End:
    GFL_PRINT( "AllPokeFinderTest SUCCESS\n" );
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

/**
 *  @brief  終了関数
 *  @param  pGameManager    ゲームマネージャ
 */
void EventDebugAllPokeFinder::EndFunc( GameSys::GameManager *pGameManager )
{
  Field::Debug::DebugTypes::s_DebugFinderParam.nTutorial = 0;
  Field::Debug::DebugTypes::s_DebugFinderParam.nForceStop = 0;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  ゾーンのチェック
 */
bool EventDebugAllPokeFinder::checkZone( GameSys::GameManager *pGameManager, u32 zone )
{
  ZoneDataLoader* pZoneData = pGameManager->GetGameData()->GetFieldZoneDataLoader();
  u16 world_id = pZoneData->GetZoneWorldId( zone );

  // ワールドに所属していない
  if( world_id == WORLD_ERROR ){ return false; }

  // ジョインフェスタはスキップする
  if( zone == Z_F0601JF0101 ){ return false; }
  if( zone == Z_F0601JF0102 ){ return false; }
  if( zone == Z_F0601JF0103 ){ return false; }

  // テストゾーンを対象外とする
  //if( zone >= Z_TESTPL1701 ){ return false; }

  // チェック対象
  return true;
}

/**
 *  @brief  進捗チェック
 */
bool EventDebugAllPokeFinder::checkAddIdx( GameSys::GameManager *pGameManager )
{
  bool bResult = false;

  if( !m_bLottery )
  {
    // 全チェックの場合
    if( Field::Debug::DebugTypes::s_DebugFinderParam.nPokeIdx >= Field::Debug::DebugTypes::s_DebugFinderParam.nMaxPokeIdx )
    {
      // 次のアクターへ行く
      bResult = true;
    }
  }
  else
  {
    // 抽選チェックの場合
    Field::TrialModel::FinderSpotReserveData *pReserveData = pGameManager->GetGameData()->GetFinderSpotReserveData( m_spotID );
    for( u32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
    {
      s32 monsID = pReserveData->monsIdx[i];
      if( monsID != -1 )
      {
        m_bAdvent[ monsID ] = true;
      }
    }
    bool bExit = true;
    for( u32 i = 0; i < 20; ++ i )
    {
      if( Field::Debug::DebugTypes::s_DebugFinderParam.nMaxPokeIdx <= i ){ continue; }
      if( !m_bAdvent[i] )
      {
        bExit = false;
        break;
      }
    }
    if( bExit ){ bResult = true; }
  }

  return bResult;
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
