// ============================================================================
/*
 * @file JoinFestaRecordRankingManager.cpp
 * @brief レコードランキング
 * @date 2016.02.09
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordRankingManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "Savedata/include/EventWork.h"
#include "Savedata/include/MyStatus.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


JoinFestaRecordRankingManager::JoinFestaRecordRankingManager() :
  m_aGuestBattleRecordRanking(),
  m_aGuestTradeRecordRanking(),
  m_aGuestGreedRecordRanking(),
  m_aGuestPokemonRecordRanking(),
  m_aGuestDailyRecordRanking(),
  m_aGuestCircleRecordRanking(),
  m_aGuestVarietyRecordRanking(),
  m_aGuestTotalRecordRanking(),
  m_aVipBattleRecordRanking(),
  m_aVipTradeRecordRanking(),
  m_aVipGreedRecordRanking(),
  m_aVipPokemonRecordRanking(),
  m_aVipDailyRecordRanking(),
  m_aVipCircleRecordRanking(),
  m_aVipVarietyRecordRanking(),
  m_aVipTotalRecordRanking(),
  m_GuestRecordRankingLength( 0 ),
  m_VipRecordRankingLength( 0 )
{
}


JoinFestaRecordRankingManager::~JoinFestaRecordRankingManager()
{
}


void JoinFestaRecordRankingManager::UpdateRankingList( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData )
{
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );

  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pVipList = pJoinFestaPersonalDataManager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pGuestList = pJoinFestaPersonalDataManager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );

  u32 size = sizeof( JoinFestaRecordRankingData ) * ARRAY_MAX;

  gfl2::std::MemClear( m_aGuestBattleRecordRanking, size );
  gfl2::std::MemClear( m_aGuestTradeRecordRanking, size );
  gfl2::std::MemClear( m_aGuestGreedRecordRanking, size );
  gfl2::std::MemClear( m_aGuestPokemonRecordRanking, size );
  gfl2::std::MemClear( m_aGuestDailyRecordRanking, size );
  gfl2::std::MemClear( m_aGuestCircleRecordRanking, size );
  gfl2::std::MemClear( m_aGuestVarietyRecordRanking, size );
  gfl2::std::MemClear( m_aGuestTotalRecordRanking, size );

  gfl2::std::MemClear( m_aVipBattleRecordRanking, size );
  gfl2::std::MemClear( m_aVipTradeRecordRanking, size );
  gfl2::std::MemClear( m_aVipGreedRecordRanking, size );
  gfl2::std::MemClear( m_aVipPokemonRecordRanking, size );
  gfl2::std::MemClear( m_aVipDailyRecordRanking, size );
  gfl2::std::MemClear( m_aVipCircleRecordRanking, size );
  gfl2::std::MemClear( m_aVipVarietyRecordRanking, size );
  gfl2::std::MemClear( m_aVipTotalRecordRanking, size );

  // -----------------------------------------------------------------------------------------------------

  u32 i = 0;

  m_VipRecordRankingLength = pVipList->Size();

  for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = pVipList->Begin(); it != pVipList->End(); ++it )
  {
    m_aVipBattleRecordRanking[i].friendKey  = (*it)->GetJoinFestaFriendKey();
    m_aVipBattleRecordRanking[i].value      = CalcBattleRecordRankValue( (*it) );

    m_aVipTradeRecordRanking[i].friendKey   = (*it)->GetJoinFestaFriendKey();
    m_aVipTradeRecordRanking[i].value       = CalcTradeRecordRankValue( (*it) );

    m_aVipGreedRecordRanking[i].friendKey   = (*it)->GetJoinFestaFriendKey();
    m_aVipGreedRecordRanking[i].value       = CalcGreedRecordRankValue( (*it) );

    m_aVipPokemonRecordRanking[i].friendKey = (*it)->GetJoinFestaFriendKey();
    m_aVipPokemonRecordRanking[i].value     = CalcPokemonRecordRankValue( (*it) );

    m_aVipDailyRecordRanking[i].friendKey   = (*it)->GetJoinFestaFriendKey();
    m_aVipDailyRecordRanking[i].value       = CalcDailyRecordRankValue( (*it) );

    m_aVipCircleRecordRanking[i].friendKey  = (*it)->GetJoinFestaFriendKey();
    m_aVipCircleRecordRanking[i].value      = CalcCircleRecordRankValue( (*it) );

    m_aVipVarietyRecordRanking[i].friendKey = (*it)->GetJoinFestaFriendKey();
    m_aVipVarietyRecordRanking[i].value     = CalcVarietyRecordRankValue( (*it) );
    
    ++i;
  }

  if( pJoinFestaPersonalData )
  {
    m_aVipBattleRecordRanking[i].friendKey  = pJoinFestaPersonalData->GetJoinFestaFriendKey();
    m_aVipBattleRecordRanking[i].value      = CalcBattleRecordRankValue( pJoinFestaPersonalData );

    m_aVipTradeRecordRanking[i].friendKey   = pJoinFestaPersonalData->GetJoinFestaFriendKey();
    m_aVipTradeRecordRanking[i].value       = CalcTradeRecordRankValue( pJoinFestaPersonalData );

    m_aVipGreedRecordRanking[i].friendKey   = pJoinFestaPersonalData->GetJoinFestaFriendKey();
    m_aVipGreedRecordRanking[i].value       = CalcGreedRecordRankValue( pJoinFestaPersonalData );

    m_aVipPokemonRecordRanking[i].friendKey = pJoinFestaPersonalData->GetJoinFestaFriendKey();
    m_aVipPokemonRecordRanking[i].value     = CalcPokemonRecordRankValue( pJoinFestaPersonalData );

    m_aVipDailyRecordRanking[i].friendKey   = pJoinFestaPersonalData->GetJoinFestaFriendKey();
    m_aVipDailyRecordRanking[i].value       = CalcDailyRecordRankValue( pJoinFestaPersonalData );

    m_aVipCircleRecordRanking[i].friendKey  = pJoinFestaPersonalData->GetJoinFestaFriendKey();
    m_aVipCircleRecordRanking[i].value      = CalcCircleRecordRankValue( pJoinFestaPersonalData );

    m_aVipVarietyRecordRanking[i].friendKey = pJoinFestaPersonalData->GetJoinFestaFriendKey();
    m_aVipVarietyRecordRanking[i].value     = CalcVarietyRecordRankValue( pJoinFestaPersonalData );

    ++m_VipRecordRankingLength;
  }

  // -----------------------------------------------------------------------------------------------------

  i = 0;

  m_GuestRecordRankingLength = pGuestList->Size();

  for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = pGuestList->Begin(); it != pGuestList->End(); ++it )
  {
    m_aGuestBattleRecordRanking[i].friendKey  = (*it)->GetJoinFestaFriendKey();
    m_aGuestBattleRecordRanking[i].value      = CalcBattleRecordRankValue( (*it) );

    m_aGuestTradeRecordRanking[i].friendKey   = (*it)->GetJoinFestaFriendKey();
    m_aGuestTradeRecordRanking[i].value       = CalcTradeRecordRankValue( (*it) );

    m_aGuestGreedRecordRanking[i].friendKey   = (*it)->GetJoinFestaFriendKey();
    m_aGuestGreedRecordRanking[i].value       = CalcGreedRecordRankValue( (*it) );

    m_aGuestPokemonRecordRanking[i].friendKey = (*it)->GetJoinFestaFriendKey();
    m_aGuestPokemonRecordRanking[i].value     = CalcPokemonRecordRankValue( (*it) );

    m_aGuestDailyRecordRanking[i].friendKey   = (*it)->GetJoinFestaFriendKey();
    m_aGuestDailyRecordRanking[i].value       = CalcDailyRecordRankValue( (*it) );

    m_aGuestCircleRecordRanking[i].friendKey  = (*it)->GetJoinFestaFriendKey();
    m_aGuestCircleRecordRanking[i].value      = CalcCircleRecordRankValue( (*it) );

    m_aGuestVarietyRecordRanking[i].friendKey = (*it)->GetJoinFestaFriendKey();
    m_aGuestVarietyRecordRanking[i].value     = CalcVarietyRecordRankValue( (*it) );

    ++i;
  }

  if( pJoinFestaPersonalData )
  {
    m_aGuestBattleRecordRanking[i].friendKey  = pJoinFestaPersonalData->GetJoinFestaFriendKey();
    m_aGuestBattleRecordRanking[i].value      = CalcBattleRecordRankValue( pJoinFestaPersonalData );

    m_aGuestTradeRecordRanking[i].friendKey   = pJoinFestaPersonalData->GetJoinFestaFriendKey();
    m_aGuestTradeRecordRanking[i].value       = CalcTradeRecordRankValue( pJoinFestaPersonalData );

    m_aGuestGreedRecordRanking[i].friendKey   = pJoinFestaPersonalData->GetJoinFestaFriendKey();
    m_aGuestGreedRecordRanking[i].value       = CalcGreedRecordRankValue( pJoinFestaPersonalData );

    m_aGuestPokemonRecordRanking[i].friendKey = pJoinFestaPersonalData->GetJoinFestaFriendKey();
    m_aGuestPokemonRecordRanking[i].value     = CalcPokemonRecordRankValue( pJoinFestaPersonalData );

    m_aGuestDailyRecordRanking[i].friendKey   = pJoinFestaPersonalData->GetJoinFestaFriendKey();
    m_aGuestDailyRecordRanking[i].value       = CalcDailyRecordRankValue( pJoinFestaPersonalData );

    m_aGuestCircleRecordRanking[i].friendKey  = pJoinFestaPersonalData->GetJoinFestaFriendKey();
    m_aGuestCircleRecordRanking[i].value      = CalcCircleRecordRankValue( pJoinFestaPersonalData );

    m_aGuestVarietyRecordRanking[i].friendKey = pJoinFestaPersonalData->GetJoinFestaFriendKey();
    m_aGuestVarietyRecordRanking[i].value     = CalcVarietyRecordRankValue( pJoinFestaPersonalData );

    ++m_GuestRecordRankingLength;
  }

  // -----------------------------------------------------------------------------------------------------

  descendingSort( m_aGuestBattleRecordRanking, m_GuestRecordRankingLength );
  descendingSort( m_aGuestTradeRecordRanking, m_GuestRecordRankingLength );
  descendingSort( m_aGuestGreedRecordRanking, m_GuestRecordRankingLength );
  descendingSort( m_aGuestPokemonRecordRanking, m_GuestRecordRankingLength );
  descendingSort( m_aGuestDailyRecordRanking, m_GuestRecordRankingLength );
  descendingSort( m_aGuestCircleRecordRanking, m_GuestRecordRankingLength );
  descendingSort( m_aGuestVarietyRecordRanking, m_GuestRecordRankingLength );

  descendingSort( m_aVipBattleRecordRanking, m_VipRecordRankingLength );
  descendingSort( m_aVipTradeRecordRanking, m_VipRecordRankingLength );
  descendingSort( m_aVipGreedRecordRanking, m_VipRecordRankingLength );
  descendingSort( m_aVipPokemonRecordRanking, m_VipRecordRankingLength );
  descendingSort( m_aVipDailyRecordRanking, m_VipRecordRankingLength );
  descendingSort( m_aVipCircleRecordRanking, m_VipRecordRankingLength );
  descendingSort( m_aVipVarietyRecordRanking, m_VipRecordRankingLength );

  // -----------------------------------------------------------------------------------------------------

  i = 0;

  for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = pVipList->Begin(); it != pVipList->End(); ++it )
  {
    m_aVipTotalRecordRanking[i].friendKey = (*it)->GetJoinFestaFriendKey();

    m_aVipTotalRecordRanking[i].value += GetVipRank( RECORD_CATEGORY_BATTLE,  m_aVipTotalRecordRanking[i].friendKey );
    m_aVipTotalRecordRanking[i].value += GetVipRank( RECORD_CATEGORY_TRADE,   m_aVipTotalRecordRanking[i].friendKey );
    m_aVipTotalRecordRanking[i].value += GetVipRank( RECORD_CATEGORY_GREED,   m_aVipTotalRecordRanking[i].friendKey );
    m_aVipTotalRecordRanking[i].value += GetVipRank( RECORD_CATEGORY_POKEMON, m_aVipTotalRecordRanking[i].friendKey );
    m_aVipTotalRecordRanking[i].value += GetVipRank( RECORD_CATEGORY_DAILY,   m_aVipTotalRecordRanking[i].friendKey );
    m_aVipTotalRecordRanking[i].value += GetVipRank( RECORD_CATEGORY_CIRCLE,  m_aVipTotalRecordRanking[i].friendKey );
    m_aVipTotalRecordRanking[i].value += GetVipRank( RECORD_CATEGORY_VARIETY, m_aVipTotalRecordRanking[i].friendKey );

    ++i;
  }

  if( pJoinFestaPersonalData )
  {
    m_aVipTotalRecordRanking[i].friendKey = pJoinFestaPersonalData->GetJoinFestaFriendKey();

    m_aVipTotalRecordRanking[i].value += GetVipRank( RECORD_CATEGORY_BATTLE,  m_aVipTotalRecordRanking[i].friendKey );
    m_aVipTotalRecordRanking[i].value += GetVipRank( RECORD_CATEGORY_TRADE,   m_aVipTotalRecordRanking[i].friendKey );
    m_aVipTotalRecordRanking[i].value += GetVipRank( RECORD_CATEGORY_GREED,   m_aVipTotalRecordRanking[i].friendKey );
    m_aVipTotalRecordRanking[i].value += GetVipRank( RECORD_CATEGORY_POKEMON, m_aVipTotalRecordRanking[i].friendKey );
    m_aVipTotalRecordRanking[i].value += GetVipRank( RECORD_CATEGORY_DAILY,   m_aVipTotalRecordRanking[i].friendKey );
    m_aVipTotalRecordRanking[i].value += GetVipRank( RECORD_CATEGORY_CIRCLE,  m_aVipTotalRecordRanking[i].friendKey );
    m_aVipTotalRecordRanking[i].value += GetVipRank( RECORD_CATEGORY_VARIETY, m_aVipTotalRecordRanking[i].friendKey );
  }

  ascendingSort( m_aVipTotalRecordRanking, m_VipRecordRankingLength );

  // -----------------------------------------------------------------------------------------------------

  i = 0;

  for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = pGuestList->Begin(); it != pGuestList->End(); ++it )
  {
    m_aGuestTotalRecordRanking[i].friendKey = (*it)->GetJoinFestaFriendKey();

    m_aGuestTotalRecordRanking[i].value += GetGuestRank( RECORD_CATEGORY_BATTLE,  m_aGuestTotalRecordRanking[i].friendKey );
    m_aGuestTotalRecordRanking[i].value += GetGuestRank( RECORD_CATEGORY_TRADE,   m_aGuestTotalRecordRanking[i].friendKey );
    m_aGuestTotalRecordRanking[i].value += GetGuestRank( RECORD_CATEGORY_GREED,   m_aGuestTotalRecordRanking[i].friendKey );
    m_aGuestTotalRecordRanking[i].value += GetGuestRank( RECORD_CATEGORY_POKEMON, m_aGuestTotalRecordRanking[i].friendKey );
    m_aGuestTotalRecordRanking[i].value += GetGuestRank( RECORD_CATEGORY_DAILY,   m_aGuestTotalRecordRanking[i].friendKey );
    m_aGuestTotalRecordRanking[i].value += GetGuestRank( RECORD_CATEGORY_CIRCLE,  m_aGuestTotalRecordRanking[i].friendKey );
    m_aGuestTotalRecordRanking[i].value += GetGuestRank( RECORD_CATEGORY_VARIETY, m_aGuestTotalRecordRanking[i].friendKey );

    ++i;
  }

  if( pJoinFestaPersonalData )
  {
    m_aGuestTotalRecordRanking[i].friendKey = pJoinFestaPersonalData->GetJoinFestaFriendKey();

    m_aGuestTotalRecordRanking[i].value += GetGuestRank( RECORD_CATEGORY_BATTLE,  m_aGuestTotalRecordRanking[i].friendKey );
    m_aGuestTotalRecordRanking[i].value += GetGuestRank( RECORD_CATEGORY_TRADE,   m_aGuestTotalRecordRanking[i].friendKey );
    m_aGuestTotalRecordRanking[i].value += GetGuestRank( RECORD_CATEGORY_GREED,   m_aGuestTotalRecordRanking[i].friendKey );
    m_aGuestTotalRecordRanking[i].value += GetGuestRank( RECORD_CATEGORY_POKEMON, m_aGuestTotalRecordRanking[i].friendKey );
    m_aGuestTotalRecordRanking[i].value += GetGuestRank( RECORD_CATEGORY_DAILY,   m_aGuestTotalRecordRanking[i].friendKey );
    m_aGuestTotalRecordRanking[i].value += GetGuestRank( RECORD_CATEGORY_CIRCLE,  m_aGuestTotalRecordRanking[i].friendKey );
    m_aGuestTotalRecordRanking[i].value += GetGuestRank( RECORD_CATEGORY_VARIETY, m_aGuestTotalRecordRanking[i].friendKey );
  }

  ascendingSort( m_aGuestTotalRecordRanking, m_GuestRecordRankingLength );

#if PM_DEBUG
  GFL_RELEASE_PRINT( "------------------------------------------\n" );
  GFL_RELEASE_PRINT( "m_aGuestBattleRecordRanking\n" );
  debugPrint( m_aGuestBattleRecordRanking, m_GuestRecordRankingLength );
  GFL_RELEASE_PRINT( "------------------------------------------\n" );
  GFL_RELEASE_PRINT( "m_aGuestTradeRecordRanking\n" );
  debugPrint( m_aGuestTradeRecordRanking, m_GuestRecordRankingLength );
  GFL_RELEASE_PRINT( "------------------------------------------\n" );
  GFL_RELEASE_PRINT( "m_aGuestGreedRecordRanking\n" );
  debugPrint( m_aGuestGreedRecordRanking, m_GuestRecordRankingLength );
  GFL_RELEASE_PRINT( "------------------------------------------\n" );
  GFL_RELEASE_PRINT( "m_aGuestPokemonRecordRanking\n" );
  debugPrint( m_aGuestPokemonRecordRanking, m_GuestRecordRankingLength );
  GFL_RELEASE_PRINT( "------------------------------------------\n" );
  GFL_RELEASE_PRINT( "m_aGuestDailyRecordRanking\n" );
  debugPrint( m_aGuestDailyRecordRanking, m_GuestRecordRankingLength );
  GFL_RELEASE_PRINT( "------------------------------------------\n" );
  GFL_RELEASE_PRINT( "m_aGuestCircleRecordRanking\n" );
  debugPrint( m_aGuestCircleRecordRanking, m_GuestRecordRankingLength );
  GFL_RELEASE_PRINT( "------------------------------------------\n" );
  GFL_RELEASE_PRINT( "m_aGuestVarietyRecordRanking\n" );
  debugPrint( m_aGuestVarietyRecordRanking, m_GuestRecordRankingLength );
  GFL_RELEASE_PRINT( "------------------------------------------\n" );
  GFL_RELEASE_PRINT( "m_aGuestTotalRecordRanking\n" );
  debugPrint( m_aGuestTotalRecordRanking, m_GuestRecordRankingLength );

  GFL_RELEASE_PRINT( "------------------------------------------\n" );
  GFL_RELEASE_PRINT( "m_aVipBattleRecordRanking\n" );
  debugPrint( m_aVipBattleRecordRanking, m_VipRecordRankingLength );
  GFL_RELEASE_PRINT( "------------------------------------------\n" );
  GFL_RELEASE_PRINT( "m_aVipTradeRecordRanking\n" );
  debugPrint( m_aVipTradeRecordRanking, m_VipRecordRankingLength );
  GFL_RELEASE_PRINT( "------------------------------------------\n" );
  GFL_RELEASE_PRINT( "m_aVipGreedRecordRanking\n" );
  debugPrint( m_aVipGreedRecordRanking, m_VipRecordRankingLength );
  GFL_RELEASE_PRINT( "------------------------------------------\n" );
  GFL_RELEASE_PRINT( "m_aVipPokemonRecordRanking\n" );
  debugPrint( m_aVipPokemonRecordRanking, m_VipRecordRankingLength );
  GFL_RELEASE_PRINT( "------------------------------------------\n" );
  GFL_RELEASE_PRINT( "m_aVipDailyRecordRanking\n" );
  debugPrint( m_aVipDailyRecordRanking, m_VipRecordRankingLength );
  GFL_RELEASE_PRINT( "------------------------------------------\n" );
  GFL_RELEASE_PRINT( "m_aVipCircleRecordRanking\n" );
  debugPrint( m_aVipCircleRecordRanking, m_VipRecordRankingLength );
  GFL_RELEASE_PRINT( "------------------------------------------\n" );
  GFL_RELEASE_PRINT( "m_aVipVarietyRecordRanking\n" );
  debugPrint( m_aVipVarietyRecordRanking, m_VipRecordRankingLength );
  GFL_RELEASE_PRINT( "------------------------------------------\n" );
  GFL_RELEASE_PRINT( "m_aVipTotalRecordRanking\n" );
  debugPrint( m_aVipTotalRecordRanking, m_VipRecordRankingLength );
#endif // PM_DEBUG

}


u32 JoinFestaRecordRankingManager::GetGuestRank( RecordCategory eRecordCategory, JoinFestaFriendKey friendKey )
{
  u32 rank = 1;

  switch( eRecordCategory )
  {
    case RECORD_CATEGORY_BATTLE:  rank = getRank( friendKey, m_aGuestBattleRecordRanking,  m_GuestRecordRankingLength ); break;
    case RECORD_CATEGORY_TRADE:   rank = getRank( friendKey, m_aGuestTradeRecordRanking,   m_GuestRecordRankingLength ); break;
    case RECORD_CATEGORY_GREED:   rank = getRank( friendKey, m_aGuestGreedRecordRanking,   m_GuestRecordRankingLength ); break;
    case RECORD_CATEGORY_POKEMON: rank = getRank( friendKey, m_aGuestPokemonRecordRanking, m_GuestRecordRankingLength ); break;
    case RECORD_CATEGORY_DAILY:   rank = getRank( friendKey, m_aGuestDailyRecordRanking,   m_GuestRecordRankingLength ); break;
    case RECORD_CATEGORY_CIRCLE:  rank = getRank( friendKey, m_aGuestCircleRecordRanking,  m_GuestRecordRankingLength ); break;
    case RECORD_CATEGORY_VARIETY: rank = getRank( friendKey, m_aGuestVarietyRecordRanking, m_GuestRecordRankingLength ); break;
    case RECORD_CATEGORY_TOTAL:   rank = getRank( friendKey, m_aGuestTotalRecordRanking,   m_GuestRecordRankingLength ); break;
  }

  return rank;
}


u32 JoinFestaRecordRankingManager::GetVipRank( RecordCategory eRecordCategory, JoinFestaFriendKey friendKey )
{
  u32 rank = 1;

  switch( eRecordCategory )
  {
    case RECORD_CATEGORY_BATTLE:  rank = getRank( friendKey, m_aVipBattleRecordRanking,  m_VipRecordRankingLength ); break;
    case RECORD_CATEGORY_TRADE:   rank = getRank( friendKey, m_aVipTradeRecordRanking,   m_VipRecordRankingLength ); break;
    case RECORD_CATEGORY_GREED:   rank = getRank( friendKey, m_aVipGreedRecordRanking,   m_VipRecordRankingLength ); break;
    case RECORD_CATEGORY_POKEMON: rank = getRank( friendKey, m_aVipPokemonRecordRanking, m_VipRecordRankingLength ); break;
    case RECORD_CATEGORY_DAILY:   rank = getRank( friendKey, m_aVipDailyRecordRanking,   m_VipRecordRankingLength ); break;
    case RECORD_CATEGORY_CIRCLE:  rank = getRank( friendKey, m_aVipCircleRecordRanking,  m_VipRecordRankingLength ); break;
    case RECORD_CATEGORY_VARIETY: rank = getRank( friendKey, m_aVipVarietyRecordRanking, m_VipRecordRankingLength ); break;
    case RECORD_CATEGORY_TOTAL:   rank = getRank( friendKey, m_aVipTotalRecordRanking,   m_VipRecordRankingLength ); break;
  }

  return rank;
}


u32 JoinFestaRecordRankingManager::getRank( JoinFestaFriendKey friendKey, JoinFestaRecordRankingData* pRecordRankingArray, u32 length )
{
  u32 rank = 1;
  
  for( u32 i = 0; i < length; ++i )
  {
    if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( friendKey, pRecordRankingArray[i].friendKey ) )
    {
      rank = pRecordRankingArray[i].rank;
      
      break;
    }
  }

  return rank;
}


void JoinFestaRecordRankingManager::descendingSort( JoinFestaRecordRankingData* pRecordRankingArray, u32 length )
{
  JoinFestaRecordRankingData temp;
  temp.rank = 0;

  for( u32 i = 0; i < length; ++i )
  {
    for( u32 j = i + 1; j < length; ++j )
    {
      if( pRecordRankingArray[i].value < pRecordRankingArray[j].value )
      {
        temp.friendKey                   = pRecordRankingArray[i].friendKey;
        temp.value                       = pRecordRankingArray[i].value;

        pRecordRankingArray[i].friendKey = pRecordRankingArray[j].friendKey;
        pRecordRankingArray[i].value     = pRecordRankingArray[j].value;

        pRecordRankingArray[j].friendKey = temp.friendKey;
        pRecordRankingArray[j].value     = temp.value;
      }
    }
  }

  u32 rank = 1;
  u32 value = pRecordRankingArray[0].value;
  u32 sameRankNum = 0;

  for( u32 i = 0; i < length; ++i )
  {
    if( value == pRecordRankingArray[i].value )
    {
      pRecordRankingArray[i].rank = rank;

      ++sameRankNum;
    }
    else
    {
      rank += sameRankNum;

      sameRankNum = 1;

      value = pRecordRankingArray[i].value;

      pRecordRankingArray[i].rank = rank;
    }
  }
}


void JoinFestaRecordRankingManager::ascendingSort( JoinFestaRecordRankingData* pRecordRankingArray, u32 length )
{
  JoinFestaRecordRankingData temp;
  temp.rank = 0;

  for( u32 i = 0; i < length; ++i )
  {
    for( u32 j = i + 1; j < length; ++j )
    {
      if( pRecordRankingArray[i].value > pRecordRankingArray[j].value )
      {
        temp.friendKey                   = pRecordRankingArray[i].friendKey;
        temp.value                       = pRecordRankingArray[i].value;

        pRecordRankingArray[i].friendKey = pRecordRankingArray[j].friendKey;
        pRecordRankingArray[i].value     = pRecordRankingArray[j].value;

        pRecordRankingArray[j].friendKey = temp.friendKey;
        pRecordRankingArray[j].value     = temp.value;
      }
    }
  }

  u32 rank = 1;
  u32 value = pRecordRankingArray[0].value;
  u32 sameRankNum = 0;

  for( u32 i = 0; i < length; ++i )
  {
    if( value == pRecordRankingArray[i].value )
    {
      pRecordRankingArray[i].rank = rank;

      ++sameRankNum;
    }
    else
    {
      rank += sameRankNum;

      sameRankNum = 1;

      value = pRecordRankingArray[i].value;

      pRecordRankingArray[i].rank = rank;
    }
  }
}


u64 JoinFestaRecordRankingManager::CalcBattleRecordRankValue( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData )
{
  Savedata::Record* pRecord = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetRecordSaveData();
  Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetEventWork();

  u64 value = 0;

  if( pJoinFestaPersonalData )
  {
    NetAppLib::JoinFesta::PacketGameStatus2 data2;
    NetAppLib::JoinFesta::PacketGameStatus3 data3;
    NetAppLib::JoinFesta::PacketGameStatus4 data4;

    bool bResult2 = pJoinFestaPersonalData->GetPacketGameStatus2( &data2 );
    bool bResult3 = pJoinFestaPersonalData->GetPacketGameStatus3( &data3 );
    bool bResult4 = pJoinFestaPersonalData->GetPacketGameStatus4( &data4 );

    bool bClearFlag_ALL = pEventWork->CheckEventFlag( SYS_FLAG_GAME_CLEAR ); // ストーリー全クリアフラグ

    if( bResult2 )
    {
      value += ( data2.record1 * 2 );   // 1 : 2 // RECID_COMM_BATTLE
      value += ( data2.record2 * 2 );   // 2 : 2 // RECID_WIFI_BATTLE
      
      if( bClearFlag_ALL )
      {
        value += ( data2.record3 * 2 ); // 3 : 2 // RECID_BTREE_CHALLENGE
      }

      value += ( data2.record4 * 2 );   // 4 : 2 // RECID_BTL_ENCOUNT

    }

    if( bResult4 )
    {
      if( bClearFlag_ALL )
      {
        value += ( data4.record1 * 1 ); // 5 : 1 // RECID_DENDOU_CNT
      }
    }

    if( bResult2 )
    {
      value += ( data2.record5 * 1 );   // 6 : 1 // RECID_WEAK_ATTACK
    }

    if( pRecord->Get( Savedata::Record::RECID_ZWAZA_CNT ) >= 1 )
    {
      if( bResult3 )
      {
        value += ( data3.record10 * 1 ); // 7 : 1 // RECID_ZWAZA_CNT
      }
    }
  }

  return value;
}


u64 JoinFestaRecordRankingManager::CalcTradeRecordRankValue( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData )
{
  u64 value = 0;

  if( pJoinFestaPersonalData )
  {
    NetAppLib::JoinFesta::PacketGameStatus2 data2;

    bool bResult2 = pJoinFestaPersonalData->GetPacketGameStatus2( &data2 );

    if( bResult2 )
    {
      value += ( data2.record6 * 4 );   // 1 : 4 // RECID_COMM_TRADE
      value += ( data2.record7 * 1 );   // 2 : 1 // RECID__MIRACLE_TRADE_COUNT
      value += ( data2.record8 * 4 );   // 3 : 4 // RECID_GTS_TRADE_COUNT
      value += ( data2.record9 * 1 );   // 4 : 1 // RECID_GTS_PUT
    }
  }

  return value;
}


u64 JoinFestaRecordRankingManager::CalcGreedRecordRankValue( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData )
{
  u64 value = 0;

  if( pJoinFestaPersonalData )
  {
    NetAppLib::JoinFesta::PacketGameStatus1 data1;
    NetAppLib::JoinFesta::PacketGameStatus2 data2;
    NetAppLib::JoinFesta::PacketGameStatus3 data3;

    bool bResult1 = pJoinFestaPersonalData->GetPacketGameStatus1( &data1 );
    bool bResult2 = pJoinFestaPersonalData->GetPacketGameStatus2( &data2 );
    bool bResult3 = pJoinFestaPersonalData->GetPacketGameStatus3( &data3 );

    if( bResult2 )
    {
      value += ( data2.record11 * 4 );   // 1 : 4 // RECID_MAME_GET_CNT
    }

    if( bResult3 )
    {
      value += ( data3.record1 * 4 );   // 2 : 4 // RECID_BERRY_HARVEST
      value += ( data3.record2 * 1 );   // 3 : 1 // RECID_SHOPPING_MONEY
      value += ( data3.record3 * 4 );   // 4 : 4 // RECID_USE_BP
    }

    // @fix : [niji] GFバグリスト #5676 パケットの受信に成功した時のみ、その値を加算するようにしました
    if( bResult1 )
    {
      const u32 DRESS_UP_SCALE_VALUE = 10;

      u32 u32AddDressUpValue = ( data1.haveDressUpItemNum * DRESS_UP_SCALE_VALUE ); // 5 : 10 // 着せ替え衣装の数
    
      u64 addDressUpValue = static_cast<u64>( u32AddDressUpValue );

      const NetAppLib::JoinFesta::PacketPersonalInfo& packetPersonalInfo = pJoinFestaPersonalData->GetPacketPersonalInfo();

      if( packetPersonalInfo.sex == PM_MALE )
      {
        // @fix : [niji] GFバグリスト #2084, [niji] System #1500 男性の場合は1.6倍
        float fAddDressUpValue = static_cast<float>( addDressUpValue );
      
        fAddDressUpValue *= 1.6f;

        addDressUpValue = static_cast<u64>( fAddDressUpValue );
      }

      value += addDressUpValue;
    }
  }

  return value;
}


u64 JoinFestaRecordRankingManager::CalcPokemonRecordRankValue( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData )
{
  u64 value = 0;

  if( pJoinFestaPersonalData )
  {
    NetAppLib::JoinFesta::PacketGameStatus3 data3;
    NetAppLib::JoinFesta::PacketGameStatus4 data4;

    bool bResult3 = pJoinFestaPersonalData->GetPacketGameStatus3( &data3 );
    bool bResult4 = pJoinFestaPersonalData->GetPacketGameStatus4( &data4 );

    if( bResult3 )
    {
      value += ( data3.record4 * 2 ); // 1 : 2 // RECID_CAPTURE_POKE
      value += ( data3.record5 * 2 ); // 2 : 2 // RECID_TAMAGO_HATCHING
    }

    if( bResult4 )
    {
      value += ( data4.record2 * 1 ); // 3 : 1 // RECID_KAWAIGARI_CNT
    }

    if( bResult3 )
    {
      value += ( data3.record6 * 2 ); // 4 : 2 // RECID_MAME_PRESENT_CNT
      value += ( data3.record7 * 2 ); // 5 : 2 // RECID_NICKNAME
      value += ( data3.record8 * 1 ); // 6 : 1 // RECID_RIDE_POKE
    }
  }

  return value;
}


u64 JoinFestaRecordRankingManager::CalcDailyRecordRankValue( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData )
{
  u64 value = 0;

  if( pJoinFestaPersonalData )
  {
    NetAppLib::JoinFesta::PacketGameStatus2 data2;
    NetAppLib::JoinFesta::PacketGameStatus4 data4;

    bool bResult2 = pJoinFestaPersonalData->GetPacketGameStatus2( &data2 );
    bool bResult4 = pJoinFestaPersonalData->GetPacketGameStatus4( &data4 );

    if( bResult2 )
    {
      value += ( data2.record10 * 1 ); // 1 : 1 // RECID_DAYMAX_EXP
    }

    if( bResult4 )
    {
      value += ( data4.record3 *  500 ); // 2 :  500 // RECID_DAYMAX_CAPTURE
      value += ( data4.record4 * 1000 ); // 3 : 1000 // RECID_DAYMAX_EVOLUTION
      value += ( data4.record5 *  100 ); // 4 :  100 // RECID_DAYMAX_KILL
      value += ( data4.record6 *  100 ); // 5 :  100 // RECID_DAYMAX_TRAINER_BATTLE
    }
  }

  return value;
}


u64 JoinFestaRecordRankingManager::CalcCircleRecordRankValue( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData )
{
  u64 value = 0;

  if( pJoinFestaPersonalData )
  {
    NetAppLib::JoinFesta::PacketGameStatus1 data1;
    NetAppLib::JoinFesta::PacketGameStatus3 data3;
    NetAppLib::JoinFesta::PacketGameStatus4 data4;

    bool bResult1 = pJoinFestaPersonalData->GetPacketGameStatus1( &data1 );
    bool bResult3 = pJoinFestaPersonalData->GetPacketGameStatus3( &data3 );
    bool bResult4 = pJoinFestaPersonalData->GetPacketGameStatus4( &data4 );

    // @fix : [niji] GFバグリスト #5676 パケットの受信に成功した時のみ、その値を加算するようにしました(集めた服以外にもパケットの受信の成功判定が抜けていたので追加)
    if( bResult1 )
    {
      value += ( data1.jfRank * 1000 ); // 1 : 1000
    }

    if( bResult4 )
    {
      value += ( data4.record7 * 5000 ); // 2 : 5000 // RECID_JF_ATT_CNT
      value += ( data4.record8 * 2500 ); // 3 : 2500 // RECID_JF_SHOP_GET_CNT
    }

    if( bResult3 )
    {
      value += ( data3.record11 * 1000 ); // 4 : 1000 // RECID_JF_GEST_TALK_CNT
      value += ( data3.record9 *     1 ); // 5 :    1 // RECID_JF_USE_JCOIN
    }
  }

  return value;
}


u64 JoinFestaRecordRankingManager::CalcVarietyRecordRankValue( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData )
{
  u64 value = 0;

  if( pJoinFestaPersonalData )
  {
    NetAppLib::JoinFesta::PacketGameStatus4 data4;

    bool bResult4 = pJoinFestaPersonalData->GetPacketGameStatus4( &data4 );

    if( bResult4 )
    {
      value += ( data4.record9  * 1000 ); // 1 : 1000 // RECID_RARE_COLOUR_ENC_COUNT
      value += ( data4.record10 *    1 ); // 2 :    1 // RECID_IDKUJI_ATARI_CNT
      value += ( data4.record11 *    1 ); // 3 :    1 // RECID_POINTUP_USE
      value += ( data4.record12 *    1 ); // 4 :    1 // RECID_KISEKAE_CNT
      value += ( data4.record13 *    1 ); // 5 :    1 // RECID_QR_SCAN
      value += ( data4.record14 *    1 ); // 6 :    1 // RECID_CAMERA_CNT
      // momiji追加：nijiからは0がくる。
      if( pJoinFestaPersonalData->GetRomVersion() == VERSION_SUN2 ||
          pJoinFestaPersonalData->GetRomVersion() == VERSION_MOON2 )
      {
        value += (data4.record15 * 1); // 7 :    1 // RECID_LUNASOL_BESTSCORE
        value += (data4.record16 * 1); // 8 :    1 // RECID_MANTAIN_SURF_BEST_BP
      }
    }
  }

  return value;
}


#if PM_DEBUG
void JoinFestaRecordRankingManager::debugPrint( JoinFestaRecordRankingData* pRecordRankingArray, u32 length )
{
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );

  u32 principalID = 0;

  gfl2::str::STRCODE name[ 64 ];

  for( u32 i = 0; i < length; ++i )
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData = pJoinFestaPersonalDataManager->GetPersonalData( pRecordRankingArray[i].friendKey );

    principalID = 0;

    gfl2::std::MemClear( name, sizeof( name ) );

    if( pJoinFestaPersonalData )
    {
      principalID = pJoinFestaPersonalData->GetPrincipalID();

      pJoinFestaPersonalData->GetName( name );
    }

    GFL_RELEASE_PRINT( "%d : rank = %d, val = %llu ", i, pRecordRankingArray[i].rank, pRecordRankingArray[i].value );
    gfl2::str::PrintCode( name, "名前：" );
  }
}
#endif // PM_DEBUG


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)
