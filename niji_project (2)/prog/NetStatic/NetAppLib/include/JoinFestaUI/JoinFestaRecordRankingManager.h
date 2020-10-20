// ============================================================================
/*
 * @file JoinFestaRecordRankingManager.h
 * @brief レコードランキング
 * @date 2016.02.09
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTARECORDRANKINGMANAGER_H_INCLUDE )
#define JOINFESTARECORDRANKINGMANAGER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaRecordRankingManager
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaRecordRankingManager );

public:

  enum RecordCategory
  {
    RECORD_CATEGORY_BATTLE    = JoinFestaScript::RECORD_CATEGORY_BATTLE,
    RECORD_CATEGORY_TRADE     = JoinFestaScript::RECORD_CATEGORY_TRADE,
    RECORD_CATEGORY_GREED     = JoinFestaScript::RECORD_CATEGORY_GREED,
    RECORD_CATEGORY_POKEMON   = JoinFestaScript::RECORD_CATEGORY_POKEMON,
    RECORD_CATEGORY_DAILY     = JoinFestaScript::RECORD_CATEGORY_DAILY,
    RECORD_CATEGORY_CIRCLE    = JoinFestaScript::RECORD_CATEGORY_CIRCLE,
    RECORD_CATEGORY_VARIETY   = JoinFestaScript::RECORD_CATEGORY_VARIETY,
    RECORD_CATEGORY_TOTAL     = JoinFestaScript::RECORD_CATEGORY_TOTAL,
    RECORD_CATEGORY_MAX
  };

private:

  typedef struct
  {
    JoinFestaFriendKey friendKey;
    u32                rank;
    u64                value;
  } JoinFestaRecordRankingData;

  enum
  {
    ARRAY_MAX = 51
  };

public:

  JoinFestaRecordRankingManager();
  virtual~JoinFestaRecordRankingManager();

  void UpdateRankingList( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData = NULL );

  u32 GetGuestRank( RecordCategory eRecordCategory, JoinFestaFriendKey friendKey );
  u32 GetVipRank( RecordCategory eRecordCategory, JoinFestaFriendKey friendKey );

  u32 GetGuestRankLength(){ return m_GuestRecordRankingLength; }
  u32 GetVipRankLength(){ return m_VipRecordRankingLength; }

  u64 CalcBattleRecordRankValue( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData );
  u64 CalcTradeRecordRankValue( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData );
  u64 CalcGreedRecordRankValue( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData );
  u64 CalcPokemonRecordRankValue( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData );
  u64 CalcDailyRecordRankValue( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData );
  u64 CalcCircleRecordRankValue( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData );
  u64 CalcVarietyRecordRankValue( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData );


private:

  u32 getRank( JoinFestaFriendKey friendKey, JoinFestaRecordRankingData* pRecordRankingArray, u32 length );
  void descendingSort( JoinFestaRecordRankingData* pRecordRankingArray, u32 length );
  void ascendingSort( JoinFestaRecordRankingData* pRecordRankingArray, u32 length );

#if PM_DEBUG
  void debugPrint( JoinFestaRecordRankingData* pRecordRankingArray, u32 length );
#endif // PM_DEBUG

private:

  JoinFestaRecordRankingData    m_aGuestBattleRecordRanking[ ARRAY_MAX ];
  JoinFestaRecordRankingData    m_aGuestTradeRecordRanking[ ARRAY_MAX ];
  JoinFestaRecordRankingData    m_aGuestGreedRecordRanking[ ARRAY_MAX ];
  JoinFestaRecordRankingData    m_aGuestPokemonRecordRanking[ ARRAY_MAX ];
  JoinFestaRecordRankingData    m_aGuestDailyRecordRanking[ ARRAY_MAX ];
  JoinFestaRecordRankingData    m_aGuestCircleRecordRanking[ ARRAY_MAX ];
  JoinFestaRecordRankingData    m_aGuestVarietyRecordRanking[ ARRAY_MAX ];
  JoinFestaRecordRankingData    m_aGuestTotalRecordRanking[ ARRAY_MAX ];

  JoinFestaRecordRankingData    m_aVipBattleRecordRanking[ ARRAY_MAX ];
  JoinFestaRecordRankingData    m_aVipTradeRecordRanking[ ARRAY_MAX ];
  JoinFestaRecordRankingData    m_aVipGreedRecordRanking[ ARRAY_MAX ];
  JoinFestaRecordRankingData    m_aVipPokemonRecordRanking[ ARRAY_MAX ];
  JoinFestaRecordRankingData    m_aVipDailyRecordRanking[ ARRAY_MAX ];
  JoinFestaRecordRankingData    m_aVipCircleRecordRanking[ ARRAY_MAX ];
  JoinFestaRecordRankingData    m_aVipVarietyRecordRanking[ ARRAY_MAX ];
  JoinFestaRecordRankingData    m_aVipTotalRecordRanking[ ARRAY_MAX ];

  u32                           m_GuestRecordRankingLength;
  u32                           m_VipRecordRankingLength;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTARECORDRANKINGMANAGER_H_INCLUDE
