#if !defined(BATTLESPOTSAVE_H_INCLUDED)
#define BATTLESPOTSAVE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotSave.h
 * @date   2015/11/26 Thu. 15:31:40
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>

#include  "Savedata/include/SaveDataInterface.h"


namespace Savedata  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  ライブ大会SaveData
  *  @note   sangoではBattleMatchとしてLive大会やRegulationなどと共にまとめられていたが、
  *          nijiではregulationがExtSaveDataへ移動したこともありBattleSpot単体で分離することとした
  *  @date   2015/11/26 Thu. 15:32:18
  */
//==============================================================================
class BattleSpotData
  : public SaveDataInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(BattleSpotData);
public:
  /**
    参加コミュニティ（参加永続ギャザリング）
    8[B]
  */
  struct ENTRY_COMMUNITY {
    u32   principalId;    ///< PrincipalID  定義はgflnet::friends::PrincipalId
    u32   community_id;   ///< コミュニティID
  };

  /**
    参加コミュニティ
    公式大会１＋レーティング5＋仲間大会8
  */
  enum ENTRY_COMMUNITY_TYPE {
    ENTRY_COMMUNITY_NET,      /**<  公式インターネット大会  */
    ENTRY_COMMUNITY_RATE_S,   /**<  レート：シングル        */
    ENTRY_COMMUNITY_RATE_D,   /**<  レート：ダブル          */
    ENTRY_COMMUNITY_RATE_SP,  /**<  レート：スペシャル      */
    ENTRY_COMMUNITY_RATE_WCS,

    ENTRY_COMMUNITY_MAX
    
  };

  enum
  {
    INVALID_LOCK_INDEX = 0xFF,
  };



  //------------------------------------------------------------------
  /**
    * @brief   参加コミュニティ設定
    * @param   community 設定するコミュニティ
    * @param   type      ENTRY_COMMUNITY_TYPE定義
    */
  //------------------------------------------------------------------
  void SetEntryCommunity( const ENTRY_COMMUNITY& rCommunity, ENTRY_COMMUNITY_TYPE type );


  //------------------------------------------------------------------
  /**
    * @brief   参加コミュニティ取得
    * @param   type      ENTRY_COMMUNITY_TYPE定義
    * @return  ENTRY_COMMUNITY
    */
  //------------------------------------------------------------------
  ENTRY_COMMUNITY*  GetEntryCommunity( ENTRY_COMMUNITY_TYPE type );


  //------------------------------------------------------------------
  /**
    * @brief   参加コミュニティのPID取得
    * @param   type      ENTRY_COMMUNITY_TYPE定義
    * @return  プリンシパルID
    */
  //------------------------------------------------------------------
  u32 GetEntryCommunityPid( ENTRY_COMMUNITY_TYPE type ) const;


  //------------------------------------------------------------------
  /**
    * @brief   参加コミュニティのコミュニティID取得
    * @param   type      ENTRY_COMMUNITY_TYPE定義
    * @return  プリンシパルID
    */
  //------------------------------------------------------------------
  u32 GetEntryCommunityId( ENTRY_COMMUNITY_TYPE type ) const;


  //------------------------------------------------------------------
  /**
    * @brief   参加コミュニティのコミュニティID設定
    * @param   type      ENTRY_COMMUNITY_TYPE定義
    * @param   community_id コミュニティID
    */
  //------------------------------------------------------------------
  void SetEntryCommunityId( ENTRY_COMMUNITY_TYPE type, u32 community_id );


  //------------------------------------------------------------------
  /**
    *  @brief    インターネット大会参加ワーク設定
    */
  //------------------------------------------------------------------
  void SetInternetEntryWork( const u8 work );


  //------------------------------------------------------------------
  /**
    *  @brief    インターネット大会参加ワーク取得
    */
  //------------------------------------------------------------------
  u8   GetInternetEntryWork( void ) const;


  //------------------------------------------------------------------
  /**
    *  @brief    インターネット大会試合数設定
    */
  //------------------------------------------------------------------
  void SetInternetMatchCount( const u8 count );

  //------------------------------------------------------------------
  /**
    *  @brief    インターネット大会試合数加算
    */
  //------------------------------------------------------------------
  void AddInternetMatchCount( void );

  //------------------------------------------------------------------
  /**
    *  @brief    インターネット大会試合数取得
    */
  //------------------------------------------------------------------
  u8   GetInternetMatchCount( void ) const;


  //------------------------------------------------------------------
  /**
    *  @brief    インターネット大会試合数設定
    */
  //------------------------------------------------------------------
  void SetInternetMatchCountToday( const u8 count );

  //------------------------------------------------------------------
  /**
    *  @brief    インターネット大会試合数加算
    */
  //------------------------------------------------------------------
  void AddInternetMatchCountToday( void );

  //------------------------------------------------------------------
  /**
    *  @brief    インターネット大会試合数取得
    */
  //------------------------------------------------------------------
  u8   GetInternetMatchCountToday( void ) const;

  //------------------------------------------------------------------
  /**
    *  @brief    インターネット大会タイムゾーン設定
    */
  //------------------------------------------------------------------
  void SetInternetTimeZone( const s8 time_zone );

  //------------------------------------------------------------------
  /**
    *  @brief    インターネット大会タイムゾーン取得
    */
  //------------------------------------------------------------------
  s8   GetInternetTimeZone( void ) const;


  void SetInternetLockTeamIndex( u8 index ){ m_BattleSpotSaveData.lock_team_index = index; };
  u8   GetInternetLockTeamIndex( void ){ return m_BattleSpotSaveData.lock_team_index; };

  void SetFriendLockTeamIndex( u8 index ){ m_BattleSpotSaveData.lock_team_index_friend = index; };
  u8   GetFriendLockTeamIndex( void ){ return m_BattleSpotSaveData.lock_team_index_friend; };

  //------------------------------------------------------------------
  /**
    *  @brief    インターネット仲間大会参加ワーク
    */
  //------------------------------------------------------------------
  void SetFriendEntryWork( const u8 work ){ m_BattleSpotSaveData.friend_entry_work = work; };
  u8   GetFriendEntryWork( void ) const{ return m_BattleSpotSaveData.friend_entry_work; };


  //------------------------------------------------------------------
  /**
    *  @brief    インターネット仲間大会試合数
    */
  //------------------------------------------------------------------
  void SetFriendMatchCount( const u8 count ){ m_BattleSpotSaveData.friend_match_count = count; };
  void AddFriendMatchCount( void ){ if( m_BattleSpotSaveData.friend_match_count < 255 ){ m_BattleSpotSaveData.friend_match_count++; } };
  u8   GetFriendMatchCount( void ) const{ return m_BattleSpotSaveData.friend_match_count; };



    //------------------------------------------------------------------
  /**
    *  @brief    インターネット仲間大会PID
    */
  //------------------------------------------------------------------
  void SetFriendMatchPID( const u32 pid ){ m_BattleSpotSaveData.friendPrincipalId = pid; };
  u32   GetFriendMatchPID( void ) const{ return m_BattleSpotSaveData.friendPrincipalId; };

      //------------------------------------------------------------------
  /**
    *  @brief    インターネット仲間大会コミュニティID
    */
  //------------------------------------------------------------------
  void SetFriendMatchCommunityID( const u32 id ){ m_BattleSpotSaveData.friendCommunityId = id; };
  u32   GetFriendMatchCommunityID( void ) const{ return m_BattleSpotSaveData.friendCommunityId; };

  /*
    ctor/dtor
  */
  BattleSpotData(void);
  virtual ~BattleSpotData();

  /*
    implement SaveDataInterface I/F
  */
  virtual void*   GetData(void);
  virtual size_t  GetDataSize(void);
  virtual void    Clear(gfl2::heap::HeapBase* pTempHeap);

#if PM_DEBUG
public:
#else
private:
#endif

  /**
    実データ
    @note   (8*4) + 1 + 1 + 1 + 1 = 36[B]
  */
  struct  BATTLESPOT_SAVEDATA
  {
    ENTRY_COMMUNITY   entry_community[ENTRY_COMMUNITY_MAX];   /**<  参加コミュニティ（参加永続ギャザリング）  */  /*   8[B] * 4 */

    u8                net_entry_work;                         /**<  ネット大会参加ワーク                      */  /*   1[B]     */
    u8                net_match_count;                        /**<  ネット大会試合数                          */  /*   1[B]     */
    s8                net_time_zone;                          /**<  ネット大会タイムゾーン                    */  /*   1[B]     */
    u8                lock_team_index;
    u8                friend_entry_work;                         /**<  仲間大会参加ワーク                      */  /*   1[B]     */
    u8                friend_match_count;                        /**<  仲間大会試合数                          */  /*   1[B]     */
    u8                lock_team_index_friend;
    u8                padding1;
    u32               friendPrincipalId;
    u32               friendCommunityId;
    u8                padding[460];                             /**<  padding                                   */  /*   1[B]     */
    
  }m_BattleSpotSaveData;

  static const u32  BATTLESPOT_SAVEDATA_SIZE = sizeof(BATTLESPOT_SAVEDATA);




};


} /*  namespace Savedata  */
#endif  /*  #if !defined(BATTLESPOTSAVE_H_INCLUDED)  */
