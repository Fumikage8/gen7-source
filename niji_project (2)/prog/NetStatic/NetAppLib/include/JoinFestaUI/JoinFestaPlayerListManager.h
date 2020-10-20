// ============================================================================
/*
 * @file JoinFestaPlayerListManager.h
 * @brief プレイヤーリスト管理クラス
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTMANAGER_H_INCLUDE )
#define JOINFESTAPLAYERLISTMANAGER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerData.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordRankingManager.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaPlayerListManager
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListManager );

public:

  JoinFestaPlayerListManager( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaPlayerListManager();

  void UpdateList( bool bFirstUpdateFlag );
  void SortList(const bool bFixStatus = false);    /**< @param [in] bFixStatus  active/passiveを固定し、ソート時には変化させない   */

  // モード３専用
  void UpdateListMode3( bool bFirstUpdateFlag );

  void UpdateListMode4And5( bool bFirstUpdateFlag, u32 listSize );

  NetAppLib::JoinFestaUI::JoinFestPlayerDataList& GetVipList() { return m_VipList; }
  NetAppLib::JoinFestaUI::JoinFestPlayerDataList& GetGuestList() { return m_GuestList; }
  NetAppLib::JoinFestaUI::JoinFestPlayerDataList& GetBlackList() { return m_BlackList; }

  bool IsOnline( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pJoinFestaPlayerData );

  /*  写真のNewフラグをSingletonのPersonalDataManagerへ反映する  */
  void ApplyPhotoFlagToPersonalDataManager(void);

  /*  photo  */
  static void UpdatePhotoDataStatus(NetAppLib::JoinFestaUI::JoinFestPlayerDataList& rTargetList);
  bool  ContainsAnyPhoto(void)  {return (ContainsAnyPhoto(GetGuestList()) || ContainsAnyPhoto(GetVipList()));}    /**<  guest|vipに写真データが1枚以上存在するか  */
  bool  ContainsAnyPhoto(NetAppLib::JoinFestaUI::JoinFestPlayerDataList& rTargetList);                            /**<  リスト内に写真データが1枚以上存在するか   */

private:

  bool isOnline( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pJoinFestaPlayerData, NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList& list );

  void copyList( NetAppLib::JoinFestaUI::JoinFestPlayerDataList& destList, NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList& srcList, NetAppLib::JoinFestaUI::JoinFestaPlayerData::Type eType );
  void copyList( NetAppLib::JoinFestaUI::JoinFestPlayerDataList& destList, NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList& srcList, NetAppLib::JoinFestaUI::JoinFestaPlayerData::Type eType, u32 listSize );
  void copyList( NetAppLib::JoinFestaUI::JoinFestPlayerDataList& destList, NetAppLib::JoinFestaUI::JoinFestPlayerDataList& srcList );
  void sortList( NetAppLib::JoinFestaUI::JoinFestPlayerDataList& destList, NetAppLib::JoinFestaUI::JoinFestPlayerDataList& srcList, const bool bFixStatus = false );
  void dateTimeSort( NetAppLib::JoinFestaUI::JoinFestPlayerDataList& list );


#if PM_DEBUG
  void debugPrint( NetAppLib::JoinFestaUI::JoinFestPlayerDataList& list );
#endif // PM_DEBUG

private:

  NetAppLib::JoinFestaUI::JoinFestPlayerDataList          m_VipList;
  NetAppLib::JoinFestaUI::JoinFestPlayerDataList          m_GuestList;
  NetAppLib::JoinFestaUI::JoinFestPlayerDataList          m_BlackList;
  NetAppLib::JoinFestaUI::JoinFestPlayerDataList          m_TempList;
  NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager   m_JoinFestaRecordRankingManager;
  bool                                                    m_bFirstUpdateCompleteFlag;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAPLAYERLISTMANAGER_H_INCLUDE
