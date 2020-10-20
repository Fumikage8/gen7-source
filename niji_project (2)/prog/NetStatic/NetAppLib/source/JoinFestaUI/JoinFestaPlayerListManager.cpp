// ============================================================================
/*
 * @file JoinFestaPlayerListManager.cpp
 * @brief プレイヤーリスト管理クラス
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <System/include/Date/gfl2_Date.h>
#include <App/FinderStudioStatic/include/FinderStudioDefine.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


JoinFestaPlayerListManager::JoinFestaPlayerListManager( gfl2::heap::HeapBase* pHeap ) :
  m_VipList( pHeap, NetAppLib::JoinFesta::JoinFestaPersonalDataManager::LIST_MAX ),
  m_GuestList( pHeap, NetAppLib::JoinFesta::JoinFestaPersonalDataManager::LIST_MAX ),
  m_BlackList( pHeap, NetAppLib::JoinFesta::JoinFestaDefine::BLACK_LIST_MAX ),
  m_TempList( pHeap, NetAppLib::JoinFesta::JoinFestaPersonalDataManager::LIST_MAX ),
  m_JoinFestaRecordRankingManager(),
  m_bFirstUpdateCompleteFlag( false )
{
}


JoinFestaPlayerListManager::~JoinFestaPlayerListManager()
{
}


void JoinFestaPlayerListManager::UpdateList( bool bFirstUpdateFlag )
{
  if( bFirstUpdateFlag )
  {
    if( m_bFirstUpdateCompleteFlag )
    {
      return;
    }
  }

  m_JoinFestaRecordRankingManager.UpdateRankingList();

  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );

  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pVipList = pJoinFestaPersonalDataManager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pGuestList = pJoinFestaPersonalDataManager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );

  if( pVipList )
  {
    copyList( m_VipList, *pVipList, NetAppLib::JoinFestaUI::JoinFestaPlayerData::TYPE_VIP );
  }

  if( pGuestList )
  {
    copyList( m_GuestList, *pGuestList, NetAppLib::JoinFestaUI::JoinFestaPlayerData::TYPE_GUEST );
  }


#if PM_DEBUG
  GFL_PRINT( "------------------------------------------\n" );
  GFL_PRINT( "Vip list\n" );
  debugPrint( m_VipList );
  GFL_PRINT( "------------------------------------------\n" );
  GFL_PRINT( "Guest list\n" );
  debugPrint( m_GuestList );
#endif // PM_DEBUG

  m_bFirstUpdateCompleteFlag = true;

}


void JoinFestaPlayerListManager::SortList(const bool bFixStatus)
{
  copyList( m_TempList, m_VipList );
  sortList( m_VipList, m_TempList, bFixStatus );

  copyList( m_TempList, m_GuestList );
  sortList( m_GuestList, m_TempList, bFixStatus );
}


void JoinFestaPlayerListManager::UpdateListMode3( bool bFirstUpdateFlag )
{
  if( bFirstUpdateFlag )
  {
    if( m_bFirstUpdateCompleteFlag )
    {
      return;
    }
  }

  m_JoinFestaRecordRankingManager.UpdateRankingList();

  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );

  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pVipList = pJoinFestaPersonalDataManager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pGuestList = pJoinFestaPersonalDataManager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );

  if( pVipList )
  {
    m_VipList.Erase();
    copyList( m_VipList, *pVipList, NetAppLib::JoinFestaUI::JoinFestaPlayerData::TYPE_VIP );
  }

  if( pGuestList )
  {
    m_GuestList.Erase();
    copyList( m_GuestList, *pGuestList, NetAppLib::JoinFestaUI::JoinFestaPlayerData::TYPE_GUEST );
  }

  Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
  
  m_BlackList.Erase();

  for( u32 i = 0; i < NetAppLib::JoinFesta::JoinFestaDefine::BLACK_LIST_MAX; ++i )
  {
    const NetAppLib::JoinFestaUI::JoinFestaBlackPlayerData::CoreData* pBlackPlayerCoreData = pJoinFestaDataSave->GetBlackListPlayerData(i);
    
    if( pBlackPlayerCoreData )
    {
      NetAppLib::JoinFestaUI::JoinFestaPlayerData temp;
      temp.SetType( NetAppLib::JoinFestaUI::JoinFestaPlayerData::TYPE_BLACK );
      temp.SetBlackPlayerCoreData( *pBlackPlayerCoreData );

      // @fix Contact#525 おことわりリストは追加した順に上から表示
      m_BlackList.PushFront( temp );
    }
  }


#if PM_DEBUG
  GFL_PRINT( "------------------------------------------\n" );
  GFL_PRINT( "Vip list\n" );
  debugPrint( m_VipList );
  GFL_PRINT( "------------------------------------------\n" );
  GFL_PRINT( "Guest list\n" );
  debugPrint( m_GuestList );
  GFL_PRINT( "------------------------------------------\n" );
  GFL_PRINT( "Black list\n" );
  debugPrint( m_BlackList );
#endif // PM_DEBUG

  m_bFirstUpdateCompleteFlag = true;
}


void JoinFestaPlayerListManager::UpdateListMode4And5( bool bFirstUpdateFlag, u32 listSize )
{
  if( bFirstUpdateFlag )
  {
    if( m_bFirstUpdateCompleteFlag )
    {
      return;
    }
  }

  m_JoinFestaRecordRankingManager.UpdateRankingList();

  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );

  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pVipList = pJoinFestaPersonalDataManager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pGuestList = pJoinFestaPersonalDataManager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );

  if( pVipList )
  {
    m_VipList.Erase();
    copyList( m_VipList, *pVipList, NetAppLib::JoinFestaUI::JoinFestaPlayerData::TYPE_VIP, listSize );
  }

  if( pGuestList )
  {
    m_GuestList.Erase();
    copyList( m_GuestList, *pGuestList, NetAppLib::JoinFestaUI::JoinFestaPlayerData::TYPE_GUEST, listSize );
  }


#if PM_DEBUG
  GFL_PRINT( "------------------------------------------\n" );
  GFL_PRINT( "Vip list\n" );
  debugPrint( m_VipList );
  GFL_PRINT( "------------------------------------------\n" );
  GFL_PRINT( "Guest list\n" );
  debugPrint( m_GuestList );
#endif // PM_DEBUG

  m_bFirstUpdateCompleteFlag = true;

}


bool JoinFestaPlayerListManager::IsOnline( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pJoinFestaPlayerData )
{
  bool bIsOnline = false;

  if( pJoinFestaPlayerData )
  {
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );

    NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pVipList = pJoinFestaPersonalDataManager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pGuestList = pJoinFestaPersonalDataManager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );

    if( pVipList )
    {
      if( isOnline( pJoinFestaPlayerData, *pVipList ) )
      {
        bIsOnline = true;
      }
    }

    if( pGuestList )
    {
      if( isOnline( pJoinFestaPlayerData, *pGuestList ) )
      {
        bIsOnline = true;
      }
    }
  }

  return bIsOnline;
}


bool JoinFestaPlayerListManager::isOnline( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pJoinFestaPlayerData, NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList& list )
{
  bool bIsOnline = false;

  if( pJoinFestaPlayerData )
  {
    for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = list.Begin(); it != list.End(); ++it )
    {
      if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( pJoinFestaPlayerData->GetJoinFestaFriendKey(), (*it)->GetJoinFestaFriendKey() ) )
      {
        if( (*it)->IsOnline() )
        {
          bIsOnline = true;
        }
      }
    }
  }

  return bIsOnline;
}


//------------------------------------------------------------------
/**
  *  @brief    写真のNewフラグをSingletonのPersonalDataManagerへ反映する
  */
//------------------------------------------------------------------
void JoinFestaPlayerListManager::ApplyPhotoFlagToPersonalDataManager(void)
{
  NetAppLib::JoinFestaUI::JoinFestPlayerDataList* pJFPDLists[] =
  {
    &GetGuestList(),
    &GetVipList(),
  };
  const NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION  jfRelations[] =
  {
    NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST,
    NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP
  };


  NetAppLib::JoinFesta::JoinFestaPersonalDataManager*     pJFPDManager  = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );
  NetAppLib::JoinFesta::JoinFestaPersonalData*            pJFPD;


  for(u32 index = 0; index < GFL_NELEMS(pJFPDLists); ++index)
  {
    NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList        jfpdItr       = pJFPDLists[index]->Begin();
    const NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList  jfpdItrEnd    = pJFPDLists[index]->End();

    for(; jfpdItr != jfpdItrEnd; ++jfpdItr)
    {
      App::FinderStudio::Static::PhotoData  photoData;

      if(!jfpdItr->GetPacketPhotoData(&photoData)) continue;
      if(jfpdItr->IsNewPhoto()) continue;

      for(u32 relationIndex = 0; relationIndex < GFL_NELEMS(jfRelations); ++relationIndex)
      {
        pJFPD = pJFPDManager->GetPersonalData( jfRelations[relationIndex], jfpdItr->GetJoinFestaFriendKey());
        if(pJFPD)
        {
          pJFPD->SetNewPhotoOffEx(photoData.deco_param.crc);    /*  写真のCRCが一致するならNewフラグを落とす  */
        }
      }
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListManager::copyList( NetAppLib::JoinFestaUI::JoinFestPlayerDataList& destList, NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList& srcList, NetAppLib::JoinFestaUI::JoinFestaPlayerData::Type eType )
{
  copyList( m_TempList, destList );

  destList.Erase();

  NetAppLib::JoinFestaUI::JoinFestaPlayerData temp;

  for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator src = srcList.Begin(); src != srcList.End(); ++src )
  {
    temp.Copy( *(*src) );
    temp.SetSelect( false );
    temp.SetPassive( false );
    temp.SetType( eType );

    if( eType == JoinFestaPlayerData::TYPE_GUEST )
    {
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_BATTLE,  m_JoinFestaRecordRankingManager.GetGuestRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_BATTLE,  temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_TRADE,   m_JoinFestaRecordRankingManager.GetGuestRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_TRADE,   temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_GREED,   m_JoinFestaRecordRankingManager.GetGuestRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_GREED,   temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_POKEMON, m_JoinFestaRecordRankingManager.GetGuestRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_POKEMON, temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_DAILY,   m_JoinFestaRecordRankingManager.GetGuestRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_DAILY,   temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_CIRCLE,  m_JoinFestaRecordRankingManager.GetGuestRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_CIRCLE,  temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_VARIETY, m_JoinFestaRecordRankingManager.GetGuestRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_VARIETY, temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_TOTAL,   m_JoinFestaRecordRankingManager.GetGuestRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_TOTAL,   temp.GetJoinFestaFriendKey() ) );
    }
    else
    if( eType == JoinFestaPlayerData::TYPE_VIP )
    {
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_BATTLE,  m_JoinFestaRecordRankingManager.GetVipRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_BATTLE,  temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_TRADE,   m_JoinFestaRecordRankingManager.GetVipRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_TRADE,   temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_GREED,   m_JoinFestaRecordRankingManager.GetVipRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_GREED,   temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_POKEMON, m_JoinFestaRecordRankingManager.GetVipRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_POKEMON, temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_DAILY,   m_JoinFestaRecordRankingManager.GetVipRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_DAILY,   temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_CIRCLE,  m_JoinFestaRecordRankingManager.GetVipRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_CIRCLE,  temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_VARIETY, m_JoinFestaRecordRankingManager.GetVipRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_VARIETY, temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_TOTAL,   m_JoinFestaRecordRankingManager.GetVipRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_TOTAL,   temp.GetJoinFestaFriendKey() ) );
    }

    for( ItJoinFestPlayerDataList it = m_TempList.Begin(); it != m_TempList.End(); ++it )
    {
      if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( (*it).GetJoinFestaFriendKey(), (*src)->GetJoinFestaFriendKey() ) )
      {
        temp.SetSelect( (*it).IsSelect() );
        temp.SetPassive( (*it).IsPassive() );
        break;
      }
    }

    destList.PushBack( temp );
  }

  dateTimeSort( destList );

  UpdatePhotoDataStatus(destList);
}


void JoinFestaPlayerListManager::copyList( NetAppLib::JoinFestaUI::JoinFestPlayerDataList& destList, NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList& srcList, NetAppLib::JoinFestaUI::JoinFestaPlayerData::Type eType, u32 listSize )
{
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );

  copyList( m_TempList, destList );

  destList.Erase();

  NetAppLib::JoinFestaUI::JoinFestaPlayerData temp;

  for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator src = srcList.Begin(); src != srcList.End(); ++src )
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* pScriptTalkFieldPersonalData = pJoinFestaPersonalDataManager->GetScriptTalkFieldPersonalData();

    if( pScriptTalkFieldPersonalData )
    {
      if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( (*src)->GetJoinFestaFriendKey(), pScriptTalkFieldPersonalData->GetJoinFestaFriendKey() ) )
      {
        continue;
      }
    }

    temp.Copy( *(*src) );
    temp.SetSelect( false );
    temp.SetPassive( false );
    temp.SetType( eType );

    if( eType == JoinFestaPlayerData::TYPE_GUEST )
    {
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_BATTLE,  m_JoinFestaRecordRankingManager.GetGuestRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_BATTLE,  temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_TRADE,   m_JoinFestaRecordRankingManager.GetGuestRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_TRADE,   temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_GREED,   m_JoinFestaRecordRankingManager.GetGuestRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_GREED,   temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_POKEMON, m_JoinFestaRecordRankingManager.GetGuestRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_POKEMON, temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_DAILY,   m_JoinFestaRecordRankingManager.GetGuestRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_DAILY,   temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_CIRCLE,  m_JoinFestaRecordRankingManager.GetGuestRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_CIRCLE,  temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_VARIETY, m_JoinFestaRecordRankingManager.GetGuestRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_VARIETY, temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_TOTAL,   m_JoinFestaRecordRankingManager.GetGuestRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_TOTAL,   temp.GetJoinFestaFriendKey() ) );
    }
    else
    if( eType == JoinFestaPlayerData::TYPE_VIP )
    {
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_BATTLE,  m_JoinFestaRecordRankingManager.GetVipRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_BATTLE,  temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_TRADE,   m_JoinFestaRecordRankingManager.GetVipRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_TRADE,   temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_GREED,   m_JoinFestaRecordRankingManager.GetVipRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_GREED,   temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_POKEMON, m_JoinFestaRecordRankingManager.GetVipRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_POKEMON, temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_DAILY,   m_JoinFestaRecordRankingManager.GetVipRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_DAILY,   temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_CIRCLE,  m_JoinFestaRecordRankingManager.GetVipRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_CIRCLE,  temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_VARIETY, m_JoinFestaRecordRankingManager.GetVipRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_VARIETY, temp.GetJoinFestaFriendKey() ) );
      temp.SetRank( JoinFestaPlayerData::RECORD_CATEGORY_TOTAL,   m_JoinFestaRecordRankingManager.GetVipRank( JoinFestaRecordRankingManager::RECORD_CATEGORY_TOTAL,   temp.GetJoinFestaFriendKey() ) );
    }

    for( ItJoinFestPlayerDataList it = m_TempList.Begin(); it != m_TempList.End(); ++it )
    {
      if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( (*it).GetJoinFestaFriendKey(), (*src)->GetJoinFestaFriendKey() ) )
      {
        temp.SetSelect( (*it).IsSelect() );
        temp.SetPassive( (*it).IsPassive() );
        break;
      }
    }

    if( !(*src)->IsScriptListSelected() )
    {
      destList.PushBack( temp );
    }
  }

  // シャッフル
  if( listSize > 0 )
  {
    if( destList.Size() > listSize )
    {
      gfl2::math::Random rand;
      rand.Initialize();

      for( u32 i = 0; i < 50; ++i )
      {
        ItJoinFestPlayerDataList it1 = destList.Begin();
        ItJoinFestPlayerDataList it2 = destList.Begin();

        u32 rand1 = rand.Next( destList.Size() );
        u32 rand2 = rand.Next( destList.Size() );

        if( rand1 != rand2 )
        {
          for( u32 j = 0; j < rand1; ++j ){ ++it1; }
          for( u32 j = 0; j < rand2; ++j ){ ++it2; }

          if( it1 != destList.End() && it2 != destList.End() )
          {
            JoinFestaPlayerData temp = (*it1);

            (*it1) = (*it2);
            (*it2) = temp;
          }
        }
      }
    }
  }

  // 指定されたサイズにリサイズ
  if( listSize > 0 )
  {
    while( destList.Size() > listSize )
    {
      destList.PopBack();
    }
  }

  dateTimeSort( destList );

  UpdatePhotoDataStatus(destList);
}


void JoinFestaPlayerListManager::copyList( NetAppLib::JoinFestaUI::JoinFestPlayerDataList& destList, NetAppLib::JoinFestaUI::JoinFestPlayerDataList& srcList )
{
  destList.Erase();

  for( ItJoinFestPlayerDataList it = srcList.Begin(); it != srcList.End(); ++it )
  {
    destList.PushBack( *it );
  }
}


void JoinFestaPlayerListManager::sortList( NetAppLib::JoinFestaUI::JoinFestPlayerDataList& destList, NetAppLib::JoinFestaUI::JoinFestPlayerDataList& srcList, const bool bFixStatus )
{
  destList.Erase();

  // フラグ操作
  if(!bFixStatus)
  {
    for( ItJoinFestPlayerDataList it = srcList.Begin(); it != srcList.End(); ++it )
    {
      if( (*it).IsPassive() && (*it).IsOnline() )
      {
        (*it).SetPassive( false );
      }
      else
      if( !(*it).IsOnline() )
      {
        (*it).SetPassive( true );

        // パッシブ状態にしたら選択状態を解除する
        (*it).SetSelect( false );
      }
    }
  }

  // 以下の順番に追加する

  ItJoinFestPlayerDataList it = srcList.Begin();

  // 選択状態の物を優先
  while( it != srcList.End() )
  {
    if( (*it).IsSelect() )
    {
      destList.PushBack( (*it) );
      it = srcList.Erase( it );
    }
    else
    {
      ++it;
    }
  }

  it = srcList.Begin();

  // オンライン状態の物を優先
  while( it != srcList.End() )
  {
    if( bFixStatus ? (!(*it).IsPassive()) : (*it).IsOnline() )  /*  状態固定時はPassive状態のみを見る（※ただ!bFixStatus時はOnline時にActiveとされるので、常にPassive状態を評価すれば良いともいえる）  */
    {
      destList.PushBack( (*it) );
      it = srcList.Erase( it );
    }
    else
    {
      ++it;
    }
  }

  it = srcList.Begin();

  while( it != srcList.End() )
  {
    destList.PushBack( (*it) );
    it = srcList.Erase( it );
  }

}


void JoinFestaPlayerListManager::dateTimeSort( NetAppLib::JoinFestaUI::JoinFestPlayerDataList& list )
{
  for( ItJoinFestPlayerDataList it1 = list.Begin(); it1 != list.End(); ++it1 )
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData coreData1;
    it1->GetCoreData( &coreData1 );
    gfl2::system::Date date1( 2000 + coreData1.m_date.year, coreData1.m_date.month, coreData1.m_date.day, coreData1.m_date.hour, coreData1.m_date.minute, coreData1.m_date.half_sec );

    ItJoinFestPlayerDataList it2 = it1;
    ++it2;

    for( ; it2 != list.End(); ++it2 )
    {
      NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData coreData2;
      it2->GetCoreData( &coreData2 );
      gfl2::system::Date date2( 2000 + coreData2.m_date.year, coreData2.m_date.month, coreData2.m_date.day, coreData2.m_date.hour, coreData2.m_date.minute, coreData2.m_date.half_sec );

      if( date1.GetDateTimeToSecond() < date2.GetDateTimeToSecond() )
      {
        JoinFestaPlayerData temp = (*it1);

        (*it1) = (*it2);
        (*it2) = temp;

#if 0
        date1.SetYear( date2.GetYear() );
        date1.SetMonth( date2.GetMonth() );
        date1.SetDay( date2.GetDay() );
        date1.SetHour( date2.GetHour() );
        date1.SetMinute( date2.GetMinute() );
        date1.SetSecond( date2.GetSecond() );
#else
        gfl2::system::Date::Copy( &date2, &date1 );
#endif
      }
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListManager::UpdatePhotoDataStatus(NetAppLib::JoinFestaUI::JoinFestPlayerDataList& rTargetList)
{
  ItJoinFestPlayerDataList              itr    = rTargetList.Begin();
  const ItJoinFestPlayerDataList        itrEnd = rTargetList.End();
  App::FinderStudio::Static::PhotoData  photoData;

  for(; itr !=  itrEnd; ++itr)
  {
    const bool  bPhotoDataAvailable = itr->GetPacketPhotoData(&photoData);
    if(bPhotoDataAvailable)
    {
      /*  写真データ有効  */
      const bool  bPhotoDataIsNew = (itr->IsNewPhoto());
      if(bPhotoDataIsNew) itr->FS_SetPhotoDataStatus_AvailableNew();
      else                itr->FS_SetPhotoDataStatus_Available();
    }
    else
    {
      /*  写真データ無効  */
      itr->FS_SetPhotoDataStatus_NotAvailable();
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    リスト内に写真データが1枚以上存在するか
  */
//------------------------------------------------------------------
bool  JoinFestaPlayerListManager::ContainsAnyPhoto(NetAppLib::JoinFestaUI::JoinFestPlayerDataList& rTargetList)
{
  ItJoinFestPlayerDataList        itr    = rTargetList.Begin();
  const ItJoinFestPlayerDataList  itrEnd = rTargetList.End();
  
  for(; itr !=  itrEnd; ++itr)
  {
    if(itr->FS_IsPhotoDataAvailable()) return true;
  }
  
  return false;
}


#if PM_DEBUG
void JoinFestaPlayerListManager::debugPrint( NetAppLib::JoinFestaUI::JoinFestPlayerDataList& list )
{
  int count = 0;
  for( ItJoinFestPlayerDataList it = list.Begin(); it != list.End(); ++it )
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData coreData;
    it->GetCoreData( &coreData );
    gfl2::system::Date date( 2000 + coreData.m_date.year, coreData.m_date.month, coreData.m_date.day, coreData.m_date.hour, coreData.m_date.minute, coreData.m_date.half_sec );

    GFL_PRINT( "%d : val = %llu : y(%d) m(%d) d(%d) h(%d) m(%d) hs(%d), name = %ls, id = %d \n", count, date.GetDateTimeToSecond(), 2000 + coreData.m_date.year, coreData.m_date.month, coreData.m_date.day, coreData.m_date.hour, coreData.m_date.minute, coreData.m_date.half_sec, it->GetName(), it->GetPrincipalID() );

    ++count;
  }
}
#endif // PM_DEBUG


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)
