//==============================================================================
/**
 * @file   BattleSpotSave.cpp
 * @date   2015/11/26 Thu. 16:56:50
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <debug/include/gfl2_Assert.h>
#include  <debug/include/gfl2_DebugPrint.h>
#include  <util/include/gfl2_std_string.h>
#include  "../include/BattleSpotSave.h"


namespace Savedata  {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @brief    ctor
  */
//------------------------------------------------------------------
BattleSpotData::BattleSpotData(void)
  : m_BattleSpotSaveData()
{
  Clear(NULL);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
BattleSpotData::~BattleSpotData()
{
  /*  nop  */
}


//------------------------------------------------------------------
/**
  * @brief   参加コミュニティ設定
  * @param   community 設定するコミュニティ
  * @param   type      ENTRY_COMMUNITY_TYPE定義
  */
//------------------------------------------------------------------
void BattleSpotData::SetEntryCommunity( const ENTRY_COMMUNITY& rCommunity, ENTRY_COMMUNITY_TYPE type )
{
  GFL_ASSERT( type < ENTRY_COMMUNITY_MAX ); // @fix
  if( type < ENTRY_COMMUNITY_MAX )
  {
    m_BattleSpotSaveData.entry_community[type]  = rCommunity;
  }
}


//------------------------------------------------------------------
/**
  * @brief   参加コミュニティ取得
  * @param   type      ENTRY_COMMUNITY_TYPE定義
  * @return  ENTRY_COMMUNITY
  */
//------------------------------------------------------------------
BattleSpotData::ENTRY_COMMUNITY*  BattleSpotData::GetEntryCommunity( ENTRY_COMMUNITY_TYPE type )
{
  GFL_ASSERT( type < ENTRY_COMMUNITY_MAX ); // @check
  return &m_BattleSpotSaveData.entry_community[ type ];
}


//------------------------------------------------------------------
/**
  * @brief   参加コミュニティのPID取得
  * @param   type      ENTRY_COMMUNITY_TYPE定義
  * @return  プリンシパルID
  */
//------------------------------------------------------------------
u32 BattleSpotData::GetEntryCommunityPid( ENTRY_COMMUNITY_TYPE type ) const
{
  GFL_ASSERT( type < ENTRY_COMMUNITY_MAX ); // @fix
  if( type < ENTRY_COMMUNITY_MAX )
  {
    return m_BattleSpotSaveData.entry_community[ type ].principalId;
  }
  return 0;
}


//------------------------------------------------------------------
/**
  * @brief   参加コミュニティのコミュニティID取得
  * @param   type      ENTRY_COMMUNITY_TYPE定義
  * @return  プリンシパルID
  */
//------------------------------------------------------------------
u32 BattleSpotData::GetEntryCommunityId( ENTRY_COMMUNITY_TYPE type ) const
{
  GFL_ASSERT( type < ENTRY_COMMUNITY_MAX ); // @fix
  if( type < ENTRY_COMMUNITY_MAX )
  {
    return m_BattleSpotSaveData.entry_community[ type ].community_id;
  }
  return 0;
}


//------------------------------------------------------------------
/**
  * @brief   参加コミュニティのコミュニティID設定
  * @param   type      ENTRY_COMMUNITY_TYPE定義
  * @param   community_id コミュニティID
  */
//------------------------------------------------------------------
void BattleSpotData::SetEntryCommunityId( ENTRY_COMMUNITY_TYPE type, u32 community_id )
{
  GFL_ASSERT( type < ENTRY_COMMUNITY_MAX ); // @check
  m_BattleSpotSaveData.entry_community[ type ].community_id = community_id;
}


//------------------------------------------------------------------
/**
  *  @brief    インターネット大会参加ワーク設定
  */
//------------------------------------------------------------------
void BattleSpotData::SetInternetEntryWork( const u8 work )
{
  m_BattleSpotSaveData.net_entry_work = work;
}


//------------------------------------------------------------------
/**
  *  @brief    インターネット大会参加ワーク取得
  */
//------------------------------------------------------------------
u8   BattleSpotData::GetInternetEntryWork( void ) const
{
  return m_BattleSpotSaveData.net_entry_work;
}


//------------------------------------------------------------------
/**
  *  @brief    インターネット大会試合数設定
  */
//------------------------------------------------------------------
void BattleSpotData::SetInternetMatchCount( const u8 count )
{
  m_BattleSpotSaveData.net_match_count = count;
}


//------------------------------------------------------------------
/**
  *  @brief    インターネット大会試合数加算
  */
//------------------------------------------------------------------
void BattleSpotData::AddInternetMatchCount( void )
{
  // 上限チェック
  if( m_BattleSpotSaveData.net_match_count < 255 )
  {
    ++m_BattleSpotSaveData.net_match_count;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    インターネット大会試合数取得
  */
//------------------------------------------------------------------
u8   BattleSpotData::GetInternetMatchCount( void ) const
{
  return m_BattleSpotSaveData.net_match_count;
}



//------------------------------------------------------------------
/**
  *  @brief    インターネット大会タイムゾーン設定
  */
//------------------------------------------------------------------
void BattleSpotData::SetInternetTimeZone( const s8 time_zone )
{
  m_BattleSpotSaveData.net_time_zone = time_zone;
}


//------------------------------------------------------------------
/**
  *  @brief    インターネット大会タイムゾーン取得
  */
//------------------------------------------------------------------
s8   BattleSpotData::GetInternetTimeZone( void ) const
{
  return m_BattleSpotSaveData.net_time_zone;
  
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void*   BattleSpotData::GetData(void)
{
  return &m_BattleSpotSaveData;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
size_t  BattleSpotData::GetDataSize(void)
{
  return BATTLESPOT_SAVEDATA_SIZE;
}

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void    BattleSpotData::Clear(gfl2::heap::HeapBase* pTempHeap)
{
  gfl2::std::MemClear(&m_BattleSpotSaveData, sizeof(m_BattleSpotSaveData));

  m_BattleSpotSaveData.lock_team_index = INVALID_LOCK_INDEX;
  m_BattleSpotSaveData.lock_team_index_friend = INVALID_LOCK_INDEX;
}


} /*  namespace Savedata  */
